#include "dlangindenter.h"

#include <cpptools/cppcodeformatter.h>
#include <cpptools/cpptoolssettings.h>
#include <cpptools/cppcodestylepreferences.h>
#include <texteditor/tabsettings.h>

#include <QChar>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextCursor>

using namespace DLangEditor;
using namespace Internal;

DLangIndenter::DLangIndenter()
{}

DLangIndenter::~DLangIndenter()
{}

bool DLangIndenter::isElectricCharacter(const QChar &ch) const
{
    if (ch == QLatin1Char('{') ||
        ch == QLatin1Char('}') ||
        ch == QLatin1Char(':') ||
        ch == QLatin1Char('#')) {
        return true;
    }
    return false;
}

void DLangIndenter::indentBlock(QTextDocument *doc,
                               const QTextBlock &block,
                               const QChar &typedChar,
                               const TextEditor::TabSettings &tabSettings)
{
    Q_UNUSED(doc)

    // TODO: do something with it
    CppTools::QtStyleCodeFormatter codeFormatter(tabSettings,
              CppTools::CppToolsSettings::instance()->cppCodeStyle()->codeStyleSettings());

    codeFormatter.updateStateUntil(block);
    int indent;
    int padding;
    codeFormatter.indentFor(block, &indent, &padding);

    // only reindent the current line when typing electric characters if the
    // indent is the same it would be if the line were empty
    if (isElectricCharacter(typedChar)) {
        int newlineIndent;
        int newlinePadding;
        codeFormatter.indentForNewLineAfter(block.previous(), &newlineIndent, &newlinePadding);
        if (tabSettings.indentationColumn(block.text()) != newlineIndent + newlinePadding)
            return;
    }

    tabSettings.indentLine(block, indent + padding, padding);
}

void DLangIndenter::indent(QTextDocument *doc,
                          const QTextCursor &cursor,
                          const QChar &typedChar,
                          const TextEditor::TabSettings &tabSettings)
{
    if (cursor.hasSelection()) {
        QTextBlock block = doc->findBlock(cursor.selectionStart());
        const QTextBlock end = doc->findBlock(cursor.selectionEnd()).next();

        // TODO: do something with it
        CppTools::QtStyleCodeFormatter codeFormatter(tabSettings,
                  CppTools::CppToolsSettings::instance()->cppCodeStyle()->codeStyleSettings());
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
        indentBlock(doc, cursor.block(), typedChar, tabSettings);
    }
}
