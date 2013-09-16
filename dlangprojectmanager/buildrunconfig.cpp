#include "buildrunconfig.h"

#include <qtsupport/qtkitinformation.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/environmentaspect.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/abi.h>

#include <coreplugin/icore.h>

#include <utils/qtcprocess.h>
#include <utils/stringutils.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include <QDir>

using namespace DLangProjectManager;
using namespace DLangProjectManager::Internal;

namespace
{
 const char CUSTOM_EXECUTABLE_ID[] = "ProjectExplorer.BuildRunConfig";
 const char EXECUTABLE_KEY[] = "ProjectExplorer.BuildRunConfig.Executable";
 const char ARGUMENTS_KEY[] = "ProjectExplorer.BuildRunConfig.Arguments";
 const char WORKING_DIRECTORY_KEY[] = "ProjectExplorer.BuildRunConfig.WorkingDirectory";
 const char USE_TERMINAL_KEY[] = "ProjectExplorer.BuildRunConfig.UseTerminal";
}

void BuildRunConfig::ctor()
{
    setDefaultDisplayName(defaultDisplayName());
}

BuildRunConfig::BuildRunConfig(ProjectExplorer::Target *parent) :
    ProjectExplorer::LocalApplicationRunConfiguration(parent, Core::Id(CUSTOM_EXECUTABLE_ID)),
    m_workingDirectory(QLatin1String(ProjectExplorer::Constants::DEFAULT_WORKING_DIR)),
    m_runMode(Gui)
{
    if (!parent->activeBuildConfiguration())
        m_workingDirectory = QLatin1String(ProjectExplorer::Constants::DEFAULT_WORKING_DIR_ALTERNATE);
    ctor();
}

BuildRunConfig::BuildRunConfig(ProjectExplorer::Target *parent,
                                                                   BuildRunConfig *source) :
    ProjectExplorer::LocalApplicationRunConfiguration(parent, source),
    m_executable(source->m_executable),
    m_workingDirectory(source->m_workingDirectory),
    m_cmdArguments(source->m_cmdArguments),
    m_runMode(source->m_runMode)
{
    ctor();
}

// Note: Qt4Project deletes all empty customexecrunconfigs for which isConfigured() == false.
BuildRunConfig::~BuildRunConfig()
{
}

// Dialog embedding the BuildRunConfigWidget
// prompting the user to complete the configuration.
class CustomExecutableDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CustomExecutableDialog(BuildRunConfig *rc, QWidget *parent = 0);

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
    BuildRunConfig *m_runConfiguration;
};

CustomExecutableDialog::CustomExecutableDialog(BuildRunConfig *rc, QWidget *parent)
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

bool BuildRunConfig::ensureConfigured(QString *errorMessage)
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
bool BuildRunConfig::validateExecutable(QString *executable, QString *errorMessage) const
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

QString BuildRunConfig::executable() const
{
    QString result;
    validateExecutable(&result);
    return result;
}

QString BuildRunConfig::rawExecutable() const
{
    return m_executable;
}

bool BuildRunConfig::isConfigured() const
{
    return !m_executable.isEmpty();
}

ProjectExplorer::LocalApplicationRunConfiguration::RunMode BuildRunConfig::runMode() const
{
    return m_runMode;
}

QString BuildRunConfig::workingDirectory() const
{
    ProjectExplorer::EnvironmentAspect *aspect = extraAspect<ProjectExplorer::EnvironmentAspect>();
    QTC_ASSERT(aspect, return baseWorkingDirectory());
    return QDir::cleanPath(aspect->environment().expandVariables(
                Utils::expandMacros(baseWorkingDirectory(), macroExpander())));
}

QString BuildRunConfig::baseWorkingDirectory() const
{
    return m_workingDirectory;
}


QString BuildRunConfig::commandLineArguments() const
{
    return Utils::QtcProcess::expandMacros(m_cmdArguments, macroExpander());
}

QString BuildRunConfig::rawCommandLineArguments() const
{
    return m_cmdArguments;
}

QString BuildRunConfig::defaultDisplayName() const
{
    if (m_executable.isEmpty())
        return tr("Custom Executable");
    else
        return tr("Run %1").arg(QDir::toNativeSeparators(m_executable));
}

QVariantMap BuildRunConfig::toMap() const
{
    QVariantMap map(LocalApplicationRunConfiguration::toMap());
    map.insert(QLatin1String(EXECUTABLE_KEY), m_executable);
    map.insert(QLatin1String(ARGUMENTS_KEY), m_cmdArguments);
    map.insert(QLatin1String(WORKING_DIRECTORY_KEY), m_workingDirectory);
    map.insert(QLatin1String(USE_TERMINAL_KEY), m_runMode == Console);
    return map;
}

bool BuildRunConfig::fromMap(const QVariantMap &map)
{
    m_executable = map.value(QLatin1String(EXECUTABLE_KEY)).toString();
    m_cmdArguments = map.value(QLatin1String(ARGUMENTS_KEY)).toString();
    m_workingDirectory = map.value(QLatin1String(WORKING_DIRECTORY_KEY)).toString();
    m_runMode = map.value(QLatin1String(USE_TERMINAL_KEY)).toBool() ? Console : Gui;

    setDefaultDisplayName(defaultDisplayName());
    return LocalApplicationRunConfiguration::fromMap(map);
}

void BuildRunConfig::setExecutable(const QString &executable)
{
    if (executable == m_executable)
        return;
    m_executable = executable;
    setDefaultDisplayName(defaultDisplayName());
    emit changed();
}

void BuildRunConfig::setCommandLineArguments(const QString &commandLineArguments)
{
    m_cmdArguments = commandLineArguments;
    emit changed();
}

void BuildRunConfig::setBaseWorkingDirectory(const QString &workingDirectory)
{
    m_workingDirectory = workingDirectory;
    emit changed();
}

void BuildRunConfig::setRunMode(ProjectExplorer::LocalApplicationRunConfiguration::RunMode runMode)
{
    m_runMode = runMode;
    emit changed();
}

QWidget *BuildRunConfig::createConfigurationWidget()
{
    return new BuildRunConfigWidget(this);
}

QString BuildRunConfig::dumperLibrary() const
{
    return QtSupport::QtKitInformation::dumperLibrary(target()->kit());
}

QStringList BuildRunConfig::dumperLibraryLocations() const
{
    return QtSupport::QtKitInformation::dumperLibraryLocations(target()->kit());
}

ProjectExplorer::Abi BuildRunConfig::abi() const
{
    return ProjectExplorer::Abi(); // return an invalid ABI: We do not know what we will end up running!
}

// Factory

BuildRunConfigFactory::BuildRunConfigFactory(QObject *parent) :
    ProjectExplorer::IRunConfigurationFactory(parent)
{ setObjectName(QLatin1String("BuildRunConfigFactory")); }

BuildRunConfigFactory::~BuildRunConfigFactory()
{ }

bool BuildRunConfigFactory::canCreate(ProjectExplorer::Target *parent,
                                                        const Core::Id id) const
{
    if (!canHandle(parent))
        return false;
    return id == CUSTOM_EXECUTABLE_ID;
}

ProjectExplorer::RunConfiguration *
BuildRunConfigFactory::doCreate(ProjectExplorer::Target *parent, const Core::Id id)
{
    Q_UNUSED(id);
    return new BuildRunConfig(parent);
}

bool BuildRunConfigFactory::canRestore(ProjectExplorer::Target *parent,
                                                         const QVariantMap &map) const
{
    if (!canHandle(parent))
        return false;
    Core::Id id(ProjectExplorer::idFromMap(map));
    return canCreate(parent, id);
}

ProjectExplorer::RunConfiguration *
BuildRunConfigFactory::doRestore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    Q_UNUSED(map);
    return new BuildRunConfig(parent);
}

bool BuildRunConfigFactory::canClone(ProjectExplorer::Target *parent,
                                                       ProjectExplorer::RunConfiguration *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::RunConfiguration *
BuildRunConfigFactory::clone(ProjectExplorer::Target *parent,
                                               ProjectExplorer::RunConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;
    return new BuildRunConfig(parent, static_cast<BuildRunConfig*>(source));
}

bool BuildRunConfigFactory::canHandle(ProjectExplorer::Target *parent) const
{
    return parent->project()->supportsKit(parent->kit());
}

QList<Core::Id> BuildRunConfigFactory::availableCreationIds(ProjectExplorer::Target *parent) const
{
    if (!canHandle(parent))
        return QList<Core::Id>();
    return QList<Core::Id>() << Core::Id(CUSTOM_EXECUTABLE_ID);
}

QString BuildRunConfigFactory::displayNameForId(const Core::Id id) const
{
    if (id == CUSTOM_EXECUTABLE_ID)
        return tr("Custom Executable");
    return QString();
}

#include "buildrunconfig.moc"

