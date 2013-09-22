#ifndef DEDITOR_H
#define DEDITOR_H

#include "deditor_global.h"

#include <extensionsystem/iplugin.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <texteditor/texteditoractionhandler.h>
#include <find/searchresultwindow.h>

namespace DEditor {
namespace Internal {

class DEditorFactory;
class DTextEditorWidget;

class DEditorPlugin : public ExtensionSystem::IPlugin
{
 Q_OBJECT
 Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DEditor.json")

public:
 DEditorPlugin();
 virtual ~DEditorPlugin();

 // IPlugin
 bool initialize(const QStringList &arguments, QString *errorMessage = 0);
 void extensionsInitialized();
 ShutdownFlag aboutToShutdown();

 static DEditorPlugin *instance() { return m_instance; }

 void initializeEditor(DTextEditorWidget *editor);
 DEditorFactory *editorFactory() { return m_editorFactory; }
 TextEditor::TextEditorActionHandler *actionHandler() const { return m_actionHandler; }

private slots:
 void updateSearchResultsFont(const TextEditor::FontSettings &);
 void clearAssistCacheAction();

private:
 static DEditorPlugin* m_instance;
 DEditorFactory* m_editorFactory;
 TextEditor::TextEditorActionHandler *m_actionHandler;

 TextEditor::TextEditorSettings* m_settings;
 Find::SearchResultWindow *m_searchResultWindow;
};

} // namespace Internal
} // namespace DEditor

#endif // DEDITOR_H

