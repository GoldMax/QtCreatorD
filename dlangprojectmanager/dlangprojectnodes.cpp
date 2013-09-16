#include "dlangprojectnodes.h"
#include "dlangproject.h"

#include <utils/fileutils.h>
#include <coreplugin/idocument.h>
#include <projectexplorer/projectexplorer.h>

#include <QFileInfo>
#include <QDir>

using namespace ProjectExplorer;

namespace DLangProjectManager {
namespace Internal {

DLangProjectNode::DLangProjectNode(DLangProject *project, Core::IDocument *projectFile)
 : ProjectNode(projectFile->filePath()), m_project(project), m_projectFile(projectFile)
{
 setDisplayName(QFileInfo(projectFile->filePath()).completeBaseName());
}

Core::IDocument *DLangProjectNode::projectFile() const
{
 return m_projectFile;
}

QString DLangProjectNode::projectFilePath() const
{
 return m_projectFile->filePath();
}

QHash<QString, QStringList> sortFilesIntoPaths(const QString &base, const QSet<QString> files)
{
 QHash<QString, QStringList> filesInPath;
 const QDir baseDir(base);

 foreach (const QString &absoluteFileName, files)
 {
  QFileInfo fileInfo(absoluteFileName);
  Utils::FileName absoluteFilePath = Utils::FileName::fromString(fileInfo.path());
  QString relativeFilePath;

  if (absoluteFilePath.isChildOf(baseDir))
  {
   relativeFilePath = absoluteFilePath.relativeChildPath(Utils::FileName::fromString(base)).toString();
  }
  else
  {
   // `file' is not part of the project.
   relativeFilePath = baseDir.relativeFilePath(absoluteFilePath.toString());
   if (relativeFilePath.endsWith(QDir::separator()))
    relativeFilePath.chop(1);
  }

  filesInPath[relativeFilePath].append(absoluteFileName);
 }
 return filesInPath;
}

void DLangProjectNode::refresh(QSet<QString> oldFileList)
{
 typedef QHash<QString, QStringList> FilesInPathHash;
 typedef FilesInPathHash::ConstIterator FilesInPathHashConstIt;

 if (oldFileList.isEmpty())
 {
  // Only do this once
  FileNode *projectFilesNode = new FileNode(m_project->filesFileName(),
                                            ProjectFileType,
                                            /* generated = */ false);

  addFileNodes(QList<FileNode *>() << projectFilesNode, this);
 }

 // Do those separately
 oldFileList.remove(m_project->filesFileName());

 QSet<QString> newFileList = m_project->files().toSet();
 newFileList.remove(m_project->filesFileName());

 QSet<QString> removed = oldFileList;
 removed.subtract(newFileList);
 QSet<QString> added = newFileList;
 added.subtract(oldFileList);

 QString baseDir = QFileInfo(path()).absolutePath();
 FilesInPathHash filesInPaths = sortFilesIntoPaths(baseDir, added);

 FilesInPathHashConstIt cend = filesInPaths.constEnd();
 for (FilesInPathHashConstIt it = filesInPaths.constBegin(); it != cend; ++it)
 {
  const QString &filePath = it.key();
  QStringList components;
  if (!filePath.isEmpty())
   components = filePath.split(QDir::separator());
  FolderNode *folder = findFolderByName(components, components.size());
  if (!folder)
   folder = createFolderByName(components, components.size());

  QList<FileNode *> fileNodes;
  foreach (const QString &file, it.value())
  {
   FileType fileType = SourceType; // ### FIXME
   FileNode *fileNode = new FileNode(file, fileType, /*generated = */ false);
   fileNodes.append(fileNode);
  }

  addFileNodes(fileNodes, folder);
 }

 filesInPaths = sortFilesIntoPaths(baseDir, removed);
 cend = filesInPaths.constEnd();
 for (FilesInPathHashConstIt it = filesInPaths.constBegin(); it != cend; ++it)
 {
  const QString &filePath = it.key();
  QStringList components;
  if (!filePath.isEmpty())
   components = filePath.split(QDir::separator());
  FolderNode *folder = findFolderByName(components, components.size());

  QList<FileNode *> fileNodes;
  foreach (const QString &file, it.value()) {
   foreach (FileNode *fn, folder->fileNodes())
    if (fn->path() == file)
     fileNodes.append(fn);
  }

  removeFileNodes(fileNodes, folder);
 }

 foreach (FolderNode *fn, subFolderNodes())
  removeEmptySubFolders(this, fn);
}

void DLangProjectNode::removeEmptySubFolders(FolderNode *gparent, FolderNode *parent)
{
 foreach (FolderNode *fn, parent->subFolderNodes())
  removeEmptySubFolders(parent, fn);

 if (parent->subFolderNodes().isEmpty() && parent->fileNodes().isEmpty())
  removeFolderNodes(QList<FolderNode*>() << parent, gparent);
}

FolderNode *DLangProjectNode::createFolderByName(const QStringList &components, int end)
{
 if (end == 0)
  return this;

 QString folderName;
 for (int i = 0; i < end; ++i)
 {
  folderName.append(components.at(i));
  folderName += QDir::separator();
 }

 const QString component = components.at(end - 1);

 const QString baseDir = QFileInfo(path()).path();
 FolderNode *folder = new FolderNode(baseDir + QDir::separator() + folderName);
 folder->setDisplayName(component);

 FolderNode *parent = findFolderByName(components, end - 1);
 if (!parent)
  parent = createFolderByName(components, end - 1);
 addFolderNodes(QList<FolderNode*>() << folder, parent);

 return folder;
}

FolderNode *DLangProjectNode::findFolderByName(const QStringList &components, int end)
{
 if (end == 0)
  return this;

 QString folderName;
 for (int i = 0; i < end; ++i)
 {
  folderName.append(components.at(i));
  folderName += QDir::separator();
 }

 FolderNode *parent = findFolderByName(components, end - 1);

 if (!parent)
  return 0;

 const QString baseDir = QFileInfo(path()).path();
 foreach (FolderNode *fn, parent->subFolderNodes())
  if (fn->path() == baseDir + QDir::separator() + folderName)
   return fn;
 return 0;
}

bool DLangProjectNode::hasBuildTargets() const
{
 return true;
}

QList<ProjectNode::ProjectAction> DLangProjectNode::supportedActions(Node *node) const
{
 Q_UNUSED(node);
 return QList<ProjectAction>()
   << AddNewFile
   << AddExistingFile
   << RemoveFile
   << Rename;
}

bool DLangProjectNode::canAddSubProject(const QString &proFilePath) const
{
 Q_UNUSED(proFilePath)
 return false;
}

bool DLangProjectNode::addSubProjects(const QStringList &proFilePaths)
{
 Q_UNUSED(proFilePaths)
 return false;
}

bool DLangProjectNode::removeSubProjects(const QStringList &proFilePaths)
{
 Q_UNUSED(proFilePaths)
 return false;
}

bool DLangProjectNode::addFiles(const QStringList &filePaths, QStringList *notAdded)
{
 Q_UNUSED(notAdded)

 return m_project->addFiles(filePaths);
}

bool DLangProjectNode::removeFiles(const QStringList &filePaths, QStringList *notRemoved)
{
 Q_UNUSED(notRemoved)

 return m_project->removeFiles(filePaths);
}

bool DLangProjectNode::deleteFiles(const QStringList &filePaths)
{
 Q_UNUSED(filePaths)
 return false;
}

bool DLangProjectNode::renameFile(const QString &filePath, const QString &newFilePath)
{
 return m_project->renameFile(filePath, newFilePath);
}

QList<RunConfiguration *> DLangProjectNode::runConfigurationsFor(Node *node)
{
 Q_UNUSED(node)
 return QList<RunConfiguration *>();
}

} // namespace Internal
} // namespace DLangProjectManager
