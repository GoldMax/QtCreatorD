#include "dproject.h"

#include "dprojectmanagerconstants.h"
#include "dproject.h"
#include "dprojectnodes.h"
#include "dmakestep.h"
#include "dbuildconfiguration.h"
#include "drunconfiguration.h"

#include "deditor/qcdassist.h"

#include <coreplugin/icontext.h>
#include <coreplugin/documentmanager.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/target.h>
#include <projectexplorer/session.h>
#include <utils/fileutils.h>
#include <utils/qtcassert.h>
#include <utils/mimetypes/mimedatabase.h>

#include <QSettings>

using namespace Core;
using namespace ProjectExplorer;
using namespace Utils;

namespace DProjectManager {
//--------------------------------------------------------------------------------------
//
// DProject
//
//--------------------------------------------------------------------------------------
DProject::DProject(const Utils::FilePath &fileName)
	: ProjectExplorer::Project(Constants::DPROJECT_MIMETYPE, fileName)
{
	setId(Constants::DPROJECT_ID);
	setProjectLanguages(Context(ProjectExplorer::Constants::CXX_LANGUAGE_ID));
	setDisplayName(fileName.toFileInfo().completeBaseName());

	if(QcdAssist::isDCDEnabled())
		QcdAssist::sendAddImportToDCD(fileName.parentDir().toString());
}

DProject::~DProject()
{
	setRootProjectNode(nullptr);
}

void DProject::setIncludes(QString value)
{
	if(m_includes == value)
		return;

	m_includes = value;
	if(QcdAssist::isDCDEnabled())
	{
		QDir dir(this->projectDirectory().toString());
		foreach(QString s, this->includes().split(QLatin1Char(' '), QString::SkipEmptyParts))
		{
			if(s.startsWith(QLatin1String("-I")))
				s = s.remove(0,2);
			if(QDir::isAbsolutePath(s))
				QcdAssist::sendAddImportToDCD(s);
			else
				QcdAssist::sendAddImportToDCD(dir.absoluteFilePath(s));
		}
	}
}

bool DProject::addFiles(const QStringList& filePaths)
{
	QSettings projectFiles(projectFilePath().toString(), QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String(Constants::INI_FILES_ROOT_KEY));

	QDir projDir(projectDirectory().toString());
	foreach (const QString &filePath, filePaths)
	{
		QString fp = projDir.relativeFilePath(filePath);
		if(files().contains(fp) == false)
			projectFiles.setValue(fp, 0);
	}
	refresh(Files);
	return true;
}
bool DProject::removeFiles(const QStringList &filePaths)
{
	QSettings projectFiles(projectFilePath().toString(), QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String("Files"));

	QDir projDir(projectDirectory().toString());
	foreach (const QString &filePath, filePaths)
	{
		QString fp = projDir.relativeFilePath(filePath);
		if(files().contains(fp))
			projectFiles.remove(fp);
	}
	refresh(Files);
	return true;
}
bool DProject::renameFile(const QString &filePath, const QString &newFilePath)
{
	QSettings projectFiles(projectFilePath().toString(), QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String("Files"));

	QDir projDir(projectDirectory().toString());
	projectFiles.remove(projDir.relativeFilePath(filePath));
	projectFiles.setValue(projDir.relativeFilePath(newFilePath), 0);
	refresh(Files);
	return true;
}

QVariantMap DProject::toMap() const
{
	QSettings sets(projectFilePath().toString(),	QSettings::IniFormat);
	sets.setValue(QLatin1String(Constants::INI_SOURCE_ROOT_KEY), sourcesDirectory());
	sets.setValue(QLatin1String(Constants::INI_INCLUDES_KEY), includes());
	sets.setValue(QLatin1String(Constants::INI_LIBRARIES_KEY), libraries());
	sets.setValue(QLatin1String(Constants::INI_EXTRA_ARGS_KEY), extraArgs());
	sets.setValue(QLatin1String(Constants::INI_COMPILE_PRIORITY_KEY), compilePriority());
	sets.sync();

	return Project::toMap();
}
Project::RestoreResult DProject::fromMap(const QVariantMap &map, QString *errorMessage)
{
	RestoreResult result = Project::fromMap(map, errorMessage);
	if (result != RestoreResult::Ok)
					return result;

	if (!activeTarget())
					addTargetForDefaultKit();

	// Sanity check: We need both a buildconfiguration and a runconfiguration!
	const QList<Target *> targetList = targets();
	if (targetList.isEmpty())
		return RestoreResult::Error;
	for (Target *t : targetList)
	{
		if (!t->activeBuildConfiguration())
		{
			removeTarget(t);
			continue;
		}
		if (!t->activeRunConfiguration())
			t->addRunConfiguration(new DRunConfiguration(t, Constants::BUILDRUN_CONFIG_ID));
	}

	refresh(Everything);
	return result;
}

bool DProject::parseProjectFile(RefreshOptions options)
{
	QSettings sets(projectFilePath().toString(), QSettings::IniFormat);

	bool result = false;
	if (options & Configuration)
	{
		QString oldSD = this->sourcesDirectory();
		setSourcesDirectory(sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString());
		setLibraries(sets.value(QLatin1String(Constants::INI_LIBRARIES_KEY)).toString());
		setIncludes(sets.value(QLatin1String(Constants::INI_INCLUDES_KEY)).toString());
		setExtraArgs(sets.value(QLatin1String(Constants::INI_EXTRA_ARGS_KEY)).toString());
		setCompilePriority(sets.value(QLatin1String(Constants::INI_COMPILE_PRIORITY_KEY)).toInt());

		if(oldSD != sourcesDirectory())
			options = static_cast<RefreshOptions>(options | RefreshOptions::Files);
		else if(this->containerNode() && this->containerNode()->priority() != compilePriority())
			result = true;
	}

	if(options & Files)
	{
		m_files.clear();

		FilePath src;
		if(sourcesDirectory() != ".")
			src = FilePath::fromString(sourcesDirectory());

		sets.beginGroup(QLatin1String(Constants::INI_FILES_ROOT_KEY));
		foreach(QString rel, sets.allKeys())
			m_files.append(rel);

		emit fileListChanged();
	}
	return result || (options & Files);
}
void DProject::refresh(RefreshOptions options)
{
	bool needRebuildTree = parseProjectFile(options);

	if (needRebuildTree)
	{
		auto root = new DProjectNode(this);
		int prior = Node::PriorityLevel::DefaultProjectPriority + this->compilePriority();
		root->setPriority(prior);

		FilePath src;
		if(sourcesDirectory() != ".")
			src = FilePath::fromString(sourcesDirectory());
		FilePath srcAbs = projectDirectory().pathAppended(src.toString());
		QDir srcDir(srcAbs.toString());

		// adding
		for (QString relS : m_files)
		{
			relS = projectDirectory().pathAppended(relS).toString();
			relS = srcDir.relativeFilePath(relS);
			FilePath	rel = FilePath::fromString(relS);

			FolderNode* folder = root->asFolderNode();
			FilePath absPath = srcAbs;

			QStringList parts = rel.toString().split(QDir::separator());
			QString fileName = parts.back();
			parts.pop_back();
			for(QString part : parts)
			{
				FolderNode* fn = nullptr;
				absPath = absPath.pathAppended(part);

				for (Node* f : folder->nodes())
					if(f->displayName() == part && f->asFolderNode() != nullptr)
					{
						fn = f->asFolderNode();
						break;
					}
				if(fn == nullptr)
				{
					fn = new FolderNode(absPath);
					fn->setDisplayName(part);
					folder->addNode(std::unique_ptr<FolderNode>(fn));
				}
				folder = fn;
			}

			//QString fileName = rel.fileName();
			absPath = absPath.pathAppended(fileName);
			FileNode* node = nullptr;
			for (Node* n : folder->nodes())
				if(n->filePath() == absPath && n->asFileNode() != nullptr)
				{
					node = n->asFileNode();
					break;
				}
			if(node == nullptr)
			{
				FileType type = FileType::Unknown;
				if(fileName.endsWith(QLatin1String(".d")))
					type = FileType::Source;
				else if(fileName.endsWith(QLatin1String(".ui")))
					type = FileType::Form;
				else if(fileName.endsWith(QLatin1String(".di")))
					type = FileType::Header;

				FileNode* fn = new FileNode(absPath, type);
				folder->addNode(std::unique_ptr<FileNode>(fn));
			}
		}

		// setRootProjectNode не хочет добавлять пустые ветви, поэтому мухлюем
		if(root->isEmpty())
		{
			auto prjNone = std::make_unique<FileNode>(projectFilePath(),	FileType::Project);
			root->addNestedNode(std::move(prjNone), projectDirectory());
		}
		setRootProjectNode(std::unique_ptr<ProjectNode>(root));
		this->containerNode()->setPriority(prior);
	}
}

bool DProject::setupTarget(Target *t)
{
	BuildConfigurationFactory *factory = BuildConfigurationFactory::find(t);
	if (!factory)
		return false;

	Utils::FilePath projectDir = t->project()->projectDirectory();

	BuildInfo info(factory);
	info.displayName = tr("Debug");
	info.typeName = tr("D Build");
	info.buildDirectory = projectDir;
	info.kitId = t->kit()->id();

	BuildConfiguration* bc = factory->create(t,info);
	if (!bc)
		return false;
	t->addBuildConfiguration(bc);
	SessionManager::instance()->setActiveBuildConfiguration(t,bc, SetActive::Cascade);

	info = BuildInfo(factory);
	info.displayName = tr("Release");
	info.typeName = tr("D Build");
	info.buildDirectory = projectDir;
	info.kitId = t->kit()->id();

	bc = factory->create(t,info);
	if (!bc)
		return false;
	t->addBuildConfiguration(bc);

	info = BuildInfo(factory);
	info.displayName = tr("Unittest");
	info.typeName = tr("D Build");
	info.buildDirectory = projectDir;
	info.kitId = t->kit()->id();

	bc = factory->create(t,info);
	if (!bc)
		return false;
	t->addBuildConfiguration(bc);

	SessionManager::instance()->setActiveTarget(this, t, SetActive::Cascade);

	return true;
}

//--------------------------------------------------------------------------------------
//
// DProjectGroup
//
//--------------------------------------------------------------------------------------
DProjectGroup::DProjectGroup(const Utils::FilePath &fileName)
	: ProjectExplorer::Project(Constants::DPROJECTGROUP_MIMETYPE, fileName)
{
	setId(Constants::DPROJECTGROUP_ID);
	setProjectLanguages(Context(ProjectExplorer::Constants::CXX_LANGUAGE_ID));
	setDisplayName(fileName.toFileInfo().completeBaseName());
}
DProjectGroup::~DProjectGroup()
{
	setRootProjectNode(nullptr);
}

bool DProjectGroup::addSubProject(const QString& projFilePath)
{
	QSettings projs(projectFilePath().toString(), QSettings::IniFormat);
	projs.beginGroup(QLatin1String(Constants::INI_PROJECTS_ROOT_KEY));

	foreach(QString p, projs.allKeys())
		if(projFilePath == projFilePath)
			return false;

	projs.setValue(projFilePath, 0);
	projs.sync();

	QString s;
	refresh(&s);
	return true;
}

QVariantMap DProjectGroup::toMap() const
{
//	QSettings sets(projectFilePath().toString(),	QSettings::IniFormat);
//	sets.setValue(QLatin1String(Constants::INI_SOURCE_ROOT_KEY), sourcesDirectory());
//	sets.setValue(QLatin1String(Constants::INI_INCLUDES_KEY), includes());
//	sets.setValue(QLatin1String(Constants::INI_LIBRARIES_KEY), libraries());
//	sets.setValue(QLatin1String(Constants::INI_EXTRA_ARGS_KEY), extraArgs());
//	sets.setValue(QLatin1String(Constants::INI_COMPILE_PRIORITY_KEY), compilePriority());
//	sets.sync();

	return Project::toMap();
}
Project::RestoreResult DProjectGroup::fromMap(const QVariantMap &map, QString *errorMessage)
{
	RestoreResult result = Project::fromMap(map, errorMessage);
	if (result != RestoreResult::Ok)
					return result;

//	if (!activeTarget())
//					addTargetForDefaultKit();


//	// Sanity check: We need both a buildconfiguration and a runconfiguration!
//	const QList<Target *> targetList = targets();
//	if (targetList.isEmpty())
//		return RestoreResult::Error;
//	for (Target *t : targetList)
//	{
//		if (!t->activeBuildConfiguration())
//		{
//			removeTarget(t);
//			continue;
//		}
//		if (!t->activeRunConfiguration())
//			t->addRunConfiguration(new DRunConfiguration(t, Constants::BUILDRUN_CONFIG_ID));
//	}

	refresh(errorMessage);
	return result;
}
static void appendError(QString &errorString, const QString &error)
{
	if (error.isEmpty())
		return;

	if (!errorString.isEmpty())
		errorString.append(QLatin1Char('\n'));
	errorString.append(error);
}
void DProjectGroup::refresh(QString *errorMessage)
{
	QSettings sets(projectFilePath().toString(), QSettings::IniFormat);

//	setSourcesDirectory(sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString());
//	setLibraries(sets.value(QLatin1String(Constants::INI_LIBRARIES_KEY)).toString());
//	setIncludes(sets.value(QLatin1String(Constants::INI_INCLUDES_KEY)).toString());
//	setExtraArgs(sets.value(QLatin1String(Constants::INI_EXTRA_ARGS_KEY)).toString());
//	setCompilePriority(sets.value(QLatin1String(Constants::INI_COMPILE_PRIORITY_KEY)).toInt());

//	if(options & Files)
//	{
//		m_files.clear();

//		FilePath src;
//		if(sourcesDirectory() != ".")
//			src = FilePath::fromString(sourcesDirectory());

		//QStringList fileNames;

		sets.beginGroup(QLatin1String(Constants::INI_PROJECTS_ROOT_KEY));
		for(QString rel : sets.allKeys())
		{
			if(rel.length() == 0)
				continue;
			if(QDir::isRelativePath(rel))
				rel = "/" + rel;
			qDebug() << "Project: " << rel;
			//fileNames.append(rel);

			QTC_ASSERT(!rel.isEmpty(), continue);


			const QFileInfo fi(rel);
			const auto filePath = Utils::FilePath::fromString(fi.absoluteFilePath());
			Project *found = Utils::findOrDefault(SessionManager::projects(),
																																									Utils::equal(&Project::projectFilePath, filePath));
			if (found)
			{
				//alreadyOpen.append(found);
			//	SessionManager::reportProjectLoadingProgress();
				continue;
			}

			Utils::MimeType mt = Utils::mimeTypeForFile(rel);
			if (ProjectManager::canOpenProjectForMimeType(mt) == false)
			{
				appendError(*errorMessage, tr("Failed opening project \"%1\": No plugin can open project type \"%2\".")
																.arg(QDir::toNativeSeparators(rel))
																.arg(mt.name()));
				continue;
			}

			if (!filePath.toFileInfo().isFile())
			{
				appendError(*errorMessage,
																tr("Failed opening project \"%1\": Project is not a file.")
																.arg(rel));
			}
			else if (Project *pro = ProjectManager::openProject(mt, filePath))
			{
//					QObject::connect(pro, &Project::parsingFinished, [pro]() {
//						emit SessionManager::instance()->projectFinishedParsing(pro);
//					});
				QString restoreError;
				Project::RestoreResult restoreResult = pro->restoreSettings(&restoreError);
				if (restoreResult == Project::RestoreResult::Ok)
				{
//						connect(pro, &Project::fileListChanged,
//														m_instance, &ProjectExplorerPlugin::fileListChanged);
//						SessionManager::addProject(pro);
					m_projects += pro;
				}
				else
				{
					if (restoreResult == Project::RestoreResult::Error)
						appendError(*errorMessage, restoreError);
					delete pro;
				}
			}

//			if (fileNames.size() > 1)
//				SessionManager::reportProjectLoadingProgress();
		}

//		ProjectExplorerPlugin::OpenProjectResult res =
//				ProjectExplorerPlugin::instance()->openProjects(fileNames);
//		*errorMessage = res.errorMessage();

//		for(Project* p : res.projects())
//			m_projects.append(p);

		auto root = std::make_unique<DProjectGroupNode>(this);
		for(Project* p : m_projects)
		{
			//root->addNode(std::unique_ptr<Node>(p->containerNode()));
			//p->containerNode()->setParentFolderNode(root.get());
			root->addNode(std::unique_ptr<Node>(p->rootProjectNode()));
			//p->rootProjectNode()->setParentFolderNode(root.get());
		}

		// setRootProjectNode не хочет добавлять пустые ветви, поэтому мухлюем
		if(root->isEmpty())
		{
			auto prjNone = std::make_unique<FileNode>(projectFilePath(),	FileType::Project);
			root->addNestedNode(std::move(prjNone), projectDirectory());
		}
		setRootProjectNode(std::move(root));
//	}
}


} // namespace DProjectManager
