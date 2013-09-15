#include "dlangeditorfactory.h"
#include "dlangeditoreditable.h"
#include "dlangeditor.h"
#include "dlangeditorconstants.h"
#include "dlangeditorplugin.h"

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>

#include <coreplugin/icore.h>

#include <QCoreApplication>
#include <QSettings>

using namespace DLangEditor::Internal;
using namespace DLangEditor::Constants;

DLangEditorFactory::DLangEditorFactory(QObject *parent)
  : Core::IEditorFactory(parent)
{
				setId(C_DLANGEDITOR_ID);
				setDisplayName(qApp->translate("OpenWith::Editors", C_DLANGEDITOR_DISPLAY_NAME));
				addMimeType(DLangEditor::Constants::DLANG_MIMETYPE);
				addMimeType(DLangEditor::Constants::DLANG_MIMETYPE_INT);

}

Core::IEditor *DLangEditorFactory::createEditor(QWidget *parent)
{
				DLangTextEditorWidget *rc = new DLangTextEditorWidget(parent);
				DLangEditorPlugin::instance()->initializeEditor(rc);
    return rc->editor();
}

void DLangEditorFactory::updateEditorInfoBar(Core::IEditor *)
{
}
