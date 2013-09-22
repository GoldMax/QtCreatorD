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
// if (!Utils::HostOsInfo::isMacHost() && !Utils::HostOsInfo::isWindowsHost()) {
//     FileIconProvider *iconProvider = FileIconProvider::instance();
//     iconProvider->registerIconOverlayForMimeType(QIcon(QLatin1String(":/cppeditor/images/qt_cpp.png")),
//                                                  MimeDatabase::findByType(QLatin1String(CppEditor::Constants::CPP_SOURCE_MIMETYPE)));
//     iconProvider->registerIconOverlayForMimeType(QIcon(QLatin1String(":/cppeditor/images/qt_c.png")),
//                                                  MimeDatabase::findByType(QLatin1String(CppEditor::Constants::C_SOURCE_MIMETYPE)));
//     iconProvider->registerIconOverlayForMimeType(QIcon(QLatin1String(":/cppeditor/images/qt_h.png")),
//                                                  MimeDatabase::findByType(QLatin1String(CppEditor::Constants::CPP_HEADER_MIMETYPE)));
// }

}

DEditorFactory::~DEditorFactory() {}

Core::IEditor* DEditorFactory::createEditor(QWidget *parent)
{
 DTextEditorWidget *rc = new DTextEditorWidget(parent);
 DEditorPlugin::instance()->initializeEditor(rc);

 connect(rc, SIGNAL(configured(Core::IEditor*)),
         this, SLOT(updateEditorInfoBar(Core::IEditor*)));
 updateEditorInfoBar(rc->editor());
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
