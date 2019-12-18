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

DProjectNode::DProjectNode(DProject* project)
	: ProjectNode(project->projectFilePath()),
			m_project(project)
{
	static int count = 0;
	//setDisplayName(projectFile->filePath().toFileInfo().completeBaseName());
	setDisplayName(project->projectFilePath().toFileInfo().completeBaseName());
	setIcon(QIcon(QLatin1String(Constants::ICON_D_PROJECT)));
	setAbsoluteFilePathAndLine(project->projectFilePath(),++count);
	setShowWhenEmpty(true);
}

bool DProjectNode::supportsAction(ProjectExplorer::ProjectAction action, const Node *) const
{
	return action == AddNewFile
			|| action == AddExistingFile
			|| action == RemoveFile
			|| action == Rename;
}

bool DProjectNode::addFiles(const QStringList &filePaths, QStringList *notAdded)
{
	Q_UNUSED(notAdded)
	return m_project->addFiles(filePaths);
}

bool DProjectNode::removeFiles(const QStringList &filePaths, QStringList *notRemoved)
{
	Q_UNUSED(notRemoved)
	return m_project->removeFiles(filePaths);;
}

bool DProjectNode::renameFile(const QString &filePath, const QString &newFilePath)
{
	return m_project->renameFile(filePath, newFilePath);
}

//QList<RunConfiguration *> DProjectNode::runConfigurationsFor(Node *node)
//{
//	Q_UNUSED(node)
//	return QList<RunConfiguration *>();
//}

//bool isEmpty(FolderNode* f)
//{
//	return f->fileNodes().length() + f->folderNodes().length() == 0;
//}

//void DProjectNode::refresh(bool needRebuild)
//{
//	if(needRebuild)
//	{
////		QStringList* l = new QStringList();
////		for(int i = 0; i < this->fileNodes().count(); i++)
////			l->append(this->fileNodes().at(i)->filePath().toString());
////		this->removeFiles(*l);
////		delete l;

////		l = new QStringList();
////		for(int i = 0; i < this->folderNodes().count(); i++)
////			l->append(this->folderNodes().at(i)->filePath().toString());
////		this->removeFolders(*l);
////		delete l;
//	}

//	//	const QHash<QString,QString>& files = m_project->files();

//	//	QHash<QString,FileNode*> nodes;
//	//	QStack<FolderNode*> stack;
//	//	stack.push(this);
//	//	while(stack.count() > 0)
//	//	{
//	//		FolderNode* n = stack.pop();
//	//		foreach(FileNode* i, n->fileNodes())
//	//			nodes[i->filePath().toString()] = i;
//	//		foreach(FolderNode* i, n->folderNodes())
//	//			stack.push(i);
//	//	}

//	//	// adding
//	//	typedef QHash<QString,QString>::ConstIterator FilesKeyValue;
//	//	for (FilesKeyValue kv = files.constBegin(); kv != files.constEnd(); ++kv)
//	//	{
//	//		if(nodes.contains(kv.key()))
//	//			continue;
//	//		FolderNode* folder = this;
//	//		QStringList parts = kv.value().split(QDir::separator());
//	//		QString absFolderPath = m_project->buildDirectory().path();
//	//		parts.pop_back();
//	//		foreach(QString part, parts)
//	//		{
//	//			FolderNode* fn = nullptr;
//	//			absFolderPath.append(QDir::separator()).append(part);

//	//			foreach(FolderNode* f, folder->folderNodes())
//	//				if(f->displayName() == part)
//	//				{
//	//					fn = f;
//	//					break;
//	//				}
//	//			if(fn == nullptr)
//	//			{
//	//				//QList<FolderNode*> list;
//	//				//list.append(fn = new FolderNode(Utils::FileName::fromString(absFolderPath)));
//	//				FolderNode* fn = new FolderNode(Utils::FileName::fromString(absFolderPath));
//	//				fn->setDisplayName(part);
//	//				folder->addNode(std::unique_ptr<FolderNode>(fn));
//	//				//folder->addNodes(list);
//	//			}
//	//			folder = fn;
//	//		}
//	//		FileNode* node = nullptr;
//	//		foreach(FileNode* n, folder->fileNodes())
//	//			if(n->filePath().toString() == kv.key())
//	//			{
//	//				node = n;
//	//				break;
//	//			}
//	//		if(node == nullptr)
//	//		{
//	//			//QList<FileNode*> list;
//	//			FileNode* fn = new FileNode(Utils::FilePath::fromString(kv.key()), FileType::Source);
//	//			//list.append(new FileNode(Utils::FileName::fromString(kv.key()), SourceType, false));
//	//			//folder->addFileslist);
//	//			folder->addNode(std::unique_ptr<FileNode>(fn));
//	//		}
//	//	}

//	//	// removing
//	//	typedef QHash<QString, FileNode*>::Iterator KeyValue;
//	//	for (KeyValue kv = nodes.begin(); kv != nodes.end(); ++kv)
//	//	{
//	//		if(files.contains(kv.key()))
//	//			continue;
//	////		QList<FileNode*> list;
//	////		list.append(kv.value());
//	////		FolderNode* parent = kv.value()->parentFolderNode();
//	////		parent->removeFileNodes(list);

//	////		while(parent != this)
//	////			if(isEmpty(parent))
//	////			{
//	////				FolderNode* p = parent->parentFolderNode();
//	////				QList<FolderNode*> list;
//	////				list.append(parent);
//	////				p->removeFolderNodes(list);
//	////				parent = p;
//	////			}
//	////		else
//	////				break;
//	//	}
//}

} // namespace DProjectManager
