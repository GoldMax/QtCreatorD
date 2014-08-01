#include "dproject.h"

#include "dprojectmanagerconstants.h"
#include "dbuildconfiguration.h"
#include "dmakestep.h"
#include "drunconfiguration.h"

#include <coreplugin/documentmanager.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <cpptools/cpptoolsconstants.h>
#include <cpptools/cppmodelmanagerinterface.h>
#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/abi.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/headerpath.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/editorconfiguration.h>
#include <qtsupport/customexecutablerunconfiguration.h>
#include <utils/fileutils.h>
#include <utils/qtcassert.h>

#include <QDir>
#include <QProcessEnvironment>
#include <QSettings>

using namespace Core;
using namespace ProjectExplorer;

namespace {
const char ACTIVE_TARGET_KEY[] = "ProjectExplorer.Project.ActiveTarget";
const char TARGET_KEY_PREFIX[] = "ProjectExplorer.Project.Target.";
const char TARGET_COUNT_KEY[] = "ProjectExplorer.Project.TargetCount";
const char EDITOR_SETTINGS_KEY[] = "ProjectExplorer.Project.EditorSettings";
const char PLUGIN_SETTINGS_KEY[] = "ProjectExplorer.Project.PluginSettings";
} // namespace

namespace DProjectManager {
//--------------------------------------------------------------------------------------
//
// DProject
//
//--------------------------------------------------------------------------------------

DProject::DProject(Manager *manager, const QString &fileName)
	: m_manager(manager),
			m_projectName(QFileInfo(fileName).completeBaseName()),
			m_projectFileName(fileName)
{
	setProjectContext(Context(DProjectManager::Constants::DPROJECTCONTEXT));
	setProjectLanguages(Context(ProjectExplorer::Constants::LANG_CXX));
 setId(Core::Id(Constants::DPROJECT_ID));
	m_projectIDocument  = new DProjectFile(this, m_projectFileName, DProject::Everything);

	DocumentManager::addDocument(m_projectIDocument);
	m_rootNode = new DProjectNode(this, m_projectIDocument);
	m_manager->registerProject(this);

	QSettings sets(m_projectFileName, QSettings::IniFormat);
	QString bds = sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString();
	Utils::FileName dir = Utils::FileName::fromString(projectDirectory());
	if(bds.length() > 0 && bds != QLatin1String("."))
		dir.appendPath(bds);
	m_buildDir.setPath(dir.toString());

}

DProject::~DProject()
{
	m_codeModelFuture.cancel();
	m_manager->unregisterProject(this);
	delete m_rootNode;
}

Core::IDocument* DProject::document() const { return m_projectIDocument; }

bool DProject::addFiles(const QStringList& filePaths)
{
	QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String("Files"));
	foreach (const QString &filePath, filePaths)
	{
		if(m_files.contains(filePath) == false)
			projectFiles.setValue(m_buildDir.relativeFilePath(filePath), 0);
	}
	refresh(Files);
	return true;
}

bool DProject::removeFiles(const QStringList &filePaths)
{
	QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String("Files"));
	foreach (const QString &filePath, filePaths)
	{
		QString rel = m_files.value(filePath);
		if(rel.length() > 0)
			projectFiles.remove(rel);
	}
	refresh(Files);
	return true;
}

bool DProject::renameFile(const QString &filePath, const QString &newFilePath)
{
	QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String("Files"));
	projectFiles.remove(m_buildDir.relativeFilePath(filePath));
	projectFiles.setValue(m_buildDir.relativeFilePath(newFilePath), 0);
	refresh(Files);
	return true;
}

bool DProject::parseProject(RefreshOptions options)
{
	QSettings sets(m_projectFileName, QSettings::IniFormat);

	QDir old;
	bool needRebuild = false;
	if (options & Configuration)
	{
		m_libs = sets.value(QLatin1String(Constants::INI_LIBRARIES_KEY)).toString();
		m_includes = sets.value(QLatin1String(Constants::INI_INCLUDES_KEY)).toString();
		m_extraArgs = sets.value(QLatin1String(Constants::INI_EXTRA_ARGS_KEY)).toString();

		QString bds = sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString();
		Utils::FileName dir = Utils::FileName::fromString(projectDirectory());
		if(bds.length() > 0 && bds != QLatin1String("."))
			dir.appendPath(bds);
		if((needRebuild = (dir.toString() != m_buildDir.path())))
		{
			old = m_buildDir;
			m_buildDir.setPath(dir.toString());
		}
	}

	if (needRebuild || (options & Files))
	{
		m_files.clear();

		sets.beginGroup(QLatin1String("Files"));
		foreach(QString rel, sets.allKeys())
		{
			QString abs;
			if(needRebuild)
			{
				abs = old.absoluteFilePath(rel);
				sets.remove(rel);
				rel = m_buildDir.relativeFilePath(abs);
				sets.setValue(rel,0);
			}
			else
				abs = m_buildDir.absoluteFilePath(rel);
			m_files[abs] = rel;
		}
		emit fileListChanged();
	}
	return needRebuild;
}

void DProject::refresh(RefreshOptions options)
{
	bool needRebuildTree = parseProject(options);

	if (needRebuildTree || (options & Files))
		m_rootNode->refresh(needRebuildTree);
}

//-------
bool DProject::setupTarget(Target* t)
{
	IBuildConfigurationFactory *factory = IBuildConfigurationFactory::find(t);
	if (!factory)
		return false;

	Utils::FileName projectDir =
			Utils::FileName::fromString(t->project()->projectDirectory());

	BuildInfo* info = new BuildInfo(factory);
	info->displayName = tr("Debug");
	info->typeName = tr("D Build");
	info->buildDirectory = projectDir;
	info->kitId = t->kit()->id();

	BuildConfiguration* bc = factory->create(t,info);
	if (!bc)
		return false;
	t->addBuildConfiguration(bc);
	t->setActiveBuildConfiguration(bc);

	info = new BuildInfo(factory);
	info->displayName = tr("Release");
	info->typeName = tr("D Build");
	info->buildDirectory = projectDir;
	info->kitId = t->kit()->id();

	bc = factory->create(t,info);
	if (!bc)
		return false;
	t->addBuildConfiguration(bc);

	info = new BuildInfo(factory);
	info->displayName = tr("Unittest");
	info->typeName = tr("D Build");
	info->buildDirectory = projectDir;
	info->kitId = t->kit()->id();

	bc = factory->create(t,info);
	if (!bc)
		return false;
	t->addBuildConfiguration(bc);

	RunConfiguration* run =
			DRunConfigurationFactory::instance()->create(t, Core::Id(Constants::BUILDRUN_CONFIG_ID));
	t->addRunConfiguration(run);
	t->setActiveRunConfiguration(run);

	setActiveTarget(t);

	return true;
}
QVariantMap DProject::toMap() const
{
	return Project::toMap();
}
bool DProject::fromMap(const QVariantMap &map)
{
	if (!Project::fromMap(map))
		return false;

	Kit *defaultKit = KitManager::defaultKit();
	if (!activeTarget() && defaultKit)
					addTarget(createTarget(defaultKit));

	// Sanity check: We need both a buildconfiguration and a runconfiguration!
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
			t->addRunConfiguration(
						DRunConfigurationFactory::instance()->create(t, ProjectExplorer::Constants::BUILDSTEPS_BUILD));
	}

	refresh(Everything);
	return true;
}

//--------------------------------------------------------------------------------------
//
// DProjectFile
//
//--------------------------------------------------------------------------------------

DProjectFile::DProjectFile(DProject *parent, QString fileName, DProject::RefreshOptions options)
	: IDocument(parent),
			m_project(parent),
			m_options(options)
{
	setFilePath(fileName);
}

bool DProjectFile::save(QString *, const QString &, bool)
{
	return false;
}

QString DProjectFile::defaultPath() const
{
	return QString();
}

QString DProjectFile::suggestedFileName() const
{
	return QString();
}

QString DProjectFile::mimeType() const
{
	return QLatin1String(Constants::DPROJECT_MIMETYPE);
}

bool DProjectFile::isModified() const
{
	return false;
}

bool DProjectFile::isSaveAsAllowed() const
{
	return false;
}

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
