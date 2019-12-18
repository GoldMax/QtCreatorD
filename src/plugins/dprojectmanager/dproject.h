#ifndef DPROJECT_H
#define DPROJECT_H

#include "dprojectmanagerconstants.h"
//#include "dprojectmanager.h"
#include "dprojectnodes.h"

#include <projectexplorer/project.h>

//#include <projectexplorer/projectmanager.h>
//#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>
//#include <projectexplorer/target.h>
//#include <projectexplorer/toolchain.h>
//#include <projectexplorer/buildconfiguration.h>
//#include <coreplugin/idocument.h>

//#include <QFuture>
//#include <QSet>

#include <utils/fileutils.h>

namespace DProjectManager {

class DProjectFile;
class DProjectNode;

class DProject : public ProjectExplorer::Project
{
	Q_OBJECT

public:
	enum RefreshOptions
	{
		ProjectFile   = 0x01,
		Files         = 0x02,
		Configuration = 0x04,
		Everything    = ProjectFile | Files | Configuration
	};

public:
	DProject(const Utils::FilePath &filename);
	~DProject() override;

//	QString displayName() const { return m_projectName; }
	//DProjectFile* document() const override;
	//DProjectNode* rootProjectNode() const final;
//	//QStringList files(FilesMode ) const { return m_files.keys(); }
//	bool setupTarget(ProjectExplorer::Target *t) override;

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
	const QString& extraArgs() const { return m_extraArgs; }
	void setExtraArgs(QString value) { m_extraArgs = value; }

protected:
	QVariantMap toMap() const override;
	RestoreResult fromMap(const QVariantMap &map, QString* errorMessage) override;

private:
	//std::unique_ptr<Core::IDocument> m_document;
	//std::unique_ptr<DProjectNode> m_rootProjectNode;

	bool parseProjectFile(RefreshOptions options);

//	QStringList processEntries(const QStringList &paths,
//																												QHash<QString, QString> *map = nullptr) const;

//	const QString m_projectName;
//	Utils::FilePath m_projectFileName;
	QDir m_buildDir;

	QHash<QString, QString> m_files;
	QString m_libs;
	QString m_includes;
	QString m_extraArgs;


//	QFuture<void> m_codeModelFuture;
};

//---------------------------------------------------------
class DProjectFile : public Core::IDocument
{
	Q_OBJECT

public:
	DProjectFile(DProject *parent, const Utils::FilePath &fileName,
														DProject::RefreshOptions options);

// QString defaultPath() const { return QString(); }
//	QString suggestedFileName() const { return QString(); }
//	QString mimeType() const	{ return QLatin1String(Constants::DPROJECT_MIMETYPE); }
//	bool isModified() const { return false; }
//	bool isSaveAsAllowed() const { return false; }
//	bool save(QString *errorString, const QString &fileName, bool autoSave) { return false; }

	ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const;
	bool reload(QString *errorString, ReloadFlag flag, ChangeType type);

private:
	DProject *m_project;
	DProject::RefreshOptions m_options;
};

} // namespace DProjectManager

#endif // DPROJECT_H
