#pragma once

#include "deditor_global.h"

#include <extensionsystem/iplugin.h>

namespace DEditor {

class DEditorPluginPrivate;

class DEditorPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DEditor.json")

public:
	DEditorPlugin();
	virtual ~DEditorPlugin();

	static DEditorPlugin *instance() { return m_instance; }

	// IPlugin
	bool initialize(const QStringList &arguments, QString *errorMessage = nullptr);
	void extensionsInitialized();
	ShutdownFlag aboutToShutdown();


private slots:
	void toggleDCDAction();
	void clearAssistCacheAction();

private:
	static DEditorPlugin* m_instance;
	DEditorPluginPrivate* d = nullptr;
};

} // namespace DEditor
