#pragma once

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/namedwidget.h>
//#include <projectexplorer/buildinfo.h>
//#include <projectexplorer/target.h>

#include <QLineEdit>

namespace Utils { class PathChooser; }

using namespace ProjectExplorer;

namespace DProjectManager {

//class DTarget;
//class DBuildConfigurationFactory;
class DBuildSettingsWidget;

class DBuildConfiguration : public BuildConfiguration
{
	Q_OBJECT
//	friend class DBuildConfigurationFactory;

public:
	DBuildConfiguration(Target *parent, Core::Id id);

	void initialize() override;
	ProjectExplorer::NamedWidget *createConfigWidget() override;
	bool fromMap(const QVariantMap &map) override;

signals:
	void configurationChanged();

	friend class DBuildSettingsWidget;
};

class DBuildConfigurationFactory : public BuildConfigurationFactory
{
	//Q_OBJECT

public:
	DBuildConfigurationFactory();
//	~DBuildConfigurationFactory();

//	int priority(const Target*) const { return 0; }
//	int priority(const Kit*, const QString&) const { return 0; }


//	bool canCreate(const Target *parent) const;
//	ProjectExplorer::BuildConfiguration *create(Target *parent, const BuildInfo *info) const;

//	// Used to see whether this factory can produce any BuildConfigurations for a kit when
//	// setting up the given project.
//	bool canSetup(const Kit *k, const QString &projectPath) const;
//	// List of build information that can be used to initially set up a new build configuration.
//	QList<BuildInfo *> availableSetups(const Kit *k, const QString &projectPath) const;


//	bool canClone(const Target *parent, BuildConfiguration *source) const;
//	BuildConfiguration *clone(Target *parent, BuildConfiguration *source);
//	bool canRestore(const Target *parent, const QVariantMap &map) const;
//	BuildConfiguration *restore(Target *parent, const QVariantMap &map);


private:
	QList<BuildInfo> availableBuilds(const Kit *k,
																																		const Utils::FilePath &projectPath,
																																		bool forSetup) const override;
//	bool canHandle(const Target *t) const;
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
