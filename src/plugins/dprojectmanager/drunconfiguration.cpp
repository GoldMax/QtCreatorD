#include "drunconfiguration.h"
#include "dprojectmanagerconstants.h"
#include "dmakestep.h"
//#include "dprojectnodes.h"

//#include <qtsupport/qtkitinformation.h>

#include <projectexplorer/runconfigurationaspects.h>
//#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/runconfiguration.h>
//#include <projectexplorer/runconfigurationaspects.h>

#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <projectexplorer/buildconfiguration.h>
//#include <projectexplorer/buildstep.h>
#include <projectexplorer/buildsteplist.h>
//#include <projectexplorer/environmentaspect.h>
//#include <projectexplorer/localenvironmentaspect.h>
//#include <projectexplorer/abi.h>
//#include <projectexplorer/devicesupport/devicemanager.h>
//#include <projectexplorer/runcontrol.h>
//#include <projectexplorer/applicationlauncher.h>

//#include <coreplugin/icore.h>
//#include <coreplugin/variablechooser.h>

//#include <utils/qtcassert.h>
//#include <utils/qtcprocess.h>
//#include <utils/stringutils.h>
//#include <utils/detailswidget.h>
//#include <utils/pathchooser.h>

//#include <QDialog>
//#include <QDialogButtonBox>
//#include <QDir>
//#include <QKeyEvent>
//#include <QLabel>
//#include <QPushButton>
//#include <QVBoxLayout>
//#include <QCheckBox>
//#include <QComboBox>
//#include <QFormLayout>
//#include <QHBoxLayout>
//#include <QLabel>
//#include <QLineEdit>

using namespace Utils;
using namespace ProjectExplorer;

namespace DProjectManager
{

/*//-----------------------------------------------------------------------------
//--- DCustomExecutableDialog
//-----------------------------------------------------------------------------
// Dialog embedding the DRunConfigurationWidget
// prompting the user to complete the configuration.
class DCustomExecutableDialog : public QDialog
{
	Q_OBJECT
public:
	DCustomExecutableDialog(DRunConfiguration *rc, QWidget *parent)
		: QDialog(parent)
		, m_dialogButtonBox(new  QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel))
	{
		setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
		QVBoxLayout *layout = new QVBoxLayout(this);
		QLabel *label = new QLabel(tr("Could not find the executable, please specify one."));
		label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		layout->addWidget(label);
		m_widget = new DRunConfigurationWidget(rc);
		m_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		connect(m_widget, &DRunConfigurationWidget::validChanged,
										this, &DCustomExecutableDialog::changed);
		layout->addWidget(m_widget);
		setOkButtonEnabled(false);
		connect(m_dialogButtonBox, &QDialogButtonBox::accepted, this, &DCustomExecutableDialog::accept);
		connect(m_dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
		layout->addWidget(m_dialogButtonBox);
		layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
	}

	void accept()
	{
		m_widget->apply();
		QDialog::accept();
	}
	bool event(QEvent *event)
	{
		if (event->type() == QEvent::ShortcutOverride)
		{
			QKeyEvent *ke = static_cast<QKeyEvent *>(event);
			if (ke->key() == Qt::Key_Escape && !ke->modifiers())
			{
				ke->accept();
				return true;
			}
		}
		return QDialog::event(event);
	}

private:
	void changed()
	{
		setOkButtonEnabled(m_widget->isValid());
	}
	inline void setOkButtonEnabled(bool e)
	{
		m_dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(e);
	}

	QDialogButtonBox *m_dialogButtonBox;
	DRunConfigurationWidget *m_widget;
};*/
//-----------------------------------------------------------------------------
//--- DRunConfiguration
//-----------------------------------------------------------------------------
DRunConfiguration::DRunConfiguration(Target *target, Core::Id /*id*/) :
		CustomExecutableRunConfiguration(target, Core::Id(Constants::BUILDRUN_CONFIG_ID))/*,
		m_dialog(nullptr)*/
{
	setDefaultDisplayName(defaultDisplayName());
//	/*auto execAspect = */addAspect<ExecutableAspect>();
//	addAspect<LocalEnvironmentAspect>(target);
//	auto argumentsAspect = addAspect<ArgumentsAspect>();
//	addAspect<WorkingDirectoryAspect>();
//	addAspect<TerminalAspect>();

//	if (target->activeBuildConfiguration())
//		m_workingDirectory = ProjectExplorer::Constants::DEFAULT_WORKING_DIR;
//	else
//		m_workingDirectory = ProjectExplorer::Constants::DEFAULT_WORKING_DIR_ALTERNATE;

	setCommandLineGetter([this/*, execAspect, argumentsAspect*/]
	{
		auto exec = this->aspect<ExecutableAspect>()->executable();
		CommandLine cmd = CommandLine(exec/*execAspect->executable()*/);
		auto argumentsAspect = this->aspect<ArgumentsAspect>();
		cmd.addArgs(argumentsAspect->arguments(macroExpander()), CommandLine::Raw);
		return cmd;
	});

//	//setConfigWidgetCreator([this] { return new DRunConfigurationWidget(this); });
}

/*DRunConfiguration::~DRunConfiguration()
{
// Note: Qt4Project deletes all empty customexecrunconfigs for which isConfigured() == false.
	if (m_dialog)
	{
		emit configurationFinished();
		disconnect(m_dialog, &QDialog::finished,
													this, &DRunConfiguration::configurationDialogFinished);
		delete m_dialog;
	}
}*/

void DRunConfiguration::updateConfig(const DMakeStep* makeStep)
{
	if(makeStep == nullptr)
	{
		BuildConfiguration* build = this->activeBuildConfiguration();
		if(!build)
			return;
		BuildStepList* bsl = build->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
		Q_ASSERT(bsl);
		auto makeStep = bsl->firstOfType<DMakeStep>();
		Q_ASSERT(makeStep);
	}

	Utils::FilePath outDir;
	if(FileUtils::isRelativePath(makeStep->targetDirName()))
	{
		outDir = makeStep->target()->project()->projectDirectory();
		outDir = outDir.pathAppended(makeStep->targetDirName());
	}
	else
		outDir = Utils::FilePath::fromString(makeStep->targetDirName());
	//m_workingDirectory = outDir.toString();
	this->aspect<WorkingDirectoryAspect>()->setDefaultWorkingDirectory(outDir);
	outDir = outDir.pathAppended(makeStep->outFileName());
	//m_executable = outDir.toString();
	//auto exec = this->aspect<ExecutableAspect>()->executable();
	this->aspect<ExecutableAspect>()->setExecutable(outDir);
}

bool DRunConfiguration::isConfigured() const
{
	//return !m_executable.isEmpty();
	//return this->aspect<ExecutableAspect>()->executable().isEmpty() == false;
	return true;
}

void DRunConfiguration::updateEnabledState()
{
	setEnabled(isConfigured());
}



//RunConfiguration::ConfigurationState DRunConfiguration::ensureConfigured(QString *errorMessage)
//{
//	Q_UNUSED(errorMessage)
//	if (m_dialog)
//	{
//		// uhm already shown
//		*errorMessage = QLatin1String(""); // no error dialog
//		m_dialog->activateWindow();
//		m_dialog->raise();
//		return UnConfigured;
//	}

//	m_dialog = new DCustomExecutableDialog(this, Core::ICore::mainWindow());
//	connect(m_dialog, &QDialog::finished,
//									this, &DRunConfiguration::configurationDialogFinished);
//	m_dialog->setWindowTitle(displayName()); // pretty pointless
//	m_dialog->show();
//	return Waiting;
//}

//void DRunConfiguration::configurationDialogFinished()
//{
//	disconnect(m_dialog, &QDialog::finished,
//												this, &DRunConfiguration::configurationDialogFinished);
//	m_dialog->deleteLater();
//	m_dialog = nullptr;
//	emit configurationFinished();
//}

//// Search the executable in the path.
//bool DRunConfiguration::validateExecutable(QString *executable, QString *errorMessage) const
//{
//	if (executable)
//		executable->clear();
//	if (m_executable.isEmpty())
//	{
//		if (errorMessage)
//			*errorMessage = tr("No executable.");
//		return false;
//	}
//	//	Utils::Environment env;
//	//	EnvironmentAspect *aspect = extraAspect<EnvironmentAspect>();
//	//	if (aspect)
//	//		env = aspect->environment();
//	//	const Utils::FileName exec = env.searchInPath(macroExpander()->expand(m_executable),
//	//																																															QStringList(workingDirectory()));
//	//	if (exec.isEmpty()) {
//	//		if (errorMessage)
//	//			*errorMessage = tr("The executable\n%1\ncannot be found in the path.").
//	//																			arg(QDir::toNativeSeparators(m_executable));
//	//		return false;
//	//	}
//	//	if (executable)
//	//		*executable = exec.toString();
//	return true;
//}

//QString DRunConfiguration::executable() const
//{
//	QString result;
//	validateExecutable(&result);
//	return result;
//}

//QString DRunConfiguration::rawExecutable() const
//{
//	return m_executable;
//}

//QString DRunConfiguration::workingDirectory() const
//{
//	return QString("");
////	EnvironmentAspect *aspect = extraAspect<EnvironmentAspect>();
////	QTC_ASSERT(aspect, return baseWorkingDirectory());
////	return QDir::cleanPath(aspect->environment().expandVariables(
////																									macroExpander()->expand(baseWorkingDirectory())));
//}

//Runnable DRunConfiguration::runnable() const
//{
//	Runnable r;

////	StandardRunnable r;
////	r.executable = executable();
////	r.commandLineArguments = extraAspect<ArgumentsAspect>()->arguments();
////	r.workingDirectory = workingDirectory();
////	r.environment = extraAspect<LocalEnvironmentAspect>()->environment();
////	r.runMode = extraAspect<TerminalAspect>()->runMode();
////	r.device = DeviceManager::instance()->defaultDevice(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);
//	return r;
//}

//QString DRunConfiguration::baseWorkingDirectory() const
//{
//	return m_workingDirectory;
//}

//QVariantMap DRunConfiguration::toMap() const
//{
//	QVariantMap map(RunConfiguration::toMap());
//	//map.insert(QLatin1String(Constants::EXECUTABLE_KEY), m_executable);
//	map.insert(QLatin1String(Constants::WORKING_DIRECTORY_KEY), m_workingDirectory);
//	//    map.insert(QLatin1String(Constants::ARGUMENTS_KEY), m_cmdArguments);
//	//    map.insert(QLatin1String(Constants::USE_TERMINAL_KEY),
//	//               m_runMode == ApplicationLauncher::Mode::Console);

//	return map;
//}

//bool DRunConfiguration::fromMap(const QVariantMap &map)
//{
//	// m_cmdArguments = map.value(QLatin1String(Constants::ARGUMENTS_KEY)).toString();
//	// m_runMode = map.value(QLatin1String(Constants::USE_TERMINAL_KEY)).toBool() ?
//	//              ApplicationLauncher::Mode::Console : ApplicationLauncher::Mode::Gui;

//	//m_executable = map.value(QLatin1String(Constants::EXECUTABLE_KEY)).toString();
//	m_workingDirectory = map.value(QLatin1String(Constants::WORKING_DIRECTORY_KEY)).toString();

//	setDefaultDisplayName(defaultDisplayName());
//	return RunConfiguration::fromMap(map);
//}

//void DRunConfiguration::setExecutable(const QString &executable)
//{
//	if (executable == m_executable)
//		return;
//	m_executable = executable;
//	setDefaultDisplayName(defaultDisplayName());
//	emit changed();
//}

//void DRunConfiguration::setCommandLineArguments(const QString &commandLineArguments)
//{
////	extraAspect<ArgumentsAspect>()->setArguments(commandLineArguments);
////	emit changed();
//}

//void DRunConfiguration::setBaseWorkingDirectory(const QString &workingDirectory)
//{
//	m_workingDirectory = workingDirectory;
//	emit changed();
//}

////void DRunConfiguration::setRunMode(ApplicationLauncher::Mode runMode)
////{
//////	extraAspect<TerminalAspect>()->setRunMode(runMode);
//////	emit changed();
////}


//---------------------------------------------------------------------------------
//--- DRunConfigurationFactory
//---------------------------------------------------------------------------------

DRunConfigurationFactory::DRunConfigurationFactory() :
		FixedRunConfigurationFactory(QLatin1String("Build Run"))
{
	registerRunConfiguration<DRunConfiguration>(Constants::BUILDRUN_CONFIG_ID);
	addSupportedProjectType(Constants::DPROJECT_ID);
}

//bool DRunConfigurationFactory::canCreate(Target *parent, Core::Id id) const
//{
//	if (!canHandle(parent))
//		return false;
//	return id == Constants::BUILDRUN_CONFIG_ID;
//}

//RunConfiguration *
//DRunConfigurationFactory::doCreate(Target *parent, Core::Id id)
//{
//	Q_UNUSED(id);
//	DRunConfiguration* run = new DRunConfiguration(parent);
//	Q_ASSERT(run);
//	run->updateConfig();
//	return run;
//}

//bool DRunConfigurationFactory::canRestore(Target *parent,
//																																										const QVariantMap &map) const
//{
//	if (!canHandle(parent))
//		return false;
//	Core::Id id(idFromMap(map));
//	return canCreate(parent, id);
//}

//RunConfiguration *
//DRunConfigurationFactory::doRestore(Target *parent, const QVariantMap &map)
//{
//	Q_UNUSED(map)
//	return new DRunConfiguration(parent);
//}

//bool DRunConfigurationFactory::canClone(Target *parent,
//																																								RunConfiguration *source) const
//{
//	return canCreate(parent, source->id());
//}

//RunConfiguration *
//DRunConfigurationFactory::clone(Target *parent, RunConfiguration *source)
//{
//	if (!canClone(parent, source))
//		return nullptr;
//	return new DRunConfiguration(parent, static_cast<DRunConfiguration*>(source));
//}

//bool DRunConfigurationFactory::canHandle(Target *parent) const
//{
//	return parent->project()->supportsKit(parent->kit());
//}

//QList<Core::Id> DRunConfigurationFactory::availableCreationIds(Target *parent, CreationMode mode) const
//{
//	Q_UNUSED(mode)
//	if (!canHandle(parent))
//		return QList<Core::Id>();
//	return QList<Core::Id>() << Core::Id(Constants::BUILDRUN_CONFIG_ID);
//}

//QString DRunConfigurationFactory::displayNameForId(Core::Id id) const
//{
//	if(id == Constants::BUILDRUN_CONFIG_ID)
//		return tr("Build Run");
//	return QString();
//}

////-------------------------------------------------------------------------------
////-- DRunConfigurationWidget
////-------------------------------------------------------------------------------

//DRunConfigurationWidget::DRunConfigurationWidget(DRunConfiguration *rc)
//	: m_runConfiguration(rc)
//{
//	auto layout = new QFormLayout;
//	layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
//	layout->setMargin(0);

//	m_executableChooser = new PathChooser(this);
//	m_executableChooser->setHistoryCompleter(QLatin1String("Qt.CustomExecutable.History"));
//	m_executableChooser->setExpectedKind(PathChooser::ExistingCommand);
//	layout->addRow(tr("Executable:"), m_executableChooser);

//	auto argumentsAspect = rc->aspect<ArgumentsAspect>();
//	argumentsAspect->addToConfigurationLayout(layout);

//	m_workingDirectory = new PathChooser(this);
//	m_workingDirectory->setHistoryCompleter(QLatin1String("Qt.WorkingDir.History"));
//	m_workingDirectory->setExpectedKind(PathChooser::Directory);
//	m_workingDirectory->setBaseFileName(rc->target()->project()->projectDirectory());
//	layout->addRow(tr("Working directory:"), m_workingDirectory);

//	auto terminalAspect = rc->aspect<TerminalAspect>();
//	terminalAspect->addToConfigurationLayout(layout);

////	auto vbox = new QVBoxLayout(this);
////	vbox->setMargin(0);

////	m_detailsContainer = new DetailsWidget(this);
////	m_detailsContainer->setState(DetailsWidget::NoSummary);
////	vbox->addWidget(m_detailsContainer);

////	auto detailsWidget = new QWidget(m_detailsContainer);
////	m_detailsContainer->setWidget(detailsWidget);
////	detailsWidget->setLayout(layout);

//	changed();

//	connect(m_executableChooser, &PathChooser::rawPathChanged,
//									this, &DRunConfigurationWidget::executableEdited);
//	connect(m_workingDirectory, &PathChooser::rawPathChanged,
//									this, &DRunConfigurationWidget::workingDirectoryEdited);

//	auto enviromentAspect = rc->aspect<EnvironmentAspect>();
//	connect(enviromentAspect, &EnvironmentAspect::environmentChanged,
//									this, &DRunConfigurationWidget::environmentWasChanged);
//										environmentWasChanged();


//	connect(m_runConfiguration, &DRunConfiguration::changed,
//									this, &DRunConfigurationWidget::changed);

////	Core::VariableChooser::addSupportForChildWidgets(this, m_runConfiguration->macroExpander());
//}

//DRunConfigurationWidget::~DRunConfigurationWidget()
//{
////	delete m_temporaryArgumentsAspect;
////	delete m_temporaryTerminalAspect;
//}

//void DRunConfigurationWidget::environmentWasChanged()
//{
////	auto aspect = m_runConfiguration->extraAspect<EnvironmentAspect>();
////	QTC_ASSERT(aspect, return);
////	m_workingDirectory->setEnvironment(aspect->environment());
////	m_executableChooser->setEnvironment(aspect->environment());
//}

//void DRunConfigurationWidget::executableEdited()
//{
////	m_ignoreChange = true;
////	m_runConfiguration->setExecutable(m_executableChooser->rawPath());
////	m_ignoreChange = false;
//}

//void DRunConfigurationWidget::workingDirectoryEdited()
//{
////	m_ignoreChange = true;
////	m_runConfiguration->setBaseWorkingDirectory(m_workingDirectory->rawPath());
////	m_ignoreChange = false;
//}

//void DRunConfigurationWidget::changed()
//{
//	// We triggered the change, don't update us
////	if (m_ignoreChange)
////		return;

////	m_executableChooser->setPath(m_runConfiguration->rawExecutable());
////	m_workingDirectory->setPath(m_runConfiguration->baseWorkingDirectory());
//}

//void DRunConfigurationWidget::apply()
//{
////	m_ignoreChange = true;
////	m_runConfiguration->setExecutable(m_executableChooser->rawPath());
////	m_runConfiguration->setCommandLineArguments(m_temporaryArgumentsAspect->unexpandedArguments());
////	m_runConfiguration->setBaseWorkingDirectory(m_workingDirectory->rawPath());
////	m_runConfiguration->setRunMode(m_temporaryTerminalAspect->runMode());
////	m_ignoreChange = false;
//}

//bool DRunConfigurationWidget::isValid() const
//{
//	return true;
//	//return !m_executableChooser->rawPath().isEmpty();
//}


} // namespace DProjectManager

//#include "drunconfiguration.moc"

