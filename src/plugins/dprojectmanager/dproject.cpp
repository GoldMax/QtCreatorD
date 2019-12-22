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
//#include <coreplugin/icore.h>
//#include <cpptools/cpptoolsconstants.h>
//#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/kitmanager.h>
//#include <projectexplorer/abi.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/target.h>
//#include <projectexplorer/projectexplorerconstants.h>
//#include <projectexplorer/editorconfiguration.h>
#include <projectexplorer/session.h>
//#include <qtsupport/customexecutablerunconfiguration.h>
#include <utils/fileutils.h>
//#include <utils/qtcassert.h>

//#include <QDir>
//#include <QProcessEnvironment>
#include <QSettings>

using namespace Core;
using namespace ProjectExplorer;
using namespace Utils;

//namespace {
//const char ACTIVE_TARGET_KEY[] = "ProjectExplorer.Project.ActiveTarget";
//const char TARGET_KEY_PREFIX[] = "ProjectExplorer.Project.Target.";
//const char TARGET_COUNT_KEY[] = "ProjectExplorer.Project.TargetCount";
//const char EDITOR_SETTINGS_KEY[] = "ProjectExplorer.Project.EditorSettings";
//const char PLUGIN_SETTINGS_KEY[] = "ProjectExplorer.Project.PluginSettings";
//} // namespace

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

//	setProjectContext(Context(DProjectManager::Constants::DPROJECTCONTEXT));
	//auto	m_projectFile  = new DProjectFile(this, fileName, RefreshOptions::Everything);
	//DocumentManager::addDocument(m_projectFile);
	//setDocument(m_projectFile);

	if(QcdAssist::isDCDEnabled())
		QcdAssist::sendAddImportToDCD(fileName.parentDir().toString());
}

DProject::~DProject()
{
//	m_codeModelFuture.cancel();
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
	projectFiles.beginGroup(QLatin1String("Files"));
	foreach (const QString &filePath, filePaths)
	{
		if(m_files.contains(filePath) == false)
			projectFiles.setValue(buildDirectory().relativeFilePath(filePath), 0);
	}
	refresh(Files);
	return true;
}

bool DProject::removeFiles(const QStringList &filePaths)
{
	QSettings projectFiles(projectFilePath().toString(), QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String("Files"));
	foreach (const QString &filePath, filePaths)
		if(m_files.contains(filePath))
		{
			projectFiles.remove(m_files[filePath]);
			m_files.remove(filePath);
		}
	refresh(Files);
	return true;
}

bool DProject::renameFile(const QString &filePath, const QString &newFilePath)
{
	QSettings projectFiles(projectFilePath().toString(), QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String("Files"));
	projectFiles.remove(buildDirectory().relativeFilePath(filePath));
	projectFiles.setValue(buildDirectory().relativeFilePath(newFilePath), 0);
	refresh(Files);
	return true;
}

QVariantMap DProject::toMap() const
{
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
		/*// Sanity check: We need both a buildconfiguration and a runconfiguration!
		QList<Target *> targetList = targets();
		foreach (Target *t, targetList)
		{
			if (!t->activeBuildConfiguration())
			{
				removeTarget(t);
				try {
					delete t;
				} catch(...) {}
				continue;
			}
			if (!t->activeRunConfiguration())
			{
				QList<IRunConfigurationFactory *> rfs = IRunConfigurationFactory::find(t);
				if (rfs.length() > 0 )
					foreach(IRunConfigurationFactory* rf, rfs)
					{
						RunConfiguration* rc = rf->create(t,Core::Id(Constants::BUILDRUN_CONFIG_ID));
						if(rc)
						{
							t->addRunConfiguration(rf->create(t, ProjectExplorer::Constants::BUILDSTEPS_BUILD));
							break;
						}
					}

			}
		}
		*/

	refresh(Everything);
	return result;
}

void DProject::refresh(RefreshOptions options)
{
	bool needRebuildTree = parseProjectFile(options);

	if (needRebuildTree || (options & Files))
	{
		auto root = std::make_unique<DProjectNode>(this);

		// adding
		typedef QHash<QString, QString>::ConstIterator FilesKeyValue;
		for (FilesKeyValue kv = files().constBegin(); kv != files().constEnd(); ++kv)
		{
			FolderNode* folder = root->asFolderNode();
			QStringList parts = kv.value().split(QDir::separator());
			QString absFolderPath = this->buildDirectory().path();
			parts.pop_back();
			foreach(QString part, parts)
			{
				FolderNode* fn = nullptr;
				absFolderPath.append(QDir::separator()).append(part);

				for (Node* f : folder->nodes())
					if(f->displayName() == part && f->asFolderNode() != nullptr)
					{
						fn = f->asFolderNode();
						break;
					}
				if(fn == nullptr)
				{
					fn = new FolderNode(Utils::FilePath::fromString(absFolderPath));
					fn->setDisplayName(part);
					folder->addNode(std::unique_ptr<FolderNode>(fn));
				}
				folder = fn;
			}
			FileNode* node = nullptr;
			//foreach(FileNode* n, folder->fileNodes())
			for (Node* n : folder->nodes())
				if(n->filePath().toString() == kv.key() && n->asFileNode() != nullptr)
				{
					node = n->asFileNode();
					break;
				}
			if(node == nullptr)
			{
				FileType type = FileType::Unknown;
				if(kv.key().endsWith(QLatin1String(".d")))
					type = FileType::Source;
				else if(kv.key().endsWith(QLatin1String(".ui")))
					type = FileType::Form;
				else if(kv.key().endsWith(QLatin1String(".di")))
					type = FileType::Header;

				FileNode* fn = new FileNode(Utils::FilePath::fromString(kv.key()), type);
				folder->addNode(std::unique_ptr<FileNode>(fn));
			}
		}

		// setRootProjectNode не хочет добавлять пустые ветви, поэтому мухлюем
		if(root->isEmpty())
		{
			auto prjNone = std::make_unique<FileNode>(projectFilePath(),	FileType::Project);
			root->addNestedNode(std::move(prjNone), projectDirectory());
		}
		setRootProjectNode(std::move(root));
	}
}

bool DProject::parseProjectFile(RefreshOptions options)
{
	QSettings sets(projectFilePath().toString(), QSettings::IniFormat);

	QDir old = buildDirectory();
	bool needRebuild = false;
	if (options & Configuration)
	{
		setSourcesDirectory(sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString());
		setLibraries(sets.value(QLatin1String(Constants::INI_LIBRARIES_KEY)).toString());
		setIncludes(sets.value(QLatin1String(Constants::INI_INCLUDES_KEY)).toString());
		setExtraArgs(sets.value(QLatin1String(Constants::INI_EXTRA_ARGS_KEY)).toString());

		Utils::FilePath dir = projectDirectory();
		if(sourcesDirectory().length() > 0 && sourcesDirectory() != QLatin1String("."))
			dir = dir.pathAppended(sourcesDirectory());

		if((needRebuild = (dir.toString() != buildDirectory().path())))
			m_buildRootDir.setPath(dir.toString());
	}

	if (needRebuild || (options & Files))
	{
		m_files.clear();
		if(needRebuild && old.path() == ".") // при инициализации проекта
			needRebuild = false;

		sets.beginGroup(QLatin1String(Constants::INI_FILES_ROOT_KEY));
		foreach(QString rel, sets.allKeys())
		{
			QString abs;
			if(needRebuild)
			{
				abs = old.absoluteFilePath(rel);
				sets.remove(rel);
				rel = buildDirectory().relativeFilePath(abs);
				sets.setValue(rel,0);
			}
			else
				abs = buildDirectory().absoluteFilePath(rel);
			m_files[abs] = rel;
		}
		emit fileListChanged();
	}
	return needRebuild;
}

bool DProject::setupTarget(Target *t)
{
	//refresh(RefreshOptions::Everything);
	//return Project::setupTarget(t);

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

//	QList<RunConfigurationFactory *> rfs = RunConfigurationFactory::find(t);
//	if (rfs.length() == 0 )
//		return false;

//	RunConfiguration* rc = 0;
//	foreach(IRunConfigurationFactory* rf, rfs)
//	{
//		rc = rf->create(t,Core::Id(Constants::BUILDRUN_CONFIG_ID));
//		if(rc)
//			break;
//	}
//	if (!rc)
//		return false;

//	t->addRunConfiguration(rc);
//	t->setActiveRunConfiguration(rc);

	SessionManager::instance()->setActiveTarget(this, t, SetActive::Cascade);

	return true;
}


////--------------------------------------------------------------------------------------
////
//// DProjectFile
////
////--------------------------------------------------------------------------------------

//DProjectFile::DProjectFile(DProject *parent, const FilePath& fileName,
//																											DProject::RefreshOptions options)
//	: IDocument(parent),
//			m_project(parent),
//			m_options(options)
//{
//	setId(Constants::DPROJECTFILE_ID);
//	setMimeType(Constants::DPROJECT_MIMETYPE);
//	setFilePath(fileName);
//}

//IDocument::ReloadBehavior DProjectFile::reloadBehavior(ChangeTrigger state, ChangeType type) const
//{
//	Q_UNUSED(state)
//	Q_UNUSED(type)
//	return BehaviorSilent;
//}

//bool DProjectFile::reload(QString *errorString, ReloadFlag flag, ChangeType type)
//{
//	Q_UNUSED(errorString)
//	Q_UNUSED(flag)
//	if (type == TypePermissions)
//		return true;
//	m_project->refresh(m_options);
//	return true;
//}

} // namespace DProjectManager
