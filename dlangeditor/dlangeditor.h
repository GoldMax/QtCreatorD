#ifndef DLangEDITOR_H
#define DLangEDITOR_H

#include <texteditor/basetexteditor.h>

#include <QSharedPointer>
#include <QSet>

QT_BEGIN_NAMESPACE
class QComboBox;
class QTimer;
QT_END_NAMESPACE

//namespace DLang {
//class Engine;
//class TranslationUnitAST;
//class Scope;
//} // namespace DLang

namespace Core { class ICore; }

namespace DLangEditor {
namespace Internal {

class DLangEditorEditable;
class DLangTextEditorWidget;

class Document
{
public:
    typedef QSharedPointer<Document> Ptr;

    Document();
    ~Document();

//				DLang::Engine *engine() const { return _engine; }
//				DLang::TranslationUnitAST *ast() const { return _ast; }
//				DLang::Scope *globalScope() const { return _globalScope; }

//				DLang::Scope *scopeAt(int position) const;
//				void addRange(const QTextCursor &cursor, DLang::Scope *scope);

private:
//    struct Range {
//        QTextCursor cursor;
//								DLang::Scope *scope;
//    };

//				DLang::Engine *_engine;
//				DLang::TranslationUnitAST *_ast;
//				DLang::Scope *_globalScope;
//    QList<Range> _cursors;

				friend class DLangTextEditorWidget;
};

class DLangTextEditorWidget : public TextEditor::BaseTextEditorWidget
{
    Q_OBJECT

public:
				DLangTextEditorWidget(QWidget *parent = 0);
				~DLangTextEditorWidget();

    virtual void unCommentSelection();

    int editorRevision() const;
    bool isOutdated() const;

    QSet<QString> identifiers() const;

    static int languageVariant(const QString &mimeType);

				Document::Ptr dDocument() const;

    TextEditor::IAssistInterface *createAssistInterface(TextEditor::AssistKind assistKind,
                                                        TextEditor::AssistReason reason) const;

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
				Document::Ptr m_dDocument;
};

} // namespace Internal
} // namespace DLangEditor

#endif // DLangEDITOR_H
