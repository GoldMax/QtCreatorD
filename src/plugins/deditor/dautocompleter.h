#pragma once

#include <texteditor/autocompleter.h>

namespace DEditor {

class DAutoCompleter : public TextEditor::AutoCompleter
{
public:
	bool contextAllowsAutoBrackets(const QTextCursor &cursor,
																																const QString &textToInsert = QString()) const override;
	bool contextAllowsAutoQuotes(const QTextCursor &cursor,
																														const QString &textToInsert = QString()) const override;
	bool contextAllowsElectricCharacters(const QTextCursor &cursor) const override;
	bool isInComment(const QTextCursor &cursor) const override;
	bool isInString(const QTextCursor &cursor) const override;
	QString insertMatchingBrace(const QTextCursor &cursor,
																													const QString &text,
																													QChar lookAhead,
																													bool skipChars,
																													int *skippedChars) const override;
	QString insertMatchingQuote(const QTextCursor &cursor,
																													const QString &text,
																													QChar lookAhead,
																													bool skipChars,
																													int *skippedChars) const override;
	QString insertParagraphSeparator(const QTextCursor &cursor) const override;
	};

} // namespace DEditor

