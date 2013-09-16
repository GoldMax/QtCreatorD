#include "dlangmakestep.h"
#include "dlangprojectconstants.h"
#include "dlangproject.h"
#include "ui_dlangmakestep.h"
#include "dlangbuildconfiguration.h"
#include "buildrunconfig.h"

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

using namespace Core;
using namespace ProjectExplorer;

namespace DLangProjectManager {
namespace Internal {

const char DLang_MS_ID[] = "DLangProjectManager.DLangMakeStep";
const char DLang_MS_DISPLAY_NAME[] = QT_TRANSLATE_NOOP("DLangProjectManager::Internal::DLangMakeStep",
                                                       "Make");

const char BUILD_TARGETS_KEY[] = "DLangProjectManager.DLangMakeStep.BuildTargets";
const char MAKE_ARGUMENTS_KEY[] = "DLangProjectManager.DLangMakeStep.MakeArguments";
const char MAKE_COMMAND_KEY[] = "DLangProjectManager.DLangMakeStep.MakeCommand";
const char TARGET_NAME_KEY[] = "DLangProjectManager.DLangMakeStep.TargetName";
const char TARGET_DIRNAME_KEY[] = "DLangProjectManager.DLangMakeStep.TargetDirName";
const char TARGET_TYPE_KEY[] = "DLangProjectManager.DLangMakeStep.TargetTypeName";
const char OBJ_DIRNAME_KEY[] = "DLangProjectManager.DLangMakeStep.ObjDirName";

DLangMakeStep::DLangMakeStep(BuildStepList *parent) :
  AbstractProcessStep(parent, Id(DLang_MS_ID))
{
 ctor();
}

DLangMakeStep::DLangMakeStep(BuildStepList *parent, const Id id) :
  AbstractProcessStep(parent, id)
{
 ctor();
}

DLangMakeStep::DLangMakeStep(BuildStepList *parent, DLangMakeStep *bs) :
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

void DLangMakeStep::ctor()
{
 setDefaultDisplayName(QCoreApplication::translate("DLangProjectManager::Internal::DLangMakeStep",
                                                   DLang_MS_DISPLAY_NAME));

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

DLangMakeStep::~DLangMakeStep()
{
}

bool DLangMakeStep::init()
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

QVariantMap DLangMakeStep::toMap() const
{
 QVariantMap map(AbstractProcessStep::toMap());

 map.insert(QLatin1String(MAKE_ARGUMENTS_KEY), m_makeArguments);
 map.insert(QLatin1String(MAKE_COMMAND_KEY), m_makeCommand);
 map.insert(QLatin1String(TARGET_NAME_KEY), m_targetName);
 map.insert(QLatin1String(TARGET_DIRNAME_KEY), m_targetDirName);
 map.insert(QLatin1String(TARGET_TYPE_KEY), m_targetType);
 map.insert(QLatin1String(OBJ_DIRNAME_KEY), m_objDirName);
 return map;
}

void DLangMakeStep::setMakeArguments(const QString val)
{
 m_makeArguments = val;
}

bool DLangMakeStep::fromMap(const QVariantMap &map)
{
 m_makeArguments = map.value(QLatin1String(MAKE_ARGUMENTS_KEY)).toString();
 m_makeCommand = map.value(QLatin1String(MAKE_COMMAND_KEY)).toString();
 m_targetName = map.value(QLatin1String(TARGET_NAME_KEY)).toString();
 m_targetDirName = map.value(QLatin1String(TARGET_DIRNAME_KEY)).toString();
 m_objDirName = map.value(QLatin1String(OBJ_DIRNAME_KEY)).toString();
 m_targetType = (TargetType)map.value(QLatin1String(TARGET_TYPE_KEY)).toInt();

 return BuildStep::fromMap(map);
}

QString DLangMakeStep::allArguments() const
{
 QString sep(QDir::separator());
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
 Utils::QtcProcess::addArgs(&args, m_makeArguments);
 if(m_targetDirName.length() == 0)
  Utils::QtcProcess::addArgs(&args, QLatin1String("-of") + outFile);
 else
  Utils::QtcProcess::addArgs(&args, QLatin1String("-of") + m_targetDirName + QDir::separator() + outFile);

 Utils::QtcProcess::addArgs(&args, QLatin1String("-od") + m_objDirName);
 Utils::QtcProcess::addArgs(&args, QLatin1String(" @") + project()->displayName() + QLatin1String(".files"));
 return args;
}

QString DLangMakeStep::outFileName() const
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

QString DLangMakeStep::makeCommand(const Utils::Environment &environment) const
{
 QString command = m_makeCommand;
 if (command.isEmpty()) {
  ToolChain *tc = ToolChainKitInformation::toolChain(target()->kit());
  if (tc)
   command = tc->makeCommand(environment);
  else
   command = QLatin1String("make");
 }
 return command;
}

void DLangMakeStep::run(QFutureInterface<bool> &fi)
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

BuildStepConfigWidget *DLangMakeStep::createConfigWidget()
{
 return new DLangMakeStepConfigWidget(this);
}

bool DLangMakeStep::immutable() const
{
 return false;
}


//
// DLangMakeStepConfigWidget
//

DLangMakeStepConfigWidget::DLangMakeStepConfigWidget(DLangMakeStep *makeStep)
 : m_makeStep(makeStep)
{
 DLangProject *pro = static_cast<DLangProject *>(m_makeStep->target()->project());
 m_ui = new Ui::DLangMakeStep;
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
}

DLangMakeStepConfigWidget::~DLangMakeStepConfigWidget()
{
 delete m_ui;
}

QString DLangMakeStepConfigWidget::displayName() const
{
 return tr("Make", "DLangMakestep display name.");
}

void DLangMakeStepConfigWidget::updateMakeOverrrideLabel()
{
 BuildConfiguration *bc = m_makeStep->buildConfiguration();
 if (!bc)
  bc = m_makeStep->target()->activeBuildConfiguration();

 m_ui->makeLabel->setText(tr("Override %1:").arg(m_makeStep->makeCommand(bc->environment())));
}

void DLangMakeStepConfigWidget::updateDetails()
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
  BuildRunConfig * brc = dynamic_cast<BuildRunConfig *>(rc);
  if(brc)
  {
   QString outDir = m_makeStep->m_targetDirName;
   if(outDir.startsWith(QDir::separator()) == false)
   {
    QString projDir = m_makeStep->target()->project()->rootProjectNode()->path();
    projDir.truncate(projDir.lastIndexOf(QDir::separator()));
    outDir = projDir + QDir::separator() + outDir;
   }
   brc->setBaseWorkingDirectory(outDir);
   outDir.append(QDir::separator() + m_makeStep->outFileName());
   brc->setExecutable(outDir);

  }
 }

 //if(run)
 {
  //run->setExecutable("ccc");
  //BuildStepList *buildSteps = run->->stepList(Constants::BUILDSTEPS_BUILD);
//  QList<IRunConfigurationAspect *> exAsp = run->extraAspects();
//  foreach(IRunConfigurationAspect * a, exAsp)
//  {
//   if(a->displayName().length() > 0)
//   {
//    //a->currentSettings()->
//   }
 //}
 }
}

QString DLangMakeStepConfigWidget::summaryText() const
{
 return m_summaryText;
}

void DLangMakeStepConfigWidget::targetTypeComboBoxSelectItem(int index)
{
 m_makeStep->m_targetType = (TargetType)index;
 updateDetails();
}

void DLangMakeStepConfigWidget::makeLineEditTextEdited()
{
 m_makeStep->m_makeCommand = m_ui->makeLineEdit->text();
 updateDetails();
}

void DLangMakeStepConfigWidget::makeArgumentsLineEditTextEdited()
{
 m_makeStep->m_makeArguments = m_ui->makeArgumentsLineEdit->text();
 updateDetails();
}
void DLangMakeStepConfigWidget::targetNameLineEditTextEdited()
{
 m_makeStep->m_targetName = m_ui->targetNameLineEdit->text();
 updateDetails();
}
void DLangMakeStepConfigWidget::targetDirNameLineEditTextEdited()
{
 m_makeStep->m_targetDirName = m_ui->targetDirLineEdit->text();
 updateDetails();
}
void DLangMakeStepConfigWidget::objDirLineEditTextEdited()
{
 m_makeStep->m_objDirName = m_ui->objDirLineEdit->text();
 updateDetails();
}
//
// DLangMakeStepFactory
//

DLangMakeStepFactory::DLangMakeStepFactory(QObject *parent) :
  IBuildStepFactory(parent)
{
}

bool DLangMakeStepFactory::canCreate(BuildStepList *parent, const Id id) const
{
 if (parent->target()->project()->id() == Constants::DLangPROJECT_ID)
  return id == DLang_MS_ID;
 return false;
}

BuildStep *DLangMakeStepFactory::create(BuildStepList *parent, const Id id)
{
 if (!canCreate(parent, id))
  return 0;
 DLangMakeStep *step = new DLangMakeStep(parent);
 return step;
}

bool DLangMakeStepFactory::canClone(BuildStepList *parent, BuildStep *source) const
{
 return canCreate(parent, source->id());
}

BuildStep *DLangMakeStepFactory::clone(BuildStepList *parent, BuildStep *source)
{
 if (!canClone(parent, source))
  return 0;
 DLangMakeStep *old(qobject_cast<DLangMakeStep *>(source));
 Q_ASSERT(old);
 return new DLangMakeStep(parent, old);
}

bool DLangMakeStepFactory::canRestore(BuildStepList *parent, const QVariantMap &map) const
{
 return canCreate(parent, idFromMap(map));
}

BuildStep *DLangMakeStepFactory::restore(BuildStepList *parent, const QVariantMap &map)
{
 if (!canRestore(parent, map))
  return 0;
 DLangMakeStep *bs(new DLangMakeStep(parent));
 if (bs->fromMap(map))
  return bs;
 delete bs;
 return 0;
}

QList<Id> DLangMakeStepFactory::availableCreationIds(BuildStepList *parent) const
{
 if (parent->target()->project()->id() == Constants::DLangPROJECT_ID)
  return QList<Id>() << Id(DLang_MS_ID);
 return QList<Id>();
}

QString DLangMakeStepFactory::displayNameForId(const Id id) const
{
 if (id == DLang_MS_ID)
  return QCoreApplication::translate("DLangProjectManager::Internal::DLangMakeStep",
                                     DLang_MS_DISPLAY_NAME);
 return QString();
}

} // namespace Internal
} // namespace DLangProjectManager
