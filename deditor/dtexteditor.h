#ifndef DTEXTEDITOR_H
#define DTEXTEDITOR_H

#include <texteditor/basetexteditor.h>
#include <texteditor/plaintexteditor.h>

namespace Core
{
class MimeType;
}

namespace DEditor {
namespace Internal {

class DTextEditorWidget;

class DTextEditor : public TextEditor::BaseTextEditor
{
 Q_OBJECT

public:
 DTextEditor(DTextEditorWidget* );

 bool duplicateSupported() const { return true; }
 Core::IEditor *duplicate();
 bool open(QString *errorString, const QString &fileName, const QString &realFileName);
	TextEditor::CompletionAssistProvider *completionAssistProvider();
};

class DTextEditorWidget : public TextEditor::BaseTextEditorWidget
{
 Q_OBJECT

public:
 DTextEditorWidget(QWidget* parent =0);
 TextEditor::IAssistInterface *createAssistInterface(TextEditor::AssistKind assistKind,
                                                     TextEditor::AssistReason reason) const;
public slots:
 virtual void unCommentSelection();

protected:
 TextEditor::BaseTextEditor *createEditor();

};

} // namespace Internal
} // namespace DEditor

#endif // DTEXTEDITOR_H
