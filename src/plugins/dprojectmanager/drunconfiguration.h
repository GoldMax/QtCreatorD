#ifndef DRUNCONFIG_H
#define DRUNCONFIG_H

#include "dmakestep.h"

#include <projectexplorer/customexecutablerunconfiguration.h>

using namespace ProjectExplorer;

namespace DProjectManager {

class DRunConfiguration : public ProjectExplorer::CustomExecutableRunConfiguration
{
	Q_OBJECT

public:
	DRunConfiguration(Target *parent, Core::Id id);

	QString defaultDisplayName() const { return tr("Build Run"); }

	void updateConfig(const DMakeStep* makeStep);
	bool isConfigured() const override;
	void updateEnabledState() override;
};

class DRunConfigurationFactory : public FixedRunConfigurationFactory
{
public:
	explicit DRunConfigurationFactory();
};

} // namespace DProjectManager
#endif // DRUNCONFIG_H
