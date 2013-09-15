#include "dlangcompletionassist.h"
#include "dlangeditorconstants.h"
#include "dlangeditorplugin.h"
#include "reuse.h"

#include <glsl/glslengine.h>
#include <glsl/glsllexer.h>
#include <glsl/glslparser.h>
#include <glsl/glslsemantic.h>
#include <glsl/glslsymbols.h>
#include <glsl/glslastdump.h>

#include <coreplugin/idocument.h>
#include <texteditor/completionsettings.h>
#include <texteditor/codeassist/basicproposalitem.h>
#include <texteditor/codeassist/basicproposalitemlistmodel.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/functionhintproposal.h>
#include <cplusplus/ExpressionUnderCursor.h>
#include <utils/faketooltip.h>

#include <QIcon>
#include <QPainter>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

using namespace DLangEditor;
using namespace Internal;
using namespace TextEditor;

namespace {

enum CompletionOrder {
    SpecialMemberOrder = -5
};


bool isActivationChar(const QChar &ch)
{
    return ch == QLatin1Char('(') || ch == QLatin1Char('.') || ch == QLatin1Char(',');
}

bool isIdentifierChar(QChar ch)
{
    return ch.isLetterOrNumber() || ch == QLatin1Char('_');
}

bool isDelimiter(QChar ch)
{
    switch (ch.unicode()) {
    case '{':
    case '}':
    case '[':
    case ']':
    case ')':
    case '?':
    case '!':
    case ':':
    case ';':
    case ',':
    case '+':
    case '-':
    case '*':
    case '/':
        return true;

    default:
        return false;
    }
}

bool checkStartOfIdentifier(const QString &word)
{
    if (! word.isEmpty()) {
        const QChar ch = word.at(0);
        if (ch.isLetter() || ch == QLatin1Char('_'))
            return true;
    }

    return false;
}

} // Anonymous

// ----------------------------
// DLangCompletionAssistProvider
// ----------------------------
bool DLangCompletionAssistProvider::supportsEditor(const Core::Id &editorId) const
{
				return editorId == Constants::C_DLANGEDITOR_ID;
}

IAssistProcessor *DLangCompletionAssistProvider::createProcessor() const
{
				return new DLangCompletionAssistProcessor;
}

int DLangCompletionAssistProvider::activationCharSequenceLength() const
{
    return 1;
}

bool DLangCompletionAssistProvider::isActivationCharSequence(const QString &sequence) const
{
    return isActivationChar(sequence.at(0));
}

// -----------------------------
// DLangFunctionHintProposalModel
// -----------------------------
class DLangFunctionHintProposalModel : public TextEditor::IFunctionHintProposalModel
{
public:
				DLangFunctionHintProposalModel(QVector<DLang::Function *> functionSymbols)
        : m_items(functionSymbols)
        , m_currentArg(-1)
    {}

    virtual void reset() {}
    virtual int size() const { return m_items.size(); }
    virtual QString text(int index) const;
    virtual int activeArgument(const QString &prefix) const;

private:
				QVector<DLang::Function *> m_items;
    mutable int m_currentArg;
};

QString DLangFunctionHintProposalModel::text(int index) const
{
				return QLatin1String("DLangFunctionHintProposalModel::text"); //m_items.at(index)->prettyPrint(m_currentArg);
}

int DLangFunctionHintProposalModel::activeArgument(const QString &prefix) const
{
    const QByteArray &str = prefix.toLatin1();
    int argnr = 0;
//    int parcount = 0;
//				DLang::Lexer lexer(0, str.constData(), str.length());
//				DLang::Token tk;
//				QList<DLang::Token> tokens;
//    do {
//        lexer.yylex(&tk);
//        tokens.append(tk);
//				} while (tk.isNot(DLang::Parser::EOF_SYMBOL));
//    for (int i = 0; i < tokens.count(); ++i) {
//								const DLang::Token &tk = tokens.at(i);
//								if (tk.is(DLang::Parser::T_LEFT_PAREN))
//            ++parcount;
//								else if (tk.is(DLang::Parser::T_RIGHT_PAREN))
//            --parcount;
//								else if (! parcount && tk.is(DLang::Parser::T_COMMA))
//            ++argnr;
//    }

//    if (parcount < 0)
//        return -1;

//    if (argnr != m_currentArg)
//        m_currentArg = argnr;

    return argnr;
}

// -----------------------------
// DLangCompletionAssistProcessor
// -----------------------------
DLangCompletionAssistProcessor::DLangCompletionAssistProcessor()
    : m_startPosition(0)
    , m_keywordIcon(QLatin1String(":/glsleditor/images/keyword.png"))
    , m_varIcon(QLatin1String(":/glsleditor/images/var.png"))
    , m_functionIcon(QLatin1String(":/glsleditor/images/func.png"))
    , m_typeIcon(QLatin1String(":/glsleditor/images/type.png"))
    , m_constIcon(QLatin1String(":/glsleditor/images/const.png"))
    , m_attributeIcon(QLatin1String(":/glsleditor/images/attribute.png"))
    , m_uniformIcon(QLatin1String(":/glsleditor/images/uniform.png"))
    , m_varyingIcon(QLatin1String(":/glsleditor/images/varying.png"))
    , m_otherIcon(QLatin1String(":/glsleditor/images/other.png"))
{}

DLangCompletionAssistProcessor::~DLangCompletionAssistProcessor()
{}

IAssistProposal *DLangCompletionAssistProcessor::perform(const IAssistInterface *interface)
{
				m_interface.reset(static_cast<const DLangCompletionAssistInterface *>(interface));

//    if (interface->reason() == IdleEditor && !acceptsIdleEditor())
//        return 0;

//    int pos = m_interface->position() - 1;
//    QChar ch = m_interface->characterAt(pos);
//    while (ch.isLetterOrNumber() || ch == QLatin1Char('_'))
//        ch = m_interface->characterAt(--pos);

//    CPlusPlus::ExpressionUnderCursor expressionUnderCursor;
//				//DLangTextEditorWidget *edit = qobject_cast<DLangTextEditorWidget *>(editor->widget());

//				QList<DLang::Symbol *> members;
//    QStringList specialMembers;

//    bool functionCall = (ch == QLatin1Char('(') && pos == m_interface->position() - 1);

//    if (ch == QLatin1Char(',')) {
//        QTextCursor tc(m_interface->textDocument());
//        tc.setPosition(pos);
//        const int start = expressionUnderCursor.startOfFunctionCall(tc);
//        if (start == -1)
//            return 0;

//        if (m_interface->characterAt(start) == QLatin1Char('(')) {
//            pos = start;
//            ch = QLatin1Char('(');
//            functionCall = true;
//        }
//    }

//    if (ch == QLatin1Char('.') || functionCall) {
//        const bool memberCompletion = ! functionCall;
//        QTextCursor tc(m_interface->textDocument());
//        tc.setPosition(pos);

//        // get the expression under cursor
//        const QByteArray code = expressionUnderCursor(tc).toLatin1();
//        //qDebug() << endl << "expression:" << code;

//        // parse the expression
//								DLang::Engine engine;
//								DLang::Parser parser(&engine, code, code.size(), languageVariant(m_interface->mimeType()));
//								DLang::ExpressionAST *expr = parser.parseExpression();

//#if 0
//        // dump it!
//        QTextStream qout(stdout, QIODevice::WriteOnly);
//								DLang::ASTDump dump(qout);
//        dump(expr);
//#endif

//        if (Document::Ptr doc = m_interface->glslDocument()) {
//												DLang::Scope *currentScope = doc->scopeAt(pos);

//												DLang::Semantic sem;
//												DLang::Semantic::ExprResult exprTy = sem.expression(expr, currentScope, doc->engine());
//            if (exprTy.type) {
//                if (memberCompletion) {
//																				if (const DLang::VectorType *vecTy = exprTy.type->asVectorType()) {
//                        members = vecTy->members();

//                        // Sort the most relevant swizzle orderings to the top.
//                        specialMembers += QLatin1String("xy");
//                        specialMembers += QLatin1String("xyz");
//                        specialMembers += QLatin1String("xyzw");
//                        specialMembers += QLatin1String("rgb");
//                        specialMembers += QLatin1String("rgba");
//                        specialMembers += QLatin1String("st");
//                        specialMembers += QLatin1String("stp");
//                        specialMembers += QLatin1String("stpq");

//																				} else if (const DLang::Struct *structTy = exprTy.type->asStructType()) {
//                        members = structTy->members();

//                    } else {
//                        // some other type
//                    }
//                } else { // function completion
//																				QVector<DLang::Function *> signatures;
//																				if (const DLang::Function *funTy = exprTy.type->asFunctionType())
//																								signatures.append(const_cast<DLang::Function *>(funTy)); // ### get rid of the const_cast
//																				else if (const DLang::OverloadSet *overload = exprTy.type->asOverloadSetType())
//                        signatures = overload->functions();

//                    if (! signatures.isEmpty()) {
//                        m_startPosition = pos + 1;
//                        return createHintProposal(signatures);
//                    }
//                }
//            } else {
//                // undefined

//            }

//        } else {
//            // sorry, there's no document
//        }

//    } else {
//        // it's a global completion
//        if (Document::Ptr doc = m_interface->glslDocument()) {
//												DLang::Scope *currentScope = doc->scopeAt(pos);
//            bool isGlobal = !currentScope || !currentScope->scope();

//            // add the members from the scope chain
//            for (; currentScope; currentScope = currentScope->scope())
//                members += currentScope->members();

//            // if this is the global scope, then add some standard Qt attribute
//            // and uniform names for autocompleting variable declarations
//            // this isn't a complete list, just the most common
//            if (isGlobal) {
//                static const char * const attributeNames[] = {
//                    "qt_Vertex",
//                    "qt_Normal",
//                    "qt_MultiTexCoord0",
//                    "qt_MultiTexCoord1",
//                    "qt_MultiTexCoord2",
//                    0
//                };
//                static const char * const uniformNames[] = {
//                    "qt_ModelViewProjectionMatrix",
//                    "qt_ModelViewMatrix",
//                    "qt_ProjectionMatrix",
//                    "qt_NormalMatrix",
//                    "qt_Texture0",
//                    "qt_Texture1",
//                    "qt_Texture2",
//                    "qt_Color",
//                    "qt_Opacity",
//                    0
//                };
//                for (int index = 0; attributeNames[index]; ++index)
//                    addCompletion(QString::fromLatin1(attributeNames[index]), m_attributeIcon);
//                for (int index = 0; uniformNames[index]; ++index)
//                    addCompletion(QString::fromLatin1(uniformNames[index]), m_uniformIcon);
//            }
//        }

// //       if (m_keywordVariant != languageVariant(m_interface->mimeType())) {
//												QStringList keywords = DLang::Lexer::keywords(languageVariant(m_interface->mimeType()));
////            m_keywordCompletions.clear();
//            for (int index = 0; index < keywords.size(); ++index)
//                addCompletion(keywords.at(index), m_keywordIcon);
////            m_keywordVariant = languageVariant(m_interface->mimeType());
////        }

//  //      m_completions += m_keywordCompletions;
//    }

//				foreach (DLang::Symbol *s, members) {
//        QIcon icon;
//								DLang::Variable *var = s->asVariable();
//        if (var) {
//												int storageType = var->qualifiers() & DLang::QualifiedTypeAST::StorageMask;
//												if (storageType == DLang::QualifiedTypeAST::Attribute)
//                icon = m_attributeIcon;
//												else if (storageType == DLang::QualifiedTypeAST::Uniform)
//                icon = m_uniformIcon;
//												else if (storageType == DLang::QualifiedTypeAST::Varying)
//                icon = m_varyingIcon;
//												else if (storageType == DLang::QualifiedTypeAST::Const)
//                icon = m_constIcon;
//            else
//                icon = m_varIcon;
//        } else if (s->asArgument()) {
//            icon = m_varIcon;
//        } else if (s->asFunction() || s->asOverloadSet()) {
//            icon = m_functionIcon;
//        } else if (s->asStruct()) {
//            icon = m_typeIcon;
//        } else {
//            icon = m_otherIcon;
//        }
//        if (specialMembers.contains(s->name()))
//            addCompletion(s->name(), icon, SpecialMemberOrder);
//        else
//            addCompletion(s->name(), icon);
//    }

//    m_startPosition = pos + 1;
    return createContentProposal();
}

IAssistProposal *DLangCompletionAssistProcessor::createContentProposal() const
{
    IGenericProposalModel *model = new BasicProposalItemListModel(m_completions);
    IAssistProposal *proposal = new GenericProposal(m_startPosition, model);
    return proposal;
}

IAssistProposal *DLangCompletionAssistProcessor::createHintProposal(
				const QVector<DLang::Function *> &symbols)
{
				IFunctionHintProposalModel *model = new DLangFunctionHintProposalModel(symbols);
    IAssistProposal *proposal = new FunctionHintProposal(m_startPosition, model);
    return proposal;
}

bool DLangCompletionAssistProcessor::acceptsIdleEditor() const
{
    const int cursorPosition = m_interface->position();
    const QChar ch = m_interface->characterAt(cursorPosition - 1);

    const QChar characterUnderCursor = m_interface->characterAt(cursorPosition);

    if (isIdentifierChar(ch) && (characterUnderCursor.isSpace() ||
                                 characterUnderCursor.isNull() ||
                                 isDelimiter(characterUnderCursor))) {
        int pos = m_interface->position() - 1;
        for (; pos != -1; --pos) {
            if (! isIdentifierChar(m_interface->characterAt(pos)))
                break;
        }
        ++pos;

        const QString word = m_interface->textAt(pos, cursorPosition - pos);
        if (word.length() > 2 && checkStartOfIdentifier(word)) {
            for (int i = 0; i < word.length(); ++i) {
                if (! isIdentifierChar(word.at(i)))
                    return false;
            }
            return true;
        }
    }

    return isActivationChar(ch);
}

void DLangCompletionAssistProcessor::addCompletion(const QString &text,
                                                  const QIcon &icon,
                                                  int order)
{
    BasicProposalItem *item = new BasicProposalItem;
    item->setText(text);
    item->setIcon(icon);
    item->setOrder(order);
    m_completions.append(item);
}

// -----------------------------
// DLangCompletionAssistInterface
// -----------------------------
DLangCompletionAssistInterface::DLangCompletionAssistInterface(QTextDocument *textDocument,
                                                             int position,
                                                             const QString &fileName,
                                                             TextEditor::AssistReason reason,
                                                             const QString &mimeType,
                                                             const Document::Ptr &glslDoc)
    : DefaultAssistInterface(textDocument, position, fileName, reason)
    , m_mimeType(mimeType)
    , m_glslDoc(glslDoc)
{
}
