#include "dprojectmanagerplugin.h"

#include "dprojectmanagerconstants.h"
#include "dprojectmanager.h"
#include "dproject.h"
#include "dprojectwizard.h"
#include "dbuildconfiguration.h"
#include "dmakestep.h"
#include "drunconfiguration.h"

#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectexplorer.h>


#include <texteditor/texteditoractionhandler.h>

#include <QtPlugin>
//#include <QDebug>

namespace DProjectManager {

DProjectPlugin::DProjectPlugin(){ }

DProjectPlugin::~DProjectPlugin(){}

bool DProjectPlugin::initialize(const QStringList &, QString *errorMessage)
{
 using namespace Core;

 const QLatin1String mimetypesXml(":/dprojectmanager/DProjectManager.mimetypes.xml");
 if (!MimeDatabase::addMimeTypes(mimetypesXml, errorMessage))
  return false;

 addAutoReleasedObject(new Manager);
 addAutoReleasedObject(new DProjectWizard);
 addAutoReleasedObject(new DMakeStepFactory);
 addAutoReleasedObject(new DBuildConfigurationFactory);
 addAutoReleasedObject(new DRunConfigurationFactory);

 return true;
}

void DProjectPlugin::extensionsInitialized() { }

} // namespace DProjectManager

Q_EXPORT_PLUGIN(DProjectManager::DProjectPlugin)
