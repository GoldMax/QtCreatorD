#include "dlanghoverhandler.h"
#include "dlangeditor.h"
#include "dlangeditoreditable.h"

#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/helpmanager.h>
#include <extensionsystem/pluginmanager.h>
#include <texteditor/itexteditor.h>
#include <texteditor/basetexteditor.h>

#include <QTextCursor>
#include <QUrl>

using namespace DLangEditor;
using namespace DLangEditor::Internal;
using namespace Core;

DLangHoverHandler::DLangHoverHandler(QObject *parent) : BaseHoverHandler(parent)
{}

DLangHoverHandler::~DLangHoverHandler()
{}

bool DLangHoverHandler::acceptEditor(IEditor *editor)
{
				if (qobject_cast<DLangEditorEditable *>(editor) != 0)
        return true;
    return false;
}

void DLangHoverHandler::identifyMatch(TextEditor::ITextEditor *editor, int pos)
{
				if (DLangTextEditorWidget *glslEditor = qobject_cast<DLangTextEditorWidget *>(editor->widget())) {
        if (! glslEditor->extraSelectionTooltip(pos).isEmpty())
            setToolTip(glslEditor->extraSelectionTooltip(pos));
    }
}

void DLangHoverHandler::decorateToolTip()
{
    if (Qt::mightBeRichText(toolTip()))
        setToolTip(Qt::escape(toolTip()));
}
