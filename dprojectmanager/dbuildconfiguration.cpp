#include "dbuildconfiguration.h"

#include "dproject.h"
#include "dmakestep.h"
#include "drunconfiguration.h"
#include "dprojectmanagerconstants.h"

#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/toolchain.h>
#include <utils/fancylineedit.h>
#include <utils/pathchooser.h>
#include <utils/qtcassert.h>
#include <coreplugin/mimedatabase.h>
//#include <qtsupport/customexecutablerunconfiguration.h>

#include <QFormLayout>
#include <QInputDialog>
#include <QSettings>
#include <QPlainTextEdit>

using namespace ProjectExplorer;
using namespace DProjectManager;

namespace DProjectManager {
namespace Internal {

DBuildConfiguration::DBuildConfiguration(Target *parent)
	: BuildConfiguration(parent, Core::Id(Constants::D_BC_ID))
{
}

DBuildConfiguration::DBuildConfiguration(Target *parent, const Core::Id id)
 : BuildConfiguration(parent, id)
{
}

DBuildConfiguration::DBuildConfiguration(Target *parent, DBuildConfiguration *source) :
  BuildConfiguration(parent, source)
{
 cloneSteps(source);
}

NamedWidget* DBuildConfiguration::createConfigWidget()
{
 return new DBuildSettingsWidget(this);
}

bool DBuildConfiguration::fromMap(const QVariantMap &map)
{
	if(BuildConfiguration::fromMap(map) == false)
		return false;
	DProject* prj = static_cast<DProject*>(target()->project());
	Utils::FileName fn = 	Utils::FileName::fromString(prj->buildDirectory().path());
	setBuildDirectory(fn);
	return true;
}

//------------------------------------------------------------------------------------------------
//  class DBuildConfigurationFactory
//------------------------------------------------------------------------------------------------

DBuildConfigurationFactory::DBuildConfigurationFactory(QObject *parent) :
  IBuildConfigurationFactory(parent) { }

DBuildConfigurationFactory::~DBuildConfigurationFactory() { }

bool DBuildConfigurationFactory::canCreate(const Target *parent) const
{
 return canHandle(parent);
}

QList<BuildInfo *> DBuildConfigurationFactory::availableBuilds(const Target *parent) const
{
 QList<BuildInfo *> result;
 QTC_ASSERT(canCreate(parent), return result);

 Utils::FileName projectDir = Utils::FileName::fromString(parent->project()->projectDirectory());

 BuildInfo* info = new BuildInfo(this);
 info->displayName = tr("Debug");
 info->typeName = tr("D Build");
 info->buildDirectory = projectDir;
 info->kitId = parent->kit()->id();
 result << info;

 return result;
}

BuildConfiguration* DBuildConfigurationFactory::create(Target *parent, const BuildInfo *info) const
{
 QTC_ASSERT(canCreate(parent), return 0);
 QTC_ASSERT(info->factory() == this, return 0);
 QTC_ASSERT(info->kitId == parent->kit()->id(), return 0);
 QTC_ASSERT(!info->displayName.isEmpty(), return 0);

 DBuildConfiguration *bc = new DBuildConfiguration(parent);
 bc->setDisplayName(info->displayName);
 bc->setDefaultDisplayName(info->displayName);

 BuildStepList *buildSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
 Q_ASSERT(buildSteps);
 DMakeStep *makeStep = new DMakeStep(buildSteps);
	if(info->displayName == QLatin1String("Debug"))
		makeStep->setBuildPreset(DMakeStep::Debug);
	else if(info->displayName == QLatin1String("Release"))
		makeStep->setBuildPreset(DMakeStep::Release);
	else if(info->displayName == QLatin1String("Unittest"))
		makeStep->setBuildPreset(DMakeStep::Unittest);
	buildSteps->insertStep(0, makeStep);

 // TODO : configure clean step
 //    BuildStepList *cleanSteps = bc->stepList(ProjectExplorer::Constants::BUILDSTEPS_CLEAN);
 //    Q_ASSERT(cleanSteps);
 //    GenericMakeStep *cleanMakeStep = new GenericMakeStep(cleanSteps);
 //    cleanSteps->insertStep(0, cleanMakeStep);
 //    cleanMakeStep->setBuildTarget(QLatin1String("clean"), /* on = */ true);
 //    cleanMakeStep->setClean(true);

 return bc;
}

bool DBuildConfigurationFactory::canClone(const Target* parent, BuildConfiguration *source) const
{
 if (!canHandle(parent))
  return false;
	return source->id() == Constants::D_BC_ID;
}

BuildConfiguration* DBuildConfigurationFactory::clone(Target *parent, BuildConfiguration *source)
{
 if (!canClone(parent, source))
  return 0;
 return new DBuildConfiguration(parent, qobject_cast<DBuildConfiguration *>(source));
}

bool DBuildConfigurationFactory::canRestore(const Target *parent, const QVariantMap &map) const
{
 if (!canHandle(parent))
  return false;
 // XXXXX
 //return map.contains(QLatin1String(Constants::D_BC_NAME));
 return ProjectExplorer::idFromMap(map) == Constants::D_BC_ID;
}

BuildConfiguration* DBuildConfigurationFactory::restore(Target *parent, const QVariantMap &map)
{
 if (!canRestore(parent, map))
  return 0;
 DBuildConfiguration *bc(new DBuildConfiguration(parent));
 if (bc->fromMap(map))
  return bc;
	delete bc;
 return 0;
}

bool DBuildConfigurationFactory::canHandle(const Target *t) const
{
 if (!t->project()->supportsKit(t->kit()))
  return false;
 return qobject_cast<DProject *>(t->project());
}

bool DBuildConfigurationFactory::canSetup(const Kit *k, const QString &projectPath) const
{
	return k && Core::MimeDatabase::findByFile(QFileInfo(projectPath))
			.matchesType(QLatin1String(Constants::D_BC_ID));
}

QList<BuildInfo *> DBuildConfigurationFactory::availableSetups(const Kit *k, const QString &projectPath) const
{
	QList<BuildInfo *> result;
	QTC_ASSERT(canSetup(k, projectPath), return result);

	BuildInfo* info = new BuildInfo(this);
	info->displayName = tr("Debug");
	info->typeName = tr("D Build");
	info->buildDirectory = Utils::FileName::fromString(ProjectExplorer::Project::projectDirectory(projectPath));
	info->kitId = k->id();
	result << info;

	return result;

}
////////////////////////////////////////////////////////////////////////////////////
// DBuildSettingsWidget
////////////////////////////////////////////////////////////////////////////////////

DBuildSettingsWidget::DBuildSettingsWidget(DBuildConfiguration *bc)
	: m_buildConfiguration(bc)
{
 QFormLayout *fl = new QFormLayout(this);
 fl->setContentsMargins(0, -1, 0, -1);
 fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

	setDisplayName(tr("Project settings"));

	// build directory
	m_pathChooser = new Utils::PathChooser(this);
	m_pathChooser->setEnabled(true);
	m_pathChooser->lineEdit()->setReadOnly(true);
	m_pathChooser->setExpectedKind(Utils::PathChooser::ExistingDirectory);
	m_pathChooser->setBaseDirectory(bc->target()->project()->projectDirectory());
	m_pathChooser->setEnvironment(bc->environment());
	QSettings sets(m_buildConfiguration->target()->project()->projectFilePath(), QSettings::IniFormat);
 QString root = sets.value(QLatin1String(Constants::INI_SOURCE_ROOT_KEY)).toString();
	m_pathChooser->setPath(root);
	connect(m_pathChooser, SIGNAL(changed(QString)), this, SLOT(buildDirectoryChanged()));
	fl->addRow(tr("Source directory:"), m_pathChooser);

	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);

	// Includes
	editIncludes = new QLineEdit(this);
	editIncludes->setText(proj->includes());
	connect(editIncludes, SIGNAL(textEdited(QString)),	this, SLOT(editsTextChanged()));
	connect(editIncludes, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Include paths:"), editIncludes);

	// Libs
	editLibs = new QLineEdit(this);
	editLibs->setText(proj->libraries());
	connect(editLibs, SIGNAL(textEdited(QString)),	this, SLOT(editsTextChanged()));
	connect(editLibs, SIGNAL(editingFinished()),	this, SLOT(editsEditingFinished()));
	fl->addRow(tr("Libraries:"), editLibs);


}
void DBuildSettingsWidget::buildDirectoryChanged()
{
	QDir d(m_buildConfiguration->target()->project()->projectDirectory());
	QString rel = d.relativeFilePath(m_pathChooser->rawPath());

	m_pathChooser->setPath(rel);
	QSettings sets(m_buildConfiguration->target()->project()->projectFilePath(), QSettings::IniFormat);
 sets.setValue(QLatin1String(Constants::INI_SOURCE_ROOT_KEY), rel);
	sets.sync();
	static_cast<DProject*>(m_buildConfiguration->target()->project())->refresh(DProject::Everything);
	m_buildConfiguration->setBuildDirectory(Utils::FileName::fromString(rel));
}

void DBuildSettingsWidget::editsTextChanged()
{
	DProject* proj = static_cast<DProject*>(m_buildConfiguration->target()->project());
	Q_ASSERT(proj);
	proj->setIncludes(editIncludes->text());
	proj->setLibraries(editLibs->text());
	m_buildConfiguration->configurationChanged();
}
void DBuildSettingsWidget::editsEditingFinished()
{
	QSettings sets(m_buildConfiguration->target()->project()->projectFilePath(), QSettings::IniFormat);
	sets.setValue(QLatin1String(Constants::INI_INCLUDES_KEY), editIncludes->text());
	sets.setValue(QLatin1String(Constants::INI_LIBRARIES_KEY), editLibs->text());
	sets.sync();
}
} // namespace Internal
} // namespace DProjectManager
