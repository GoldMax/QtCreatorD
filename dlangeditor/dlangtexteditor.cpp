#include "_dlangeditorconstants.h"
#include "dlangeditorplugin.h"
#include "dlangtexteditor.h"
//#include "dlangautocompleter.h"
//#include "dlangindenter.h"
//#include "dlangcompletionassist.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/basetexteditor.h>
#include <texteditor/plaintexteditor.h>

using namespace DLangEditor;
using namespace TextEditor;

DLangTextEditor::DLangTextEditor(DLangTextEditorWidget* editor)
 : BaseTextEditor(editor)
{
 setContext(Core::Context(DLangEditor::Constants::C_DLANGEDITOR_ID,
                          TextEditor::Constants::C_TEXTEDITOR));
}

Core::IEditor* DLangTextEditor::duplicate(QWidget *parent)
{
 DLangTextEditorWidget *newWidget = new DLangTextEditorWidget(parent);
 newWidget->duplicateFrom(editorWidget());
 DLangEditorPlugin::instance()->initializeEditor(newWidget);
 return newWidget->editor();
}

Core::Id DLangTextEditor::id() const
{
 //return Core::Constants::K_DEFAULT_TEXT_EDITOR_ID;
 return DLangEditor::Constants::C_DLANGEDITOR_ID;
}

DLangTextEditorWidget::DLangTextEditorWidget(QWidget *parent)
  : PlainTextEditorWidget(parent)
{

}

/*#include "dlangeditor.h"
#include "dlangeditoreditable.h"
#include "dlangeditorconstants.h"
#include "dlangeditorplugin.h"
#include "dlangautocompleter.h"
#include "dlangindenter.h"
#include "dlangcompletionassist.h"

//#include <glsl/glsllexer.h>
//#include <glsl/glslparser.h>
//#include <glsl/glslengine.h>
//#include <glsl/glslsemantic.h>
//#include <glsl/glslsymbols.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/id.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <extensionsystem/pluginmanager.h>
#include <texteditor/basetextdocument.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/syntaxhighlighter.h>
#include <texteditor/refactoroverlay.h>
#include <texteditor/generichighlighter/highlighter.h>
#include <qmldesigner/qmldesignerconstants.h>
#include <utils/changeset.h>
#include <utils/uncommentselection.h>

#include <QFileInfo>
#include <QSignalMapper>
#include <QTimer>
#include <QTextBlock>

#include <QMenu>
#include <QComboBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QToolBar>
#include <QTreeView>
#include <QSharedPointer>

using namespace DLang;
using namespace DLangEditor;
using namespace DLangEditor::Internal;

enum {
    UPDATE_DOCUMENT_DEFAULT_INTERVAL = 150
};

//namespace {

//class CreateRanges: protected DLang::Visitor
//{
//    QTextDocument *textDocument;
//    Document::Ptr glslDocument;

//public:
//    CreateRanges(QTextDocument *textDocument, Document::Ptr glslDocument)
//        : textDocument(textDocument), glslDocument(glslDocument) {}

//				void operator()(DLang::AST *ast) { accept(ast); }

//protected:
//				using DLang::Visitor::visit;

//				virtual void endVisit(DLang::CompoundStatementAST *ast)
//    {
//        if (ast->symbol) {
//            QTextCursor tc(textDocument);
//            tc.setPosition(ast->start);
//            tc.setPosition(ast->end, QTextCursor::KeepAnchor);
//            glslDocument->addRange(tc, ast->symbol);
//        }
//    }
//};

//} // end of anonymous namespace

Document::Document()
//    : _engine(0)
//    , _ast(0)
//    , _globalScope(0)
{

}

Document::~Document()
{
//    delete _globalScope;
//    delete _engine;
}

//DLang::Scope *Document::scopeAt(int position) const
//{
//    foreach (const Range &c, _cursors) {
//        if (position >= c.cursor.selectionStart() && position <= c.cursor.selectionEnd())
//            return c.scope;
//    }
//    return _globalScope;
//}

//void Document::addRange(const QTextCursor &cursor, DLang::Scope *scope)
//{
//    Range c;
//    c.cursor = cursor;
//    c.scope = scope;
//    _cursors.append(c);
//}


DLangEditorEditable::DLangEditorEditable(DLangTextEditorWidget *editor)
    : BaseTextEditor(editor)
{
    setContext(Core::Context(DLangEditor::Constants::C_DLANGEDITOR_ID,
                             TextEditor::Constants::C_TEXTEDITOR));
}

DLangTextEditorWidget::DLangTextEditorWidget(QWidget *parent) :
    TextEditor::PlainTextEditorWidget(parent),
    m_outlineCombo(0)
{
 setParenthesesMatchingEnabled(true);
 setMarksVisible(true);
 setCodeFoldingSupported(true);
 setIndenter(new DLangIndenter());
 setAutoCompleter(new DLangCompleter());

 m_updateDocumentTimer = new QTimer(this);
 m_updateDocumentTimer->setInterval(UPDATE_DOCUMENT_DEFAULT_INTERVAL);
 m_updateDocumentTimer->setSingleShot(true);
 connect(m_updateDocumentTimer, SIGNAL(timeout()), this, SLOT(updateDocumentNow()));

 connect(this, SIGNAL(textChanged()), this, SLOT(updateDocument()));

 //-- Highlighter init --

 //    if (m_modelManager) {
 //        m_semanticHighlighter->setModelManager(m_modelManager);
 //        connect(m_modelManager, SIGNAL(documentUpdated(DLang::Document::Ptr)),
 //                this, SLOT(onDocumentUpdated(DLang::Document::Ptr)));
 //        connect(m_modelManager, SIGNAL(libraryInfoUpdated(QString,DLang::LibraryInfo)),
 //                this, SLOT(forceSemanticRehighlight()));
 //        connect(this->document(), SIGNAL(modificationChanged(bool)), this, SLOT(modificationChanged(bool)));
 //    }
}

DLangTextEditorWidget::~DLangTextEditorWidget()
{
}

bool DLangEditorEditable::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
 editorWidget()->setMimeType(Core::MimeDatabase::findByFile(QFileInfo(fileName)).type());
 bool b = TextEditor::BaseTextEditor::open(errorString, fileName, realFileName);
 return b;
}



int DLangTextEditorWidget::editorRevision() const
{
    //return document()->revision();
    return 0;
}

bool DLangTextEditorWidget::isOutdated() const
{
//    if (m_semanticInfo.revision() != editorRevision())
//        return true;

    return false;
}

Core::IEditor *DLangEditorEditable::duplicate(QWidget *parent)
{
				DLangTextEditorWidget *newEditor = new DLangTextEditorWidget(parent);
    newEditor->duplicateFrom(editorWidget());
				DLangEditorPlugin::instance()->initializeEditor(newEditor);
    return newEditor->editor();
}

Core::Id DLangEditorEditable::id() const
{
				return DLangEditor::Constants::C_DLANGEDITOR_ID;
}

QString DLangTextEditorWidget::wordUnderCursor() const
{
    QTextCursor tc = textCursor();
    const QChar ch = document()->characterAt(tc.position() - 1);
    // make sure that we're not at the start of the next word.
    if (ch.isLetterOrNumber() || ch == QLatin1Char('_'))
        tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::StartOfWord);
    tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    const QString word = tc.selectedText();
    return word;
}

TextEditor::BaseTextEditor *DLangTextEditorWidget::createEditor()
{
				DLangEditorEditable *editable = new DLangEditorEditable(this);
    createToolBar(editable);
    return editable;
}

void DLangTextEditorWidget::createToolBar(DLangEditorEditable *editor)
{
    m_outlineCombo = new QComboBox;
    m_outlineCombo->setMinimumContentsLength(22);

    // ### m_outlineCombo->setModel(m_outlineModel);

    QTreeView *treeView = new QTreeView;
    treeView->header()->hide();
    treeView->setItemsExpandable(false);
    treeView->setRootIsDecorated(false);
    m_outlineCombo->setView(treeView);
    treeView->expandAll();

    //m_outlineCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    // Make the combo box prefer to expand
    QSizePolicy policy = m_outlineCombo->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    m_outlineCombo->setSizePolicy(policy);

    editor->insertExtraToolBarWidget(TextEditor::BaseTextEditor::Left, m_outlineCombo);
}

bool DLangTextEditorWidget::event(QEvent *e)
{
    return BaseTextEditorWidget::event(e);
}

void DLangTextEditorWidget::unCommentSelection()
{
    Utils::unCommentSelection(this);
}

void DLangTextEditorWidget::updateDocument()
{
    m_updateDocumentTimer->start();
}

void DLangTextEditorWidget::updateDocumentNow()
{
    m_updateDocumentTimer->stop();

//    int variant = languageVariant(mimeType());
//    const QString contents = toPlainText(); // get the code from the editor
//    const QByteArray preprocessedCode = contents.toLatin1(); // ### use the QtCreator C++ preprocessor.

//    Document::Ptr doc(new Document());
//				DLang::Engine *engine = new DLang::Engine();
//				doc->_engine = new DLang::Engine();
//    Parser parser(doc->_engine, preprocessedCode.constData(), preprocessedCode.size(), variant);
//    TranslationUnitAST *ast = parser.parse();
//    if (ast != 0 || extraSelections(CodeWarningsSelection).isEmpty()) {
//								DLangEditorPlugin *plugin = DLangEditorPlugin::instance();

//        Semantic sem;
//        Scope *globalScope = engine->newNamespace();
//        doc->_globalScope = globalScope;
//        sem.translationUnit(plugin->shaderInit(variant)->ast, globalScope, plugin->shaderInit(variant)->engine);
//        if (variant & Lexer::Variant_VertexShader)
//            sem.translationUnit(plugin->vertexShaderInit(variant)->ast, globalScope, plugin->vertexShaderInit(variant)->engine);
//        if (variant & Lexer::Variant_FragmentShader)
//            sem.translationUnit(plugin->fragmentShaderInit(variant)->ast, globalScope, plugin->fragmentShaderInit(variant)->engine);
//        sem.translationUnit(ast, globalScope, engine);

//        CreateRanges createRanges(document(), doc);
//        createRanges(ast);

//        QTextCharFormat errorFormat;
//        errorFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
//        errorFormat.setUnderlineColor(Qt::red);

//        QTextCharFormat warningFormat;
//        warningFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
//        warningFormat.setUnderlineColor(Qt::darkYellow);

//        QList<QTextEdit::ExtraSelection> sels;
//        QSet<int> errors;

//        foreach (const DiagnosticMessage &m, engine->diagnosticMessages()) {
//            if (! m.line())
//                continue;
//            else if (errors.contains(m.line()))
//                continue;

//            errors.insert(m.line());

//            QTextCursor cursor(document()->findBlockByNumber(m.line() - 1));
//            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

//            QTextEdit::ExtraSelection sel;
//            sel.cursor = cursor;
//            sel.format = m.isError() ? errorFormat : warningFormat;
//            sel.format.setToolTip(m.message());
//            sels.append(sel);
//        }

//        setExtraSelections(CodeWarningsSelection, sels);
//        m_glslDocument = doc;
//    }
}

int DLangTextEditorWidget::languageVariant(const QString &type)
{
    int variant = 0;
//    bool isVertex = false;
//    bool isFragment = false;
//    bool isDesktop = false;
//    if (type.isEmpty()) {
//        // ### Before file has been opened, so don't know the mime type.
//        isVertex = true;
//        isFragment = true;
//    } else if (type == QLatin1String("text/x-glsl") ||
//               type == QLatin1String("application/x-glsl")) {
//        isVertex = true;
//        isFragment = true;
//        isDesktop = true;
//    } else if (type == QLatin1String("text/x-glsl-vert")) {
//        isVertex = true;
//        isDesktop = true;
//    } else if (type == QLatin1String("text/x-glsl-frag")) {
//        isFragment = true;
//        isDesktop = true;
//    } else if (type == QLatin1String("text/x-glsl-es-vert")) {
//        isVertex = true;
//    } else if (type == QLatin1String("text/x-glsl-es-frag")) {
//        isFragment = true;
//    }
//    if (isDesktop)
//								variant |= Lexer::Variant_DLang_120;
//    else
//								variant |= Lexer::Variant_DLang_ES_100;
//    if (isVertex)
//        variant |= Lexer::Variant_VertexShader;
//    if (isFragment)
//        variant |= Lexer::Variant_FragmentShader;
    return variant;
}

Document::Ptr DLangTextEditorWidget::dDocument() const
{
				return m_dDocument;
}

TextEditor::IAssistInterface *DLangTextEditorWidget::createAssistInterface(
    TextEditor::AssistKind kind,
    TextEditor::AssistReason reason) const
{
    if (kind == TextEditor::Completion)
								return new DLangCompletionAssistInterface(document(),
                                                 position(),
                                                 editor()->document()->filePath(),
                                                 reason,
                                                 mimeType(),
																																																	dDocument());
    return BaseTextEditorWidget::createAssistInterface(kind, reason);
}
*/
