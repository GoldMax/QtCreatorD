#include "deditorconstants.h"
#include "deditorplugin.h"
#include "dtexteditor.h"
//#include "dautocompleter.h"
//#include "dindenter.h"
#include "dcompletionassist.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/basetexteditor.h>
#include <texteditor/plaintexteditor.h>

using namespace DEditor::Internal;
using namespace TextEditor;

DTextEditor::DTextEditor(DTextEditorWidget* editor)
 : BaseTextEditor(editor)
{
 setContext(Core::Context(DEditor::Constants::C_DEDITOR_ID,
                          TextEditor::Constants::C_TEXTEDITOR));
}

Core::IEditor* DTextEditor::duplicate(QWidget *parent)
{
 DTextEditorWidget *newWidget = new DTextEditorWidget(parent);
 newWidget->duplicateFrom(editorWidget());
 DEditorPlugin::instance()->initializeEditor(newWidget);
 return newWidget->editor();
}

Core::Id DTextEditor::id() const
{
 //return Core::Constants::K_DEFAULT_TEXT_EDITOR_ID;
 return DEditor::Constants::C_DEDITOR_ID;
}

//-----------------------------
//- DTextEditorWidget
//-----------------------------

DTextEditorWidget::DTextEditorWidget(QWidget *parent)
  : PlainTextEditorWidget(parent) // BaseTextEditorWidget(parent)
{
 // Indenter вызывает исключения в стандартном hightlighter
 //setIndenter(new DIndenter());
}

TextEditor::IAssistInterface* DTextEditorWidget::createAssistInterface(
  TextEditor::AssistKind kind, TextEditor::AssistReason reason) const
{
 if (kind == TextEditor::Completion)
  return new DCompletionAssistInterface(document(),
                                            position(),
                                            editor()->document()->filePath(),
                                            reason);
 return BaseTextEditorWidget::createAssistInterface(kind, reason);
}

TextEditor::BaseTextEditor* DTextEditorWidget::createEditor()
{
 DTextEditor* edit = new DTextEditor(this);
 //createToolBar(edit);
 return edit;
}

