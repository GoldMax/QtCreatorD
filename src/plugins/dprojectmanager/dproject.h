#ifndef DPROJECT_H
#define DPROJECT_H

#include "dprojectmanagerconstants.h"

#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>

#include <utils/fileutils.h>

namespace DProjectManager {

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
	uint compilePriority() const { return m_priority; }
	void setCompilePriority(uint value) { m_priority = value; }
	const QList<QString>& files() const { return m_files; }

protected:
	QVariantMap toMap() const override;
	RestoreResult fromMap(const QVariantMap &map, QString* errorMessage) override;
	bool setupTarget(ProjectExplorer::Target *t) override;

private:
	bool parseProjectFile(RefreshOptions options);

	QString m_srcDir;
	QString m_libs;
	QString m_includes;
	QString m_extraArgs;
	uint m_priority;
	QList<QString> m_files;
};

class DProjectNode : public ProjectExplorer::ProjectNode
{
public:
	DProjectNode(DProject* project);

	bool canAddSubProject(const QString &) const override { return false; }
	bool addSubProject(const QString &) override { return false; }
	bool removeSubProject(const QString &) override { return false; }

private:
	DProject *m_project;
};

} // namespace DProjectManager


#endif // DPROJECT_H
