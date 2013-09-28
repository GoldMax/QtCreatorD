#include "dbuildconfiguration.h"

#include "dproject.h"
#include "dmakestep.h"
#include "drunconfiguration.h"

#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/toolchain.h>
#include <utils/pathchooser.h>
#include <utils/qtcassert.h>
//#include <qtsupport/customexecutablerunconfiguration.h>

#include <QFormLayout>
#include <QInputDialog>

using namespace ProjectExplorer;

namespace DProjectManager {
namespace Internal {

const char D_BC_ID[] = "DProjectManager.DBuildConfiguration";

DBuildConfiguration::DBuildConfiguration(Target *parent)
 : BuildConfiguration(parent, Core::Id(D_BC_ID))
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
 if(map.contains(QLatin1String("BuildConfigurationName")))
  this->setDisplayName(map[QLatin1String("BuildConfigurationName")].toString());

 BuildStepList *buildSteps = this->stepList(ProjectExplorer::Constants::BUILDSTEPS_BUILD);
 Q_ASSERT(buildSteps);
 DMakeStep *makeStep = new DMakeStep(buildSteps);
 makeStep->fromMap(map);
 buildSteps->insertStep(0, makeStep);

 // TODO : configure clean step

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
 return source->id() == D_BC_ID;
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
 return true; //ProjectExplorer::idFromMap(map) == D_BC_ID;
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

////////////////////////////////////////////////////////////////////////////////////
// DBuildSettingsWidget
////////////////////////////////////////////////////////////////////////////////////

DBuildSettingsWidget::DBuildSettingsWidget(DBuildConfiguration *bc)
 : m_buildConfiguration(bc)
{
 QFormLayout *fl = new QFormLayout(this);
 fl->setContentsMargins(0, -1, 0, -1);
 fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

 setDisplayName(tr("D language Project Manager"));
}


} // namespace Internal
} // namespace DProjectManager
