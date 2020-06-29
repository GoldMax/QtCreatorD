#ifndef DHOVERHANDLER_H
#define DHOVERHANDLER_H

#include <texteditor/basehoverhandler.h>

namespace DEditor {

class DHoverHandler : public TextEditor::BaseHoverHandler
{
public:
	DHoverHandler();

private:
	void identifyMatch(TextEditor::TextEditorWidget *editorWidget,
																				int pos,
																				BaseHoverHandler::ReportPriority report) override;
};

} // namespace DEditor

#endif // DHOVERHANDLER_H
