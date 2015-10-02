#include "dhoverhandler.h"
#include "deditorfactory.h"

using namespace DEditor;

DHoverHandler::DHoverHandler() : BaseHoverHandler()
{}

void DHoverHandler::identifyMatch(TextEditor::TextEditorWidget* editorWidget, int pos)
{
	if (!editorWidget->extraSelectionTooltip(pos).isEmpty())
		setToolTip(editorWidget->extraSelectionTooltip(pos));
}

void DHoverHandler::decorateToolTip()
{
	if (Qt::mightBeRichText(toolTip()))
		setToolTip(toolTip().toHtmlEscaped());
}
