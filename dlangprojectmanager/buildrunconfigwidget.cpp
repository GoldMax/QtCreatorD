#include "buildrunconfigwidget.h"
#include "buildrunconfig.h"

#include <projectexplorer/environmentaspect.h>
#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <utils/detailswidget.h>
#include <utils/pathchooser.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>


namespace DLangProjectManager {
namespace Internal {

BuildRunConfigWidget::BuildRunConfigWidget(BuildRunConfig *rc)
 : m_ignoreChange(false), m_runConfiguration(rc)
{
 QFormLayout *layout = new QFormLayout;
 layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
 layout->setMargin(0);

 m_executableChooser = new Utils::PathChooser(this);
 m_executableChooser->setExpectedKind(Utils::PathChooser::Command);
 layout->addRow(tr("Executable:"), m_executableChooser);

 m_commandLineArgumentsLineEdit = new QLineEdit(this);
 m_commandLineArgumentsLineEdit->setMinimumWidth(200); // this shouldn't be fixed here...
 layout->addRow(tr("Arguments:"), m_commandLineArgumentsLineEdit);

 m_workingDirectory = new Utils::PathChooser(this);
 m_workingDirectory->setExpectedKind(Utils::PathChooser::Directory);
 m_workingDirectory->setBaseDirectory(rc->target()->project()->projectDirectory());

 layout->addRow(tr("Working directory:"), m_workingDirectory);

 m_useTerminalCheck = new QCheckBox(tr("Run in &terminal"), this);
 layout->addRow(QString(), m_useTerminalCheck);

 QVBoxLayout *vbox = new QVBoxLayout(this);
 vbox->setMargin(0);

 m_detailsContainer = new Utils::DetailsWidget(this);
 m_detailsContainer->setState(Utils::DetailsWidget::NoSummary);
 vbox->addWidget(m_detailsContainer);

 QWidget *detailsWidget = new QWidget(m_detailsContainer);
 m_detailsContainer->setWidget(detailsWidget);
 detailsWidget->setLayout(layout);

 changed();

 connect(m_executableChooser, SIGNAL(changed(QString)),
         this, SLOT(executableEdited()));
 connect(m_commandLineArgumentsLineEdit, SIGNAL(textEdited(QString)),
         this, SLOT(argumentsEdited(QString)));
 connect(m_workingDirectory, SIGNAL(changed(QString)),
         this, SLOT(workingDirectoryEdited()));
 connect(m_useTerminalCheck, SIGNAL(toggled(bool)),
         this, SLOT(termToggled(bool)));

 ProjectExplorer::EnvironmentAspect *aspect = rc->extraAspect<ProjectExplorer::EnvironmentAspect>();
 if (aspect) {
  connect(aspect, SIGNAL(environmentChanged()), this, SLOT(environmentWasChanged()));
  environmentWasChanged();
 }

 connect(m_runConfiguration, SIGNAL(changed()), this, SLOT(changed()));
}

void BuildRunConfigWidget::environmentWasChanged()
{
 ProjectExplorer::EnvironmentAspect *aspect
   = m_runConfiguration->extraAspect<ProjectExplorer::EnvironmentAspect>();
 QTC_ASSERT(aspect, return);
 m_workingDirectory->setEnvironment(aspect->environment());
 m_executableChooser->setEnvironment(aspect->environment());
}

void BuildRunConfigWidget::executableEdited()
{
 m_ignoreChange = true;
 m_runConfiguration->setExecutable(m_executableChooser->rawPath());
 m_ignoreChange = false;
}
void BuildRunConfigWidget::argumentsEdited(const QString &arguments)
{
 m_ignoreChange = true;
 m_runConfiguration->setCommandLineArguments(arguments);
 m_ignoreChange = false;
}
void BuildRunConfigWidget::workingDirectoryEdited()
{
 m_ignoreChange = true;
 m_runConfiguration->setBaseWorkingDirectory(m_workingDirectory->rawPath());
 m_ignoreChange = false;
}

void BuildRunConfigWidget::termToggled(bool on)
{
 m_ignoreChange = true;
 m_runConfiguration->setRunMode(on ? ProjectExplorer::LocalApplicationRunConfiguration::Console
                                   : ProjectExplorer::LocalApplicationRunConfiguration::Gui);
 m_ignoreChange = false;
}

void BuildRunConfigWidget::changed()
{
 // We triggered the change, don't update us
 if (m_ignoreChange)
  return;

 m_executableChooser->setPath(m_runConfiguration->rawExecutable());
 m_commandLineArgumentsLineEdit->setText(m_runConfiguration->rawCommandLineArguments());
 m_workingDirectory->setPath(m_runConfiguration->baseWorkingDirectory());
 m_useTerminalCheck->setChecked(m_runConfiguration->runMode()
                                == ProjectExplorer::LocalApplicationRunConfiguration::Console);
}

} // namespace Internal
} // namespace QtSupport
