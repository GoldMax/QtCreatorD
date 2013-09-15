#include "dlanghighlighterfactory.h"
#include "dlangeditorconstants.h"
#include "dlanghighlighter.h"

using namespace DLangEditor::Internal;

DLangHighlighterFactory::DLangHighlighterFactory()
{
				setId(DLangEditor::Constants::C_DLANGEDITOR_ID);
				addMimeType(DLangEditor::Constants::DLANG_MIMETYPE);
				addMimeType(DLangEditor::Constants::DLANG_MIMETYPE_INT);

}

TextEditor::SyntaxHighlighter *DLangHighlighterFactory::createHighlighter() const
{
    return new Highlighter;
}
