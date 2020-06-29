#include "dhoverhandler.h"

#include <texteditor/texteditor.h>
#include <texteditor/basehoverhandler.h>

using namespace DEditor;

DHoverHandler::DHoverHandler() : BaseHoverHandler()
{

}

void DHoverHandler::identifyMatch(TextEditor::TextEditorWidget *editorWidget, int pos,
																																		BaseHoverHandler::ReportPriority report)
{
	BaseHoverHandler::identifyMatch(editorWidget, pos, report);

}

