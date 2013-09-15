#ifndef DLangHIGHLIGHTERFACTORY_H
#define DLangHIGHLIGHTERFACTORY_H

#include <texteditor/ihighlighterfactory.h>

namespace DLangEditor {
namespace Internal {

class DLangHighlighterFactory : public TextEditor::IHighlighterFactory
{
    Q_OBJECT

public:
				DLangHighlighterFactory();

    virtual TextEditor::SyntaxHighlighter *createHighlighter() const;
};

} // namespace Internal
} // namespace DLangEditor

#endif // DLangHIGHLIGHTERFACTORY_H
