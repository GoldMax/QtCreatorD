#ifndef DLangHIGHLIGHTER_H
#define DLangHIGHLIGHTER_H

#include <texteditor/syntaxhighlighter.h>

namespace DLangEditor {
namespace Internal {

class DLangTextEditorWidget;

class Highlighter : public TextEditor::SyntaxHighlighter
{
    Q_OBJECT

public:
    enum Formats {
								DLangNumberFormat,
								DLangStringFormat,
								DLangTypeFormat,
								DLangKeywordFormat,
								DLangOperatorFormat,
								DLangPreprocessorFormat,
								DLangLabelFormat,
								DLangCommentFormat,
								DLangDoxygenCommentFormat,
								DLangDoxygenTagFormat,
								DLangVisualWhitespace,
								DLangReservedKeyword,
								NumDLangFormats
    };

    explicit Highlighter(QTextDocument *parent = 0);
    explicit Highlighter(TextEditor::BaseTextDocument *parent);
    virtual ~Highlighter();

protected:
    void highlightBlock(const QString &text);
    void highlightLine(const QString &text, int position, int length, const QTextCharFormat &format);
    bool isPPKeyword(const QStringRef &text) const;

private:
    void init();
};

} // namespace Internal
} // namespace DLangEditor

#endif // DLangHIGHLIGHTER_H
