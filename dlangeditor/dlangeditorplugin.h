#ifndef DLANGEDITORPLUGIN_H
#define DLANGEDITORPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <find/searchresultwindow.h>

namespace DLangEditor {

class DLangEditorFactory;
class DLangTextEditorWidget;

class DLangEditorPlugin : public ExtensionSystem::IPlugin
{
 Q_OBJECT
 Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DLangEditor.json")

public:
 DLangEditorPlugin();
 virtual ~DLangEditorPlugin();

 // IPlugin
 bool initialize(const QStringList &arguments, QString *errorMessage = 0);
 void extensionsInitialized();
 ShutdownFlag aboutToShutdown() { return IPlugin::aboutToShutdown(); }

 static DLangEditorPlugin *instance() { return m_instance; }

 void initializeEditor(DLangTextEditorWidget *editor);
 DLangEditorFactory *editorFactory() { return m_editorFactory; }

private slots:
 void updateSearchResultsFont(const TextEditor::FontSettings &);

private:
 static DLangEditorPlugin* m_instance;
 DLangEditorFactory* m_editorFactory;

 TextEditor::TextEditorSettings* m_settings;
 Find::SearchResultWindow *m_searchResultWindow;
};

}
#endif // DLANGEDITORPLUGIN_H
