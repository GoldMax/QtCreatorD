#include "dautocompleter.h"

#include <cplusplus/Token.h>
#include <cplusplus/SimpleLexer.h>
#include <cplusplus/MatchingText.h>
#include <cplusplus/BackwardsScanner.h>

#include <QLatin1Char>
#include <QTextCursor>

using namespace CPlusPlus;

namespace DEditor {

DCompleter::DCompleter()
{}

DCompleter::~DCompleter()
{}

bool DCompleter::contextAllowsAutoParentheses(const QTextCursor &cursor,
																																														const QString &textToInsert) const
{
	QChar ch;

	if (! textToInsert.isEmpty())
		ch = textToInsert.at(0);

	if (! (MatchingText::shouldInsertMatchingText(cursor)
								|| ch == QLatin1Char('\'')
								|| ch == QLatin1Char('"')))
		return false;
	else if (isInComment(cursor))
		return false;

	return true;
}

bool DCompleter::contextAllowsElectricCharacters(const QTextCursor &cursor) const
{
	const Token tk = SimpleLexer::tokenAt(cursor.block().text(), cursor.positionInBlock(),
																																							BackwardsScanner::previousBlockState(cursor.block()),
																																							LanguageFeatures::defaultFeatures());

	// XXX Duplicated from CppEditor::isInComment to avoid tokenizing twice
	if (tk.isComment())
	{
		const unsigned pos = cursor.selectionEnd() - cursor.block().position();

		if (pos == tk.utf16charsEnd())
		{
			if (tk.is(T_CPP_COMMENT) || tk.is(T_CPP_DOXY_COMMENT))
				return false;

			const int state = cursor.block().userState() & 0xFF;
			if (state > 0)
				return false;
		}

		if (pos < tk.utf16charsEnd())
			return false;
	}
	else if (tk.isStringLiteral() || tk.isCharLiteral())
	{
		const unsigned pos = cursor.selectionEnd() - cursor.block().position();
		if (pos <= tk.utf16charsEnd())
			return false;
	}

	return true;
}

bool DCompleter::isInComment(const QTextCursor &cursor) const
{
	const Token tk = SimpleLexer::tokenAt(cursor.block().text(), cursor.positionInBlock(),
																																							BackwardsScanner::previousBlockState(cursor.block()),
																																							LanguageFeatures::defaultFeatures());

	if (tk.isComment())
	{
		const unsigned pos = cursor.selectionEnd() - cursor.block().position();

		if (pos == tk.utf16charsEnd())
		{
			if (tk.is(T_CPP_COMMENT) || tk.is(T_CPP_DOXY_COMMENT))
				return true;

			const int state = cursor.block().userState() & 0xFF;
			if (state > 0)
				return true;
		}

		if (pos < tk.utf16charsEnd())
			return true;
	}

	return false;
}

QString DCompleter::insertMatchingBrace(const QTextCursor &cursor,
																																								const QString &text,
																																								QChar la,
																																								int *skippedChars) const
{
	return MatchingText::insertMatchingBrace(cursor, text, la, skippedChars);
}

QString DCompleter::insertParagraphSeparator(const QTextCursor &cursor) const
{
	return MatchingText::insertParagraphSeparator(cursor);
}

} // namespace DEditor
