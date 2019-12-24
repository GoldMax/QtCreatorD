#include "dmakestep.h"
#include "dprojectmanagerconstants.h"
#include "dproject.h"
#include "ui_dmakestep.h"
#include "dbuildconfiguration.h"
#include "drunconfiguration.h"

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>
#include <projectexplorer/taskhub.h>
#include <projectexplorer/abi.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/abstractprocessstep.h>
#include <projectexplorer/processparameters.h>
#include <projectexplorer/gnumakeparser.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/toolchain.h>
//#include <qtsupport/qtkitinformation.h>
//#include <qtsupport/qtparser.h>
//#include <utils/stringutils.h>
//#include <utils/qtcassert.h>
//#include <utils/qtcprocess.h>

//#include <QSettings>

using namespace ProjectExplorer;
using namespace DProjectManager;

namespace DProjectManager {

DMakeStep::DMakeStep(BuildStepList *parent) :
		AbstractProcessStep(parent, Core::Id(Constants::D_MS_ID)),
		m_targetType(Executable), m_buildPreset(Debug)
{
	setDefaultDisplayName(QLatin1String(Constants::D_MS_DISPLAY_NAME));

	m_targetName = project()->displayName();

	QString bname = this->buildConfiguration()->displayName().toLower();
	// this->buildConfiguration()->displayName() может быть пустым,
	// так как имя еще может быть не установлено
	if(bname.length() == 0)
		bname = "debug";
	if(m_targetDirName.length() == 0)
		m_targetDirName = QLatin1String("bin") + QDir::separator() + bname;
	if(m_objDirName.length() == 0)
		m_objDirName = QLatin1String("obj") + QDir::separator() + bname;

	if(m_makeArguments.length() == 0)
	{
		ProjectExplorer::Abi abi = ProjectExplorer::Abi::hostAbi();
		if(abi.wordWidth() == 64)
			m_makeArguments = QLatin1String("-m64");
	}
}

//DMakeStep::~DMakeStep() { }

QVariantMap DMakeStep::toMap() const
{
	QVariantMap map = AbstractProcessStep::toMap();
	map.insert(QLatin1String(Constants::INI_TARGET_TYPE_KEY), m_targetType);
	map.insert(QLatin1String(Constants::INI_BUILD_PRESET_KEY), m_buildPreset);
	map.insert(QLatin1String(Constants::INI_TARGET_NAME_KEY), m_targetName);
	map.insert(QLatin1String(Constants::INI_TARGET_DIRNAME_KEY), m_targetDirName);
	map.insert(QLatin1String(Constants::INI_OBJ_DIRNAME_KEY), m_objDirName);
	map.insert(QLatin1String(Constants::INI_MAKE_ARGUMENTS_KEY), m_makeArguments);
	return map;
}
bool DMakeStep::fromMap(const QVariantMap &map)
{
	m_targetType =
			static_cast<TargetType>(map.value(QLatin1String(Constants::INI_TARGET_TYPE_KEY)).toInt());
	m_buildPreset =
			static_cast<BuildPreset>(map.value(QLatin1String(Constants::INI_BUILD_PRESET_KEY)).toInt());
	m_targetName = map.value(QLatin1String(Constants::INI_TARGET_NAME_KEY)).toString();
	m_targetDirName = map.value(QLatin1String(Constants::INI_TARGET_DIRNAME_KEY)).toString();
	m_objDirName = map.value(QLatin1String(Constants::INI_OBJ_DIRNAME_KEY)).toString();
	m_makeArguments = map.value(QLatin1String(Constants::INI_MAKE_ARGUMENTS_KEY)).toString();
	return BuildStep::fromMap(map);
}
BuildStepConfigWidget *DMakeStep::createConfigWidget()
{
	return new DMakeStepConfigWidget(this);
}

Utils::FilePath DMakeStep::makeCommand() const
{
	BuildConfiguration *bc = buildConfiguration();
	if (!bc)
		bc = target()->activeBuildConfiguration();
	if (!bc)
		return {};
	const Utils::Environment env = bc->environment();

	for (const ToolChain *tc :  ToolChainKitAspect::toolChains(target()->kit()))
	{
		Utils::FilePath make = tc->makeCommand(env);
		if (!make.isEmpty())
			return make;
	}
	return Utils::FilePath::fromString("dmd");
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
QString DMakeStep::allArguments() const
{
	QString args;

	if(m_buildPreset == Debug)
		args += QLatin1String("-debug -g");
	else if(m_buildPreset == Unittest)
		args += QLatin1String("-debug -g -unittest");
	else if(m_buildPreset == Release)
		args += QLatin1String(" -release -O -inline");

	if(m_targetType == StaticLibrary)
		args += QLatin1String(" -lib");
	else if(m_targetType == SharedLibrary)
		args += QLatin1String(" -shared -fPIC");

	QDir buildDir(this->buildConfiguration()->buildDirectory().toString());
	QString projDir = project()->projectDirectory().toString();
	QString relTargetDir = m_targetDirName;
	if(QDir(m_targetDirName).isRelative())
		relTargetDir = buildDir.relativeFilePath(projDir + QDir::separator() + m_targetDirName);

	QString outFile = outFileName();
	QString makargs = m_makeArguments;
	makargs.replace(QLatin1String("%{TargetDir}"),relTargetDir);
	Utils::QtcProcess::addArgs(&args, makargs);

	if(relTargetDir.length() > 0)
		Utils::QtcProcess::addArgs(&args, QLatin1String("-of") + relTargetDir + QDir::separator() + outFile);
	if(m_objDirName.length() > 0)
		if(QDir(m_objDirName).isRelative())
		{
			QString relDir = buildDir.relativeFilePath(projDir + QDir::separator() + m_objDirName);
			if(relDir.length() > 0)
				Utils::QtcProcess::addArgs(&args, QLatin1String("-od") + relDir);
		}

	DProject* proj = static_cast<DProject*>(project());
	// Libs
	QStringList libs = proj->libraries().split(QLatin1Char(' '), QString::SkipEmptyParts);
		foreach(QString s, libs)
	{
		s = s.replace(QLatin1String("%{TargetDir}"),relTargetDir);
		if(s.startsWith(QLatin1String("-L")))
			Utils::QtcProcess::addArgs(&args, s);
		else
			Utils::QtcProcess::addArgs(&args, QLatin1String("-L-l") + s);
	}
	// Includes
	QStringList incs = proj->includes().split(QLatin1Char(' '), QString::SkipEmptyParts);
	foreach(QString s, incs)
	{
		s = s.replace(QLatin1String("%{TargetDir}"),relTargetDir);
		if(s.startsWith(QLatin1String("-I")))
			Utils::QtcProcess::addArgs(&args, s);
		else
			Utils::QtcProcess::addArgs(&args, QLatin1String("-I") + s);
	}
	// Extra Args
	makargs = proj->extraArgs();
	Utils::QtcProcess::addArgs(&args, makargs.replace(QLatin1String("%{TargetDir}"),relTargetDir));
	// Files
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

bool DMakeStep::init()
{
	const auto bc = static_cast<DBuildConfiguration *>(buildConfiguration());
	if(!bc)
		emit addTask(Task::buildConfigurationMissingTask());

	Utils::CommandLine cmd = Utils::CommandLine(makeCommand());
	if(cmd.executable().isEmpty())
		emit addTask(Task::compilerMissingTask());

	if(!bc || cmd.executable().isEmpty())
	{
		emitFaultyConfigurationMessage();
		return false;
	}

	ProcessParameters *pp = processParameters();
	pp->setMacroExpander(bc->macroExpander());
	pp->setWorkingDirectory(bc->buildDirectory());
	pp->setEnvironment(bc->environment());
	cmd.addArgs(allArguments(), Utils::CommandLine::RawType::Raw);
	pp->setCommandLine(cmd);
	pp->resolveAll();

	setOutputParser(new ProjectExplorer::GnuMakeParser());
	IOutputParser *parser = target()->kit()->createOutputParser();
	if (parser)
		appendOutputParser(parser);
	outputParser()->setWorkingDirectory(pp->effectiveWorkingDirectory());

	return AbstractProcessStep::init();
}
void DMakeStep::doRun()
{
	AbstractProcessStep::doRun();
}

QString ddemangle(const QString& line)
{
	QString res = line;
	try
	{
		QProcess proc;
		proc.setProcessChannelMode(QProcess::MergedChannels);
		proc.start(QLatin1String("ddemangle"));
		if (!proc.waitForStarted(10000))
			return line;
		proc.write(line.toUtf8());
		proc.closeWriteChannel();
		if(!proc.waitForFinished(2000))
		{
			proc.close();
			return line;
		}
		else if(proc.exitCode() != 0)
		{
			proc.close();
			return line;
		}
		else
		{
			res = QString::fromUtf8(proc.readAllStandardOutput());
			proc.close();
		}
	}
	catch(...){}
	return res;
}
void DMakeStep::stdOutput(const QString &line)
{
	QString res = ddemangle(line);
	AbstractProcessStep::stdError(res);
}
void DMakeStep::stdError(const QString &line)
{
	QString res = ddemangle(line);
	AbstractProcessStep::stdError(res);
}

////-------------------------------------------------------------------------
////-- DMakeStepFactory
////-------------------------------------------------------------------------
DMakeStepFactory::DMakeStepFactory()
{
	registerStep<DMakeStep>(Constants::D_MS_ID);
	setDisplayName(QLatin1String(Constants::D_MS_DISPLAY_NAME)); //MakeStep::defaultDisplayName());
	setSupportedProjectType(Constants::DPROJECT_ID);
}

//QList<BuildStepInfo> DMakeStepFactory::availableSteps(BuildStepList* parent) const
//{
//	if (parent->target()->project()->id() != Constants::DPROJECT_ID)
//									return {};

//					return {{ Constants::D_MS_ID,
//															QCoreApplication::translate("DProjectManager::DMakeStep",
//															Constants::D_MS_DISPLAY_NAME) }};
//}

//bool DMakeStepFactory::canCreate(BuildStepList *parent, const Id id) const
//{
//	Core::Id idd = parent->target()->project()->id();
//	if (idd == Constants::DPROJECT_ID)
//		return id == Constants::D_MS_ID;
//	return false;
//}

//BuildStep *DMakeStepFactory::create(BuildStepList *parent, const Id id)
//{
//	if (!canCreate(parent, id))
//		return 0;
//	DMakeStep *step = new DMakeStep(parent);
//	return step;
//}

//bool DMakeStepFactory::canClone(BuildStepList *parent, BuildStep *source) const
//{
//	return canCreate(parent, source->id());
//}

//BuildStep *DMakeStepFactory::clone(BuildStepList *parent, BuildStep *source)
//{
//	if (!canClone(parent, source))
//		return nullptr;
//	DMakeStep *old(qobject_cast<DMakeStep *>(source));
//	Q_ASSERT(old);
//	return new DMakeStep(parent, old);
//}

//bool DMakeStepFactory::canRestore(BuildStepList *parent, const QVariantMap &map) const
//{
//	return canCreate(parent, idFromMap(map));
//}

//BuildStep *DMakeStepFactory::restore(BuildStepList *parent, const QVariantMap &map)
//{
//	if (!canRestore(parent, map))
//		return nullptr;
//	DMakeStep *bs(new DMakeStep(parent));
//	if (bs->fromMap(map))
//		return bs;
//	delete bs;
//	return nullptr;
//}

//QList<Id> DMakeStepFactory::availableCreationIds(BuildStepList *parent) const
//{
//	if (parent->target()->project()->id() == Constants::DPROJECT_ID)
//		return QList<Id>() << Id(Constants::D_MS_ID);
//	return QList<Id>();
//}

//QString DMakeStepFactory::displayNameForId(const Id id) const
//{
//	if (id == Constants::D_MS_ID)
//		return QCoreApplication::translate("DProjectManager::DMakeStep",
//																																					Constants::D_MS_DISPLAY_NAME);
//	return QString();
//}

//-------------------------------------------------------------------------
//-- DMakeStepConfigWidget
//-------------------------------------------------------------------------

DMakeStepConfigWidget::DMakeStepConfigWidget(DMakeStep *makeStep)
	: BuildStepConfigWidget(makeStep)
			, m_makeStep(makeStep)
{
	setDisplayName("D make");

	Project *pro = m_makeStep->target()->project();
	m_ui = new DProjectManager::Ui::DMakeStepUi;
	m_ui->setupUi(this);

	m_ui->targetTypeComboBox->addItem(QLatin1String("Executable"));
	m_ui->targetTypeComboBox->addItem(QLatin1String("Static Library"));
	m_ui->targetTypeComboBox->addItem(QLatin1String("Shared Library"));

	m_ui->buildPresetComboBox->addItem(QLatin1String("Debug"));
	m_ui->buildPresetComboBox->addItem(QLatin1String("Release"));
	m_ui->buildPresetComboBox->addItem(QLatin1String("Unittest"));
	m_ui->buildPresetComboBox->addItem(QLatin1String("None"));

	m_ui->targetTypeComboBox->setCurrentIndex(static_cast<int>(m_makeStep->m_targetType));
	m_ui->buildPresetComboBox->setCurrentIndex(static_cast<int>(m_makeStep->m_buildPreset));
	m_ui->makeArgumentsLineEdit->setPlainText(m_makeStep->m_makeArguments);
	m_ui->targetNameLineEdit->setText(m_makeStep->m_targetName);
	m_ui->targetDirLineEdit->setText(m_makeStep->m_targetDirName);
	m_ui->objDirLineEdit->setText(m_makeStep->m_objDirName);

	updateDetails();

	connect(m_ui->targetTypeComboBox, SIGNAL(currentIndexChanged (int)),
									this, SLOT(targetTypeComboBoxSelectItem(int)));
	connect(m_ui->buildPresetComboBox, SIGNAL(currentIndexChanged (int)),
									this, SLOT(buildPresetComboBoxSelectItem(int)));
	connect(m_ui->targetNameLineEdit, SIGNAL(textEdited(QString)),
									this, SLOT(targetNameLineEditTextEdited()));
	connect(m_ui->targetDirLineEdit, SIGNAL(textEdited(QString)),
									this, SLOT(targetDirNameLineEditTextEdited()));
	connect(m_ui->objDirLineEdit, SIGNAL(textEdited(QString)),
									this, SLOT(objDirLineEditTextEdited()));
	connect(m_ui->makeArgumentsLineEdit, SIGNAL(textChanged()),
									this, SLOT(makeArgumentsLineEditTextEdited()));

	connect(ProjectExplorerPlugin::instance(), SIGNAL(settingsChanged()),
									this, SLOT(updateDetails()));
	connect(pro, SIGNAL(environmentChanged()),
									this, SLOT(updateDetails()));
	connect(m_makeStep->buildConfiguration(), SIGNAL(buildDirectoryChanged()),
									this, SLOT(updateDetails()));
	connect(m_makeStep->buildConfiguration(), SIGNAL(configurationChanged()),
									this, SLOT(updateDetails()));
}

DMakeStepConfigWidget::~DMakeStepConfigWidget()
{
	delete m_ui;
}

void DMakeStepConfigWidget::updateDetails()
{
	BuildConfiguration *bc = m_makeStep->buildConfiguration();
	if (!bc)
		bc = m_makeStep->target()->activeBuildConfiguration();

	ProcessParameters param;
	param.setMacroExpander(bc->macroExpander());
	param.setWorkingDirectory(bc->buildDirectory());
	param.setEnvironment(bc->environment());
	Utils::CommandLine cmd = Utils::CommandLine(m_makeStep->makeCommand());
	cmd.addArgs(m_makeStep->allArguments(), Utils::CommandLine::RawType::Raw);
	param.setCommandLine(cmd);
	this->setSummaryText(param.summary(displayName()));
	emit updateSummary();

	foreach(RunConfiguration* rc, m_makeStep->target()->runConfigurations())
	{
		DRunConfiguration * brc = dynamic_cast<DRunConfiguration *>(rc);
		if(brc)
			brc->updateConfig(m_makeStep);
	}
}

void DMakeStepConfigWidget::targetTypeComboBoxSelectItem(int index)
{
	m_makeStep->m_targetType = static_cast<DMakeStep::TargetType>(index);
	updateDetails();
}
void DMakeStepConfigWidget::buildPresetComboBoxSelectItem(int index)
{
	m_makeStep->m_buildPreset = static_cast<DMakeStep::BuildPreset>(index);
	updateDetails();
}
void DMakeStepConfigWidget::makeArgumentsLineEditTextEdited()
{
	m_makeStep->m_makeArguments = m_ui->makeArgumentsLineEdit->toPlainText();
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


} // namespace DProjectManager
