#ifndef DLANGHOVERHANDLER_H
#define DLANGHOVERHANDLER_H

#include <texteditor/basehoverhandler.h>

#include <QObject>

namespace Core {
class IEditor;
}

namespace TextEditor {
class ITextEditor;
}

namespace DLangEditor {

class DLangHoverHandler : public TextEditor::BaseHoverHandler
{
 Q_OBJECT
public:
 DLangHoverHandler(QObject *parent = 0);
 virtual ~DLangHoverHandler();

private:
 virtual bool acceptEditor(Core::IEditor *editor);
 virtual void identifyMatch(TextEditor::ITextEditor *editor, int pos);
 virtual void decorateToolTip();
};

} // namespace DLangEditor

#endif // DLANGHOVERHANDLER_H
