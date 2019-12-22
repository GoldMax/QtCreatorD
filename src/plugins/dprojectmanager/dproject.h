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
		Project       = ProjectFile | Configuration,
		Everything    = ProjectFile | Files | Configuration
	};

public:
	DProject(const Utils::FilePath &filename);
	~DProject() override;

//	bool setupTarget(ProjectExplorer::Target *t) override;

	bool addFiles(const QStringList &filePaths);
	bool removeFiles(const QStringList &filePaths);
	bool renameFile(const QString &filePath, const QString &newFilePath);

	void refresh(RefreshOptions options);

	const QString& sourcesDirectory() const { return m_srcDir; }
	void setSourcesDirectory(QString value) { m_srcDir = value; }
	const QString& libraries() const { return m_libs; }
	void setLibraries(QString value) { m_libs = value; }
	const QString& includes() const { return m_includes; }
	void setIncludes(QString value); // { m_includes = value; }
	const QString& extraArgs() const { return m_extraArgs; }
	void setExtraArgs(QString value) { m_extraArgs = value; }
	const QHash<QString,QString>& files() { return m_files; }

	const QDir buildDirectory() const { return m_buildRootDir; }

protected:
	QVariantMap toMap() const override;
	RestoreResult fromMap(const QVariantMap &map, QString* errorMessage) override;
	bool setupTarget(ProjectExplorer::Target *t) override;

private:
	bool parseProjectFile(RefreshOptions options);

//	QStringList processEntries(const QStringList &paths,
//																												QHash<QString, QString> *map = nullptr) const;
	QString m_srcDir;
	QString m_libs;
	QString m_includes;
	QString m_extraArgs;
	QHash<QString, QString> m_files;

	QDir m_buildRootDir;

//	QFuture<void> m_codeModelFuture;
};

////---------------------------------------------------------
//class DProjectFile : public Core::IDocument
//{
//	Q_OBJECT

//public:
//	DProjectFile(DProject *parent, const Utils::FilePath &fileName,
//														DProject::RefreshOptions options);

//// QString defaultPath() const { return QString(); }
////	QString suggestedFileName() const { return QString(); }
////	QString mimeType() const	{ return QLatin1String(Constants::DPROJECT_MIMETYPE); }
////	bool isModified() const { return false; }
////	bool isSaveAsAllowed() const { return false; }
////	bool save(QString *errorString, const QString &fileName, bool autoSave) { return false; }

//	ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const;
//	bool reload(QString *errorString, ReloadFlag flag, ChangeType type);

//private:
//	DProject *m_project;
//	DProject::RefreshOptions m_options;
//};

} // namespace DProjectManager

#endif // DPROJECT_H
