#include "dlangproject.h"

#include "dlangbuildconfiguration.h"
#include "dlangmakestep.h"
#include "dlangprojectconstants.h"

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

using namespace Core;
using namespace ProjectExplorer;

namespace DLangProjectManager {
namespace Internal {

//--------------------------------------------------------------------------------------
//
// DLangProject
//
//--------------------------------------------------------------------------------------

DLangProject::DLangProject(Manager *manager, const QString &fileName)
 : m_manager(manager),
   m_fileName(fileName)
{
 setProjectContext(Context(DLangProjectManager::Constants::PROJECTCONTEXT));
 setProjectLanguages(Context(ProjectExplorer::Constants::LANG_CXX));

 QFileInfo fileInfo(m_fileName);
 QDir dir = fileInfo.dir();

 m_projectName      = fileInfo.completeBaseName();
 m_filesFileName    = QFileInfo(dir, m_projectName + QLatin1String(".files")).absoluteFilePath();

 m_creatorIDocument  = new DLangProjectFile(this, m_fileName, DLangProject::Everything);
 m_filesIDocument    = new DLangProjectFile(this, m_filesFileName, DLangProject::Files);

 DocumentManager::addDocument(m_creatorIDocument);
 DocumentManager::addDocument(m_filesIDocument);

 m_rootNode = new DLangProjectNode(this, m_creatorIDocument);

 m_manager->registerProject(this);
}

DLangProject::~DLangProject()
{
 m_codeModelFuture.cancel();
 m_manager->unregisterProject(this);

 delete m_rootNode;
}

QString DLangProject::filesFileName() const
{
 return m_filesFileName;
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

bool DLangProject::saveRawFileList(const QStringList &rawFileList)
{
 // Make sure we can open the file for writing
 Utils::FileSaver saver(filesFileName(), QIODevice::Text);
 if (!saver.hasError()) {
  QTextStream stream(saver.file());
  foreach (const QString &filePath, rawFileList)
   stream << filePath << QLatin1Char('\n');
  saver.setResult(&stream);
 }
 if (!saver.finalize(ICore::mainWindow()))
  return false;
 refresh(DLangProject::Files);
 return true;
}

bool DLangProject::addFiles(const QStringList &filePaths)
{
 QStringList newList = m_rawFileList;

 QDir baseDir(QFileInfo(m_fileName).dir());
 foreach (const QString &filePath, filePaths)
  newList.append(baseDir.relativeFilePath(filePath));

 return saveRawFileList(newList);
}

bool DLangProject::removeFiles(const QStringList &filePaths)
{
 QStringList newList = m_rawFileList;

 foreach (const QString &filePath, filePaths) {
  QHash<QString, QString>::iterator i = m_rawListEntries.find(filePath);
  if (i != m_rawListEntries.end())
   newList.removeOne(i.value());
 }

 return saveRawFileList(newList);
}

bool DLangProject::setFiles(const QStringList &filePaths)
{
 QStringList newList;
 QDir baseDir(QFileInfo(m_fileName).dir());
 foreach (const QString &filePath, filePaths)
  newList.append(baseDir.relativeFilePath(filePath));

 return saveRawFileList(newList);
}

bool DLangProject::renameFile(const QString &filePath, const QString &newFilePath)
{
 QStringList newList = m_rawFileList;

 QHash<QString, QString>::iterator i = m_rawListEntries.find(filePath);
 if (i != m_rawListEntries.end()) {
  int index = newList.indexOf(i.value());
  if (index != -1) {
   QDir baseDir(QFileInfo(m_fileName).dir());
   newList.replace(index, baseDir.relativeFilePath(newFilePath));
  }
 }

 return saveRawFileList(newList);
}

void DLangProject::parseProject(RefreshOptions options)
{
 if (options & Files)
 {
  m_rawListEntries.clear();
  m_rawFileList = readLines(filesFileName());
  m_files = processEntries(m_rawFileList, &m_rawListEntries);
 }

 if (options & Configuration)
 {
  // TODO : Possibly load some configuration from the project file
  //QSettings projectInfo(m_fileName, QSettings::IniFormat);
 }

 if (options & Files)
  emit fileListChanged();
}

void DLangProject::refresh(RefreshOptions options)
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
QStringList DLangProject::processEntries(const QStringList &paths,
                                         QHash<QString, QString> *map) const
{
 const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
 const QDir projectDir(QFileInfo(m_fileName).dir());

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

QStringList DLangProject::files() const
{
 return m_files;
}

QString DLangProject::displayName() const
{
 return m_projectName;
}

Id DLangProject::id() const
{
 return Id(Constants::DLangPROJECT_ID);
}

IDocument *DLangProject::document() const
{
 return m_creatorIDocument;
}

IProjectManager *DLangProject::projectManager() const
{
 return m_manager;
}

DLangProjectNode *DLangProject::rootProjectNode() const
{
 return m_rootNode;
}

QStringList DLangProject::files(FilesMode fileMode) const
{
 Q_UNUSED(fileMode)
 return m_files;
}


bool DLangProject::fromMap(const QVariantMap &map)
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
   QtSupport::CustomExecutableRunConfiguration* run = new QtSupport::CustomExecutableRunConfiguration(t);

   t->addRunConfiguration(run);
  }
 }

 refresh(Everything);
 return true;
}

//--------------------------------------------------------------------------------------
//
// DLangProjectFile
//
//--------------------------------------------------------------------------------------

DLangProjectFile::DLangProjectFile(DLangProject *parent, QString fileName, DLangProject::RefreshOptions options)
 : IDocument(parent),
   m_project(parent),
   m_options(options)
{
 setFilePath(fileName);
}

bool DLangProjectFile::save(QString *, const QString &, bool)
{
 return false;
}

QString DLangProjectFile::defaultPath() const
{
 return QString();
}

QString DLangProjectFile::suggestedFileName() const
{
 return QString();
}

QString DLangProjectFile::mimeType() const
{
 return QLatin1String(Constants::DLangMIMETYPE);
}

bool DLangProjectFile::isModified() const
{
 return false;
}

bool DLangProjectFile::isSaveAsAllowed() const
{
 return false;
}

IDocument::ReloadBehavior DLangProjectFile::reloadBehavior(ChangeTrigger state, ChangeType type) const
{
 Q_UNUSED(state)
 Q_UNUSED(type)
 return BehaviorSilent;
}

bool DLangProjectFile::reload(QString *errorString, ReloadFlag flag, ChangeType type)
{
 Q_UNUSED(errorString)
 Q_UNUSED(flag)
 if (type == TypePermissions)
  return true;
 m_project->refresh(m_options);
 return true;
}

} // namespace Internal
} // namespace DLangProjectManager
