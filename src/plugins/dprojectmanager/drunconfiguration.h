//#ifndef DRUNCONFIG_H
//#define DRUNCONFIG_H

////#include "dmakestep.h"

//#include <projectexplorer/runconfiguration.h>
//#include <projectexplorer/applicationlauncher.h>
//#include <projectexplorer/runnables.h>

//QT_BEGIN_NAMESPACE
//class QCheckBox;
//class QLineEdit;
//class QComboBox;
//class QLabel;
//class QAbstractButton;
//QT_END_NAMESPACE

//namespace Utils {
//class DetailsWidget;
//class PathChooser;
//}
//namespace ProjectExplorer {

//class ArgumentsAspect;
//class TerminalAspect;
//}
//using namespace ProjectExplorer;

//namespace DProjectManager {

//class DRunConfigurationWidget;
//class DRunConfigurationFactory;
//class DCustomExecutableDialog;

//class DRunConfiguration : public RunConfiguration
//{
//    Q_OBJECT
//    // the configuration widget needs to setExecutable setWorkingDirectory and setCommandLineArguments
//    friend class DRunConfigurationWidget;
//    friend class DRunConfigurationFactory;

//public:
//    explicit DRunConfiguration(Target *parent);
//    ~DRunConfiguration() override;

//    /**
//     * Returns the executable, looks in the environment for it and might even
//     * ask the user if none is specified
//     */
//    QString executable() const;
//    QString workingDirectory() const;
//    Runnable runnable() const override;

//    /** Returns whether this runconfiguration ever was configured with an executable
//     */
//    bool isConfigured() const override;
//    QWidget *createConfigurationWidget() override;
//    Abi abi() const override;
//    QVariantMap toMap() const override;
//    ConfigurationState ensureConfigured(QString *errorMessage) override;

//    void updateConfig(/*const DMakeStep* makeStep*/);

//signals:
//    void changed();

//protected:
//    DRunConfiguration(Target *parent,
//                                     DRunConfiguration *source);
//    virtual bool fromMap(const QVariantMap &map) override;
//    QString defaultDisplayName() const;

//private:
//    void ctor();

//    void configurationDialogFinished();
//    void setExecutable(const QString &executable);
//    QString rawExecutable() const;
//    void setCommandLineArguments(const QString &commandLineArguments);
//    void setBaseWorkingDirectory(const QString &workingDirectory);
//    QString baseWorkingDirectory() const;
//    void setUserName(const QString &name);
//    void setRunMode(ApplicationLauncher::Mode runMode);
//    bool validateExecutable(QString *executable = 0, QString *errorMessage = 0) const;

//    QString m_executable;
//    QString m_workingDirectory;
//    DCustomExecutableDialog *m_dialog;
//};

//class DRunConfigurationFactory : public IRunConfigurationFactory
//{
//    Q_OBJECT

//public:
//    explicit DRunConfigurationFactory(QObject *parent = 0);

//    QList<Core::Id> availableCreationIds(Target *parent, CreationMode mode) const override;
//    QString displayNameForId(Core::Id id) const override;

//    bool canCreate(Target *parent, Core::Id id) const override;
//    bool canRestore(Target *parent, const QVariantMap &map) const override;
//    bool canClone(Target *parent, RunConfiguration *product) const override;
//    RunConfiguration *clone(Target *parent, RunConfiguration *source) override;

//private:
//    bool canHandle(Target *parent) const;

//    RunConfiguration *doCreate(Target *parent, Core::Id id) override;
//    RunConfiguration *doRestore(Target *parent, const QVariantMap &map) override;
//};

//class DRunConfigurationWidget : public QWidget
//{
//    Q_OBJECT

//public:
//    enum ApplyMode { InstantApply, DelayedApply};
//    DRunConfigurationWidget(DRunConfiguration *rc, ApplyMode mode);
//    ~DRunConfigurationWidget();

//    void apply(); // only used for DelayedApply
//    bool isValid() const;

//signals:
//    void validChanged();

//private:
//    void changed();
//    void executableEdited();
//    void workingDirectoryEdited();
//    void environmentWasChanged();

//    bool m_ignoreChange = false;
//    DRunConfiguration *m_runConfiguration = 0;
//    ProjectExplorer::ArgumentsAspect *m_temporaryArgumentsAspect = 0;
//    ProjectExplorer::TerminalAspect *m_temporaryTerminalAspect = 0;
//    Utils::PathChooser *m_executableChooser = 0;
//    Utils::PathChooser *m_workingDirectory = 0;
//    Utils::DetailsWidget *m_detailsContainer = 0;
//};

//} // namespace DProjectManager
//#endif // DRUNCONFIG_H
