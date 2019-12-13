//#include "dautocompleter.h"

////#include <cplusplus/Token.h>
////#include <cplusplus/SimpleLexer.h>
////#include <cplusplus/MatchingText.h>
////#include <cplusplus/BackwardsScanner.h>

////#include <QLatin1Char>
////#include <QTextCursor>

//using namespace CPlusPlus;

//namespace DEditor {

//static const Token tokenAtPosition(const Tokens &tokens, const unsigned pos)
//{
//	for (int i = tokens.size() - 1; i >= 0; --i) {
//		const Token tk = tokens.at(i);
//		if (pos >= tk.utf16charsBegin() && pos < tk.utf16charsEnd())
//			return tk;
//	}
//	return Token();
//}

//DCompleter::DCompleter()
//{}

//DCompleter::~DCompleter()
//{}

//bool DCompleter::contextAllowsAutoParentheses(const QTextCursor &cursor,
//																																														const QString &textToInsert) const
//{
//	QChar ch;

//	if (! textToInsert.isEmpty())
//		ch = textToInsert.at(0);

//	if (! (MatchingText::shouldInsertMatchingText(cursor)
//								|| ch == QLatin1Char('\'')
//								|| ch == QLatin1Char('"')))
//		return false;
//	else if (isInComment(cursor))
//		return false;

//	return true;
//}

//bool DCompleter::contextAllowsElectricCharacters(const QTextCursor &cursor) const
//{
//	const Token tk = SimpleLexer::tokenAt(cursor.block().text(), cursor.positionInBlock(),
//																																							BackwardsScanner::previousBlockState(cursor.block()),
//																																							LanguageFeatures::defaultFeatures());

//	// XXX Duplicated from CppEditor::isInComment to avoid tokenizing twice
//	if (tk.isComment())
//	{
//		const unsigned pos = cursor.selectionEnd() - cursor.block().position();

//		if (pos == tk.utf16charsEnd())
//		{
//			if (tk.is(T_CPP_COMMENT) || tk.is(T_CPP_DOXY_COMMENT))
//				return false;

//			const int state = cursor.block().userState() & 0xFF;
//			if (state > 0)
//				return false;
//		}

//		if (pos < tk.utf16charsEnd())
//			return false;
//	}
//	else if (tk.isStringLiteral() || tk.isCharLiteral())
//	{
//		const unsigned pos = cursor.selectionEnd() - cursor.block().position();
//		if (pos <= tk.utf16charsEnd())
//			return false;
//	}

//	return true;
//}

//bool DCompleter::isInComment(const QTextCursor &cursor) const
//{
//	const Token tk = SimpleLexer::tokenAt(cursor.block().text(), cursor.positionInBlock(),
//																																							BackwardsScanner::previousBlockState(cursor.block()),
//																																							LanguageFeatures::defaultFeatures());

//	if (tk.isComment())
//	{
//		const unsigned pos = cursor.selectionEnd() - cursor.block().position();

//		if (pos == tk.utf16charsEnd())
//		{
//			if (tk.is(T_CPP_COMMENT) || tk.is(T_CPP_DOXY_COMMENT))
//				return true;

//			const int state = cursor.block().userState() & 0xFF;
//			if (state > 0)
//				return true;
//		}

//		if (pos < tk.utf16charsEnd())
//			return true;
//	}

//	return false;
//}
//bool DCompleter::isInString(const QTextCursor &cursor) const
//{
//	//return isInStringHelper(cursor);
//	LanguageFeatures features;
//	features.qtEnabled = false;
//	features.qtKeywordsEnabled = false;
//	features.qtMocRunEnabled = false;
//	features.cxx11Enabled = true;
//	features.c99Enabled = true;

//	SimpleLexer tokenize;
//	tokenize.setLanguageFeatures(features);

//	const int prevState = BackwardsScanner::previousBlockState(cursor.block()) & 0xFF;
//	const Tokens tokens = tokenize(cursor.block().text(), prevState);

//	const unsigned pos = cursor.selectionEnd() - cursor.block().position();

//	if (tokens.isEmpty() || pos <= tokens.first().utf16charsBegin())
//					return false;

//	if (pos >= tokens.last().utf16charsEnd()) {
//					const Token tk = tokens.last();
//					return tk.isStringLiteral(); // && prevState > 0;
//	}

//	Token tk = tokenAtPosition(tokens, pos);
//	return tk.isStringLiteral() && pos > tk.utf16charsBegin();
//}

//QString DCompleter::insertMatchingBrace(const QTextCursor &cursor, const
//                                        QString &text,
//                                        QChar lookAhead, bool skipChars,
//                                        int *skippedChars) const
//{
// return MatchingText::insertMatchingBrace(cursor, text, lookAhead, skipChars, skippedChars);
//}

//QString DCompleter::insertParagraphSeparator(const QTextCursor &cursor) const
//{
//	return MatchingText::insertParagraphSeparator(cursor);
//}

//} // namespace DEditor
