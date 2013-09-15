#ifndef DLangHOVERHANDLER_H
#define DLangHOVERHANDLER_H

#include <texteditor/basehoverhandler.h>

#include <QObject>

namespace Core {
class IEditor;
}

namespace TextEditor {
class ITextEditor;
}

namespace DLangEditor {
namespace Internal {

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

} // namespace Internal
} // namespace DLangEditor

#endif // DLangHOVERHANDLER_H
