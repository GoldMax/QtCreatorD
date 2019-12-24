#ifndef DRUNCONFIG_H
#define DRUNCONFIG_H

#include "dmakestep.h"

#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/runcontrol.h>
//#include <projectexplorer/applicationlauncher.h>
//#include <projectexplorer/runnables.h>
#include <utils/detailswidget.h>

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
using namespace ProjectExplorer;

namespace DProjectManager {

//class DRunConfigurationWidget;
//class DRunConfigurationFactory;
//class DCustomExecutableDialog;

class DRunConfiguration : public ProjectExplorer::RunConfiguration
{
	Q_OBJECT
				// the configuration widget needs to setExecutable setWorkingDirectory and setCommandLineArguments
			//	friend class DRunConfigurationWidget;
				//friend class DRunConfigurationFactory;

public:
	DRunConfiguration(Target *parent, Core::Id id);
	//~DRunConfiguration() override;

	QString defaultDisplayName() const { return tr("Build Run"); }
	QString executable() const { return m_executable; }
	QString workingDirectory() const { return m_workingDirectory; }

	void updateConfig(const DMakeStep* makeStep);
	bool isConfigured() const override;
	void updateEnabledState() override;

//				Runnable runnable() const override;
//				ConfigurationState ensureConfigured(QString *errorMessage) override;

//				QVariantMap toMap() const override;


signals:
	void changed();

//protected:
//				DRunConfiguration(Target *parent,
//																																					DRunConfiguration *source);
//				virtual bool fromMap(const QVariantMap &map) override;
//				QString defaultDisplayName() const;

private:
//				void configurationDialogFinished();
//				void setExecutable(const QString &executable);
//				QString rawExecutable() const;
//				void setCommandLineArguments(const QString &commandLineArguments);
//				void setBaseWorkingDirectory(const QString &workingDirectory);
//				QString baseWorkingDirectory() const;
//				void setUserName(const QString &name);
//				//void setRunMode(ApplicationLauncher::Mode runMode);
//				bool validateExecutable(QString *executable = 0, QString *errorMessage = 0) const;

	QString m_executable;
	QString m_workingDirectory;
	//				DCustomExecutableDialog *m_dialog;
};

class DRunConfigurationFactory : public RunConfigurationFactory
{
public:
				explicit DRunConfigurationFactory();
//				QList<Core::Id> availableCreationIds(Target *parent, CreationMode mode) const override;
//				QString displayNameForId(Core::Id id) const override;

//				bool canCreate(Target *parent, Core::Id id) const override;
//				bool canRestore(Target *parent, const QVariantMap &map) const override;
//				bool canClone(Target *parent, RunConfiguration *product) const override;
//				RunConfiguration *clone(Target *parent, RunConfiguration *source) override;

//private:
//				bool canHandle(Target *parent) const;

//				RunConfiguration *doCreate(Target *parent, Core::Id id) override;
//				RunConfiguration *doRestore(Target *parent, const QVariantMap &map) override;
};

class DRunConfigurationWidget : public QWidget
{
	Q_OBJECT

public:
	DRunConfigurationWidget(DRunConfiguration *rc);
	~DRunConfigurationWidget();

	void apply(); // only used for DelayedApply
	bool isValid() const;

signals:
	void validChanged();

private:
	void changed();
	void executableEdited();
	void workingDirectoryEdited();
	void environmentWasChanged();

	bool m_ignoreChange = false;
	DRunConfiguration *m_runConfiguration = nullptr;
	ProjectExplorer::ArgumentsAspect *m_temporaryArgumentsAspect = nullptr;
	ProjectExplorer::TerminalAspect *m_temporaryTerminalAspect = nullptr;
	Utils::PathChooser *m_executableChooser = nullptr;
	Utils::PathChooser *m_workingDirectory = nullptr;
	Utils::DetailsWidget *m_detailsContainer = nullptr;
};

} // namespace DProjectManager
#endif // DRUNCONFIG_H
