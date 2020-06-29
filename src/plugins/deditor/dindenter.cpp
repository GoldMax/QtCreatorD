#include "dindenter.h"

#include <cpptools/cppcodeformatter.h>
#include <cpptools/cpptoolssettings.h>
#include <cpptools/cppcodestylepreferences.h>
#include <texteditor/tabsettings.h>

#include <QChar>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextCursor>

namespace DEditor {

DIndenter::DIndenter(QTextDocument *doc) : TextEditor::TextIndenter(doc)
{}

DIndenter::~DIndenter()
{}

void DIndenter::setCodeStylePreferences(TextEditor::ICodeStylePreferences *preferences)
{
	auto cppCodeStylePreferences =
			qobject_cast<CppTools::CppCodeStylePreferences *>(preferences);
	if (cppCodeStylePreferences)
		m_cppCodeStylePreferences = cppCodeStylePreferences;
}

CppTools::CppCodeStyleSettings DIndenter::codeStyleSettings() const
{
	if (m_cppCodeStylePreferences)
		return m_cppCodeStylePreferences->currentCodeStyleSettings();
	return {};
}

bool DIndenter::isElectricCharacter(const QChar &ch) const
{
	return ch == QLatin1Char('{')
			|| ch == QLatin1Char('}')
			|| ch == QLatin1Char(':')
			|| ch == QLatin1Char('#');
}

static bool isElectricInLine(const QChar ch, const QString &text)
{
	switch (ch.toLatin1()) {
		case ';':
		return text.contains(QLatin1String("break"));
		case ':':
			// switch cases and access declarations should be reindented
			if (text.contains(QLatin1String("case")) || text.contains(QLatin1String("default"))
							|| text.contains(QLatin1String("public")) || text.contains(QLatin1String("private"))
							|| text.contains(QLatin1String("protected")) || text.contains(QLatin1String("signals"))
							|| text.contains(QLatin1String("Q_SIGNALS"))) {
				return true;
			}

			Q_FALLTHROUGH();
			// lines that start with : might have a constructor initializer list
		case '<':
		case '>': {
			// Electric if at line beginning (after space indentation)
			for (int i = 0, len = text.count(); i < len; ++i) {
				if (!text.at(i).isSpace())
					return text.at(i) == ch;
			}
			return false;
		}
	}

	return true;
}


void DIndenter::indentBlock(const QTextBlock &block,
																												const QChar &typedChar,
																												const TextEditor::TabSettings &tabSettings,
																												int /*cursorPositionInEditor*/)
{
	CppTools::QtStyleCodeFormatter codeFormatter(tabSettings, codeStyleSettings());

	codeFormatter.updateStateUntil(block);
	int indent;
	int padding;
	codeFormatter.indentFor(block, &indent, &padding);

	if (isElectricCharacter(typedChar)) {
		// : should not be electric for labels
		if (!isElectricInLine(typedChar, block.text()))
			return;

		// only reindent the current line when typing electric characters if the
		// indent is the same it would be if the line were empty
		int newlineIndent;
		int newlinePadding;
		codeFormatter.indentForNewLineAfter(block.previous(), &newlineIndent, &newlinePadding);
		if (tabSettings.indentationColumn(block.text()) != newlineIndent + newlinePadding)
			return;
	}

	tabSettings.indentLine(block, indent + padding, padding);
}

void DIndenter::indent(const QTextCursor &cursor,
																							const QChar &typedChar,
																							const TextEditor::TabSettings &tabSettings,
																							int /*cursorPositionInEditor*/)
{
	if (cursor.hasSelection()) {
		QTextBlock block = m_doc->findBlock(cursor.selectionStart());
		const QTextBlock end = m_doc->findBlock(cursor.selectionEnd()).next();

		CppTools::QtStyleCodeFormatter codeFormatter(tabSettings, codeStyleSettings());
		codeFormatter.updateStateUntil(block);

		QTextCursor tc = cursor;
		tc.beginEditBlock();
		do {
			int indent;
			int padding;
			codeFormatter.indentFor(block, &indent, &padding);
			tabSettings.indentLine(block, indent + padding, padding);
			codeFormatter.updateLineStateChange(block);
			block = block.next();
		} while (block.isValid() && block != end);
		tc.endEditBlock();
	} else {
		indentBlock(cursor.block(), typedChar, tabSettings);
	}
}

} // namespace DEditor
