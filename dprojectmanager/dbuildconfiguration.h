#ifndef DBUILDCONFIGURATION_H
#define DBUILDCONFIGURATION_H

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/namedwidget.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/target.h>

//#include <QPlainTextEdit>
#include <QLineEdit>

namespace Utils { class PathChooser; }

using namespace ProjectExplorer;

namespace DProjectManager {
namespace Internal {

class DTarget;
class DBuildConfigurationFactory;
class DBuildSettingsWidget;

class DBuildConfiguration : public ProjectExplorer::BuildConfiguration
{
 Q_OBJECT
 friend class DBuildConfigurationFactory;

public:
 explicit DBuildConfiguration(Target *parent);

 NamedWidget *createConfigWidget();
 BuildType buildType() const { return Unknown; }
 bool fromMap(const QVariantMap &map);

signals:
	void configurationChanged();

protected:
 DBuildConfiguration(Target *parent, DBuildConfiguration *source);
 DBuildConfiguration(Target *parent, const Core::Id id);

 friend class DBuildSettingsWidget;
};

class DBuildConfigurationFactory : public ProjectExplorer::IBuildConfigurationFactory
{
 Q_OBJECT

public:
 explicit DBuildConfigurationFactory(QObject *parent = 0);
 ~DBuildConfigurationFactory();

 bool canCreate(const Target *parent) const;
 QList<BuildInfo *> availableBuilds(const Target *parent) const;
 ProjectExplorer::BuildConfiguration *create(Target *parent, const BuildInfo *info) const;

	// Used to see whether this factory can produce any BuildConfigurations for a kit when
	// setting up the given project.
	bool canSetup(const Kit *k, const QString &projectPath) const;
	// List of build information that can be used to initially set up a new build configuration.
	QList<BuildInfo *> availableSetups(const Kit *k, const QString &projectPath) const;


 bool canClone(const Target *parent, BuildConfiguration *source) const;
 BuildConfiguration *clone(Target *parent, BuildConfiguration *source);
 bool canRestore(const Target *parent, const QVariantMap &map) const;
 BuildConfiguration *restore(Target *parent, const QVariantMap &map);


private:
 bool canHandle(const Target *t) const;

};

class DBuildSettingsWidget : public ProjectExplorer::NamedWidget
{
 Q_OBJECT

public:
 DBuildSettingsWidget(DBuildConfiguration *bc);

private:
 DBuildConfiguration *m_buildConfiguration;
	Utils::PathChooser *m_pathChooser;
//	QPlainTextEdit* editIncludes;
//	QPlainTextEdit* editLibs;
	QLineEdit* editIncludes;
	QLineEdit* editLibs;
 QLineEdit* editExtra;

private slots:
	void buildDirectoryChanged();
	void editsTextChanged();
	void editsEditingFinished();

};

} // namespace Internal
} // namespace DProjectManager

#endif // DBUILDCONFIGURATION_H
