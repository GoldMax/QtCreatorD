#ifndef DPROJECT_H
#define DPROJECT_H

#include "dprojectmanagerconstants.h"
#include "dprojectmanager.h"
#include "dprojectnodes.h"

#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/target.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/buildconfiguration.h>
#include <coreplugin/idocument.h>

#include <QFuture>
#include <QSet>

namespace DProjectManager {
namespace Internal {

class DProjectFile;

class DProject : public ProjectExplorer::Project
{
 Q_OBJECT

public:
 enum RefreshOptions
 {
  Files         = 0x01,
  Configuration = 0x02,
  Everything    = Files | Configuration
 };

public:
 DProject(Manager *manager, const QString &filename);
 ~DProject();

 Core::Id id() const { return Core::Id(Constants::DPROJECT_ID); }
 QString displayName() const { return m_projectName; }
 Core::IDocument *document() const;
 ProjectExplorer::IProjectManager *projectManager() const { return m_manager; }
 DProjectNode *rootProjectNode() const { return m_rootNode; }
	QStringList files(FilesMode ) const { return m_files.keys(); }
 bool setupTarget(ProjectExplorer::Target *t);

 bool addFiles(const QStringList &filePaths);
 bool removeFiles(const QStringList &filePaths);
 bool renameFile(const QString &filePath, const QString &newFilePath);

 void refresh(RefreshOptions options);

	const QHash<QString,QString>& files() { return m_files; }
	const QDir buildDirectory() const { return m_buildDir; }
 const QString& libraries() const { return m_libs; }
	void setLibraries(QString value) { m_libs = value; }
 const QString& includes() const { return m_includes; }
	void setIncludes(QString value) { m_includes = value; }

protected:
 QVariantMap toMap() const;
 bool fromMap(const QVariantMap &map);

private:
	bool parseProject(RefreshOptions options);
 QStringList processEntries(const QStringList &paths,
                            QHash<QString, QString> *map = 0) const;

 Manager *m_manager;
 const QString m_projectName;
 const QString m_projectFileName;
 DProjectFile* m_projectIDocument;
	QDir m_buildDir;

	QHash<QString,QString> m_files;
 QString m_libs;
 QString m_includes;

 DProjectNode *m_rootNode;
	QFuture<void> m_codeModelFuture;
};

class DProjectFile : public Core::IDocument
{
 Q_OBJECT

public:
 DProjectFile(DProject *parent, QString fileName, DProject::RefreshOptions options);

 bool save(QString *errorString, const QString &fileName, bool autoSave);

 QString defaultPath() const;
 QString suggestedFileName() const;
 QString mimeType() const;

 bool isModified() const;
 bool isSaveAsAllowed() const;

 ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const;
 bool reload(QString *errorString, ReloadFlag flag, ChangeType type);

private:
 DProject *m_project;
 DProject::RefreshOptions m_options;
};

} // namespace Internal
} // namespace DProjectManager

#endif // DPROJECT_H
