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

DProjectNode::DProjectNode(DProject *project, Core::IDocument *projectFile)
	: ProjectNode(projectFile->filePath()), m_project(project), m_projectFile(projectFile)
{
	static uint count = 0;
	setDisplayName(projectFile->filePath().toFileInfo().completeBaseName());
	setLine(++count);
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
	m_project->removeFiles(filePaths);
	refresh(false);
	return true;
}

bool DProjectNode::renameFile(const QString &filePath, const QString &newFilePath)
{
	return m_project->renameFile(filePath, newFilePath);
}

QList<ProjectAction> DProjectNode::supportedActions(Node *node) const
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

void DProjectNode::refresh(bool needRebuild)
{
	if(needRebuild)
	{
		this->removeFileNodes(this->fileNodes());
		this->removeFolderNodes(this->subFolderNodes());
	}

	const QHash<QString,QString>& files = m_project->files();

	QHash<QString,FileNode*> nodes;
	QStack<FolderNode*> stack;
	stack.push(this);
	while(stack.count() > 0)
	{
		FolderNode* n = stack.pop();
		foreach(FileNode* i, n->fileNodes())
			nodes[i->path().toString()] = i;
		foreach(FolderNode* i, n->subFolderNodes())
			stack.push(i);
	}

	// adding
	typedef QHash<QString,QString>::ConstIterator FilesKeyValue;
	for (FilesKeyValue kv = files.constBegin(); kv != files.constEnd(); ++kv)
	{
		if(nodes.contains(kv.key()))
			continue;
		FolderNode* folder = this;
		QStringList parts = kv.value().split(QDir::separator());
		QString absFolderPath = m_project->buildDirectory().path();
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
				list.append(fn = new FolderNode(Utils::FileName::fromString(absFolderPath)));
				fn->setDisplayName(part);
				folder->addFolderNodes(list);
			}
			folder = fn;
		}
		FileNode* node = 0;
		foreach(FileNode* n, folder->fileNodes())
			if(n->path().toString() == kv.key())
			{
				node = n;
				break;
			}
		if(node == 0)
		{
			QList<FileNode*> list;
			list.append(new FileNode(Utils::FileName::fromString(kv.key()), SourceType, false));
			folder->addFileNodes(list);
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
		parent->removeFileNodes(list);

		while(parent != this)
			if(isEmpty(parent))
			{
				FolderNode* p = parent->parentFolderNode();
				QList<FolderNode*> list;
				list.append(parent);
				p->removeFolderNodes(list);
				parent = p;
			}
		else
				break;
	}
}

} // namespace DProjectManager
