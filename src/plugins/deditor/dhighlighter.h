#pragma once

#include "deditorconstants.h"

#include <texteditor/syntaxhighlighter.h>
#include <cplusplus/SimpleLexer.h>
#include <cplusplus/Token.h>

namespace DEditor {

class DHighlighter : public TextEditor::SyntaxHighlighter
{
	Q_OBJECT

public:
	DHighlighter();

	void setLanguageFeatures(const CPlusPlus::LanguageFeatures &languageFeatures);
	void highlightBlock(const QString &text);


protected:
	void highlightLine(const QString &line, int position, int length,
																				const QTextCharFormat &format);
	void highlightWord(QStringRef word, int position, int length);

	void highlightDoxygenComment(const QString &text, int position,
																														int length);

private:
	CPlusPlus::LanguageFeatures m_languageFeatures = CPlusPlus::LanguageFeatures::defaultFeatures();
	bool isPPKeyword(const QStringRef &text) const;
	void correctTokens(CPlusPlus::Tokens& tokens, const QString& text);
};

} // namespace DEditor

