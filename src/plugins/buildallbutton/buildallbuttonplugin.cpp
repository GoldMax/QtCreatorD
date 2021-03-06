#include "buildallbuttonplugin.h"
#include "buildallbuttonconstants.h"

#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectexplorericons.h>
#include <projectexplorer/projectexplorer_export.h>
#include <coreplugin/modemanager.h>

#include <QAction>
#include <QIcon>
#include <QtPlugin>

using namespace BuildAllButton::Internal;
using namespace Core;

BuildAllButtonPlugin::BuildAllButtonPlugin()
{
	// Create your members
}

BuildAllButtonPlugin::~BuildAllButtonPlugin()
{
	// Unregister objects from the plugin manager's object pool
	// Delete members
}

bool BuildAllButtonPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	// Register objects in the plugin manager's object pool
	// Load settings
	// Add actions to menus
	// Connect to other plugins' signals
	// In the initialize function, a plugin can be sure that the plugins it
	// depends on have initialized their members.

	Q_UNUSED(arguments)
	Q_UNUSED(errorString)

 QIcon rebuildIcon = ProjectExplorer::Icons::REBUILD.icon();

	Command* cmd = ActionManager::command(Constants::BUILDSESSION);
	if(cmd)
	{
		cmd->action()->setIcon(rebuildIcon);
		ModeManager::addAction(cmd->action(), Constants::P_ACTION_BUILDPROJECT-1);
	}

	return true;
}

void BuildAllButtonPlugin::extensionsInitialized()
{
	// Retrieve objects from the plugin manager's object pool
	// In the extensionsInitialized function, a plugin can be sure that all
	// plugins that depend on it are completely initialized.
}

