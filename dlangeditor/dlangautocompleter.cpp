#include "dlangautocompleter.h"

#include <cplusplus/Token.h>
#include <cplusplus/SimpleLexer.h>
#include <cplusplus/MatchingText.h>
#include <cplusplus/BackwardsScanner.h>

#include <QLatin1Char>
#include <QTextCursor>

using namespace DLangEditor;
using namespace Internal;
using namespace CPlusPlus;

DLangCompleter::DLangCompleter()
{}

DLangCompleter::~DLangCompleter()
{}

bool DLangCompleter::contextAllowsAutoParentheses(const QTextCursor &cursor,
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

bool DLangCompleter::contextAllowsElectricCharacters(const QTextCursor &cursor) const
{
    const Token tk = SimpleLexer::tokenAt(cursor.block().text(), cursor.positionInBlock(),
                                          BackwardsScanner::previousBlockState(cursor.block()));

    // XXX Duplicated from CPPEditor::isInComment to avoid tokenizing twice
    if (tk.isComment()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();

        if (pos == tk.end()) {
            if (tk.is(T_CPP_COMMENT) || tk.is(T_CPP_DOXY_COMMENT))
                return false;

            const int state = cursor.block().userState() & 0xFF;
            if (state > 0)
                return false;
        }

        if (pos < tk.end())
            return false;
    } else if (tk.isStringLiteral() || tk.isCharLiteral()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();
        if (pos <= tk.end())
            return false;
    }

    return true;
}

bool DLangCompleter::isInComment(const QTextCursor &cursor) const
{
    const Token tk = SimpleLexer::tokenAt(cursor.block().text(), cursor.positionInBlock(),
                                          BackwardsScanner::previousBlockState(cursor.block()));

    if (tk.isComment()) {
        const unsigned pos = cursor.selectionEnd() - cursor.block().position();

        if (pos == tk.end()) {
            if (tk.is(T_CPP_COMMENT) || tk.is(T_CPP_DOXY_COMMENT))
                return true;

            const int state = cursor.block().userState() & 0xFF;
            if (state > 0)
                return true;
        }

        if (pos < tk.end())
            return true;
    }

    return false;
}

QString DLangCompleter::insertMatchingBrace(const QTextCursor &cursor,
                                           const QString &text,
                                           QChar la,
                                           int *skippedChars) const
{
    MatchingText m;
    return m.insertMatchingBrace(cursor, text, la, skippedChars);
}

QString DLangCompleter::insertParagraphSeparator(const QTextCursor &cursor) const
{
    MatchingText m;
    return m.insertParagraphSeparator(cursor);
}
