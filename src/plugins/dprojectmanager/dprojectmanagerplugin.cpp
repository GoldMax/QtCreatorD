#include "dprojectmanagerplugin.h"
#include "dprojectmanagerconstants.h"
#include "dproject.h"
//#include "dprojectmanager.h"
#include "dprojectwizard.h"
//#include "dbuildconfiguration.h"
//#include "dmakestep.h"
//#include "drunconfiguration.h"

#include <coreplugin/icore.h>
//#include <coreplugin/actionmanager/actionmanager.h>
//#include <coreplugin/actionmanager/actioncontainer.h>
//#include <coreplugin/actionmanager/command.h>

//#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectmanager.h>
//#include <projectexplorer/projecttree.h>
//#include <projectexplorer/selectablefilesmodel.h>

#include <utils/stringutils.h>
#include <utils/mimetypes/mimedatabase.h>

//#include <QtPlugin>
//#include <QDebug>


using namespace Core;
using namespace ProjectExplorer;
using namespace DProjectManager;


class DProjectPluginPrivate : public QObject
{
public:
				DProjectPluginPrivate();

//    ProjectFilesFactory projectFilesFactory;
//    GenericMakeAllStepFactory makeAllStepFactory;
//    GenericMakeCleanStepFactory makeCleanStepFactory;
//    GenericBuildConfigurationFactory buildConfigFactory;

//    QAction editFilesAction{GenericProjectPlugin::tr("Edit Files..."), nullptr};
};
DProjectPluginPrivate::DProjectPluginPrivate()
{
	ProjectManager::registerProjectType<DProject>(Constants::DPROJECT_MIMETYPE);
	IWizardFactory::registerFactoryCreator([]()
	{
		return QList<IWizardFactory *>{ new DProjectWizard };
	});

}

static DProjectPluginPrivate *dd = nullptr;

DProjectManagerPlugin::DProjectManagerPlugin()
{
}

DProjectManagerPlugin::~DProjectManagerPlugin()
{
	delete dd;
	dd = nullptr;
}

bool DProjectManagerPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	Q_UNUSED(arguments)
	Q_UNUSED(errorString)

	const QLatin1String mimetypesXml(":/dprojectmanager/DProjectManager.mimetypes.xml");
	QFile f(mimetypesXml);
	QByteArray data;
	if(f.open(QIODevice::OpenModeFlag::ReadOnly))
	{
		data = f.readAll();
		f.close();
	}
	if(data.isEmpty() == false)
		Utils::addMimeTypes(Constants::DPROJECT_ID,data);

//	addAutoReleasedObject(new DManager);
//	addAutoReleasedObject(new DMakeStepFactory);
//	addAutoReleasedObject(new DBuildConfigurationFactory);
//	addAutoReleasedObject(new DRunConfigurationFactory);
	dd = new DProjectPluginPrivate;

	return true;
}

void DProjectManagerPlugin::extensionsInitialized()
{
	// Retrieve objects from the plugin manager's object pool
	// In the extensionsInitialized function, a plugin can be sure that all
	// plugins that depend on it are completely initialized.
}
