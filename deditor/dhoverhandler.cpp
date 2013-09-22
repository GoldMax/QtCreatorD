#include "dhoverhandler.h"
#include "dtexteditor.h"

#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/helpmanager.h>
#include <extensionsystem/pluginmanager.h>
#include <texteditor/itexteditor.h>
#include <texteditor/basetexteditor.h>

#include <QTextCursor>
#include <QUrl>

using namespace DEditor::Internal;
using namespace Core;

DHoverHandler::DHoverHandler(QObject *parent) : BaseHoverHandler(parent)
{}

DHoverHandler::~DHoverHandler()
{}

bool DHoverHandler::acceptEditor(IEditor *editor)
{
 if (qobject_cast<DTextEditor *>(editor) != 0)
  return true;
 return false;
}

void DHoverHandler::identifyMatch(TextEditor::ITextEditor *editor, int pos)
{
 if (DTextEditorWidget *dEditor = qobject_cast<DTextEditorWidget *>(editor->widget())) {
  if (! dEditor->extraSelectionTooltip(pos).isEmpty())
   setToolTip(dEditor->extraSelectionTooltip(pos));
 }
}

void DHoverHandler::decorateToolTip()
{
 if (Qt::mightBeRichText(toolTip()))
  setToolTip(Qt::escape(toolTip()));
}
