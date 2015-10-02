#ifndef DEDITORHIGHLIGHTER_H
#define DEDITORHIGHLIGHTER_H

#include "deditorconstants.h"

#include <texteditor/syntaxhighlighter.h>
#include <cplusplus/Token.h>
#include <cplusplus/SimpleLexer.h>


namespace DEditor {

class DEditorHighlighter : public TextEditor::SyntaxHighlighter
{
	Q_OBJECT

public:
	DEditorHighlighter();

protected:
	void highlightBlock(const QString &text);
	void highlightLine(const QString &line, int position, int length,
																				const QTextCharFormat &format);
	void highlightWord(QStringRef word, int position, int length);

	void highlightDoxygenComment(const QString &text, int position,
																														int length);

private:
	bool isPPKeyword(const QStringRef &text) const;
	void correctTokens(CPlusPlus::Tokens& tokens, const QString& text);
};

} // namespace DEditor

#endif // DEDITORHIGHLIGHTER_H
