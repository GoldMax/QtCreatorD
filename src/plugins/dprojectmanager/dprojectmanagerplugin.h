#ifndef DPROJECTMANAGERPLUGIN_H
#define DPROJECTMANAGERPLUGIN_H

#include "dprojectmanager_global.h"

#include <extensionsystem/iplugin.h>

namespace DProjectManager {

class DProjectManagerPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DProjectManager.json")

public:
	DProjectManagerPlugin();
	~DProjectManagerPlugin() override;

	bool initialize(const QStringList &arguments, QString *errorString) override;
	void extensionsInitialized() override;
};

} // namespace DProjectManager

#endif // DPROJECTMANAGERPLUGIN_H

