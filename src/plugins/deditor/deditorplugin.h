#ifndef DEDITOR_H
#define DEDITOR_H

#include "deditor_global.h"

#include <extensionsystem/iplugin.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <texteditor/texteditoractionhandler.h>

namespace DEditor {

class DEditorFactory;
class DEditorWidget;

class DEditorPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DEditor.json")

public:
	DEditorPlugin();
	virtual ~DEditorPlugin();

	// IPlugin
	bool initialize(const QStringList &arguments, QString *errorMessage = nullptr);
	void extensionsInitialized();
	ShutdownFlag aboutToShutdown();

	static DEditorPlugin *instance() { return m_instance; }

private slots:
	void toggleDCDAction();
	void clearAssistCacheAction();

private:
	static DEditorPlugin* m_instance;
};

} // namespace DEditor

#endif // DEDITOR_H

