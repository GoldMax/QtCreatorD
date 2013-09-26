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
#include <qtsupport/customexecutablerunconfiguration.h>
#include <utils/fileutils.h>
#include <utils/qtcassert.h>

#include <QDir>
#include <QProcessEnvironment>
#include <QSettings>

using namespace Core;
using namespace ProjectExplorer;

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
			m_projectFileName(fileName),
			m_projectDir(QFileInfo(fileName).dir())
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

QStringList DProject::files() const { return m_files; }

QStringList DProject::files(FilesMode ) const { return m_files; }

bool DProject::addFiles(const QStringList &filePaths)
{
	QStringList newList = m_rawFileList;
	foreach (const QString &filePath, filePaths)
		newList.append(m_projectDir.relativeFilePath(filePath));
	return saveRawFileList(newList);
}



static QStringList readLines(const QString &absoluteFileName)
{
 QStringList lines;

 QFile file(absoluteFileName);
 if (file.open(QFile::ReadOnly))
 {
  QTextStream stream(&file);

  forever
  {
   QString line = stream.readLine();
   if (line.isNull())
    break;

   lines.append(line);
  }
 }

 return lines;
}

bool DProject::saveRawFileList(const QStringList &rawFileList)
{
	QSettings projectFiles(m_projectFileName, QSettings::IniFormat);
	projectFiles.beginGroup(QLatin1String("Files"));
	foreach (const QString& filePath, rawFileList)
		projectFiles.setValue(filePath, 0);
	projectFiles.sync();
	refresh(DProject::Files);
 return true;
}


bool DProject::removeFiles(const QStringList &filePaths)
{
 QStringList newList = m_rawFileList;

 foreach (const QString &filePath, filePaths) {
  QHash<QString, QString>::iterator i = m_rawListEntries.find(filePath);
  if (i != m_rawListEntries.end())
   newList.removeOne(i.value());
 }

 return saveRawFileList(newList);
}

bool DProject::setFiles(const QStringList &filePaths)
{
 QStringList newList;
	QDir baseDir(QFileInfo(m_projectFileName).dir());
 foreach (const QString &filePath, filePaths)
  newList.append(baseDir.relativeFilePath(filePath));

 return saveRawFileList(newList);
}

bool DProject::renameFile(const QString &filePath, const QString &newFilePath)
{
 QStringList newList = m_rawFileList;

 QHash<QString, QString>::iterator i = m_rawListEntries.find(filePath);
 if (i != m_rawListEntries.end()) {
  int index = newList.indexOf(i.value());
  if (index != -1) {
			QDir baseDir(QFileInfo(m_projectFileName).dir());
   newList.replace(index, baseDir.relativeFilePath(newFilePath));
  }
 }

 return saveRawFileList(newList);
}

void DProject::parseProject(RefreshOptions options)
{
// if (options & Files)
// {
//  m_rawListEntries.clear();
//  m_rawFileList = readLines(filesFileName());
//  m_files = processEntries(m_rawFileList, &m_rawListEntries);
// }

// if (options & Configuration)
// {
//  // TODO : Possibly load some configuration from the project file
//  //QSettings projectInfo(m_projectFileName, QSettings::IniFormat);
// }

// if (options & Files)
//  emit fileListChanged();
}

void DProject::refresh(RefreshOptions options)
{
 QSet<QString> oldFileList;
 if (!(options & Configuration))
  oldFileList = m_files.toSet();

 parseProject(options);

 if (options & Files)
  m_rootNode->refresh(oldFileList);
}

/*
 * Expands environment variables in the given \a string when they are written
 * like $$(VARIABLE).
 */
static void expandEnvironmentVariables(const QProcessEnvironment &env, QString &string)
{
 static QRegExp candidate(QLatin1String("\\$\\$\\((.+)\\)"));

 int index = candidate.indexIn(string);
 while (index != -1)
 {
  const QString value = env.value(candidate.cap(1));

  string.replace(index, candidate.matchedLength(), value);
  index += value.length();

  index = candidate.indexIn(string, index);
 }
}

/*
 * Expands environment variables and converts the path from relative to the
 * project to an absolute path.
 *
 * The \a map variable is an optional argument that will map the returned
 * absolute paths back to their original \a entries.
 */
QStringList DProject::processEntries(const QStringList &paths,
                                         QHash<QString, QString> *map) const
{
 const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	const QDir projectDir(QFileInfo(m_projectFileName).dir());

 QFileInfo fileInfo;
 QStringList absolutePaths;
 foreach (const QString &path, paths)
 {
  QString trimmedPath = path.trimmed();
  if (trimmedPath.isEmpty())
   continue;

  expandEnvironmentVariables(env, trimmedPath);

  trimmedPath = Utils::FileName::fromUserInput(trimmedPath).toString();

  fileInfo.setFile(projectDir, trimmedPath);
  if (fileInfo.exists())
  {
   const QString absPath = fileInfo.absoluteFilePath();
   absolutePaths.append(absPath);
   if (map)
    map->insert(absPath, trimmedPath);
  }
 }
 absolutePaths.removeDuplicates();
 return absolutePaths;
}



bool DProject::fromMap(const QVariantMap &map)
{
 if (!Project::fromMap(map))
  return false;

 Kit *defaultKit = KitManager::defaultKit();
 if (!activeTarget() && defaultKit)
 {
  Target* t = createTarget(defaultKit);
  addTarget(t);


  IBuildConfigurationFactory *factory = IBuildConfigurationFactory::find(t);
  if (!factory)
   return false;
  const Core::Id id = factory->availableCreationIds(t)[0];


  QString name = QLatin1String("Release");
  BuildConfiguration *bc = factory->create(t,id, name);
  if (!bc)
   return false;
  t->addBuildConfiguration(bc);

  name = QLatin1String("Unittest");
  bc = factory->create(t, id,name);
  if (!bc)
   return false;
  t->addBuildConfiguration(bc);

 }
 // Sanity check: We need both a buildconfiguration and a runconfiguration!
 QList<Target *> targetList = targets();
 foreach (Target *t, targetList)
 {
  if (!t->activeBuildConfiguration())
  {
   removeTarget(t);
   delete t;
   continue;
  }
  if (!t->activeRunConfiguration())
  {
   //QtSupport::CustomExecutableRunConfiguration* run = new QtSupport::CustomExecutableRunConfiguration(t);
   DRunConfiguration* run = new DRunConfiguration(t);

   t->addRunConfiguration(run);
  }
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
