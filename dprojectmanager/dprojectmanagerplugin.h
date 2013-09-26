#ifndef DPROJECTMANAGERPLUGIN_H
#define DPROJECTMANAGERPLUGIN_H

#include "dprojectmanager_global.h"

#include <extensionsystem/iplugin.h>

namespace ProjectExplorer {
class Project;
class Node;
}

namespace DProjectManager {
namespace Internal {

class ProjectFilesFactory;

class DProjectPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DProjectManager.json")

public:
    DProjectPlugin();
    ~DProjectPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
};

} // namespace Internal
} // namespace DProjectManager

#endif // DPROJECTMANAGERPLUGIN_H

