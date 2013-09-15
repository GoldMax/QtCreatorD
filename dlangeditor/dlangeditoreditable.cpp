#include "dlangeditoreditable.h"
#include "dlangeditor.h"
#include "dlangeditorconstants.h"

#include <texteditor/texteditorconstants.h>
#include <qmldesigner/qmldesignerconstants.h>

#include <coreplugin/mimedatabase.h>
#include <coreplugin/icore.h>
#include <coreplugin/designmode.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/coreconstants.h>

namespace DLangEditor {
namespace Internal {

DLangEditorEditable::DLangEditorEditable(DLangTextEditorWidget *editor)
    : BaseTextEditor(editor)
{
				setContext(Core::Context(DLangEditor::Constants::C_DLANGEDITOR_ID,
                             TextEditor::Constants::C_TEXTEDITOR));
}

} // namespace Internal
} // namespace DLangEditor
