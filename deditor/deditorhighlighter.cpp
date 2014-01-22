#include "deditorhighlighter.h"
#include "dtexteditor.h"
#include "deditorconstants.h"
#include <texteditor/basetextdocumentlayout.h>
#include <texteditor/basetextdocument.h>

#include <cplusplus/SimpleLexer.h>
#include <3rdparty/cplusplus/Lexer.h>
#include <cpptools/cppdoxygen.h>
#include <cpptools/cpptoolsreuse.h>

#include <QDebug>

using namespace DEditor::Internal;
using namespace TextEditor;

using namespace CPlusPlus;

DEditorHighlighter::DEditorHighlighter(QTextDocument *parent)
	: TextEditor::SyntaxHighlighter(parent)
{
	init();
}

DEditorHighlighter::DEditorHighlighter(BaseTextDocument *parent)
	: TextEditor::SyntaxHighlighter(parent)
{
	init();
}

void DEditorHighlighter::init()
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

DEditorHighlighter::~DEditorHighlighter()
{

}

void DEditorHighlighter::highlightBlock(const QString &text)
{
	const int previousState = previousBlockState();
	int state = 0, initialBraceDepth = 0;
	if (previousState != -1) {
		state = previousState & 0xff;
		initialBraceDepth = previousState >> 8;
	}

	int braceDepth = initialBraceDepth;

	SimpleLexer tokenize;
//	tokenize.setQtMocRunEnabled(false);
//	tokenize.setObjCEnabled(false);
//	tokenize.setCxx0xEnabled(true);

	int initialState = state;
	QList<Token> tokens = tokenize(text, initialState);
	correctTokens(tokens, text);
	state = tokenize.state(); // refresh the state

	int foldingIndent = initialBraceDepth;
	if (TextBlockUserData *userData = BaseTextDocumentLayout::testUserData(currentBlock())) {
		userData->setFoldingIndent(0);
		userData->setFoldingStartIncluded(false);
		userData->setFoldingEndIncluded(false);
	}

	if (tokens.isEmpty()) {
		setCurrentBlockState(previousState);
		BaseTextDocumentLayout::clearParentheses(currentBlock());
		if (text.length())  {// the empty line can still contain whitespace
			if (initialState == Lexer::State_MultiLineComment)
				highlightLine(text, 0, text.length(), formatForCategory(CppCommentFormat));
			else if (initialState == Lexer::State_MultiLineDoxyComment)
				highlightLine(text, 0, text.length(), formatForCategory(CppDoxygenCommentFormat));
			else
				setFormat(0, text.length(), formatForCategory(CppVisualWhitespace));
		}
		BaseTextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);
		return;
	}

	const unsigned firstNonSpace = tokens.first().begin();

	Parentheses parentheses;
	parentheses.reserve(20); // assume wizard level ;-)

	bool expectPreprocessorKeyword = false;
	bool onlyHighlightComments = false;

	for (int i = 0; i < tokens.size(); ++i) {
		const Token &tk = tokens.at(i);

		unsigned previousTokenEnd = 0;
		if (i != 0) {
			// mark the whitespaces
			previousTokenEnd = tokens.at(i - 1).begin() +
					tokens.at(i - 1).length();
		}

		if (previousTokenEnd != tk.begin())
			setFormat(previousTokenEnd, tk.begin() - previousTokenEnd, formatForCategory(CppVisualWhitespace));

		if (tk.is(T_LPAREN) || tk.is(T_LBRACE) || tk.is(T_LBRACKET)) {
			const QChar c = text.at(tk.begin());
			parentheses.append(Parenthesis(Parenthesis::Opened, c, tk.begin()));
			if (tk.is(T_LBRACE)) {
				++braceDepth;

				// if a folding block opens at the beginning of a line, treat the entire line
				// as if it were inside the folding block
				if (tk.begin() == firstNonSpace) {
					++foldingIndent;
					BaseTextDocumentLayout::userData(currentBlock())->setFoldingStartIncluded(true);
				}
			}
		} else if (tk.is(T_RPAREN) || tk.is(T_RBRACE) || tk.is(T_RBRACKET)) {
			const QChar c = text.at(tk.begin());
			parentheses.append(Parenthesis(Parenthesis::Closed, c, tk.begin()));
			if (tk.is(T_RBRACE)) {
				--braceDepth;
				if (braceDepth < foldingIndent) {
					// unless we are at the end of the block, we reduce the folding indent
					if (i == tokens.size()-1 || tokens.at(i+1).is(T_SEMICOLON))
						BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
					else
						foldingIndent = qMin(braceDepth, foldingIndent);
				}
			}
		}

		bool highlightCurrentWordAsPreprocessor = expectPreprocessorKeyword;

		if (expectPreprocessorKeyword)
			expectPreprocessorKeyword = false;

		if (onlyHighlightComments && !tk.isComment())
			continue;

		if (i == 0 && tk.is(T_POUND))
		{
			highlightLine(text, tk.begin(), tk.length(), formatForCategory(CppPreprocessorFormat));
			expectPreprocessorKeyword = true;
		}
		else if (highlightCurrentWordAsPreprocessor &&
			(tk.isKeyword() || tk.is(T_IDENTIFIER)) && isPPKeyword(text.midRef(tk.begin(), tk.length())))
		{
			setFormat(tk.begin(), tk.length(), formatForCategory(CppPreprocessorFormat));
			const QStringRef ppKeyword = text.midRef(tk.begin(), tk.length());
			if (ppKeyword == QLatin1String("error")
							|| ppKeyword == QLatin1String("warning")
							|| ppKeyword == QLatin1String("pragma")) {
				onlyHighlightComments = true;
			}

		} else if (tk.is(T_NUMERIC_LITERAL)) {
			setFormat(tk.begin(), tk.length(), formatForCategory(CppNumberFormat));
		} else if (tk.isStringLiteral() || tk.isCharLiteral()) {
			highlightLine(text, tk.begin(), tk.length(), formatForCategory(CppStringFormat));
		} else if (tk.isComment()) {
			const int startPosition = initialState ? previousTokenEnd : tk.begin();
			if (tk.is(T_COMMENT) || tk.is(T_CPP_COMMENT))
				highlightLine(text, startPosition, tk.end() - startPosition, formatForCategory(CppCommentFormat));

			else // a doxygen comment
				highlightDoxygenComment(text, startPosition, tk.end() - startPosition);

			// we need to insert a close comment parenthesis, if
			//  - the line starts in a C Comment (initalState != 0)
			//  - the first token of the line is a T_COMMENT (i == 0 && tk.is(T_COMMENT))
			//  - is not a continuation line (tokens.size() > 1 || !state)
			if (initialState && i == 0 && (tokens.size() > 1 || !state)) {
				--braceDepth;
				// unless we are at the end of the block, we reduce the folding indent
				if (i == tokens.size()-1)
					BaseTextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
				else
					foldingIndent = qMin(braceDepth, foldingIndent);
				const int tokenEnd = tk.begin() + tk.length() - 1;
				parentheses.append(Parenthesis(Parenthesis::Closed, QLatin1Char('-'), tokenEnd));

				// clear the initial state.
				initialState = 0;
			}

		} else if (tk.isKeyword() || CppTools::isQtKeyword(text.midRef(tk.begin(), tk.length())) || tk.isObjCAtKeyword()) {
			setFormat(tk.begin(), tk.length(), formatForCategory(CppKeywordFormat));
		} else if (tk.isOperator()) {
			setFormat(tk.begin(), tk.length(), formatForCategory(CppOperatorFormat));
		} else if (i == 0 && tokens.size() > 1 && tk.is(T_IDENTIFIER) && tokens.at(1).is(T_COLON)) {
			setFormat(tk.begin(), tk.length(), formatForCategory(CppLabelFormat));
		}
		else if (tk.is(T_IDENTIFIER) && isPPKeyword(text.midRef(tk.begin(), tk.length())))
		{
			highlightLine(text, tk.begin(), tk.length(), formatForCategory(CppPreprocessorFormat));
			expectPreprocessorKeyword = true;
		}
		else if (tk.is(T_IDENTIFIER))
		{
			highlightWord(text.midRef(tk.begin(), tk.length()), tk.begin(), tk.length());
		}
	}

	// mark the trailing white spaces
	const int lastTokenEnd = tokens.last().end();
	if (text.length() > lastTokenEnd)
		highlightLine(text, lastTokenEnd, text.length() - lastTokenEnd, formatForCategory(CppVisualWhitespace));

	if (!initialState && state && !tokens.isEmpty()) {
		parentheses.append(Parenthesis(Parenthesis::Opened, QLatin1Char('+'),
																																	tokens.last().begin()));
		++braceDepth;
	}

	BaseTextDocumentLayout::setParentheses(currentBlock(), parentheses);

	// if the block is ifdefed out, we only store the parentheses, but

	// do not adjust the brace depth.
	if (BaseTextDocumentLayout::ifdefedOut(currentBlock())) {
		braceDepth = initialBraceDepth;
		foldingIndent = initialBraceDepth;
	}

	BaseTextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);

	// optimization: if only the brace depth changes, we adjust subsequent blocks
	// to have QSyntaxHighlighter stop the rehighlighting
	int currentState = currentBlockState();
	if (currentState != -1) {
		int oldState = currentState & 0xff;
		int oldBraceDepth = currentState >> 8;
		if (oldState == tokenize.state() && oldBraceDepth != braceDepth) {
			BaseTextDocumentLayout::FoldValidator foldValidor;
			foldValidor.setup(qobject_cast<BaseTextDocumentLayout *>(document()->documentLayout()));
			int delta = braceDepth - oldBraceDepth;
			QTextBlock block = currentBlock().next();
			while (block.isValid() && block.userState() != -1) {
				BaseTextDocumentLayout::changeBraceDepth(block, delta);
				BaseTextDocumentLayout::changeFoldingIndent(block, delta);
				foldValidor.process(block);
				block = block.next();
			}
			foldValidor.finalize();
		}
	}

	setCurrentBlockState((braceDepth << 8) | tokenize.state());
}

bool DEditorHighlighter::isPPKeyword(const QStringRef &text) const
{
	switch (text.length())
	{
		case 5: switch (text.at(0).toLatin1())
		{
			case 'd':
				if (text == QLatin1String("debug")) return true;
				break;
			case 'm':
				if (text == QLatin1String("mixin")) return true;
				break;
		} break;
		case 6: switch (text.at(0).toLatin1())
		{
			case 'i':
				if (text == QLatin1String("import")) return true;
				break;
			case 'p':
				if (text == QLatin1String("pragma")) return true;
				break;
			case 'm':
				if (text == QLatin1String("module")) return true;
				break;
		} break;
		case 7: switch (text.at(0).toLatin1())
		{
			case 'v':
				if (text == QLatin1String("version"))	return true;
				break;
		}	break;
		case 8: switch (text.at(0).toLatin1())
		{
			case 'u':
				if (text == QLatin1String("unittest")) return true;
				break;
			case 't':
				if (text == QLatin1String("template")) return true;
				break;
			case '_':
				if (text == QLatin1String("__FILE__")) return true;
				if (text == QLatin1String("__LINE__")) return true;
				break;
		}	break;
		case 10: switch (text.at(0).toLatin1())
		{
			case '_':
				if (text == QLatin1String("__MODULE__"))	return true;
				break;
		}	break;
		case 12: switch (text.at(0).toLatin1())
		{
			case '_':
				if (text == QLatin1String("__FUNCTION__"))	return true;
				break;
		}	break;
		case 19: switch (text.at(0).toLatin1())
		{
			case '_':
				if (text == QLatin1String("__PRETTY_FUNCTION__"))	return true;
				break;
		}	break;
		default:	break;
	}

	return false;
}

void DEditorHighlighter::highlightLine(const QString &text, int position, int length,
																																							const QTextCharFormat &format)
{
	QTextCharFormat visualSpaceFormat = formatForCategory(CppVisualWhitespace);
	visualSpaceFormat.setBackground(format.background());

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

void DEditorHighlighter::highlightWord(QStringRef word, int position, int length)
{
	// try to highlight Qt 'identifiers' like QObject and Q_PROPERTY

	if (word.length() > 2 && word.at(0) == QLatin1Char('Q'))
	{
		if (word.at(1) == QLatin1Char('_') // Q_
						|| (word.at(1) == QLatin1Char('T') && word.at(2) == QLatin1Char('_'))) // QT_
		{
			for (int i = 1; i < word.length(); ++i) {
				const QChar &ch = word.at(i);
				if (!(ch.isUpper() || ch == QLatin1Char('_')))
					return;
			}

			setFormat(position, length, formatForCategory(CppTypeFormat));
			return;
		}
	}
	if(word.length() > 0 && word.at(0).isUpper())
		setFormat(position, length, formatForCategory(CppTypeFormat));
}

void DEditorHighlighter::highlightDoxygenComment(const QString &text, int position, int)
{
	int initial = position;

	const QChar *uc = text.unicode();
	const QChar *it = uc + position;

	const QTextCharFormat &format = formatForCategory(CppDoxygenCommentFormat);
	const QTextCharFormat &kwFormat = formatForCategory(CppDoxygenTagFormat);

	while (!it->isNull()) {
		if (it->unicode() == QLatin1Char('\\') ||
						it->unicode() == QLatin1Char('@')) {
			++it;

			const QChar *start = it;
			while (it->isLetterOrNumber() || it->unicode() == '_')
				++it;

			int k = CppTools::classifyDoxygenTag(start, it - start);
			if (k != CppTools::T_DOXY_IDENTIFIER) {
				highlightLine(text, initial, start - uc - initial, format);
				setFormat(start - uc - 1, it - start + 1, kwFormat);
				initial = it - uc;
			}
		} else
			++it;
	}

	highlightLine(text, initial, it - uc - initial, format);
}

void DEditorHighlighter::correctTokens(QList<Token>& tokens, const QString & text)
{
	//foreach(Token t, tokens)
	unsigned kind = 0;
	for(int i = 0; i < tokens.length(); i++)
	{
		bool skipReset = false;
		Token t = tokens[i];
		if(kind == 0)
		{
			if(text.at(t.begin()) == QLatin1Char('@'))
			{
				kind = (unsigned)T_FIRST_KEYWORD;
				skipReset = true;
			}
			else if(t.f.kind != T_IDENTIFIER)
				continue;
			QStringRef name = text.midRef(t.begin(),t.length());
			switch (name.length())
			{
				case 2: switch(name.at(0).toLatin1())
				{
					case 'i':
						if (name.at(1).toLatin1() == 'n') kind = (unsigned)T_FIRST_KEYWORD;
						else if (name.at(1).toLatin1() == 's') kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 3: switch(name.at(0).toLatin1())
				{
					case 'r':
						if (name == QLatin1String("ref")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'o':
						if (name == QLatin1String("out")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 4: switch(name.at(0).toLatin1())
				{
					case 'b':
						if (name == QLatin1String("byte")) kind = (unsigned)T_INT;
						break;
					case 'c':
						if (name == QLatin1String("cast")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'u':
						if (name == QLatin1String("uint")) kind = (unsigned)T_INT;
					break;
					case 'r':
						if (name == QLatin1String("real")) kind = (unsigned)T_INT;
					break;
					case 'l':
						if (name == QLatin1String("lazy")) kind = (unsigned)T_FIRST_KEYWORD;
					break;
					case 'n':
						if (name == QLatin1String("null")) kind = (unsigned)T_FIRST_KEYWORD;
					break;
					case 'p':
						if (name == QLatin1String("pure")) kind = (unsigned)T_FIRST_KEYWORD;
					break;
				} break;
				case 5: switch(name.at(0).toLatin1())
				{
					case 'a':
						if (name == QLatin1String("alias")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'c':
						if (name == QLatin1String("creal")) kind = (unsigned)T_INT;
						break;
					case 'd':
						if (name == QLatin1String("dchar")) kind = (unsigned)T_INT;
						break;
					case 'f':
						if (name == QLatin1String("final")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'i':
						if (name == QLatin1String("inout")) kind = (unsigned)T_FIRST_KEYWORD;
						else if (name == QLatin1String("ireal")) kind = (unsigned)T_INT;
						break;
					case 's':
						if (name == QLatin1String("scope")) kind = (unsigned)T_FIRST_KEYWORD;
						else if (name == QLatin1String("super")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'w':
						if (name == QLatin1String("wchar")) kind = (unsigned)T_INT;
						break;
					case 'u':
						if (name == QLatin1String("ubyte")) kind = (unsigned)T_INT;
						else if (name == QLatin1String("ulong")) kind = (unsigned)T_INT;
						break;
				} break;
				case 6: switch (name.at(0).toLatin1())
				{
					case 'a':
						if (name == QLatin1String("assert")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 's':
						if (name == QLatin1String("string")) kind = (unsigned)T_INT;
						else if (name == QLatin1String("shared")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'c':
						if (name == QLatin1String("cfloat")) kind = (unsigned)T_INT;
						break;
					case 'i':
						if (name == QLatin1String("ifloat")) kind = (unsigned)T_INT;
						break;
					case 'u':
						if (name == QLatin1String("ushort")) kind = (unsigned)T_INT;
						break;
				} break;
				case 7: switch (name.at(0).toLatin1())
				{
					case 'd':
						if (name == QLatin1String("dstring")) kind = (unsigned)T_INT;
						break;
					case 'c':
						if (name == QLatin1String("cdouble")) kind = (unsigned)T_INT;
						break;
					case 'i':
						if (name == QLatin1String("idouble")) kind = (unsigned)T_INT;
						break;
					case 'w':
						if (name == QLatin1String("wstring")) kind = (unsigned)T_INT;
						break;
					case 'p':
						if (name == QLatin1String("package")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'n':
						if (name == QLatin1String("nothrow")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 8: switch (name.at(0).toLatin1())
				{
					case 'a':
						if (name == QLatin1String("abstract")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'd':
						if (name == QLatin1String("delegate")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'f':
						if (name == QLatin1String("function")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'o':
						if (name == QLatin1String("override")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 9: switch (name.at(0).toLatin1())
				{
					case 'i':
						if (name == QLatin1String("immutable")) kind = (unsigned)T_FIRST_KEYWORD;
						else if (name == QLatin1String("interface")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 10: switch (name.at(0).toLatin1())
				{
					case 'd':
						if (name == QLatin1String("deprecated")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 12: switch (name.at(0).toLatin1())
				{
					case 's':
						if (name == QLatin1String("synchronized")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				default: break;
			}
		}
		if(kind > 0)
		{
			t.f.kind = kind;
			tokens[i] = t;
			if(skipReset == false)
				kind = 0;
		}
	}
}
