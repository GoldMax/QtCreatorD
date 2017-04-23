#include "deditorhighlighter.h"
#include "deditorconstants.h"
#include <texteditor/textdocumentlayout.h>
#include <texteditor/textdocument.h>

#include <cplusplus/SimpleLexer.h>
#include <3rdparty/cplusplus/Lexer.h>
#include <cpptools/cppdoxygen.h>
#include <cpptools/cpptoolsreuse.h>
#include <cppeditor/cppeditorenums.h>

#include <QDebug>

using namespace DEditor;
using namespace TextEditor;
using namespace CPlusPlus;
using namespace CppEditor::Internal;

DEditorHighlighter::DEditorHighlighter()
{
	static QVector<TextStyle> categories;
	if (categories.isEmpty())
	{
		categories << C_NUMBER
													<< C_STRING
													<< C_TYPE
													<< C_KEYWORD
													<< C_PRIMITIVE_TYPE
													<< C_OPERATOR
													<< C_PREPROCESSOR
													<< C_LABEL
													<< C_COMMENT
													<< C_DOXYGEN_COMMENT
													<< C_DOXYGEN_TAG
													<< C_VISUAL_WHITESPACE;
	}
	setTextFormatCategories(categories);
}

void DEditorHighlighter::highlightBlock(const QString &text)
{
	const int previousBlockState_ = previousBlockState();
	int lexerState = 0, initialBraceDepth = 0;
	if (previousBlockState_ != -1) {
		lexerState = previousBlockState_ & 0xff;
		initialBraceDepth = previousBlockState_ >> 8;
	}

	int braceDepth = initialBraceDepth;

	SimpleLexer tokenize;

	int initialLexerState = lexerState;
	Tokens tokens = tokenize(text, initialLexerState);
	correctTokens(tokens, text);
	lexerState = tokenize.state(); // refresh lexer state

	initialLexerState &= ~0x80; // discard newline expected bit
	int foldingIndent = initialBraceDepth;
	if (TextBlockUserData *userData = TextDocumentLayout::testUserData(currentBlock())) {
		userData->setFoldingIndent(0);
		userData->setFoldingStartIncluded(false);
		userData->setFoldingEndIncluded(false);
	}

	if (tokens.isEmpty()) {
		setCurrentBlockState((braceDepth << 8) | lexerState);
		TextDocumentLayout::clearParentheses(currentBlock());
		if (text.length())  {// the empty line can still contain whitespace
			if (initialLexerState == T_COMMENT)
				highlightLine(text, 0, text.length(), formatForCategory(CppCommentFormat));
			else if (initialLexerState == T_DOXY_COMMENT)
				highlightLine(text, 0, text.length(), formatForCategory(CppDoxygenCommentFormat));
			else
				setFormat(0, text.length(), formatForCategory(CppVisualWhitespace));
		}
		TextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);
		return;
	}


	const unsigned firstNonSpace = tokens.first().utf16charsBegin();

	Parentheses parentheses;
	parentheses.reserve(5);

	bool expectPreprocessorKeyword = false;
	bool onlyHighlightComments = false;

	for (int i = 0; i < tokens.size(); ++i) {
		const Token &tk = tokens.at(i);

		unsigned previousTokenEnd = 0;
		if (i != 0) {
			// mark the whitespaces
			previousTokenEnd = tokens.at(i - 1).utf16charsBegin() +
					tokens.at(i - 1).utf16chars();
		}

		if (previousTokenEnd != tk.utf16charsBegin()) {
			setFormat(previousTokenEnd,
													tk.utf16charsBegin() - previousTokenEnd,
													formatForCategory(CppVisualWhitespace));
		}

		if (tk.is(T_LPAREN) || tk.is(T_LBRACE) || tk.is(T_LBRACKET)) {
			const QChar c = text.at(tk.utf16charsBegin());
			parentheses.append(Parenthesis(Parenthesis::Opened, c, tk.utf16charsBegin()));
			if (tk.is(T_LBRACE)) {
				++braceDepth;

				// if a folding block opens at the beginning of a line, treat the entire line
				// as if it were inside the folding block
				if (tk.utf16charsBegin() == firstNonSpace) {
					++foldingIndent;
					TextDocumentLayout::userData(currentBlock())->setFoldingStartIncluded(true);
				}
			}
		} else if (tk.is(T_RPAREN) || tk.is(T_RBRACE) || tk.is(T_RBRACKET)) {
			const QChar c = text.at(tk.utf16charsBegin());
			parentheses.append(Parenthesis(Parenthesis::Closed, c, tk.utf16charsBegin()));
			if (tk.is(T_RBRACE)) {
				--braceDepth;
				if (braceDepth < foldingIndent) {
					// unless we are at the end of the block, we reduce the folding indent
					if (i == tokens.size()-1 || tokens.at(i+1).is(T_SEMICOLON))
						TextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
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

		if (tk.is(T_LAST_TOKEN) || (i == 0 && tk.is(T_POUND)))
		{
			highlightLine(text, tk.utf16charsBegin(), tk.utf16chars(),
																	formatForCategory(CppPreprocessorFormat));
			expectPreprocessorKeyword = true;
		}
		else if (highlightCurrentWordAsPreprocessor	&&
											(tk.isKeyword() || tk.is(T_IDENTIFIER))	&&
											isPPKeyword(text.midRef(tk.utf16charsBegin(), tk.utf16chars())))
		{
			setFormat(tk.utf16charsBegin(), tk.utf16chars(), formatForCategory(CppPreprocessorFormat));
			const QStringRef ppKeyword = text.midRef(tk.utf16charsBegin(), tk.utf16chars());
			if (ppKeyword == QLatin1String("error")
							|| ppKeyword == QLatin1String("warning")
							|| ppKeyword == QLatin1String("pragma")) {
				onlyHighlightComments = true;
			}

		} else if (tk.is(T_NUMERIC_LITERAL)) {
			setFormat(tk.utf16charsBegin(), tk.utf16chars(), formatForCategory(CppNumberFormat));
		} else if (tk.isStringLiteral() || tk.isCharLiteral()) {
			highlightLine(text, tk.utf16charsBegin(), tk.utf16chars(), formatForCategory(CppStringFormat));
		} else if (tk.isComment()) {
			const int startPosition = initialLexerState ? previousTokenEnd : tk.utf16charsBegin();
			if (tk.is(T_COMMENT) || tk.is(T_CPP_COMMENT)) {
				highlightLine(text, startPosition, tk.utf16charsEnd() - startPosition,
																		formatForCategory(CppCommentFormat));
			}

			else // a doxygen comment
				highlightDoxygenComment(text, startPosition, tk.utf16charsEnd() - startPosition);

			// we need to insert a close comment parenthesis, if
			//  - the line starts in a C Comment (initalState != 0)
			//  - the first token of the line is a T_COMMENT (i == 0 && tk.is(T_COMMENT))
			//  - is not a continuation line (tokens.size() > 1 || !state)
			if (initialLexerState && i == 0 && (tokens.size() > 1 || !lexerState)) {
				--braceDepth;
				// unless we are at the end of the block, we reduce the folding indent
				if (i == tokens.size()-1)
					TextDocumentLayout::userData(currentBlock())->setFoldingEndIncluded(true);
				else
					foldingIndent = qMin(braceDepth, foldingIndent);
				const int tokenEnd = tk.utf16charsBegin() + tk.utf16chars() - 1;
				parentheses.append(Parenthesis(Parenthesis::Closed, QLatin1Char('-'), tokenEnd));

				// clear the initial state.
				initialLexerState = 0;
			}

		} else if (tk.isKeyword()
													|| CppTools::isQtKeyword(text.midRef(tk.utf16charsBegin(), tk.utf16chars()))
													|| tk.isObjCAtKeyword()) {
			setFormat(tk.utf16charsBegin(), tk.utf16chars(), formatForCategory(CppKeywordFormat));
		} else if (tk.isPrimitiveType())
		{
			setFormat(tk.utf16charsBegin(), tk.utf16chars(),	formatForCategory(CppPrimitiveTypeFormat));
		}
		else if (tk.isOperator())
		{
			setFormat(tk.utf16charsBegin(), tk.utf16chars(), formatForCategory(CppOperatorFormat));
		} else if (i == 0 && tokens.size() > 1 && tk.is(T_IDENTIFIER) && tokens.at(1).is(T_COLON)) {
			setFormat(tk.utf16charsBegin(), tk.utf16chars(), formatForCategory(CppLabelFormat));
		} else if (tk.is(T_IDENTIFIER)) {
			highlightWord(text.midRef(tk.utf16charsBegin(), tk.utf16chars()), tk.utf16charsBegin(),
																	tk.utf16chars());
		}
	}

	// mark the trailing white spaces
	const int lastTokenEnd = tokens.last().utf16charsEnd();
	if (text.length() > lastTokenEnd)
		highlightLine(text, lastTokenEnd, text.length() - lastTokenEnd, formatForCategory(CppVisualWhitespace));

	if (!initialLexerState && lexerState && !tokens.isEmpty()) {
		const Token &lastToken = tokens.last();
		if (lastToken.is(T_COMMENT) || lastToken.is(T_DOXY_COMMENT)) {
			parentheses.append(Parenthesis(Parenthesis::Opened, QLatin1Char('+'),
																																		lastToken.utf16charsBegin()));
			++braceDepth;
		}
	}

	TextDocumentLayout::setParentheses(currentBlock(), parentheses);

	// if the block is ifdefed out, we only store the parentheses, but

	// do not adjust the brace depth.
	if (TextDocumentLayout::ifdefedOut(currentBlock())) {
		braceDepth = initialBraceDepth;
		foldingIndent = initialBraceDepth;
	}

	TextDocumentLayout::setFoldingIndent(currentBlock(), foldingIndent);

	// optimization: if only the brace depth changes, we adjust subsequent blocks
	// to have QSyntaxHighlighter stop the rehighlighting
	int currentState = currentBlockState();
	if (currentState != -1) {
		int oldState = currentState & 0xff;
		int oldBraceDepth = currentState >> 8;
		if (oldState == tokenize.state() && oldBraceDepth != braceDepth) {
			TextDocumentLayout::FoldValidator foldValidor;
			foldValidor.setup(qobject_cast<TextDocumentLayout *>(document()->documentLayout()));
			int delta = braceDepth - oldBraceDepth;
			QTextBlock block = currentBlock().next();
			while (block.isValid() && block.userState() != -1) {
				TextDocumentLayout::changeBraceDepth(block, delta);
				TextDocumentLayout::changeFoldingIndent(block, delta);
				foldValidor.process(block);
				block = block.next();
			}
			foldValidor.finalize();
		}
	}

	setCurrentBlockState((braceDepth << 8) | tokenize.state());
}

bool DEditorHighlighter::isPPKeyword(const QStringRef&) const
{
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

void DEditorHighlighter::correctTokens(Tokens& tokens, const QString & text)
{
	unsigned kind = 0;
	for(int i = 0; i < tokens.length(); i++)
	{
		bool skipReset = false;
		Token t = tokens[i];
		if(kind == 0)
		{
			if(text.at(t.utf16charsBegin()) == QLatin1Char('@'))
			{
				kind = (unsigned)T_FIRST_KEYWORD;
				skipReset = true;
			}
			else if(t.f.kind != T_IDENTIFIER)
				continue;
			QStringRef name = text.midRef(t.utf16charsBegin(),t.utf16chars());
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
					case 'n':
						if (name == QLatin1String("new")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'o':
						if (name == QLatin1String("out")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 4: switch(name.at(0).toLatin1())
				{
					case 'a':
							if (name == QLatin1String("auto")) kind = (unsigned)T_FIRST_KEYWORD;
							break;
					case 'b':
						if (name == QLatin1String("byte")) kind = (unsigned)T_INT;
						break;
					case 'c':
						if (name == QLatin1String("cast")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'u':
						if (name == QLatin1String("uint")) kind = (unsigned)T_FIRST_PRIMITIVE;
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
					case 't':
						if (name == QLatin1String("this")) kind = (unsigned)T_FIRST_KEYWORD;
					break;
				} break;
				case 5: switch(name.at(0).toLatin1())
				{
					case 'a':
						if (name == QLatin1String("alias")) kind = (unsigned)T_FIRST_KEYWORD;
						else if (name == QLatin1String("await")) kind = (unsigned)T_FIRST_KEYWORD;
						else if (name == QLatin1String("async")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'c':
						if (name == QLatin1String("creal")) kind = (unsigned)T_INT;
						break;
					case 'd':
						if (name == QLatin1String("dchar")) kind = (unsigned)T_INT;
						else if (name == QLatin1String("debug")) kind = (unsigned)T_LAST_TOKEN;
						break;
					case 'f':
						if (name == QLatin1String("final")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'i':
						if (name == QLatin1String("inout")) kind = (unsigned)T_FIRST_KEYWORD;
						else if (name == QLatin1String("ireal")) kind = (unsigned)T_INT;
						break;
					case 'm':
						if (name == QLatin1String("mixin")) kind = (unsigned)T_LAST_TOKEN;
						break;
					case 's':
						if (name == QLatin1String("scope")) kind = (unsigned)T_FIRST_KEYWORD;
						else if (name == QLatin1String("super")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 't':
						if (name == QLatin1String("throw")) kind = (unsigned)T_FIRST_KEYWORD;
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
						if (name == QLatin1String("assert")) kind = (unsigned)T_LAST_TOKEN;
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
						else if (name == QLatin1String("import")) kind = (unsigned)T_LAST_TOKEN;
					break;
					case 'm':
						if (name == QLatin1String("module")) kind = (unsigned)T_LAST_TOKEN;
					break;
					case 'p':
						if (name == QLatin1String("pragma")) kind = (unsigned)T_LAST_TOKEN;
					break;
					case 't':
						if (name == QLatin1String("t_size")) kind = (unsigned)T_LAST_TOKEN;
					break;
					case 'u':
						if (name == QLatin1String("ushort")) kind = (unsigned)T_INT;
					break;
				} break;
				case 7: switch (name.at(0).toLatin1())
				{
					case 'c':
						if (name == QLatin1String("cdouble")) kind = (unsigned)T_INT;
						break;
					case 'd':
						if (name == QLatin1String("dstring")) kind = (unsigned)T_INT;
						break;
					case 'e':
						if (name == QLatin1String("enforce"))	kind = (unsigned)T_LAST_TOKEN;
						break;
					case 'f':
						if (name == QLatin1String("finally"))	kind = (unsigned)T_FIRST_KEYWORD;
						break;
					case 'i':
						if (name == QLatin1String("idouble")) kind = (unsigned)T_INT;
						break;
					case 'v':
						if (name == QLatin1String("version"))	kind = (unsigned)T_LAST_TOKEN;
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
					case '_':
						if (name == QLatin1String("__FILE__")) kind = (unsigned)T_LAST_TOKEN;
						else if (name == QLatin1String("__LINE__")) kind = (unsigned)T_LAST_TOKEN;
						else if (name == QLatin1String("__traits")) kind = (unsigned)T_LAST_TOKEN;
						break;
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
					case 't':
						if (name == QLatin1String("template")) kind = (unsigned)T_LAST_TOKEN;
						break;
					case 'u':
						if (name == QLatin1String("unittest")) kind = (unsigned)T_LAST_TOKEN;
						break;
				} break;
				case 9: switch (name.at(0).toLatin1())
				{
					case '_':
							if (name == QLatin1String("__gshared"))	kind = (unsigned)T_LAST_TOKEN;
							break;
					case 'i':
						if (name == QLatin1String("immutable")) kind = (unsigned)T_FIRST_KEYWORD;
						else if (name == QLatin1String("interface")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 10: switch (name.at(0).toLatin1())
				{
					case '_':
						if (name == QLatin1String("__MODULE__"))	kind = (unsigned)T_LAST_TOKEN;
						break;
					case 'd':
						if (name == QLatin1String("deprecated")) kind = (unsigned)T_FIRST_KEYWORD;
						break;
				} break;
				case 12: switch (name.at(0).toLatin1())
				{
					case '_':
						if (name == QLatin1String("__FUNCTION__"))	kind = (unsigned)T_LAST_TOKEN;
					break;
					case 's':
						if (name == QLatin1String("synchronized")) kind = (unsigned)T_FIRST_KEYWORD;
					break;
				} break;
				case 19: switch (name.at(0).toLatin1())
				{
					case '_':
						if (name == QLatin1String("__PRETTY_FUNCTION__"))	kind = (unsigned)T_LAST_TOKEN;
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
