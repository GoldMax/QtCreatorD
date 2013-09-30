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
namespace Internal {

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

	m_projectIDocument  = new DProjectFile(this, m_projectFileName, DProject::Everything);

	DocumentManager::addDocument(m_projectIDocument);
	m_rootNode = new DProjectNode(this, m_projectIDocument);
 m_manager->registerProject(this);
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
 QDir projDir(projectDirectory());
 QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
 projectFiles.beginGroup(QLatin1String("Files"));
 foreach (const QString &filePath, filePaths)
 {
  if(m_files.contains(filePath) == false)
   projectFiles.setValue(projDir.relativeFilePath(filePath), 0);
 }
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
 return true;
}

bool DProject::renameFile(const QString &filePath, const QString &newFilePath)
{
 QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
 projectFiles.beginGroup(QLatin1String("Files"));
 QDir projDir(projectDirectory());

 projectFiles.remove(projDir.relativeFilePath(filePath));
	projectFiles.setValue(projDir.relativeFilePath(newFilePath), 0);

 return true;
}

void DProject::parseProject(RefreshOptions options)
{
 // if (options & Configuration)
 // {
 //  // TODO : Possibly load some configuration from the project file
 //  //QSettings projectInfo(m_projectFileName, QSettings::IniFormat);
 // }

 if (options & Files)
 {
  m_files.clear();

  QDir projDir(projectDirectory());
  QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
  projectFiles.beginGroup(QLatin1String("Files"));
  foreach(QString rel, projectFiles.allKeys())
  {
   QString abs = projDir.absoluteFilePath(rel);
			m_files[abs] = rel;
  }
  emit fileListChanged();
 }
}

void DProject::refresh(RefreshOptions options)
{
 parseProject(options);

 if (options & Files)
  m_rootNode->refresh();
}

//-------
bool DProject::setupTarget(Target* t)
{
 //if(Project::setupTarget(t) == false)
 // return false;

 IBuildConfigurationFactory *factory = IBuildConfigurationFactory::find(t);
 if (!factory)
  return false;

 Utils::FileName projectDir =
   Utils::FileName::fromString(t->project()->projectDirectory());

 QSettings sets(m_projectFileName, QSettings::IniFormat);
 QStringList groups = sets.childGroups();
 foreach(QString group, groups)
  if(group.startsWith(QLatin1String("BC.")))
  {
   QVariantMap map;
			map[QLatin1String(DProjectManager::Constants::D_BC_NAME)] = group.remove(0,3);
   BuildConfiguration* bc = factory->restore(t,map); //create(t,info);
   if (!bc)
    return false;
   t->addBuildConfiguration(bc);
  }

 if(t->buildConfigurations().length() > 0)
  return true;

 BuildInfo* info = new BuildInfo(factory);
 info->displayName = tr("Debug");
 info->typeName = tr("D Build");
 info->buildDirectory = projectDir;
 info->kitId = t->kit()->id();

 BuildConfiguration* bc = factory->create(t,info);
 if (!bc)
  return false;
 t->addBuildConfiguration(bc);

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

 return true;
}

QVariantMap DProject::toMap() const
{
 QVariantMap map = Project::toMap();
 map.remove(QLatin1String(ACTIVE_TARGET_KEY));
 map.remove(QLatin1String(TARGET_COUNT_KEY));
 const QList<Target *> ts = targets();
 for (int i = 0; i < ts.size(); ++i)
  map.remove(QString::fromLatin1(TARGET_KEY_PREFIX) + QString::number(i));
// const QList<Target *> ts = targets();

// QVariantMap map;
//// map.insert(QLatin1String(ACTIVE_TARGET_KEY), ts.indexOf(d->m_activeTarget));
// map.insert(QLatin1String(TARGET_COUNT_KEY), 0);
//// for (int i = 0; i < ts.size(); ++i)
////  map.insert(QString::fromLatin1(TARGET_KEY_PREFIX) + QString::number(i), ts.at(i)->toMap());

// map.insert(QLatin1String(EDITOR_SETTINGS_KEY), editorConfiguration()->toMap());
 return map;
}
bool DProject::fromMap(const QVariantMap &map)
{
 if (!Project::fromMap(map))
  return false;


 /*QSettings sets(m_projectFileName, QSettings::IniFormat);
 QVariantMap map;
 // TODO send QSettings
 // map[QLatin1String("QSettings")] = QVariant(sets;

 QStringList groups = sets.childGroups();
 foreach(QString group, groups)
  if(group.startsWith(QLatin1String("BC.")))
  {
   map[QLatin1String("Name")] = group.remove(0,3);
   BuildConfiguration* bc = factory->restore(t,map);
   if (!bc)
    return false;
   t->addBuildConfiguration(bc);
  }

 if(t->buildConfigurations().length() > 0)
  return true;
*/

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

} // namespace Internal
} // namespace DProjectManager
