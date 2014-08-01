#ifndef DRUNCONFIG_H
#define DRUNCONFIG_H

#include "dmakestep.h"

#include <projectexplorer/localapplicationrunconfiguration.h>

#include <QVariantMap>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QLineEdit;
class QComboBox;
class QLabel;
class QAbstractButton;
QT_END_NAMESPACE

namespace Utils {
class DetailsWidget;
class PathChooser;
}

namespace ProjectExplorer { class Target; }

namespace DProjectManager {

class DRunConfigurationWidget;
class DRunConfigurationFactory;

class DRunConfiguration : public ProjectExplorer::LocalApplicationRunConfiguration
{
 Q_OBJECT
 // the configuration widget needs to setExecutable setWorkingDirectory and setCommandLineArguments
 friend class DRunConfigurationWidget;
 friend class DRunConfigurationFactory;

public:
 explicit DRunConfiguration(ProjectExplorer::Target *parent);
 ~DRunConfiguration();

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

 void updateConfig(const DMakeStep* makeStep);
signals:
 void changed();

protected:
 DRunConfiguration(ProjectExplorer::Target *parent, DRunConfiguration *source);
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

class DRunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
 Q_OBJECT

public:
 explicit DRunConfigurationFactory(QObject *parent = 0);
 ~DRunConfigurationFactory();

 static DRunConfigurationFactory* instance() { return m_instance; }

 QList<Core::Id> availableCreationIds(ProjectExplorer::Target *parent) const;
 QString displayNameForId(const Core::Id id) const;

 bool canCreate(ProjectExplorer::Target *parent, const Core::Id id) const;
 bool canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const;
 bool canClone(ProjectExplorer::Target *parent, ProjectExplorer::RunConfiguration *product) const;
 ProjectExplorer::RunConfiguration *clone(ProjectExplorer::Target *parent,
                                          ProjectExplorer::RunConfiguration *source);

private:
 static DRunConfigurationFactory* m_instance;
 bool canHandle(ProjectExplorer::Target *parent) const;

 ProjectExplorer::RunConfiguration *doCreate(ProjectExplorer::Target *parent, const Core::Id id);
 ProjectExplorer::RunConfiguration *doRestore(ProjectExplorer::Target *parent,
                                              const QVariantMap &map);
};

class DRunConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    DRunConfigurationWidget(DRunConfiguration *rc);

private slots:
    void changed();

    void executableEdited();
    void argumentsEdited(const QString &arguments);
    void workingDirectoryEdited();
    void termToggled(bool);
    void environmentWasChanged();

private:
    bool m_ignoreChange;
    DRunConfiguration *m_runConfiguration;
    Utils::PathChooser *m_executableChooser;
    QLineEdit *m_commandLineArgumentsLineEdit;
    Utils::PathChooser *m_workingDirectory;
    QCheckBox *m_useTerminalCheck;
    Utils::DetailsWidget *m_detailsContainer;
};

} // namespace DProjectManager
#endif // DRUNCONFIG_H
