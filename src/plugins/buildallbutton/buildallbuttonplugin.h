#pragma once

#include "buildallbutton_global.h"

#include <extensionsystem/iplugin.h>

namespace BuildAllButton {
namespace Internal {

class BuildAllButtonPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "BuildAllButton.json")

public:
	BuildAllButtonPlugin();
	~BuildAllButtonPlugin();

	bool initialize(const QStringList &arguments, QString *errorString);
	void extensionsInitialized();

};

} // namespace Internal
} // namespace BuildAllButton
