#ifndef DPROJECTNODE_H
#define DPROJECTNODE_H

#include "dprojectmanager/dproject.h"

#include <projectexplorer/projectnodes.h>

namespace Core
{
class IDocument;
}

using namespace ProjectExplorer;

namespace DProjectManager {

class DProject;

class DProjectNode : public ProjectExplorer::ProjectNode
{
public:
	DProjectNode(DProject* project);

	bool canAddSubProject(const QString &) const override { return false; }
	bool addSubProject(const QString &) override { return false; }
	bool removeSubProject(const QString &) override { return false; }

	bool supportsAction(ProjectExplorer::ProjectAction action, const Node *) const override;
	bool addFiles(const QStringList &filePaths, QStringList *notAdded = nullptr) override;
	RemovedFilesFromProject removeFiles(const QStringList &filePaths, QStringList *notRemoved = nullptr) override;
	bool deleteFiles(const QStringList &) override { return false; }
	bool canRenameFile(const QString &, const QString &) override { return true; }
	bool renameFile(const QString &filePath, const QString &newFilePath) override;

private:
	DProject *m_project;
};

class DProjectGroupNode : public ProjectExplorer::ProjectNode
{
public:
	DProjectGroupNode(QString name);

	bool supportsAction(ProjectExplorer::ProjectAction action, const Node *) const override;
	bool canAddSubProject(const QString &) const override;
	bool addSubProject(const QString &) override ;
	bool removeSubProject(const QString &) override ;

};

} // namespace DProjectManager

#endif // DPROJECTNODE_H
