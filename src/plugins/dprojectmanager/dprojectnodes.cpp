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
	setDisplayName(project->projectFilePath().toFileInfo().completeBaseName());
	setIcon(QIcon(QLatin1String(Constants::ICON_DPROJECT)));
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
RemovedFilesFromProject DProjectNode::removeFiles(const QStringList &filePaths, QStringList *notRemoved)
{
	Q_UNUSED(notRemoved)
	m_project->removeFiles(filePaths);
	return RemovedFilesFromProject::Ok;
}
bool DProjectNode::renameFile(const QString &filePath, const QString &newFilePath)
{
	return m_project->renameFile(filePath, newFilePath);
}


DProjectGroupNode::DProjectGroupNode(DProjectGroup* project)
	: ProjectNode(project->projectFilePath())
	, m_project(project)
{
	//static int count = 0;
	setDisplayName(project->projectFilePath().toFileInfo().completeBaseName());
	setIcon(QIcon(QLatin1String(Constants::ICON_DPROJECTGROUP)));
	setAbsoluteFilePathAndLine(project->projectFilePath(),0);
	setShowWhenEmpty(true);
}

bool DProjectGroupNode::supportsAction(ProjectExplorer::ProjectAction action, const Node *) const
{
	return
						action == InheritedFromParent
			|| action == AddSubProject
			|| action == AddExistingProject
			|| action == RemoveSubProject
			;

}
bool DProjectGroupNode::canAddSubProject(const QString &) const { return true; }
QStringList DProjectGroupNode::subProjectFileNamePatterns() const
{
	return QStringList() << "*.qcd";
}
bool DProjectGroupNode::addSubProject(const QString &projFilePath)
{
	return m_project->addSubProject(projFilePath);
}
bool DProjectGroupNode::removeSubProject(const QString &)
{
	return false;
}


} // namespace DProjectManager
