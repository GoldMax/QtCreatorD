#ifndef DLANGEDITOR_H
#define DLANGEDITOR_H

#include <texteditor/basetexteditor.h>
#include <texteditor/plaintexteditor.h>


namespace DLangEditor {

class DLangTextEditorWidget;

class DLangTextEditor : public TextEditor::BaseTextEditor
{
 Q_OBJECT

public:
 DLangTextEditor(DLangTextEditorWidget* );

 bool duplicateSupported() const { return true; }
 Core::IEditor *duplicate(QWidget *parent);
 Core::Id id() const;
};

class DLangTextEditorWidget : public TextEditor::PlainTextEditorWidget // TextEditor::BaseTextEditorWidget
{
    Q_OBJECT

public:
    DLangTextEditorWidget(QWidget* parent);
    TextEditor::BaseTextEditor *createEditor();
    TextEditor::IAssistInterface *createAssistInterface(TextEditor::AssistKind assistKind,
                                                        TextEditor::AssistReason reason) const;


    /*virtual void unCommentSelection();

    int editorRevision() const;
    bool isOutdated() const;

    QSet<QString> identifiers() const;

    static int languageVariant(const QString &mimeType);

				Document::Ptr dDocument() const;


private slots:
    void updateDocument();
    void updateDocumentNow();

protected:
    bool event(QEvent *e);
    TextEditor::BaseTextEditor *createEditor();
				void createToolBar(Internal::DLangEditorEditable *editable);

private:
    void setSelectedElements();
    QString wordUnderCursor() const;

    QTimer *m_updateDocumentTimer;
    QComboBox *m_outlineCombo;
    Document::Ptr m_dDocument;*/
};


} // namespace DLangEditor

#endif // DLANGEDITOR_H
