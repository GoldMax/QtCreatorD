#ifndef DEDITORFACTORY_H
#define DEDITORFACTORY_H

#include <coreplugin/editormanager/ieditorfactory.h>

namespace DEditor {
namespace Internal {


//using namespace TextEditor;

class DEditorFactory : public Core::IEditorFactory
{
 Q_OBJECT

public:
 // parent is DEditorPlugin
 DEditorFactory(QObject *parent);
 ~DEditorFactory();

 using Core::IEditorFactory::addMimeType;
 Core::IEditor *createEditor(QWidget *parent);

private slots:
 void updateEditorInfoBar(Core::IEditor *editor);

private:
 //TextEditor::TextEditorActionHandler* m_actionHandler;
};

} // namespace Internal
} // namespace DEditor

#endif // DEDITORFACTORY_H
