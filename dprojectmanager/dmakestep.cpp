#include "dmakestep.h"
#include "dprojectmanagerconstants.h"
#include "dproject.h"
#include "ui_dmakestep.h"
#include "dbuildconfiguration.h"
#include "drunconfiguration.h"

#include <extensionsystem/pluginmanager.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/gnumakeparser.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/toolchain.h>
#include <qtsupport/qtkitinformation.h>
#include <qtsupport/qtparser.h>
#include <coreplugin/variablemanager.h>
#include <utils/stringutils.h>
#include <utils/qtcassert.h>
#include <utils/qtcprocess.h>

#include <QSettings>

using namespace Core;
using namespace ProjectExplorer;

namespace DProjectManager {
namespace Internal {

DMakeStep::DMakeStep(BuildStepList *parent) :
		AbstractProcessStep(parent, Id(Constants::D_MS_ID))
{
 ctor();
}

DMakeStep::DMakeStep(BuildStepList *parent, const Id id) :
  AbstractProcessStep(parent, id)
{
 ctor();
}

DMakeStep::DMakeStep(BuildStepList *parent, DMakeStep *bs) :
  AbstractProcessStep(parent, bs),
  m_targetType(bs->m_targetType),
  m_makeArguments(bs->m_makeArguments),
  m_makeCommand(bs->m_makeCommand),
  m_targetName(bs->m_targetName),
  m_targetDirName(bs->m_targetDirName),
  m_objDirName(bs->m_objDirName)
{
 ctor();
}

void DMakeStep::ctor()
{
 setDefaultDisplayName(QCoreApplication::translate("DProjectManager::Internal::DMakeStep",
																																																			Constants::D_MS_DISPLAY_NAME));

 m_targetType = Executable;

 if(m_targetName.length() == 0)
  m_targetName =  project()->displayName();

 QString bname = this->buildConfiguration()->displayName().toLower();
 QString sep(QDir::separator());
 if(m_targetDirName.length() == 0)
  m_targetDirName = QLatin1String("bin") + sep + bname;
 if(m_objDirName.length() == 0)
  m_objDirName = QLatin1String("obj") + sep + bname;

 if(m_makeArguments.length() == 0)
 {
  ProjectExplorer::Abi abi = ProjectExplorer::Abi::hostAbi();
  if(abi.wordWidth() == 64)
   m_makeArguments = QLatin1String("-m64");
 }
}

DMakeStep::~DMakeStep()
{
}

bool DMakeStep::init()
{
 BuildConfiguration *bc = buildConfiguration();
 if (!bc)
  bc = target()->activeBuildConfiguration();

 m_tasks.clear();
 ToolChain *tc = ToolChainKitInformation::toolChain(target()->kit());
 if (!tc) {
  m_tasks.append(Task(Task::Error, tr("Qt Creator needs a compiler set up to build. Configure a compiler in the kit options."),
                      Utils::FileName(), -1,
                      Core::Id(ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM)));
  return true; // otherwise the tasks will not get reported
 }

 ProcessParameters *pp = processParameters();
 pp->setMacroExpander(bc->macroExpander());
 pp->setWorkingDirectory(bc->buildDirectory().toString());
 Utils::Environment env = bc->environment();
 // Force output to english for the parsers. Do this here and not in the toolchain's
 // addToEnvironment() to not screw up the users run environment.
 env.set(QLatin1String("LC_ALL"), QLatin1String("C"));
 pp->setEnvironment(env);
 pp->setCommand(makeCommand(bc->environment()));
 pp->setArguments(allArguments());
 pp->resolveAll();

 setOutputParser(new GnuMakeParser());
 IOutputParser *parser = target()->kit()->createOutputParser();
 if (parser)
  appendOutputParser(parser);
 outputParser()->setWorkingDirectory(pp->effectiveWorkingDirectory());

 return AbstractProcessStep::init();
}

QVariantMap DMakeStep::toMap() const
{
 QSettings projectFiles(this->project()->projectFilePath(), QSettings::IniFormat);
 projectFiles.beginGroup(QLatin1String("BC.") + this->buildConfiguration()->displayName());

	projectFiles.setValue(QLatin1String(Constants::MAKE_ARGUMENTS_KEY), m_makeArguments);
	projectFiles.setValue(QLatin1String(Constants::MAKE_COMMAND_KEY), m_makeCommand);
	projectFiles.setValue(QLatin1String(Constants::TARGET_NAME_KEY), m_targetName);
	projectFiles.setValue(QLatin1String(Constants::TARGET_DIRNAME_KEY), m_targetDirName);
	projectFiles.setValue(QLatin1String(Constants::TARGET_TYPE_KEY), m_targetType);
	projectFiles.setValue(QLatin1String(Constants::OBJ_DIRNAME_KEY), m_objDirName);

 projectFiles.sync();

 return AbstractProcessStep::toMap();
}

bool DMakeStep::fromMap(const QVariantMap &map)
{
 QSettings sets(this->project()->projectFilePath(), QSettings::IniFormat);
 //QSettings* sets = map[QLatin1String("QSettings")].value<QSettings*>();
 QString group = QLatin1String("BC.") + this->buildConfiguration()->displayName();
 if(sets.childGroups().contains(group))
 {
  sets.beginGroup(group);

		m_makeArguments = sets.value(QLatin1String(Constants::MAKE_ARGUMENTS_KEY),m_makeArguments).toString();
		m_makeCommand = sets.value(QLatin1String(Constants::MAKE_COMMAND_KEY),m_makeCommand).toString();
		m_targetName = sets.value(QLatin1String(Constants::TARGET_NAME_KEY),m_targetName).toString();
		m_targetDirName = sets.value(QLatin1String(Constants::TARGET_DIRNAME_KEY),m_targetDirName).toString();
		m_targetType = (TargetType)sets.value(QLatin1String(Constants::TARGET_TYPE_KEY),m_targetType).toInt();
		m_objDirName = sets.value(QLatin1String(Constants::OBJ_DIRNAME_KEY),m_objDirName).toString();
 }
 return AbstractProcessStep::fromMap(map);
}

QString DMakeStep::allArguments() const
{
 QString bname = this->buildConfiguration()->displayName().toLower();

 QString args;
 if(bname == QLatin1String("debug"))
  args += QLatin1String("-debug -gc");
 else if(bname == QLatin1String("unittest"))
  args += QLatin1String("-debug -gc -unittest");
 else if(bname == QLatin1String("release"))
  args += QLatin1String(" -release -O -inline");

 if(m_targetType == StaticLibrary)
  args += QLatin1String(" -lib");
 else if(m_targetType == SharedLibrary)
  args += QLatin1String(" -shared -fPIC");

 //Utils::QtcProcess::addArgs(&args, QLatin1String("-m64"));
 QString outFile = outFileName();
	QString makargs = m_makeArguments;
	makargs.replace(QLatin1String("%{TargetDir}"),m_targetDirName);
	Utils::QtcProcess::addArgs(&args, makargs);

	QDir buildDir(this->buildConfiguration()->buildDirectory().toString());
	QString projDir = project()->projectDirectory();
	if(QDir(m_targetDirName).isRelative())
	{
		QString relDir = buildDir.relativeFilePath(projDir + QDir::separator() + m_targetDirName);
		if(relDir.length() == 0)
			Utils::QtcProcess::addArgs(&args, QLatin1String("-of") + outFile);
		else
			Utils::QtcProcess::addArgs(&args, QLatin1String("-of") + relDir + QDir::separator() + outFile);
	}
	if(QDir(m_targetDirName).isRelative())
	{
		QString relDir = buildDir.relativeFilePath(projDir + QDir::separator() + m_objDirName);
		if(relDir.length() > 0)
			Utils::QtcProcess::addArgs(&args, QLatin1String("-od") + relDir);
	}
	static QLatin1String dotd(".d");
	static QLatin1String dotdi(".di");
	static QLatin1String space(" ");
	QString srcs = QLatin1String(" ");
	const QHash<QString,QString>& files = static_cast<DProject*>(project())->files();
	foreach(QString file, files.values())
		if(file.endsWith(dotd) || file.endsWith(dotdi))
			srcs.append(file).append(space);
	Utils::QtcProcess::addArgs(&args, srcs);
 return args;
}

QString DMakeStep::outFileName() const
{
 QString outName = m_targetName;
 if(m_targetType > 0)
 {
  QString fix = QLatin1String("lib");
  if(outName.startsWith(fix) == false)
   outName = fix + outName;

  if(m_targetType == StaticLibrary)
    fix = QLatin1String(".a");
  else if(m_targetType == SharedLibrary)
   fix = QLatin1String(".so");

  if(outName.endsWith(fix) == false)
   outName.append(fix);
 }

 return outName;
}

QString DMakeStep::makeCommand(const Utils::Environment &environment) const
{
 QString command = m_makeCommand;
 if (command.isEmpty()) {
  ToolChain *tc = ToolChainKitInformation::toolChain(target()->kit());
  if (tc)
   command = tc->makeCommand(environment);
  else
   command = QLatin1String("dmd");
 }
 return command;
}

void DMakeStep::run(QFutureInterface<bool> &fi)
{
 bool canContinue = true;
 foreach (const Task &t, m_tasks)
 {
  addTask(t);
  canContinue = false;
 }
 if (!canContinue)
 {
  emit addOutput(tr("Configuration is faulty. Check the Issues view for details."), BuildStep::MessageOutput);
  fi.reportResult(false);
  emit finished();
  return;
 }
 //processParameters()->setWorkingDirectory(project()->projectDirectory());
 AbstractProcessStep::run(fi);
}

BuildStepConfigWidget *DMakeStep::createConfigWidget()
{
 return new DMakeStepConfigWidget(this);
}

bool DMakeStep::immutable() const
{
 return false;
}


//
// DMakeStepConfigWidget
//

DMakeStepConfigWidget::DMakeStepConfigWidget(DMakeStep *makeStep)
 : m_makeStep(makeStep)
{
	Project *pro = m_makeStep->target()->project();
 m_ui = new Ui::DMakeStep;
 m_ui->setupUi(this);

 m_ui->targetTypeComboBox->addItem(QLatin1String("Executable"));
 m_ui->targetTypeComboBox->addItem(QLatin1String("Static Library"));
 m_ui->targetTypeComboBox->addItem(QLatin1String("Shared Library"));

 m_ui->targetTypeComboBox->setCurrentIndex((int)m_makeStep->m_targetType);
 m_ui->makeLineEdit->setText(m_makeStep->m_makeCommand);
 m_ui->makeArgumentsLineEdit->setText(m_makeStep->m_makeArguments);
 m_ui->targetNameLineEdit->setText(m_makeStep->m_targetName);
 m_ui->targetDirLineEdit->setText(m_makeStep->m_targetDirName);
 m_ui->objDirLineEdit->setText(m_makeStep->m_objDirName);
 updateMakeOverrrideLabel();
 updateDetails();

 connect(m_ui->targetTypeComboBox, SIGNAL(currentIndexChanged (int)),
         this, SLOT(targetTypeComboBoxSelectItem(int)));

 connect(m_ui->makeLineEdit, SIGNAL(textEdited(QString)),
         this, SLOT(makeLineEditTextEdited()));
 connect(m_ui->makeArgumentsLineEdit, SIGNAL(textEdited(QString)),
         this, SLOT(makeArgumentsLineEditTextEdited()));
 connect(m_ui->targetNameLineEdit, SIGNAL(textEdited(QString)),
         this, SLOT(targetNameLineEditTextEdited()));
 connect(m_ui->targetDirLineEdit, SIGNAL(textEdited(QString)),
         this, SLOT(targetDirNameLineEditTextEdited()));
 connect(m_ui->objDirLineEdit, SIGNAL(textEdited(QString)),
         this, SLOT(objDirLineEditTextEdited()));

 connect(ProjectExplorerPlugin::instance(), SIGNAL(settingsChanged()),
         this, SLOT(updateMakeOverrrideLabel()));
 connect(ProjectExplorerPlugin::instance(), SIGNAL(settingsChanged()),
         this, SLOT(updateDetails()));

 connect(m_makeStep->target(), SIGNAL(kitChanged()),
         this, SLOT(updateMakeOverrrideLabel()));

 connect(pro, SIGNAL(environmentChanged()),
         this, SLOT(updateMakeOverrrideLabel()));
	connect(pro, SIGNAL(environmentChanged()),
									this, SLOT(updateDetails()));
	connect(m_makeStep->buildConfiguration(), SIGNAL(buildDirectoryChanged()),
									this, SLOT(updateDetails()));

}

DMakeStepConfigWidget::~DMakeStepConfigWidget()
{
 delete m_ui;
}

QString DMakeStepConfigWidget::displayName() const
{
 return tr("Make", "DMakestep display name.");
}

void DMakeStepConfigWidget::updateMakeOverrrideLabel()
{
 BuildConfiguration *bc = m_makeStep->buildConfiguration();
 if (!bc)
  bc = m_makeStep->target()->activeBuildConfiguration();

 m_ui->makeLabel->setText(tr("Override %1:").arg(m_makeStep->makeCommand(bc->environment())));
}

void DMakeStepConfigWidget::updateDetails()
{
 BuildConfiguration *bc = m_makeStep->buildConfiguration();
 if (!bc)
  bc = m_makeStep->target()->activeBuildConfiguration();

 ProcessParameters param;
 param.setMacroExpander(bc->macroExpander());
 param.setWorkingDirectory(bc->buildDirectory().toString());
 param.setEnvironment(bc->environment());
 param.setCommand(m_makeStep->makeCommand(bc->environment()));
 param.setArguments(m_makeStep->allArguments());
 m_summaryText = param.summary(displayName());
 emit updateSummary();

 //QList<RunConfiguration*> list = m_makeStep->target()->runConfigurations();
 foreach(RunConfiguration* rc, m_makeStep->target()->runConfigurations())
 {
  DRunConfiguration * brc = dynamic_cast<DRunConfiguration *>(rc);
  if(brc)
   brc->updateConfig(m_makeStep);
 }
}

QString DMakeStepConfigWidget::summaryText() const
{
 return m_summaryText;
}

void DMakeStepConfigWidget::targetTypeComboBoxSelectItem(int index)
{
 m_makeStep->m_targetType = (TargetType)index;
 updateDetails();
}

void DMakeStepConfigWidget::makeLineEditTextEdited()
{
 m_makeStep->m_makeCommand = m_ui->makeLineEdit->text();
 updateDetails();
}

void DMakeStepConfigWidget::makeArgumentsLineEditTextEdited()
{
 m_makeStep->m_makeArguments = m_ui->makeArgumentsLineEdit->text();
 updateDetails();
}
void DMakeStepConfigWidget::targetNameLineEditTextEdited()
{
 m_makeStep->m_targetName = m_ui->targetNameLineEdit->text();
 updateDetails();
}
void DMakeStepConfigWidget::targetDirNameLineEditTextEdited()
{
 m_makeStep->m_targetDirName = m_ui->targetDirLineEdit->text();
 updateDetails();
}
void DMakeStepConfigWidget::objDirLineEditTextEdited()
{
 m_makeStep->m_objDirName = m_ui->objDirLineEdit->text();
 updateDetails();
}
//
// DMakeStepFactory
//

DMakeStepFactory::DMakeStepFactory(QObject *parent) :
  IBuildStepFactory(parent)
{
}

bool DMakeStepFactory::canCreate(BuildStepList *parent, const Id id) const
{
 if (parent->target()->project()->id() == Constants::DPROJECT_ID)
		return id == Constants::D_MS_ID;
 return false;
}

BuildStep *DMakeStepFactory::create(BuildStepList *parent, const Id id)
{
 if (!canCreate(parent, id))
  return 0;
 DMakeStep *step = new DMakeStep(parent);
 return step;
}

bool DMakeStepFactory::canClone(BuildStepList *parent, BuildStep *source) const
{
 return canCreate(parent, source->id());
}

BuildStep *DMakeStepFactory::clone(BuildStepList *parent, BuildStep *source)
{
 if (!canClone(parent, source))
  return 0;
 DMakeStep *old(qobject_cast<DMakeStep *>(source));
 Q_ASSERT(old);
 return new DMakeStep(parent, old);
}

bool DMakeStepFactory::canRestore(BuildStepList *parent, const QVariantMap &map) const
{
 return canCreate(parent, idFromMap(map));
}

BuildStep *DMakeStepFactory::restore(BuildStepList *parent, const QVariantMap &map)
{
 if (!canRestore(parent, map))
  return 0;
 DMakeStep *bs(new DMakeStep(parent));
 if (bs->fromMap(map))
  return bs;
 delete bs;
 return 0;
}

QList<Id> DMakeStepFactory::availableCreationIds(BuildStepList *parent) const
{
 if (parent->target()->project()->id() == Constants::DPROJECT_ID)
		return QList<Id>() << Id(Constants::D_MS_ID);
 return QList<Id>();
}

QString DMakeStepFactory::displayNameForId(const Id id) const
{
	if (id == Constants::D_MS_ID)
  return QCoreApplication::translate("DProjectManager::Internal::DMakeStep",
																																					Constants::D_MS_DISPLAY_NAME);
 return QString();
}

} // namespace Internal
} // namespace DProjectManager
