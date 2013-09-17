#include "dlangprojectplugin.h"

#include "dlangbuildconfiguration.h"
#include "dlangprojectmanager.h"
#include "dlangprojectwizard.h"
#include "dlangprojectconstants.h"
#include "dlangmakestep.h"
#include "dlangproject.h"

#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectexplorer.h>


#include <texteditor/texteditoractionhandler.h>

#include <QtPlugin>
#include <QDebug>

namespace DLangProjectManager {
namespace Internal {

DLangProjectPlugin::DLangProjectPlugin(){ }

DLangProjectPlugin::~DLangProjectPlugin(){}

bool DLangProjectPlugin::initialize(const QStringList &, QString *errorMessage)
{
    using namespace Core;

    const QLatin1String mimetypesXml(":/dlangproject/DLangProjectManager.mimetypes.xml");
    if (!MimeDatabase::addMimeTypes(mimetypesXml, errorMessage))
        return false;

    Manager *manager = new Manager;
    addAutoReleasedObject(manager);
    addAutoReleasedObject(new DLangMakeStepFactory);
    addAutoReleasedObject(new DLangProjectWizard);
    addAutoReleasedObject(new DLangBuildConfigurationFactory);

    return true;
}

void DLangProjectPlugin::extensionsInitialized()
{ }

} // namespace Internal
} // namespace DLangProjectManager

Q_EXPORT_PLUGIN(DLangProjectManager::Internal::DLangProjectPlugin)
