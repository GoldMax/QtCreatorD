#include "dprojectmanagerplugin.h"
#include "dprojectmanagerconstants.h"
#include "dprojectmanager.h"
#include "dprojectwizard.h"
#include "dbuildconfiguration.h"
#include "dmakestep.h"
#include "drunconfiguration.h"

#include <coreplugin/icore.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/command.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/selectablefilesmodel.h>

#include <utils/mimetypes/mimedatabase.h>

#include <QtPlugin>
#include <QDebug>


using namespace Core;
using namespace ProjectExplorer;
using namespace DProjectManager;

DProjectManagerPlugin::DProjectManagerPlugin()
{
	// Create your members
}

DProjectManagerPlugin::~DProjectManagerPlugin()
{
	// Unregister objects from the plugin manager's object pool
	// Delete members
}

bool DProjectManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	// Register objects in the plugin manager's object pool
	// Load settings
	// Add actions to menus
	// Connect to other plugins' signals
	// In the initialize function, a plugin can be sure that the plugins it
	// depends on have initialized their members.

	Q_UNUSED(arguments)
	Q_UNUSED(errorString)

	const QLatin1String mimetypesXml(":/dprojectmanager/DProjectManager.mimetypes.xml");
	Utils::MimeDatabase::addMimeTypes(mimetypesXml);

	addAutoReleasedObject(new DManager);
	addAutoReleasedObject(new DProjectWizard);
	addAutoReleasedObject(new DMakeStepFactory);
	addAutoReleasedObject(new DBuildConfigurationFactory);
	addAutoReleasedObject(new DRunConfigurationFactory);

	IWizardFactory::registerFactoryCreator([]() { return QList<IWizardFactory *>() << new DProjectWizard; });

	return true;
}

void DProjectManagerPlugin::extensionsInitialized()
{
	// Retrieve objects from the plugin manager's object pool
	// In the extensionsInitialized function, a plugin can be sure that all
	// plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag DProjectManagerPlugin::aboutToShutdown()
{
	// Save settings
	// Disconnect from signals that are not needed during shutdown
	// Hide UI (if you add UI that is not in the main window directly)
	return SynchronousShutdown;
}
