#include "deditordocument.h"
#include "deditorconstants.h"
#include "dhighlighter.h"

#include <texteditor/textdocument.h>

using namespace DEditor;

DEditorDocument::DEditorDocument()
{
	setId(DEditor::Constants::C_DEDITOR_ID);
	setSyntaxHighlighter(new DHighlighter);
}
