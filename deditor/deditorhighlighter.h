#ifndef DEDITORHIGHLIGHTER_H
#define DEDITORHIGHLIGHTER_H

#include "deditorconstants.h"

#include <texteditor/syntaxhighlighter.h>
#include <texteditor/ihighlighterfactory.h>
#include <cplusplus/Token.h>

namespace DEditor {
namespace Internal {

enum CppFormats {
    CppNumberFormat,
    CppStringFormat,
    CppTypeFormat,
    CppKeywordFormat,
    CppOperatorFormat,
    CppPreprocessorFormat,
    CppLabelFormat,
    CppCommentFormat,
    CppDoxygenCommentFormat,
    CppDoxygenTagFormat,
    CppVisualWhitespace,
    NumCppFormats
};

class DTextEditorWidget;

class DEditorHighlighter : public TextEditor::SyntaxHighlighter
{
 Q_OBJECT

public:
 explicit DEditorHighlighter(QTextDocument *parent = 0);
 explicit DEditorHighlighter(TextEditor::BaseTextDocument *parent);
 virtual ~DEditorHighlighter();

protected:
 void highlightBlock(const QString &text);

private:
 void init();

 void highlightWord(QStringRef word, int position, int length);
 void highlightLine(const QString &line, int position, int length,
                    const QTextCharFormat &format);

 void highlightDoxygenComment(const QString &text, int position,
                              int length);

 bool isPPKeyword(const QStringRef &text) const;

 void correctTokens(QList<CPlusPlus::Token>& tokens, const QString& text);
};

class DEditorHighlighterFactory : public TextEditor::IHighlighterFactory
{
 Q_OBJECT
public:
 DEditorHighlighterFactory()
 {
  setId(DEditor::Constants::C_DEDITOR_ID);
  addMimeType(DEditor::Constants::D_MIMETYPE_SRC);
  addMimeType(DEditor::Constants::D_MIMETYPE_HDR);
 }

 virtual TextEditor::SyntaxHighlighter *createHighlighter() const
 {
  return new DEditorHighlighter;
 }
};

} // namespace Internal
} // namespace DEditor

#endif // DEDITORHIGHLIGHTER_H
