#ifndef DLangINDENTER_H
#define DLangINDENTER_H

#include <texteditor/indenter.h>

namespace DLangEditor {
namespace Internal {

class DLangIndenter : public TextEditor::Indenter
{
public:
				DLangIndenter();
				virtual ~DLangIndenter();

    virtual bool isElectricCharacter(const QChar &ch) const;
    virtual void indentBlock(QTextDocument *doc,
                             const QTextBlock &block,
                             const QChar &typedChar,
                             const TextEditor::TabSettings &tabSettings);

    virtual void indent(QTextDocument *doc,
                        const QTextCursor &cursor,
                        const QChar &typedChar,
                        const TextEditor::TabSettings &tabSettings);
};

} // Internal
} // DLangEditor

#endif // DLangINDENTER_H
