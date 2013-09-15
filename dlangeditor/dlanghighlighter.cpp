#include "dlanghighlighter.h"
#include "dlangeditor.h"
#include <glsl/glsllexer.h>
#include <glsl/glslparser.h>
#include <texteditor/basetextdocumentlayout.h>
#include <texteditor/basetextdocument.h>

#include <QDebug>

using namespace DLangEditor;
using namespace DLangEditor::Internal;
using namespace TextEditor;

Highlighter::Highlighter(QTextDocument *parent)
    : TextEditor::SyntaxHighlighter(parent)
{
    init();
}

Highlighter::Highlighter(BaseTextDocument *parent)
    : TextEditor::SyntaxHighlighter(parent)
{
    init();
}

void Highlighter::init()
{
    static QVector<TextEditor::TextStyle> categories;
    if (categories.isEmpty()) {
        categories << TextEditor::C_NUMBER
                   << TextEditor::C_STRING
                   << TextEditor::C_TYPE
                   << TextEditor::C_KEYWORD
                   << TextEditor::C_OPERATOR
                   << TextEditor::C_PREPROCESSOR
                   << TextEditor::C_LABEL
                   << TextEditor::C_COMMENT
                   << TextEditor::C_DOXYGEN_COMMENT
                   << TextEditor::C_DOXYGEN_TAG
                   << TextEditor::C_VISUAL_WHITESPACE
                   << TextEditor::C_REMOVED_LINE;
    }
    setTextFormatCategories(categories);
}

Highlighter::~Highlighter()
{

}

void Highlighter::highlightBlock(const QString &text)
{
//    const int previousState = previousBlockState();
//    int state = 0, initialBraceDepth = 0;
//    if (previousState != -1) {
//        state = previousState & 0xff;
//        initialBraceDepth = previousState >> 8;
//    }

//    int braceDepth = initialBraceDepth;

//    const QByteArray data = text.toLatin1();
//				DLang::Lexer lex(/*engine=*/ 0, data.constData(), data.size());
//    lex.setState(state);
//    lex.setScanKeywords(false);
//    lex.setScanComments(true);
//    const int variant =
//								DLangTextEditorWidget::languageVariant(static_cast<BaseTextDocument*>(parent())->mimeType());
//    lex.setVariant(variant);

//    int initialState = state;

//				QList<DLang::Token> tokens;
//				DLang::Token tk;
//    do {
//        lex.yylex(&tk);
//        tokens.append(tk);
//				} while (tk.isNot(DLang::Parser::EOF_SYMBOL));

//    state = lex.state(); // refresh the state

//    int foldingIndent = initialBraceDepth;
//    if (TextBlockUserData *userData = BaseTextDocumentLayout::testUserData(currentBlock())) {
//        userData->setFoldingIndent(0);
//        userData->setFoldingStartIncluded(false);
//        userData->setFoldingEndIncluded(false);
//    }

//    if (tokens.isEmpty()) {
//        setCurrentBlockState(previousState);
//        BaseTextDocumentLayout::clearParentheses(currentBlock());
//        if (text.length()) // the empty line can still contain whitespace
//												setFormat(0, text.length(), formatForCategory(DLangVisualWhitespace));
//        BaseTextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);
//        return;
//    }

//    const int firstNonSpace = tokens.first().begin();

//    Parentheses parentheses;
//    parentheses.reserve(20); // assume wizard level ;-)

//    bool highlightAsPreprocessor = false;

//    for (int i = 0; i < tokens.size(); ++i) {
//								const DLang::Token &tk = tokens.at(i);

//        int previousTokenEnd = 0;
//        if (i != 0) {
//            // mark the whitespaces
//            previousTokenEnd = tokens.at(i - 1).begin() +
//                               tokens.at(i - 1).length;
//        }

//        if (previousTokenEnd != tk.begin()) {
//            setFormat(previousTokenEnd, tk.begin() - previousTokenEnd,
//																						formatForCategory(DLangVisualWhitespace));
//        }

//								if (tk.is(DLang::Parser::T_LEFT_PAREN) || tk.is(DLang::Parser::T_LEFT_BRACE) || tk.is(DLang::Parser::T_LEFT_BRACKET)) {
//            const QChar c = text.at(tk.begin());
//            parentheses.append(Parenthesis(Parenthesis::Opened, c, tk.begin()));
//												if (tk.is(DLang::Parser::T_LEFT_BRACE)) {
//                ++braceDepth;

//                // if a folding block opens at the beginning of a line, treat the entire line
//                // as if it were inside the folding block
//                if (tk.begin() == firstNonSpace) {
//                    ++foldingIndent;
//                    BaseTextDocumentLayout::userData(currentBlock())->setFoldingStartIncluded(true);
//                }
//            }
//								} else if (tk.is(DLang::Parser::T_RIGHT_PAREN) || tk.is(DLang::Parser::T_RIGHT_BRACE) || tk.is(DLang::Parser::T_RIGHT_BRACKET)) {
//            const QChar c = text.at(tk.begin());
//            parentheses.append(Parenthesis(Parenthesis::Closed, c, tk.begin()));
//												if (tk.is(DLang::Parser::T_RIGHT_BRACE)) {
//                --braceDepth;
//                if (braceDepth < foldingIndent) {
//                    // unless we are at the end of the block, we reduce the folding indent
//																				if (i == tokens.size()-1 || tokens.at(i+1).is(DLang::Parser::T_SEMICOLON))
//                        BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
//                    else
//                        foldingIndent = qMin(braceDepth, foldingIndent);
//                }
//            }
//        }

//        bool highlightCurrentWordAsPreprocessor = highlightAsPreprocessor;

//        if (highlightAsPreprocessor)
//            highlightAsPreprocessor = false;

//								if (false /* && i == 0 && tk.is(DLang::Parser::T_POUND)*/) {
//												highlightLine(text, tk.begin(), tk.length, formatForCategory(DLangPreprocessorFormat));
//            highlightAsPreprocessor = true;

//        } else if (highlightCurrentWordAsPreprocessor && isPPKeyword(text.midRef(tk.begin(), tk.length))) {
//												setFormat(tk.begin(), tk.length, formatForCategory(DLangPreprocessorFormat));

//								} else if (tk.is(DLang::Parser::T_NUMBER)) {
//												setFormat(tk.begin(), tk.length, formatForCategory(DLangNumberFormat));

//								} else if (tk.is(DLang::Parser::T_COMMENT)) {
//												highlightLine(text, tk.begin(), tk.length, formatForCategory(DLangCommentFormat));

//            // we need to insert a close comment parenthesis, if
//            //  - the line starts in a C Comment (initalState != 0)
//            //  - the first token of the line is a T_COMMENT (i == 0 && tk.is(T_COMMENT))
//            //  - is not a continuation line (tokens.size() > 1 || ! state)
//            if (initialState && i == 0 && (tokens.size() > 1 || ! state)) {
//                --braceDepth;
//                // unless we are at the end of the block, we reduce the folding indent
//                if (i == tokens.size()-1)
//                    BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
//                else
//                    foldingIndent = qMin(braceDepth, foldingIndent);
//                const int tokenEnd = tk.begin() + tk.length - 1;
//                parentheses.append(Parenthesis(Parenthesis::Closed, QLatin1Char('-'), tokenEnd));

//                // clear the initial state.
//                initialState = 0;
//            }

//								} else if (tk.is(DLang::Parser::T_IDENTIFIER)) {
//            int kind = lex.findKeyword(data.constData() + tk.position, tk.length);
//												if (kind == DLang::Parser::T_RESERVED)
//																setFormat(tk.position, tk.length, formatForCategory(DLangReservedKeyword));
//												else if (kind != DLang::Parser::T_IDENTIFIER)
//																setFormat(tk.position, tk.length, formatForCategory(DLangKeywordFormat));
//        }
//    }

//    // mark the trailing white spaces
//    {
//								const DLang::Token tk = tokens.last();
//        const int lastTokenEnd = tk.begin() + tk.length;
//        if (text.length() > lastTokenEnd)
//            highlightLine(text, lastTokenEnd, text.length() - lastTokenEnd, QTextCharFormat());
//    }

//    if (! initialState && state && ! tokens.isEmpty()) {
//        parentheses.append(Parenthesis(Parenthesis::Opened, QLatin1Char('+'),
//                                       tokens.last().begin()));
//        ++braceDepth;
//    }

//    BaseTextDocumentLayout::setParentheses(currentBlock(), parentheses);

//    // if the block is ifdefed out, we only store the parentheses, but

//    // do not adjust the brace depth.
//    if (BaseTextDocumentLayout::ifdefedOut(currentBlock())) {
//        braceDepth = initialBraceDepth;
//        foldingIndent = initialBraceDepth;
//    }

//    BaseTextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);

//    // optimization: if only the brace depth changes, we adjust subsequent blocks
//    // to have QSyntaxHighlighter stop the rehighlighting
//    int currentState = currentBlockState();
//    if (currentState != -1) {
//        int oldState = currentState & 0xff;
//        int oldBraceDepth = currentState >> 8;
//        if (oldState == lex.state() && oldBraceDepth != braceDepth) {
//            int delta = braceDepth - oldBraceDepth;
//            QTextBlock block = currentBlock().next();
//            while (block.isValid() && block.userState() != -1) {
//                BaseTextDocumentLayout::changeBraceDepth(block, delta);
//                BaseTextDocumentLayout::changeFoldingIndent(block, delta);
//                block = block.next();
//            }
//        }
//    }

//    setCurrentBlockState((braceDepth << 8) | lex.state());
}

void Highlighter::highlightLine(const QString &text, int position, int length,
                                const QTextCharFormat &format)
{
				const QTextCharFormat visualSpaceFormat = formatForCategory(DLangVisualWhitespace);

    const int end = position + length;
    int index = position;

    while (index != end) {
        const bool isSpace = text.at(index).isSpace();
        const int start = index;

        do { ++index; }
        while (index != end && text.at(index).isSpace() == isSpace);

        const int tokenLength = index - start;
        if (isSpace)
            setFormat(start, tokenLength, visualSpaceFormat);
        else if (format.isValid())
            setFormat(start, tokenLength, format);
    }
}

bool Highlighter::isPPKeyword(const QStringRef &text) const
{
    switch (text.length())
    {
    case 2:
        if (text.at(0) == QLatin1Char('i') && text.at(1) == QLatin1Char('f'))
            return true;
        break;

    case 4:
        if (text.at(0) == QLatin1Char('e') && text == QLatin1String("elif"))
            return true;
        else if (text.at(0) == QLatin1Char('e') && text == QLatin1String("else"))
            return true;
        break;

    case 5:
        if (text.at(0) == QLatin1Char('i') && text == QLatin1String("ifdef"))
            return true;
        else if (text.at(0) == QLatin1Char('u') && text == QLatin1String("undef"))
            return true;
        else if (text.at(0) == QLatin1Char('e') && text == QLatin1String("endif"))
            return true;
        else if (text.at(0) == QLatin1Char('e') && text == QLatin1String("error"))
            return true;
        break;

    case 6:
        if (text.at(0) == QLatin1Char('i') && text == QLatin1String("ifndef"))
            return true;
        if (text.at(0) == QLatin1Char('i') && text == QLatin1String("import"))
            return true;
        else if (text.at(0) == QLatin1Char('d') && text == QLatin1String("define"))
            return true;
        else if (text.at(0) == QLatin1Char('p') && text == QLatin1String("pragma"))
            return true;
        break;

    case 7:
        if (text.at(0) == QLatin1Char('i') && text == QLatin1String("include"))
            return true;
        else if (text.at(0) == QLatin1Char('w') && text == QLatin1String("warning"))
            return true;
        break;

    case 12:
        if (text.at(0) == QLatin1Char('i') && text == QLatin1String("include_next"))
            return true;
        break;

    default:
        break;
    }

    return false;
}

#if 0
void Highlighter::highlightBlock(const QString &text)
{
    const int previousState = previousBlockState();
    int state = 0, initialBraceDepth = 0;
    if (previousState != -1) {
        state = previousState & 0xff;
        initialBraceDepth = previousState >> 8;
    }

    int braceDepth = initialBraceDepth;

    Parentheses parentheses;
    parentheses.reserve(20); // assume wizard level ;-)

    const QByteArray data = text.toLatin1();
				DLang::Lexer lex(/*engine=*/ 0, data.constData(), data.size());
    lex.setState(qMax(0, previousState));
    lex.setScanKeywords(false);
    lex.setScanComments(true);
    const int variant = m_editor->languageVariant();
    lex.setVariant(variant);

    int foldingIndent = initialBraceDepth;
    if (TextBlockUserData *userData = BaseTextDocumentLayout::testUserData(currentBlock())) {
        userData->setFoldingIndent(0);
        userData->setFoldingStartIncluded(false);
        userData->setFoldingEndIncluded(false);
    }

				QList<DLang::Token> tokens;
				DLang::Token tk;
    do {
        lex.yylex(&tk);
        tokens.append(tk);
				} while (tk.isNot(DLang::Parser::EOF_SYMBOL));

    for (int i = 0; i < tokens.size(); ++i) {
								const DLang::Token &tk = tokens.at(i);

								if (tk.is(DLang::Parser::T_NUMBER)) {
												setFormat(tk.position, tk.length, formatForCategory(DLangNumberFormat);
								} else if (tk.is(DLang::Parser::T_COMMENT)) {
												setFormat(tk.position, tk.length, Qt::darkGreen); // ### FIXME: formatForCategory(DLangCommentFormat);
								} else if (tk.is(DLang::Parser::T_IDENTIFIER)) {
            int kind = lex.findKeyword(data.constData() + tk.position, tk.length);
												if (kind == DLang::Parser::T_RESERVED)
																setFormat(tk.position, tk.length, formatForCategory(DLangReservedKeyword);
												else if (kind != DLang::Parser::T_IDENTIFIER)
																setFormat(tk.position, tk.length, formatForCategory(DLangKeywordFormat);
								} else if (tk.is(DLang::Parser::T_LEFT_PAREN) || tk.is(DLang::Parser::T_LEFT_BRACE) || tk.is(DLang::Parser::T_LEFT_BRACKET)) {
            const QChar c = text.at(tk.begin());
            parentheses.append(Parenthesis(Parenthesis::Opened, c, tk.begin()));
												if (tk.is(DLang::Parser::T_LEFT_BRACE)) {
                ++braceDepth;

                // if a folding block opens at the beginning of a line, treat the entire line
                // as if it were inside the folding block
//                if (tk.begin() == firstNonSpace) {
//                    ++foldingIndent;
//                    BaseTextDocumentLayout::userData(currentBlock())->setFoldingStartIncluded(true);
//                }
            }
								} else if (tk.is(DLang::Parser::T_RIGHT_PAREN) || tk.is(DLang::Parser::T_RIGHT_BRACE) || tk.is(DLang::Parser::T_RIGHT_BRACKET)) {
            const QChar c = text.at(tk.begin());
            parentheses.append(Parenthesis(Parenthesis::Closed, c, tk.begin()));
												if (tk.is(DLang::Parser::T_RIGHT_BRACE)) {
                --braceDepth;
                if (braceDepth < foldingIndent) {
                    // unless we are at the end of the block, we reduce the folding indent
																				if (i == tokens.size()-1 || tokens.at(i+1).is(DLang::Parser::T_SEMICOLON))
                        BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
                    else
                        foldingIndent = qMin(braceDepth, foldingIndent);
                }
            }
        }

				} while (tk.isNot(DLang::Parser::EOF_SYMBOL));
    setCurrentBlockState((braceDepth << 8) | lex.state());
}
#endif
