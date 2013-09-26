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
 Core::IEditor *duplicate(QWidget *parent);
 Core::Id id() const;
 bool open(QString *errorString, const QString &fileName, const QString &realFileName);

};

class DTextEditorWidget : public TextEditor::BaseTextEditorWidget
{
 Q_OBJECT

public:
 DTextEditorWidget(QWidget* parent);
 TextEditor::IAssistInterface *createAssistInterface(TextEditor::AssistKind assistKind,
                                                     TextEditor::AssistReason reason) const;
 void configure(const QString& mimeType);
 void configure(const Core::MimeType &mimeType);

public slots:
 virtual void unCommentSelection();

private slots:
 void configure();

signals:
 void configured(Core::IEditor *editor);

protected:
 TextEditor::BaseTextEditor *createEditor();

};

} // namespace Internal
} // namespace DEditor

#endif // DTEXTEDITOR_H
