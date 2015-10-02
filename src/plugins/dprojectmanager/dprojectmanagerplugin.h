#ifndef DPROJECTMANAGERPLUGIN_H
#define DPROJECTMANAGERPLUGIN_H

#include "dprojectmanager_global.h"

#include <extensionsystem/iplugin.h>

namespace ProjectExplorer
{
	class Project;
	class Node;
}

namespace DProjectManager {

class ProjectFilesFactory;

class DProjectManagerPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DProjectManager.json")

public:
	DProjectManagerPlugin();
	~DProjectManagerPlugin();

	bool initialize(const QStringList &arguments, QString *errorString);
	void extensionsInitialized();
	ShutdownFlag aboutToShutdown();
};

} // namespace DProjectManager

#endif // DPROJECTMANAGERPLUGIN_H

