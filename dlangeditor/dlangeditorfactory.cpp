#include "_dlangeditorconstants.h"
#include "dlangeditorfactory.h"
#include "dlangeditorplugin.h"
#include "dlangtexteditor.h"

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <coreplugin/icore.h>
#include <coreplugin/infobar.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/basetextdocument.h>

#include <QCoreApplication>

using namespace DLangEditor;
using namespace DLangEditor::Constants;

DLangEditorFactory::DLangEditorFactory(QObject *parent)
 : Core::IEditorFactory(parent)
{
 setId(C_DLANGEDITOR_ID);
 setDisplayName(qApp->translate("OpenWith::Editors", C_DLANGEDITOR_DISPLAY_NAME));
 addMimeType(DLangEditor::Constants::DLANG_MIMETYPE);

 m_actionHandler = new TextEditorActionHandler(
     Constants::C_DLANGEDITOR_ID,
     TextEditorActionHandler::Format |
     TextEditorActionHandler::UnCommentSelection |
     TextEditorActionHandler::UnCollapseAll);
}
DLangEditorFactory::~DLangEditorFactory()
{
 delete m_actionHandler;
}

Core::IEditor *DLangEditorFactory::createEditor(QWidget *parent)
{
 DLangTextEditorWidget *rc = new DLangTextEditorWidget(parent);
 DLangEditorPlugin::instance()->initializeEditor(rc);

 connect(rc, SIGNAL(configured(Core::IEditor*)),
         this, SLOT(updateEditorInfoBar(Core::IEditor*)));
 updateEditorInfoBar(rc->editor());
 return rc->editor();
}
/**
 * Test if syntax highlighter is available (or unneeded) for editor.
 * If not found, show a warning with a link to the relevant settings page.
 */
void DLangEditorFactory::updateEditorInfoBar(Core::IEditor* editor)
{
 DLangTextEditor *editorEditable = qobject_cast<DLangTextEditor *>(editor);
 if (editorEditable)
 {
  BaseTextDocument *file = qobject_cast<BaseTextDocument *>(editor->document());
  if (!file)
   return;
  DLangTextEditorWidget *textEditor =
    static_cast<DLangTextEditorWidget *>(editorEditable->editorWidget());
  Core::Id infoSyntaxDefinition(TextEditor::Constants::INFO_SYNTAX_DEFINITION);
  Core::InfoBar *infoBar = file->infoBar();
  if (!textEditor->isMissingSyntaxDefinition()) {
   infoBar->removeInfo(infoSyntaxDefinition);
  } else if (infoBar->canInfoBeAdded(infoSyntaxDefinition)) {
   Core::InfoBarEntry info(infoSyntaxDefinition,
                           tr("A highlight definition was not found for this file. "
                              "Would you like to try to find one?"),
                           Core::InfoBarEntry::GlobalSuppressionEnabled);
   info.setCustomButtonInfo(tr("Show highlighter options..."),
                            textEditor, SLOT(acceptMissingSyntaxDefinitionInfo()));
   infoBar->addInfo(info);
  }
 }
}
