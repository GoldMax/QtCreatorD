#pragma once

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildsystem.h>
#include <projectexplorer/namedwidget.h>

#include <QLineEdit>
#include <QSpinBox>

namespace Utils { class PathChooser; }

using namespace ProjectExplorer;

namespace DProjectManager {

class DBuildSettingsWidget;
class DBuildSystem;

class DBuildConfiguration : public BuildConfiguration
{
	Q_OBJECT

public:
	DBuildConfiguration(Target *parent, Utils::Id id);

	ProjectExplorer::BuildSystem *buildSystem() const final;
	//void initialize() override;
	ProjectExplorer::NamedWidget *createConfigWidget() override;

	bool isEnabled() const  override;

signals:
	void configurationChanged();

private:
	void emitConfigurationChanged(bool);

	DBuildSystem *m_buildSystem = nullptr;

	friend class DBuildSettingsWidget;
};

class DBuildConfigurationFactory : public BuildConfigurationFactory
{
public:
	DBuildConfigurationFactory();

//private:
//	QList<BuildInfo> availableBuilds(const Kit *k,
//																																		const Utils::FilePath &projectPath,
//																																		bool forSetup) const override;
};

class DBuildSettingsWidget : public ProjectExplorer::NamedWidget
{
	Q_OBJECT

public:
	DBuildSettingsWidget(DBuildConfiguration *bc);

private:
	DBuildConfiguration *m_buildConfiguration;
	Utils::PathChooser *m_pathChooser;
	QLineEdit* editIncludes;
	QLineEdit* editLibs;
	QLineEdit* editExtra;
	QSpinBox* editPriority;

private slots:
	void sourceDirectoryChanged();
	//void editsTextChanged();
	//void priorityValueChanged(int);
	void editsEditingFinished();

};

class DBuildSystem : public ProjectExplorer::BuildSystem
{
	Q_OBJECT

public:
	explicit DBuildSystem(DBuildConfiguration *bc);
	~DBuildSystem();

	void triggerParsing() final;
	bool supportsAction(Node *context, ProjectAction action, const Node *node) const final;

	bool addFiles(Node *context, const QStringList &filePaths, QStringList *notAdded = nullptr) final;
	RemovedFilesFromProject removeFiles(Node *context, const QStringList &filePaths,
																																					QStringList *notRemoved = nullptr) final;
	bool renameFile(Node *context, const QString &filePath, const QString &newFilePath) final;
};

} // namespace DProjectManager
