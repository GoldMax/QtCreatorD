#include "deditorconstants.h"
#include "deditorfactory.h"
#include "deditorplugin.h"
#include "dtexteditor.h"

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <coreplugin/icore.h>
#include <coreplugin/infobar.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/basetextdocument.h>

#include <QCoreApplication>

using namespace DEditor::Internal;
using namespace DEditor::Constants;
using namespace TextEditor;

DEditorFactory::DEditorFactory(QObject *parent)
 : Core::IEditorFactory(parent)
{
 setId(C_DEDITOR_ID);
 setDisplayName(qApp->translate("OpenWith::Editors", C_DEDITOR_DISPLAY_NAME));
 addMimeType(DEditor::Constants::D_MIMETYPE_SRC);
 addMimeType(DEditor::Constants::D_MIMETYPE_HDR);
 new TextEditor::TextEditorActionHandler(this, Constants::C_DEDITOR_ID,
                             TextEditor::TextEditorActionHandler::Format
                             | TextEditor::TextEditorActionHandler::UnCommentSelection
                             | TextEditor::TextEditorActionHandler::UnCollapseAll);
}

DEditorFactory::~DEditorFactory() {}

Core::IEditor* DEditorFactory::createEditor()
{
 DTextEditorWidget *rc = new DTextEditorWidget();
 DEditorPlugin::instance()->initializeEditor(rc);
 return rc->editor();
 //return 0;
}

/**
 * Test if syntax highlighter is available (or unneeded) for editor.
 * If not found, show a warning with a link to the relevant settings page.
 */
void DEditorFactory::updateEditorInfoBar(Core::IEditor* editor)
{
 Q_UNUSED(editor);
// DTextEditor *editorEditable = qobject_cast<DTextEditor *>(editor);
// if (editorEditable)
// {
//  BaseTextDocument *file = qobject_cast<BaseTextDocument *>(editor->document());
//  if (!file)
//   return;
//  DTextEditorWidget *textEditor =
//    static_cast<DTextEditorWidget *>(editorEditable->editorWidget());
//  Core::Id infoSyntaxDefinition(TextEditor::Constants::INFO_SYNTAX_DEFINITION);
//  Core::InfoBar *infoBar = file->infoBar();
//  if (!textEditor->isMissingSyntaxDefinition()) {
//   infoBar->removeInfo(infoSyntaxDefinition);
//  } else if (infoBar->canInfoBeAdded(infoSyntaxDefinition)) {
//   Core::InfoBarEntry info(infoSyntaxDefinition,
//                           tr("A highlight definition was not found for this file. "
//                              "Would you like to try to find one?"),
//                           Core::InfoBarEntry::GlobalSuppressionEnabled);
//   info.setCustomButtonInfo(tr("Show highlighter options..."),
//                            textEditor, SLOT(acceptMissingSyntaxDefinitionInfo()));
//   infoBar->addInfo(info);
//  }
// }
}
