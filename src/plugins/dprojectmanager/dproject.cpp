#include "dproject.h"

#include "dprojectmanagerconstants.h"
//#include "dbuildconfiguration.h"
//#include "dmakestep.h"
//#include "drunconfiguration.h"

#include "deditor/qcdassist.h"

//#include <coreplugin/documentmanager.h>
#include <coreplugin/icontext.h>
//#include <coreplugin/icore.h>
//#include <cpptools/cpptoolsconstants.h>
//#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
//#include <projectexplorer/abi.h>
//#include <projectexplorer/buildsteplist.h>
//#include <projectexplorer/headerpath.h>
//#include <projectexplorer/kitinformation.h>
//#include <projectexplorer/kitmanager.h>
//#include <projectexplorer/projectexplorerconstants.h>
//#include <projectexplorer/editorconfiguration.h>
//#include <projectexplorer/session.h>
//#include <qtsupport/customexecutablerunconfiguration.h>
//#include <utils/fileutils.h>
//#include <utils/qtcassert.h>

//#include <QDir>
//#include <QProcessEnvironment>
//#include <QSettings>

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
	: Project(Constants::DPROJECT_MIMETYPE, fileName, [this]() { refresh(Everything); })/*,
			m_projectName(fileName.fileName()),
			m_projectFileName(fileName)*/
{
	setId(Constants::DPROJECT_ID);
	setProjectLanguages(Context(ProjectExplorer::Constants::CXX_LANGUAGE_ID));
	setDisplayName(fileName.toFileInfo().completeBaseName());

//	setProjectContext(Context(DProjectManager::Constants::DPROJECTCONTEXT));
//	m_projectIDocument  = new DProjectFile(this, m_projectFileName, DProject::Everything);

//	DocumentManager::addDocument(m_projectIDocument);
// setDocument(m_projectIDocument);
//	m_rootNode = new DProjectNode(this, m_projectIDocument);
// //m_manager->registerProject(this);

//	QSettings sets(m_projectFileName, QSettings::IniFormat);
//	QString bds = sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString();
//	Utils::FileName dir = projectDirectory();
//	if(bds.length() > 0 && bds != QLatin1String("."))
//		dir.appendPath(bds);
//	m_buildDir.setPath(dir.toString());


	if(QcdAssist::isDCDEnabled())
	{
		QString path = fileName.parentDir().toString();
		QcdAssist::sendAddImportToDCD(path);
		QDir dir(path);
		// TODO : доделать includes
//		foreach(QString s, this->includes().split(QLatin1Char(' '), QString::SkipEmptyParts))
//		{
//			if(s.startsWith(QLatin1String("-I")))
//				s = s.remove(0,2);
//			if(QDir::isAbsolutePath(s))
//				QcdAssist::sendAddImportToDCD(path);
//			else
//				QcdAssist::sendAddImportToDCD(dir.absoluteFilePath(s));
//		}
	}

}

DProject::~DProject()
{
//	m_codeModelFuture.cancel();
//	//m_manager->unregisterProject(this);
//	delete m_rootNode;
}

//Core::IDocument* DProject::document() const { return m_projectIDocument; }

bool DProject::addFiles(const QStringList& filePaths)
{
//	QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
//	projectFiles.beginGroup(QLatin1String("Files"));
//	foreach (const QString &filePath, filePaths)
//	{
//		if(m_files.contains(filePath) == false)
//			projectFiles.setValue(m_buildDir.relativeFilePath(filePath), 0);
//	}
//	refresh(Files);
	return true;
}

bool DProject::removeFiles(const QStringList &filePaths)
{
//	QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
//	projectFiles.beginGroup(QLatin1String("Files"));
//	foreach (const QString &filePath, filePaths)
//	{
//		QString rel = m_files.value(filePath);
//		if(rel.length() > 0)
//			projectFiles.remove(rel);
//	}
//	refresh(Files);
	return true;
}

bool DProject::renameFile(const QString &filePath, const QString &newFilePath)
{
//	QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
//	projectFiles.beginGroup(QLatin1String("Files"));
//	projectFiles.remove(m_buildDir.relativeFilePath(filePath));
//	projectFiles.setValue(m_buildDir.relativeFilePath(newFilePath), 0);
//	refresh(Files);
	return true;
}

//bool DProject::parseProject(RefreshOptions options)
//{
////	QSettings sets(m_projectFileName, QSettings::IniFormat);

////	QDir old;
//	bool needRebuild = false;
////	if (options & Configuration)
////	{
////		m_libs = sets.value(QLatin1String(Constants::INI_LIBRARIES_KEY)).toString();
////		m_includes = sets.value(QLatin1String(Constants::INI_INCLUDES_KEY)).toString();
////		m_extraArgs = sets.value(QLatin1String(Constants::INI_EXTRA_ARGS_KEY)).toString();

////		QString bds = sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString();
////		Utils::FileName dir = projectDirectory();
////		if(bds.length() > 0 && bds != QLatin1String("."))
////			dir.appendPath(bds);
////		if((needRebuild = (dir.toString() != m_buildDir.path())))
////		{
////			old = m_buildDir;
////			m_buildDir.setPath(dir.toString());
////		}
////	}

////	if (needRebuild || (options & Files))
////	{
////		m_files.clear();

////		sets.beginGroup(QLatin1String("Files"));
////		foreach(QString rel, sets.allKeys())
////		{
////			QString abs;
////			if(needRebuild)
////			{
////				abs = old.absoluteFilePath(rel);
////				sets.remove(rel);
////				rel = m_buildDir.relativeFilePath(abs);
////				sets.setValue(rel,0);
////			}
////			else
////				abs = m_buildDir.absoluteFilePath(rel);
////			m_files[abs] = rel;
////		}
////		emit fileListChanged();
////	}
//	return needRebuild;
//}

void DProject::refresh(RefreshOptions options)
{
//	bool needRebuildTree = parseProject(options);

//	if (needRebuildTree || (options & Files))
//		m_rootNode->refresh(needRebuildTree);
}

////-------
//bool DProject::setupTarget(Target* t)
//{
////	IBuildConfigurationFactory *factory = IBuildConfigurationFactory::find(t);
////	if (!factory)
////		return false;

////	Utils::FileName projectDir = t->project()->projectDirectory();

////	BuildInfo* info = new BuildInfo(factory);
////	info->displayName = tr("Debug");
////	info->typeName = tr("D Build");
////	info->buildDirectory = projectDir;
////	info->kitId = t->kit()->id();

////	BuildConfiguration* bc = factory->create(t,info);
////	if (!bc)
////		return false;
////	t->addBuildConfiguration(bc);
////	SessionManager::instance()->setActiveBuildConfiguration(t,bc, SetActive::Cascade);

////	info = new BuildInfo(factory);
////	info->displayName = tr("Release");
////	info->typeName = tr("D Build");
////	info->buildDirectory = projectDir;
////	info->kitId = t->kit()->id();

////	bc = factory->create(t,info);
////	if (!bc)
////		return false;
////	t->addBuildConfiguration(bc);

////	info = new BuildInfo(factory);
////	info->displayName = tr("Unittest");
////	info->typeName = tr("D Build");
////	info->buildDirectory = projectDir;
////	info->kitId = t->kit()->id();

////	bc = factory->create(t,info);
////	if (!bc)
////		return false;
////	t->addBuildConfiguration(bc);

////	QList<IRunConfigurationFactory *> rfs = IRunConfigurationFactory::find(t);
////	if (rfs.length() == 0 )
////		return false;

////	RunConfiguration* rc = 0;
////	foreach(IRunConfigurationFactory* rf, rfs)
////	{
////		rc = rf->create(t,Core::Id(Constants::BUILDRUN_CONFIG_ID));
////		if(rc)
////			break;
////	}
////	if (!rc)
////		return false;

////	t->addRunConfiguration(rc);
////	t->setActiveRunConfiguration(rc);

////	SessionManager::instance()->setActiveTarget(this, t, SetActive::Cascade);

//	return true;
//}
//QVariantMap DProject::toMap() const
//{
//	return Project::toMap();
//}
//Project::RestoreResult DProject::fromMap(const QVariantMap &map, QString *errorMessage)
//{
//	RestoreResult result = Project::fromMap(map, errorMessage);
//	if (result != RestoreResult::Ok)
//					return result;

////	Kit *defaultKit = KitManager::defaultKit();
////	if (!activeTarget() && defaultKit)
////					addTarget(createTarget(defaultKit));

////	// Sanity check: We need both a buildconfiguration and a runconfiguration!
////	QList<Target *> targetList = targets();
////	foreach (Target *t, targetList)
////	{
////		if (!t->activeBuildConfiguration())
////		{
////			removeTarget(t);
////			try {
////				delete t;
////			} catch(...) {}
////			continue;
////		}
////		if (!t->activeRunConfiguration())
////		{
////			QList<IRunConfigurationFactory *> rfs = IRunConfigurationFactory::find(t);
////			if (rfs.length() > 0 )
////				foreach(IRunConfigurationFactory* rf, rfs)
////				{
////					RunConfiguration* rc = rf->create(t,Core::Id(Constants::BUILDRUN_CONFIG_ID));
////					if(rc)
////					{
////						t->addRunConfiguration(rf->create(t, ProjectExplorer::Constants::BUILDSTEPS_BUILD));
////						break;
////					}
////				}

////		}
////	}

//	refresh(Everything);
//	return result;
//}

//--------------------------------------------------------------------------------------
//
// DProjectFile
//
//--------------------------------------------------------------------------------------

DProjectFile::DProjectFile(DProject *parent, const FilePath& fileName,
																											DProject::RefreshOptions options)
	: IDocument(parent),
			m_project(parent),
			m_options(options)
{
	setId(Constants::DPROJECTFILE_ID);
	setMimeType(Constants::DPROJECT_MIMETYPE);
	setFilePath(fileName);
}

//bool DProjectFile::save(QString *, const QString &, bool)
//{
//	return false;
//}

//QString DProjectFile::defaultPath() const
//{
//	return QString();
//}

//QString DProjectFile::suggestedFileName() const
//{
//	return QString();
//}

//QString DProjectFile::mimeType() const
//{
//	return QLatin1String(Constants::DPROJECT_MIMETYPE);
//}

//bool DProjectFile::isModified() const
//{
//	return false;
//}

//bool DProjectFile::isSaveAsAllowed() const
//{
//	return false;
//}

IDocument::ReloadBehavior DProjectFile::reloadBehavior(ChangeTrigger state, ChangeType type) const
{
	Q_UNUSED(state)
	Q_UNUSED(type)
	return BehaviorSilent;
}

bool DProjectFile::reload(QString *errorString, ReloadFlag flag, ChangeType type)
{
	Q_UNUSED(errorString)
	Q_UNUSED(flag)
	if (type == TypePermissions)
		return true;
	m_project->refresh(m_options);
	return true;
}

} // namespace DProjectManager
