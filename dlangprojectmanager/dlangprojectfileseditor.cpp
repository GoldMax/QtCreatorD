#include "dlangprojectfileseditor.h"
#include "dlangprojectmanager.h"
#include "dlangprojectconstants.h"

#include <coreplugin/editormanager/editormanager.h>
#include <texteditor/fontsettings.h>
#include <texteditor/texteditoractionhandler.h>
#include <texteditor/texteditorsettings.h>

#include <QCoreApplication>
#include <QSharedPointer>

using namespace TextEditor;

namespace DLangProjectManager {
namespace Internal {

//--------------------------------------------------------------------------------------
//
// ProjectFilesFactory
//
//--------------------------------------------------------------------------------------

ProjectFilesFactory::ProjectFilesFactory(Manager *manager, TextEditorActionHandler *handler)
    : Core::IEditorFactory(manager),
      m_actionHandler(handler)
{
 setId(Constants::FILES_EDITOR_ID);
 setDisplayName(QCoreApplication::translate("OpenWith::Editors", ".files Editor"));
 addMimeType(Constants::FILES_MIMETYPE);
}

Core::IEditor *ProjectFilesFactory::createEditor(QWidget *parent)
{
 ProjectFilesEditorWidget *ed = new ProjectFilesEditorWidget(parent, this, m_actionHandler);
 TextEditorSettings::instance()->initializeEditor(ed);
 return ed->editor();
}

//--------------------------------------------------------------------------------------
//
// ProjectFilesEditable
//
//--------------------------------------------------------------------------------------

ProjectFilesEditor::ProjectFilesEditor(ProjectFilesEditorWidget *editor)
 : BaseTextEditor(editor)
{
 setContext(Core::Context(Constants::C_FILESEDITOR));
}

Core::Id ProjectFilesEditor::id() const
{
 return Core::Id(Constants::FILES_EDITOR_ID);
}

bool ProjectFilesEditor::duplicateSupported() const
{
 return true;
}

Core::IEditor *ProjectFilesEditor::duplicate(QWidget *parent)
{
 ProjectFilesEditorWidget *parentEditor = qobject_cast<ProjectFilesEditorWidget *>(editorWidget());
 ProjectFilesEditorWidget *editor = new ProjectFilesEditorWidget(parent,
                                                                 parentEditor->factory(),
                                                                 parentEditor->actionHandler());
 TextEditorSettings::instance()->initializeEditor(editor);
 return editor->editor();
}

//--------------------------------------------------------------------------------------
//
// ProjectFilesEditor
//
//--------------------------------------------------------------------------------------

ProjectFilesEditorWidget::ProjectFilesEditorWidget(QWidget *parent, ProjectFilesFactory *factory,
                                                   TextEditorActionHandler *handler)
 : BaseTextEditorWidget(parent),
   m_factory(factory),
   m_actionHandler(handler)
{
 QSharedPointer<BaseTextDocument> doc(new BaseTextDocument());
 setBaseTextDocument(doc);

 handler->setupActions(this);
}

ProjectFilesFactory *ProjectFilesEditorWidget::factory() const
{
 return m_factory;
}

TextEditorActionHandler *ProjectFilesEditorWidget::actionHandler() const
{
 return m_actionHandler;
}

BaseTextEditor *ProjectFilesEditorWidget::createEditor()
{
 return new ProjectFilesEditor(this);
}

} // namespace Internal
} // namespace DLangProjectManager
