#ifndef DLANGEDITORFACTORY_H
#define DLANGEDITORFACTORY_H

#include <coreplugin/editormanager/ieditorfactory.h>
#include <texteditor/texteditoractionhandler.h>

namespace DLangEditor {

using namespace TextEditor;

class DLangEditorFactory : public Core::IEditorFactory
{
 Q_OBJECT

public:
 // parent is DLangEditorPlugin
 DLangEditorFactory(QObject *parent);
 ~DLangEditorFactory();

 using Core::IEditorFactory::addMimeType;
 Core::IEditor *createEditor(QWidget *parent);
 TextEditor::TextEditorActionHandler *actionHandler() const { return m_actionHandler; }

private slots:
 void updateEditorInfoBar(Core::IEditor *editor);

private:
 TextEditor::TextEditorActionHandler* m_actionHandler;
};

} // namespace DLangEditor

#endif // DLANGEDITORFACTORY_H
