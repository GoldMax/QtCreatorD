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

namespace DProjectManager {
namespace Internal {

class DProjectFile;

class DProject : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    DProject(Manager *manager, const QString &filename);
    ~DProject();

				Core::Id id() const { return Core::Id(Constants::DPROJECT_ID); }
				QString displayName() const { return m_projectName; }
				Core::IDocument *document() const;
				ProjectExplorer::IProjectManager *projectManager() const { return m_manager; }
				DProjectNode *rootProjectNode() const { return m_rootNode; }
    QStringList files(FilesMode fileMode) const;

    bool addFiles(const QStringList &filePaths);
    bool removeFiles(const QStringList &filePaths);
    bool setFiles(const QStringList &filePaths);
    bool renameFile(const QString &filePath, const QString &newFilePath);

    enum RefreshOptions {
        Files         = 0x01,
        Configuration = 0x02,
        Everything    = Files | Configuration
    };

    void refresh(RefreshOptions options);

    QStringList files() const;

protected:
    bool fromMap(const QVariantMap &map);

private:
    bool saveRawFileList(const QStringList &rawFileList);
    void parseProject(RefreshOptions options);
    QStringList processEntries(const QStringList &paths,
                               QHash<QString, QString> *map = 0) const;

    Manager *m_manager;
				const QString m_projectName;
				const QString m_projectFileName;
				const QDir m_projectDir;

				DProjectFile* m_projectIDocument;

    QStringList m_rawFileList;
    QStringList m_files;
    QHash<QString, QString> m_rawListEntries;

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
