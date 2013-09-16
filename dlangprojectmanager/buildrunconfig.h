#ifndef BUILDRUNCONFIG_H
#define BUILDRUNCONFIG_H

//#include <qt qtsupport_global.h>
#include "buildrunconfigwidget.h"
#include <projectexplorer/localapplicationrunconfiguration.h>

#include <QVariantMap>

namespace ProjectExplorer { class Target; }

namespace DLangProjectManager
{
 namespace Internal { class BuildRunConfigWidget; }

class BuildRunConfigFactory;

class BuildRunConfig : public ProjectExplorer::LocalApplicationRunConfiguration
{
 Q_OBJECT
 // the configuration widget needs to setExecutable setWorkingDirectory and setCommandLineArguments
 friend class Internal::BuildRunConfigWidget;
 friend class BuildRunConfigFactory;

public:
 explicit BuildRunConfig(ProjectExplorer::Target *parent);
 ~BuildRunConfig();

 /*
  * Returns the executable, looks in the environment for it and might even
  * ask the user if none is specified
  */
 QString executable() const;

 /* Returns whether this runconfiguration ever was configured with an executable */
 bool isConfigured() const;

 RunMode runMode() const;
 QString workingDirectory() const;
 QString commandLineArguments() const;

 QWidget *createConfigurationWidget();
 QString dumperLibrary() const;
 QStringList dumperLibraryLocations() const;

 ProjectExplorer::Abi abi() const;
 QVariantMap toMap() const;

 bool ensureConfigured(QString *errorMessage);

 void setExecutable(const QString &executable);
 QString rawExecutable() const;
 void setCommandLineArguments(const QString &commandLineArguments);
 QString rawCommandLineArguments() const;
 void setBaseWorkingDirectory(const QString &workingDirectory);
 QString baseWorkingDirectory() const;
 void setUserName(const QString &name);
 void setRunMode(ProjectExplorer::LocalApplicationRunConfiguration::RunMode runMode);
 bool validateExecutable(QString *executable = 0, QString *errorMessage = 0) const;

signals:
 void changed();

protected:
 BuildRunConfig(ProjectExplorer::Target *parent, BuildRunConfig *source);
 virtual bool fromMap(const QVariantMap &map);
 QString defaultDisplayName() const;

private:
 void ctor();

 QString m_executable;
 QString m_workingDirectory;
 QString m_cmdArguments;
 RunMode m_runMode;
 bool m_userSetName;
 QString m_userName;
};

class BuildRunConfigFactory : public ProjectExplorer::IRunConfigurationFactory
{
 Q_OBJECT

public:
 explicit BuildRunConfigFactory(QObject *parent = 0);
 ~BuildRunConfigFactory();

 QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent) const;
 QString displayNameForId(const Core::Id id) const;

 bool canCreate(ProjectExplorer::Target *parent, const Core::Id id) const;
 bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const;
 bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const;
 ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent,
                                          ProjectExplorer::RunConfiguration *source);

private:
 bool canHandle(ProjectExplorer::Target *parent) const;

 ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent, const Core::Id id);
 ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent,
                                              const QVariantMap &map);
};

} // namespace QtSupport
#endif // BUILDRUNCONFIG_H
