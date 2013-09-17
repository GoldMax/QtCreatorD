#include "dlanghoverhandler.h"
#include "dlangtexteditor.h"

#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/helpmanager.h>
#include <extensionsystem/pluginmanager.h>
#include <texteditor/itexteditor.h>
#include <texteditor/basetexteditor.h>

#include <QTextCursor>
#include <QUrl>

using namespace DLangEditor;
using namespace Core;

DLangHoverHandler::DLangHoverHandler(QObject *parent) : BaseHoverHandler(parent)
{}

DLangHoverHandler::~DLangHoverHandler()
{}

bool DLangHoverHandler::acceptEditor(IEditor *editor)
{
 if (qobject_cast<DLangTextEditor *>(editor) != 0)
  return true;
 return false;
}

void DLangHoverHandler::identifyMatch(TextEditor::ITextEditor *editor, int pos)
{
 if (DLangTextEditorWidget *dEditor = qobject_cast<DLangTextEditorWidget *>(editor->widget())) {
  if (! dEditor->extraSelectionTooltip(pos).isEmpty())
   setToolTip(dEditor->extraSelectionTooltip(pos));
 }
}

void DLangHoverHandler::decorateToolTip()
{
 if (Qt::mightBeRichText(toolTip()))
  setToolTip(Qt::escape(toolTip()));
}
