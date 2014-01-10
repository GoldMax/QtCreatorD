#ifndef BUILDALLBUTTON_H
#define BUILDALLBUTTON_H

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
	ShutdownFlag aboutToShutdown();

};

} // namespace Internal
} // namespace BuildAllButton

#endif // BUILDALLBUTTON_H

