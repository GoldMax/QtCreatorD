#ifndef DLangPROJECTFILESEDITOR_H
#define DLangPROJECTFILESEDITOR_H

#include <texteditor/basetexteditor.h>
#include <texteditor/basetextdocument.h>

#include <coreplugin/editormanager/ieditorfactory.h>

namespace TextEditor {
class TextEditorActionHandler;
}

namespace DLangProjectManager {
namespace Internal {

class Manager;
class ProjectFilesEditor;
class ProjectFilesEditorWidget;
class ProjectFilesFactory;

class ProjectFilesFactory: public Core::IEditorFactory
{
    Q_OBJECT

public:
    ProjectFilesFactory(Manager *manager, TextEditor::TextEditorActionHandler *handler);

    Core::IEditor *createEditor(QWidget *parent);

private:
    TextEditor::TextEditorActionHandler *m_actionHandler;
};

class ProjectFilesEditor : public TextEditor::BaseTextEditor
{
    Q_OBJECT

public:
    ProjectFilesEditor(ProjectFilesEditorWidget *editorWidget);

    Core::Id id() const;
    bool duplicateSupported() const;
    Core::IEditor *duplicate(QWidget *parent);
};

class ProjectFilesEditorWidget : public TextEditor::BaseTextEditorWidget
{
    Q_OBJECT

public:
    ProjectFilesEditorWidget(QWidget *parent, ProjectFilesFactory *factory,
                       TextEditor::TextEditorActionHandler *handler);

    ProjectFilesFactory *factory() const;
    TextEditor::TextEditorActionHandler *actionHandler() const;
    TextEditor::BaseTextEditor *createEditor();

private:
    ProjectFilesFactory *m_factory;
    TextEditor::TextEditorActionHandler *m_actionHandler;
};

} // namespace Internal
} // namespace DLangProjectManager

#endif // DLangPROJECTFILESEDITOR_H
