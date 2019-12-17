#ifndef DPROJECTNODE_H
#define DPROJECTNODE_H

#include "dprojectmanager/dproject.h"

#include <projectexplorer/projectnodes.h>
//#include <projectexplorer/runconfiguration.h>

//#include <QStringList>
//#include <QHash>
//#include <QSet>

//namespace Core
//{
//class IDocument;
//}

using namespace ProjectExplorer;

namespace DProjectManager {

//class DProject;

class DProjectNode : public ProjectExplorer::ProjectNode
{
public:
	DProjectNode(DProject *project/*, Core::IDocument *projectFile*/);

	//Core::IDocument *projectFile() const;

	//bool hasBuildTargets() const { return true; }
	bool canAddSubProject(const QString &) const override { return false; }
	bool addSubProject(const QString &) override { return false; }
	bool removeSubProject(const QString &) override { return false; }

	bool supportsAction(ProjectExplorer::ProjectAction action, const Node *) const override
	{
					return action == AddNewFile
													|| action == AddExistingFile
													// action == AddExistingDirectory
													|| action == RemoveFile
													|| action == Rename;
	}
	bool addFiles(const QStringList &filePaths, QStringList *notAdded = nullptr) override;
	bool removeFiles(const QStringList &filePaths, QStringList *notRemoved = nullptr) override;
	bool deleteFiles(const QStringList &) override { return false; }
	bool renameFile(const QString &filePath, const QString &newFilePath) override;

//	QList<ProjectExplorer::ProjectAction> supportedActions(Node *node) const;
//	QList<ProjectExplorer::RunConfiguration *> runConfigurationsFor(Node *node);

	//void refresh(bool needRebuild);

private:
	DProject *m_project;
	//Core::IDocument *m_projectFile;
};

} // namespace DProjectManager

#endif // DPROJECTNODE_H
