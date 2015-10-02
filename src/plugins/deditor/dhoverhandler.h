#ifndef DHOVERHANDLER_H
#define DHOVERHANDLER_H

#include <texteditor/basehoverhandler.h>

namespace DEditor {

class DHoverHandler : public TextEditor::BaseHoverHandler
{
	Q_OBJECT
public:
	DHoverHandler();

private:
	virtual void identifyMatch(TextEditor::TextEditorWidget *editorWidget, int pos);
	virtual void decorateToolTip();
};

} // namespace DEditor

#endif // DHOVERHANDLER_H
