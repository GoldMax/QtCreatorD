#include "dlangbuildconfiguration.h"

#include "dlangmakestep.h"
#include "dlangproject.h"
#include "buildrunconfig.h"

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

namespace DLangProjectManager {
namespace Internal {

const char DLang_BC_ID[] = "DLangProjectManager.DLangBuildConfiguration";

DLangBuildConfiguration::DLangBuildConfiguration(Target *parent)
 : BuildConfiguration(parent, Core::Id(DLang_BC_ID))
{
}

DLangBuildConfiguration::DLangBuildConfiguration(Target *parent, const Core::Id id)
 : BuildConfiguration(parent, id)
{
}

DLangBuildConfiguration::DLangBuildConfiguration(Target *parent, DLangBuildConfiguration *source) :
  BuildConfiguration(parent, source)
{
 cloneSteps(source);
}

NamedWidget *DLangBuildConfiguration::createConfigWidget()
{
 return new DLangBuildSettingsWidget(this);
}

//------------------------------------------------------------------------------------------------
//  class DLangBuildConfigurationFactory
//------------------------------------------------------------------------------------------------

DLangBuildConfigurationFactory::DLangBuildConfigurationFactory(QObject *parent) :
  IBuildConfigurationFactory(parent) { }

DLangBuildConfigurationFactory::~DLangBuildConfigurationFactory() { }

QList<Core::Id> DLangBuildConfigurationFactory::availableCreationIds(const Target *parent) const
{
 if (!canHandle(parent))
  return QList<Core::Id>();
 return QList<Core::Id>() << Core::Id(DLang_BC_ID);
}

QString DLangBuildConfigurationFactory::displayNameForId(const Core::Id id) const
{
 if (id == DLang_BC_ID)
  return tr("Build");
 return QString();
}

bool DLangBuildConfigurationFactory::canCreate(const Target *parent, const Core::Id id) const
{
 if (!canHandle(parent))
  return false;
 if (id == DLang_BC_ID)
  return true;
 return false;
}

BuildConfiguration *DLangBuildConfigurationFactory::create(Target *parent, const Core::Id id, const QString &name)
{
 if (!canCreate(parent, id))
  return 0;

 bool ok = true;
 QString buildConfigurationName = name == QLatin1String("Default build") ? QLatin1String("Debug") : name;
 if (buildConfigurationName.isNull())
  buildConfigurationName = QInputDialog::getText(0,
                                                 tr("New Configuration"),
                                                 tr("New configuration name:"),
                                                 QLineEdit::Normal,
                                                 QString(), &ok);
 buildConfigurationName = buildConfigurationName.trimmed();
 if (!ok || buildConfigurationName.isEmpty())
  return 0;

 DLangBuildConfiguration *bc = new DLangBuildConfiguration(parent);
 bc->setDisplayName(buildConfigurationName);

 // QString buildDirName = QLatin1String(".") + sep + QLatin1String("bin") + sep + buildConfigurationName.toLower();
 // QString sep(QDir::separator());
 // bc->setBuildDirectory(Utils::FileName::fromString(buildDirName));

 BuildStepList *buildSteps = bc->stepList(Constants::BUILDSTEPS_BUILD);
 Q_ASSERT(buildSteps);
 DLangMakeStep *makeStep = new DLangMakeStep(buildSteps);
 buildSteps->insertStep(0, makeStep);

 // TODO : configure clean step

 // BuildStepList *cleanSteps = bc->stepList(Constants::BUILDSTEPS_CLEAN);
 // Q_ASSERT(cleanSteps);
 // GenericProjectManager::Internal::GenericMakeStep *cleanMakeStep = new GenericProjectManager::Internal::GenericMakeStep(cleanSteps);
 // cleanSteps->insertStep(0, cleanMakeStep);
 // cleanMakeStep->setClean(true);

 if(parent->runConfigurations().length() == 0)
 {
  BuildRunConfig* run = new BuildRunConfig(parent);
  run->setDisplayName(QLatin1String("Build Run"));
  parent->addRunConfiguration(run);
 }

 return bc;
}

bool DLangBuildConfigurationFactory::canClone(const Target *parent, BuildConfiguration *source) const
{
 return canCreate(parent, source->id());
}

BuildConfiguration *DLangBuildConfigurationFactory::clone(Target *parent, BuildConfiguration *source)
{
 if (!canClone(parent, source))
  return 0;
 return new DLangBuildConfiguration(parent, qobject_cast<DLangBuildConfiguration *>(source));
}

bool DLangBuildConfigurationFactory::canRestore(const Target *parent, const QVariantMap &map) const
{
 return canCreate(parent, ProjectExplorer::idFromMap(map));
}

BuildConfiguration *DLangBuildConfigurationFactory::restore(Target *parent, const QVariantMap &map)
{
 if (!canRestore(parent, map))
  return 0;
 DLangBuildConfiguration *bc(new DLangBuildConfiguration(parent));
 if (bc->fromMap(map))
  return bc;
 delete bc;
 return 0;
}

bool DLangBuildConfigurationFactory::canHandle(const Target *t) const
{
 if (!t->project()->supportsKit(t->kit()))
  return false;
 return qobject_cast<DLangProject *>(t->project());
}

BuildConfiguration::BuildType DLangBuildConfiguration::buildType() const
{
 return Unknown;
}

////////////////////////////////////////////////////////////////////////////////////
// DLangBuildSettingsWidget
////////////////////////////////////////////////////////////////////////////////////

DLangBuildSettingsWidget::DLangBuildSettingsWidget(DLangBuildConfiguration *bc)
 : m_buildConfiguration(0)
{
 QFormLayout *fl = new QFormLayout(this);
 fl->setContentsMargins(0, -1, 0, -1);
 fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

 // build directory
 m_pathChooser = new Utils::PathChooser(this);
 m_pathChooser->setEnabled(true);
 fl->addRow(tr("Build directory:"), m_pathChooser);
 connect(m_pathChooser, SIGNAL(changed(QString)), this, SLOT(buildDirectoryChanged()));

 m_buildConfiguration = bc;
 m_pathChooser->setBaseDirectory(bc->target()->project()->projectDirectory());
 m_pathChooser->setEnvironment(bc->environment());
 m_pathChooser->setPath(m_buildConfiguration->rawBuildDirectory().toString());
 setDisplayName(tr("D language Project Manager"));

 connect(bc, SIGNAL(environmentChanged()), this, SLOT(environmentHasChanged()));
}

void DLangBuildSettingsWidget::buildDirectoryChanged()
{
 m_buildConfiguration->setBuildDirectory(Utils::FileName::fromString(m_pathChooser->rawPath()));
 //    BuildStepList *bsl = m_buildConfiguration->stepList(Constants::BUILDSTEPS_BUILD);
 //    for(int a = 0; a < bsl->count(); a++)
 //     (( bsl->at(a)->updateDetails();

}

void DLangBuildSettingsWidget::environmentHasChanged()
{
 m_pathChooser->setEnvironment(m_buildConfiguration->environment());
}

} // namespace Internal
} // namespace DLangProjectManager
