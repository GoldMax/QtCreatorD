#include "drunconfiguration.h"
#include "dmakestep.h"
#include "dprojectnodes.h"
#include "dprojectmanagerconstants.h"

#include <qtsupport/qtkitinformation.h>

#include <projectexplorer/buildconfiguration.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/environmentaspect.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/abi.h>

#include <coreplugin/icore.h>

#include <utils/qtcprocess.h>
#include <utils/stringutils.h>
#include <utils/detailswidget.h>
#include <utils/pathchooser.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QDir>
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>

using namespace DProjectManager;

//-----------------------------------------------------------------------------
//--- CustomExecutableDialog
//-----------------------------------------------------------------------------
// Dialog embedding the DRunConfigurationWidget
// prompting the user to complete the configuration.
class CustomExecutableDialog : public QDialog
{
	Q_OBJECT
public:
	explicit CustomExecutableDialog(DRunConfiguration *rc, QWidget *parent = 0);

private slots:
	void changed()
	{
		setOkButtonEnabled(m_runConfiguration->isConfigured());
	}

private:
	inline void setOkButtonEnabled(bool e)
	{
		m_dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(e);
	}

	QDialogButtonBox *m_dialogButtonBox;
	DRunConfiguration *m_runConfiguration;
};

CustomExecutableDialog::CustomExecutableDialog(DRunConfiguration *rc, QWidget *parent)
	: QDialog(parent)
	, m_dialogButtonBox(new  QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel))
	, m_runConfiguration(rc)
{
	connect(rc, SIGNAL(changed()), this, SLOT(changed()));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	QVBoxLayout *layout = new QVBoxLayout(this);
	QLabel *label = new QLabel(tr("Could not find the executable, please specify one."));
	label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	layout->addWidget(label);
	QWidget *configWidget = rc->createConfigurationWidget();
	configWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	layout->addWidget(configWidget);
	setOkButtonEnabled(false);
	connect(m_dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
	layout->addWidget(m_dialogButtonBox);
	layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
}

//-----------------------------------------------------------------------------
//--- DRunConfiguration
//-----------------------------------------------------------------------------
void DRunConfiguration::ctor()
{
	setDefaultDisplayName(defaultDisplayName());
	addExtraAspects();
	if (!extraAspect<ProjectExplorer::EnvironmentAspect>())
		addExtraAspect(new ProjectExplorer::LocalEnvironmentAspect(this));
}

DRunConfiguration::DRunConfiguration(ProjectExplorer::Target *parent) :
		ProjectExplorer::LocalApplicationRunConfiguration(parent, Core::Id(Constants::BUILDRUN_CONFIG_ID)),
		m_workingDirectory(QLatin1String(ProjectExplorer::Constants::DEFAULT_WORKING_DIR)),
		m_runMode(Gui)
{
	if (!parent->activeBuildConfiguration())
		m_workingDirectory = QLatin1String(ProjectExplorer::Constants::DEFAULT_WORKING_DIR_ALTERNATE);
	ctor();
}

DRunConfiguration::DRunConfiguration(ProjectExplorer::Target *parent, DRunConfiguration *source) :
		ProjectExplorer::LocalApplicationRunConfiguration(parent, source),
		m_executable(source->m_executable),
		m_workingDirectory(source->m_workingDirectory),
		m_cmdArguments(source->m_cmdArguments),
		m_runMode(source->m_runMode)
{
	ctor();
}

// Note: Qt4Project deletes all empty customexecrunconfigs for which isConfigured() == false.
DRunConfiguration::~DRunConfiguration()
{
}

bool DRunConfiguration::ensureConfigured(QString *errorMessage)
{
	if (isConfigured())
		return validateExecutable(0, errorMessage);
	CustomExecutableDialog dialog(this, Core::ICore::mainWindow());
	dialog.setWindowTitle(displayName());
	const QString oldExecutable = m_executable;
	const QString oldWorkingDirectory = m_workingDirectory;
	const QString oldCmdArguments = m_cmdArguments;
	if (dialog.exec() == QDialog::Accepted)
		return validateExecutable(0, errorMessage);
	// User canceled: Hack: Silence the error dialog.
	if (errorMessage)
		*errorMessage = QLatin1String("");
	// Restore values changed by the configuration widget.
	if (m_executable != oldExecutable
					|| m_workingDirectory != oldWorkingDirectory
					|| m_cmdArguments != oldCmdArguments) {
		m_executable = oldExecutable;
		m_workingDirectory = oldWorkingDirectory;
		m_cmdArguments = oldCmdArguments;
		emit changed();
	}
	return false;
}

// Search the executable in the path.
bool DRunConfiguration::validateExecutable(QString *executable, QString *errorMessage) const
{
	if (executable)
		executable->clear();
	if (m_executable.isEmpty()) {
		if (errorMessage)
			*errorMessage = tr("No executable.");
		return false;
	}
	Utils::Environment env;
	ProjectExplorer::EnvironmentAspect *aspect = extraAspect<ProjectExplorer::EnvironmentAspect>();
	if (aspect)
		env = aspect->environment();
	const QString exec = env.searchInPath(Utils::expandMacros(m_executable, macroExpander()),
																																							QStringList(workingDirectory()));
	if (exec.isEmpty()) {
		if (errorMessage)
			*errorMessage = tr("The executable\n%1\ncannot be found in the path.").
				arg(QDir::toNativeSeparators(m_executable));
		return false;
	}
	if (executable)
		*executable = QDir::cleanPath(exec);
	return true;
}

QString DRunConfiguration::executable() const
{
	QString result;
	validateExecutable(&result);
	return result;
}

QString DRunConfiguration::rawExecutable() const
{
	return m_executable;
}

bool DRunConfiguration::isConfigured() const
{
	return !m_executable.isEmpty();
}

ProjectExplorer::LocalApplicationRunConfiguration::RunMode DRunConfiguration::runMode() const
{
	return m_runMode;
}

QString DRunConfiguration::workingDirectory() const
{
	ProjectExplorer::EnvironmentAspect *aspect = extraAspect<ProjectExplorer::EnvironmentAspect>();
	QTC_ASSERT(aspect, return baseWorkingDirectory());
	return QDir::cleanPath(aspect->environment().expandVariables(
																									Utils::expandMacros(baseWorkingDirectory(), macroExpander())));
}

QString DRunConfiguration::baseWorkingDirectory() const
{
	return m_workingDirectory;
}

QString DRunConfiguration::commandLineArguments() const
{
	return Utils::QtcProcess::expandMacros(m_cmdArguments, macroExpander());
}

QString DRunConfiguration::rawCommandLineArguments() const
{
	return m_cmdArguments;
}

QString DRunConfiguration::defaultDisplayName() const
{
	return tr("Build Run");
}

QVariantMap DRunConfiguration::toMap() const
{
	QVariantMap map(LocalApplicationRunConfiguration::toMap());
	map.insert(QLatin1String(Constants::ARGUMENTS_KEY), m_cmdArguments);
	map.insert(QLatin1String(Constants::USE_TERMINAL_KEY), m_runMode == Console);
	return map;
}

bool DRunConfiguration::fromMap(const QVariantMap &map)
{
	m_cmdArguments = map.value(QLatin1String(Constants::ARGUMENTS_KEY)).toString();
	m_runMode = map.value(QLatin1String(Constants::USE_TERMINAL_KEY)).toBool() ? Console : Gui;

	setDefaultDisplayName(defaultDisplayName());
	return LocalApplicationRunConfiguration::fromMap(map);
}

void DRunConfiguration::setExecutable(const QString &executable)
{
	if (executable == m_executable)
		return;
	m_executable = executable;
	setDefaultDisplayName(defaultDisplayName());
	emit changed();
}

void DRunConfiguration::setCommandLineArguments(const QString &commandLineArguments)
{
	m_cmdArguments = commandLineArguments;
	emit changed();
}

void DRunConfiguration::setBaseWorkingDirectory(const QString &workingDirectory)
{
	m_workingDirectory = workingDirectory;
	emit changed();
}

void DRunConfiguration::setRunMode(ProjectExplorer::LocalApplicationRunConfiguration::RunMode runMode)
{
	m_runMode = runMode;
	emit changed();
}

QWidget *DRunConfiguration::createConfigurationWidget()
{
	return new DRunConfigurationWidget(this);
}

ProjectExplorer::Abi DRunConfiguration::abi() const
{
	return ProjectExplorer::Abi(); // return an invalid ABI: We do not know what we will end up running!
}

void DRunConfiguration::updateConfig(const DMakeStep* makeStep)
{
	QString outDir = makeStep->targetDirName();
	if(outDir.startsWith(QDir::separator()) == false)
	{
		QString projDir = makeStep->target()->project()->projectDirectory();
		outDir = projDir + QDir::separator() + outDir;
	}
	setBaseWorkingDirectory(outDir);
	outDir.append(QDir::separator() + makeStep->outFileName());
	setExecutable(outDir);
}
//---------------------------------------------------------------------------------
//--- DRunConfigurationFactory
//---------------------------------------------------------------------------------

DRunConfigurationFactory* DRunConfigurationFactory::m_instance = 0;

DRunConfigurationFactory::DRunConfigurationFactory(QObject *parent) :
		ProjectExplorer::IRunConfigurationFactory(parent)
{
	setObjectName(QLatin1String("DRunConfigurationFactory"));
	m_instance = this;
}

DRunConfigurationFactory::~DRunConfigurationFactory() { }

bool DRunConfigurationFactory::canCreate(ProjectExplorer::Target *parent,
																																									const Core::Id id) const
{
	if (!canHandle(parent))
		return false;
	return id == Constants::BUILDRUN_CONFIG_ID;
}

ProjectExplorer::RunConfiguration *
DRunConfigurationFactory::doCreate(ProjectExplorer::Target *parent, const Core::Id id)
{
	using namespace ProjectExplorer;
	Q_UNUSED(id);
	DRunConfiguration* run = new DRunConfiguration(parent);
	BuildConfiguration* build = parent->activeBuildConfiguration();
	if(build)
	{
		BuildStepList* bsl = build->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
		Q_ASSERT(bsl);
		DMakeStep *makeStep = new DMakeStep(bsl);
		if(makeStep)
		{
			run->updateConfig(makeStep);
			return run;
		}
	}
	return run;
}

bool DRunConfigurationFactory::canRestore(ProjectExplorer::Target *parent,
																																										const QVariantMap &map) const
{
	if (!canHandle(parent))
		return false;
	Core::Id id(ProjectExplorer::idFromMap(map));
	return canCreate(parent, id);
}

ProjectExplorer::RunConfiguration *
DRunConfigurationFactory::doRestore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
	Q_UNUSED(map);
	return new DRunConfiguration(parent);
}

bool DRunConfigurationFactory::canClone(ProjectExplorer::Target *parent,
																																								ProjectExplorer::RunConfiguration *source) const
{
	return canCreate(parent, source->id());
}

ProjectExplorer::RunConfiguration *
DRunConfigurationFactory::clone(ProjectExplorer::Target *parent,
																																ProjectExplorer::RunConfiguration *source)
{
	if (!canClone(parent, source))
		return 0;
	return new DRunConfiguration(parent, static_cast<DRunConfiguration*>(source));
}

bool DRunConfigurationFactory::canHandle(ProjectExplorer::Target *parent) const
{
	return parent->project()->supportsKit(parent->kit());
}

QList<Core::Id> DRunConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent) const
{
	if (!canHandle(parent))
		return QList<Core::Id>();
	return QList<Core::Id>() << Core::Id(Constants::BUILDRUN_CONFIG_ID);
}

QString DRunConfigurationFactory::displayNameForId(const Core::Id id) const
{
	if (id == Constants::BUILDRUN_CONFIG_ID)
		return tr("Build Run");
	return QString();
}

//-------------------------------------------------------------------------------
//-- DRunConfigurationWidget
//-------------------------------------------------------------------------------
DRunConfigurationWidget::DRunConfigurationWidget(DRunConfiguration *rc)
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

void DRunConfigurationWidget::environmentWasChanged()
{
	ProjectExplorer::EnvironmentAspect *aspect
			= m_runConfiguration->extraAspect<ProjectExplorer::EnvironmentAspect>();
	QTC_ASSERT(aspect, return);
	m_workingDirectory->setEnvironment(aspect->environment());
	m_executableChooser->setEnvironment(aspect->environment());
}

void DRunConfigurationWidget::executableEdited()
{
	m_ignoreChange = true;
	m_runConfiguration->setExecutable(m_executableChooser->rawPath());
	m_ignoreChange = false;
}
void DRunConfigurationWidget::argumentsEdited(const QString &arguments)
{
	m_ignoreChange = true;
	m_runConfiguration->setCommandLineArguments(arguments);
	m_ignoreChange = false;
}
void DRunConfigurationWidget::workingDirectoryEdited()
{
	m_ignoreChange = true;
	m_runConfiguration->setBaseWorkingDirectory(m_workingDirectory->rawPath());
	m_ignoreChange = false;
}

void DRunConfigurationWidget::termToggled(bool on)
{
	m_ignoreChange = true;
	m_runConfiguration->setRunMode(on ? ProjectExplorer::LocalApplicationRunConfiguration::Console
																																			: ProjectExplorer::LocalApplicationRunConfiguration::Gui);
	m_ignoreChange = false;
}

void DRunConfigurationWidget::changed()
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

#include "drunconfiguration.moc"

