#ifndef DLangPROJECT_H
#define DLangPROJECT_H

#include "dlangprojectmanager.h"
#include "dlangprojectnodes.h"

#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
#include <projectexplorer/target.h>
#include <projectexplorer/toolchain.h>
#include <projectexplorer/buildconfiguration.h>
#include <coreplugin/idocument.h>

#include <QFuture>

namespace DLangProjectManager {
namespace Internal {

class DLangProjectFile;

class DLangProject : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    DLangProject(Manager *manager, const QString &filename);
    ~DLangProject();

    QString filesFileName() const;

    QString displayName() const;
    Core::Id id() const;
    Core::IDocument *document() const;
    ProjectExplorer::IProjectManager *projectManager() const;

    DLangProjectNode *rootProjectNode() const;
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
    QString m_fileName;
    QString m_filesFileName;

    QString m_projectName;
    DLangProjectFile *m_creatorIDocument;
    DLangProjectFile *m_filesIDocument;

    QStringList m_rawFileList;
    QStringList m_files;
    QHash<QString, QString> m_rawListEntries;

    DLangProjectNode *m_rootNode;
    QFuture<void> m_codeModelFuture;
};

class DLangProjectFile : public Core::IDocument
{
    Q_OBJECT

public:
    DLangProjectFile(DLangProject *parent, QString fileName, DLangProject::RefreshOptions options);

    bool save(QString *errorString, const QString &fileName, bool autoSave);

    QString defaultPath() const;
    QString suggestedFileName() const;
    QString mimeType() const;

    bool isModified() const;
    bool isSaveAsAllowed() const;

    ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const;
    bool reload(QString *errorString, ReloadFlag flag, ChangeType type);

private:
    DLangProject *m_project;
    DLangProject::RefreshOptions m_options;
};

} // namespace Internal
} // namespace DLangProjectManager

#endif // DLangPROJECT_H
