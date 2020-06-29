#include "dautocompleter.h"

#include <cplusplus/MatchingText.h>

#include <texteditor/tabsettings.h>

#include <QTextBlock>
#include <QTextCursor>

using namespace DEditor;

bool DAutoCompleter::contextAllowsAutoBrackets(const QTextCursor &cursor,
																																															const QString &textToInsert) const
{
	const CPlusPlus::MatchingText::IsNextBlockDeeperIndented isIndented
			= [this](const QTextBlock &b) { return isNextBlockIndented(b); };
	return CPlusPlus::MatchingText::contextAllowsAutoParentheses(cursor, textToInsert, isIndented);
}

bool DAutoCompleter::contextAllowsAutoQuotes(const QTextCursor &cursor,
																																													const QString &textToInsert) const
{
	return CPlusPlus::MatchingText::contextAllowsAutoQuotes(cursor, textToInsert);
}

bool DAutoCompleter::contextAllowsElectricCharacters(const QTextCursor &cursor) const
{
	return CPlusPlus::MatchingText::contextAllowsElectricCharacters(cursor);
}

bool DAutoCompleter::isInComment(const QTextCursor &cursor) const
{
	return CPlusPlus::MatchingText::isInCommentHelper(cursor);
}

bool DAutoCompleter::isInString(const QTextCursor &cursor) const
{
	return CPlusPlus::MatchingText::stringKindAtCursor(cursor) != CPlusPlus::T_EOF_SYMBOL;
}

QString DAutoCompleter::insertMatchingBrace(const QTextCursor &cursor, const QString &text,
																																												QChar lookAhead, bool skipChars, int *skippedChars) const
{
	return CPlusPlus::MatchingText::insertMatchingBrace(cursor, text, lookAhead,
																																																					skipChars, skippedChars);
}

QString DAutoCompleter::insertMatchingQuote(const QTextCursor &cursor, const QString &text,
																																												QChar lookAhead, bool skipChars, int *skippedChars) const
{
	return CPlusPlus::MatchingText::insertMatchingQuote(cursor, text, lookAhead,
																																																					skipChars, skippedChars);
}

QString DAutoCompleter::insertParagraphSeparator(const QTextCursor &cursor) const
{
	return CPlusPlus::MatchingText::insertParagraphSeparator(cursor);
}

