#include "drunconfiguration.h"
#include "dprojectmanagerconstants.h"
#include "dmakestep.h"

#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/runconfiguration.h>

#include <projectexplorer/target.h>
#include <projectexplorer/project.h>
#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildsteplist.h>

using namespace Utils;
using namespace ProjectExplorer;

namespace DProjectManager
{
//-----------------------------------------------------------------------------
//--- DRunConfiguration
//-----------------------------------------------------------------------------
DRunConfiguration::DRunConfiguration(Target *target, Core::Id /*id*/) :
		CustomExecutableRunConfiguration(target, Core::Id(Constants::BUILDRUN_CONFIG_ID))
{
	setDefaultDisplayName(defaultDisplayName());

	setCommandLineGetter([this]
	{
		auto exec = this->aspect<ExecutableAspect>()->executable();
		CommandLine cmd = CommandLine(exec);
		auto argumentsAspect = this->aspect<ArgumentsAspect>();
		cmd.addArgs(argumentsAspect->arguments(macroExpander()), CommandLine::Raw);
		return cmd;
	});
}

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
	this->aspect<WorkingDirectoryAspect>()->setDefaultWorkingDirectory(outDir);

	outDir = outDir.pathAppended(makeStep->outFileName());
	this->aspect<ExecutableAspect>()->setExecutable(outDir);
}

bool DRunConfiguration::isConfigured() const
{
	//return this->aspect<ExecutableAspect>()->executable().isEmpty() == false;
	return true;
}

void DRunConfiguration::updateEnabledState()
{
	setEnabled(isConfigured());
}

//---------------------------------------------------------------------------------
//--- DRunConfigurationFactory
//---------------------------------------------------------------------------------
DRunConfigurationFactory::DRunConfigurationFactory() :
		FixedRunConfigurationFactory(QLatin1String("Build Run"))
{
	registerRunConfiguration<DRunConfiguration>(Constants::BUILDRUN_CONFIG_ID);
	addSupportedProjectType(Constants::DPROJECT_ID);
}

} // namespace DProjectManager

