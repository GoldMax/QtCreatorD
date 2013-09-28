#include "dprojectnodes.h"
#include "dproject.h"

#include <coreplugin/messagemanager.h>

#include <utils/fileutils.h>
#include <coreplugin/idocument.h>
#include <projectexplorer/projectexplorer.h>

#include <QFileInfo>
#include <QDir>
#include <QStack>
#include <QSet>

using namespace ProjectExplorer;

namespace DProjectManager {
namespace Internal {

DProjectNode::DProjectNode(DProject *project, Core::IDocument *projectFile)
 : ProjectNode(projectFile->filePath()), m_project(project), m_projectFile(projectFile)
{
 setDisplayName(QFileInfo(projectFile->filePath()).completeBaseName());
}

Core::IDocument *DProjectNode::projectFile() const
{
 return m_projectFile;
}

bool DProjectNode::addFiles(const QStringList &filePaths, QStringList *notAdded)
{
 Q_UNUSED(notAdded)
 return m_project->addFiles(filePaths);
}

bool DProjectNode::removeFiles(const QStringList &filePaths, QStringList *notRemoved)
{
 Q_UNUSED(notRemoved)
 return m_project->removeFiles(filePaths);
}

bool DProjectNode::renameFile(const QString &filePath, const QString &newFilePath)
{
 return m_project->renameFile(filePath, newFilePath);
}

QList<ProjectNode::ProjectAction> DProjectNode::supportedActions(Node *node) const
{
 Q_UNUSED(node);
 return QList<ProjectAction>()
   << AddNewFile
   << AddExistingFile
   << RemoveFile
   << Rename;
}

QList<RunConfiguration *> DProjectNode::runConfigurationsFor(Node *node)
{
 Q_UNUSED(node)
 return QList<RunConfiguration *>();
}

bool isEmpty(FolderNode* f)
{
 return f->fileNodes().length() + f->subFolderNodes().length() == 0;
}

void DProjectNode::refresh()
{
 //Core::MessageManager::write(QLatin1String("refresh"));
 const QSet<QString>& files = m_project->files();

 QHash<QString,FileNode*> nodes;
 QStack<FolderNode*> stack;
 stack.push(this);
 while(stack.count() > 0)
 {
  FolderNode* n = stack.pop();
  foreach(FileNode* i, n->fileNodes())
   nodes[i->path()] = i;
  foreach(FolderNode* i, n->subFolderNodes())
   stack.push(i);
 }
 QDir base(m_project->projectDirectory());

 // adding
 foreach(QString filePath, files)
 {
  if(nodes.contains(filePath))
   continue;
  FolderNode* folder = this;
  QStringList parts = base.relativeFilePath(filePath).split(QDir::separator());
  QString absFolderPath = m_project->projectDirectory();
  parts.pop_back();
  foreach(QString part, parts)
  {
   FolderNode* fn = 0;
   absFolderPath.append(QDir::separator()).append(part);

   foreach(FolderNode* f, folder->subFolderNodes())
    if(f->displayName() == part)
    {
     fn = f;
     break;
    }
   if(fn == 0)
   {
    QList<FolderNode*> list;
    list.append(fn = new FolderNode(absFolderPath));
    fn->setDisplayName(part);
    addFolderNodes(list, folder);
   }
   folder = fn;
  }
  FileNode* node = 0;
  foreach(FileNode* n, folder->fileNodes())
   if(n->path() == filePath)
   {
    node = n;
    break;
   }
  if(node == 0)
  {
   QList<FileNode*> list;
   list.append(new FileNode(filePath, SourceType, false));
   addFileNodes(list, folder);
  }
 }

 // removing
 typedef QHash<QString, FileNode*>::Iterator KeyValue;
 for (KeyValue kv = nodes.begin(); kv != nodes.end(); ++kv)
 {
  if(files.contains(kv.key()))
   continue;
  QList<FileNode*> list;
  list.append(kv.value());
  FolderNode* parent = kv.value()->parentFolderNode();
  removeFileNodes(list, parent);

  while(parent != this)
   if(isEmpty(parent))
   {
    FolderNode* p = parent->parentFolderNode();
    QList<FolderNode*> list;
    list.append(parent);
    removeFolderNodes(list, p);
    parent = p;
   }
  else
    break;
 }
}
} // namespace Internal
} // namespace DProjectManager
