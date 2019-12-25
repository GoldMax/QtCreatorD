#pragma once

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/namedwidget.h>

#include <QLineEdit>

namespace Utils { class PathChooser; }

using namespace ProjectExplorer;

namespace DProjectManager {

class DBuildSettingsWidget;

class DBuildConfiguration : public BuildConfiguration
{
	Q_OBJECT

public:
	DBuildConfiguration(Target *parent, Core::Id id);

	void initialize() override;
	ProjectExplorer::NamedWidget *createConfigWidget() override;

	bool isEnabled() const  override;

signals:
	void configurationChanged();

	friend class DBuildSettingsWidget;
};

class DBuildConfigurationFactory : public BuildConfigurationFactory
{
public:
	DBuildConfigurationFactory();

private:
	QList<BuildInfo> availableBuilds(const Kit *k,
																																		const Utils::FilePath &projectPath,
																																		bool forSetup) const override;
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

private slots:
	void buildDirectoryChanged();
	void editsTextChanged();
	void editsEditingFinished();

};

} // namespace DProjectManager
