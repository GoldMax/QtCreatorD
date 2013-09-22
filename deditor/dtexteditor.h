#ifndef DTEXTEDITOR_H
#define DTEXTEDITOR_H

#include <texteditor/basetexteditor.h>
#include <texteditor/plaintexteditor.h>


namespace DEditor {
namespace Internal {

class DTextEditorWidget;

class DTextEditor : public TextEditor::BaseTextEditor
{
 Q_OBJECT

public:
 DTextEditor(DTextEditorWidget* );

 bool duplicateSupported() const { return true; }
 Core::IEditor *duplicate(QWidget *parent);
 Core::Id id() const;
};

class DTextEditorWidget : public TextEditor::PlainTextEditorWidget // TextEditor::BaseTextEditorWidget
{
    Q_OBJECT

public:
    DTextEditorWidget(QWidget* parent);
    TextEditor::BaseTextEditor *createEditor();
    TextEditor::IAssistInterface *createAssistInterface(TextEditor::AssistKind assistKind,
                                                        TextEditor::AssistReason reason) const;
};

} // namespace Internal
} // namespace DEditor

#endif // DTEXTEDITOR_H
