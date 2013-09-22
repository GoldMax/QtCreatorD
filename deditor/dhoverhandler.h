#ifndef DHOVERHANDLER_H
#define DHOVERHANDLER_H

#include <texteditor/basehoverhandler.h>

#include <QObject>

namespace Core {
class IEditor;
}

namespace TextEditor {
class ITextEditor;
}

namespace DEditor {
namespace Internal {

class DHoverHandler : public TextEditor::BaseHoverHandler
{
 Q_OBJECT
public:
 DHoverHandler(QObject *parent = 0);
 virtual ~DHoverHandler();

private:
 virtual bool acceptEditor(Core::IEditor *editor);
 virtual void identifyMatch(TextEditor::ITextEditor *editor, int pos);
 virtual void decorateToolTip();
};

} // namespace Internal
} // namespace DEditor

#endif // DHOVERHANDLER_H
