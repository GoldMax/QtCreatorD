#ifndef DPROJECTNODE_H
#define DPROJECTNODE_H

#include <projectexplorer/projectnodes.h>

#include <QStringList>
#include <QHash>
#include <QSet>

namespace Core
{
class IDocument;
}

namespace DProjectManager {
namespace Internal {

class DProject;

class DProjectNode : public ProjectExplorer::ProjectNode
{
public:
 DProjectNode(DProject *project, Core::IDocument *projectFile);

 Core::IDocument *projectFile() const;

 bool hasBuildTargets() const { return true; }
 bool canAddSubProject(const QString &) const { return false; }
 bool addSubProjects(const QStringList &) { return false; }
 bool removeSubProjects(const QStringList &) { return false; }

 bool addFiles(const QStringList &filePaths, QStringList *notAdded = 0);
 bool removeFiles(const QStringList &filePaths, QStringList *notRemoved = 0);
 bool deleteFiles(const QStringList &) { return false; }
 bool renameFile(const QString &filePath, const QString &newFilePath);

 QList<ProjectExplorer::ProjectNode::ProjectAction> supportedActions(Node *node) const;
 QList<ProjectExplorer::RunConfiguration *> runConfigurationsFor(Node *node);

 void refresh();

private:
 DProject *m_project;
 Core::IDocument *m_projectFile;
};

} // namespace Internal
} // namespace DProjectManager

#endif // DPROJECTNODE_H
