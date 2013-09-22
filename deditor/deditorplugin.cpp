#include "deditorconstants.h"
#include "deditorplugin.h"
#include "dtexteditor.h"
#include "deditorfactory.h"
#include "dfilewizard.h"
#include "dhoverhandler.h"
#include "dcompletionassist.h"
#include "qcdassist.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/id.h>
#include <coreplugin/fileiconprovider.h>
#include <utils/qtcassert.h>
#include <texteditor/generichighlighter/manager.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QtPlugin>
#include <QCoreApplication>
#include <QShortcut>

using namespace Core;
using namespace TextEditor;
using namespace TextEditor::Internal;

using namespace DEditor::Internal;

DEditorPlugin *DEditorPlugin::m_instance = 0;

DEditorPlugin::DEditorPlugin()
 : m_editorFactory(0),
   m_settings(0),
   m_searchResultWindow(0)
{
 QTC_ASSERT(!m_instance, return);
 m_instance = this;
}
DEditorPlugin::~DEditorPlugin()
{
 delete m_actionHandler;
 m_instance = 0;
}

bool DEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
 // Register objects in the plugin manager's object pool
 // Load settings
 // Add actions to menus
 // Connect to other plugins' signals
 // In the initialize method, a plugin can be sure that the plugins it
 // depends on have initialized their members.

 Q_UNUSED(arguments)

 if (!MimeDatabase::addMimeTypes(QLatin1String(":/deditor/DEditor.mimetypes.xml"), errorString))
  return false;

 QObject *core = ICore::instance();
 BaseFileWizardParameters wizardParameters(IWizard::FileWizard);
 wizardParameters.setKind(IWizard::FileWizard);
 wizardParameters.setCategory(QLatin1String(Constants::WIZARD_CATEGORY_D));
 wizardParameters.setDisplayCategory(QCoreApplication::translate("DEditor", Constants::WIZARD_TR_CATEGORY_D));

 wizardParameters.setDescription(tr("Creates a D source file."));
 wizardParameters.setDisplayName(tr("D Source File"));
 wizardParameters.setId(QLatin1String("A.Source"));
 addAutoReleasedObject(new DFileWizard(wizardParameters, DFileWizard::Source, core));

 wizardParameters.setDescription(tr("Creates a D header file."));
 wizardParameters.setDisplayName(tr("D Header File"));
 wizardParameters.setId(QLatin1String("B.Header"));
 addAutoReleasedObject(new DFileWizard(wizardParameters, DFileWizard::Header, core));

 FileIconProvider *iconProvider = FileIconProvider::instance();
 iconProvider->registerIconOverlayForMimeType(
    QIcon(QLatin1String(":/deditor/images/d.png")),
    MimeDatabase::findByType(QLatin1String(Constants::D_MIMETYPE_SRC)));
 iconProvider->registerIconOverlayForMimeType(
    QIcon(QLatin1String(":/deditor/images/di.png")),
    MimeDatabase::findByType(QLatin1String(Constants::D_MIMETYPE_HDR)));

 QAction *action = new QAction(tr("Clear code assist cache"), this);
 Core::Command *cmd = Core::ActionManager::registerAction(action, Constants::D_ACTION_ID,
                                                          Core::Context(Core::Constants::C_GLOBAL));
 connect(action, SIGNAL(triggered()), this, SLOT(clearAssistCacheAction()));

 Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::D_MENU_ID);
 menu->menu()->setTitle(tr("D"));
 menu->addAction(cmd);
 Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);

 m_settings = TextEditorSettings::instance();
 if(!m_settings)
  m_settings = new TextEditorSettings(this);

 m_editorFactory = new DEditorFactory(this);
 addAutoReleasedObject(m_editorFactory);

 m_actionHandler = new TextEditorActionHandler(
    Constants::C_DEDITOR_ID,
    TextEditorActionHandler::Format |
    TextEditorActionHandler::UnCommentSelection |
    TextEditorActionHandler::UnCollapseAll);
 m_actionHandler->initializeActions();

 addAutoReleasedObject(new DCompletionAssistProvider);
 addAutoReleasedObject(new DHoverHandler(this));


 errorString->clear();
 return true;
}

void DEditorPlugin::clearAssistCacheAction()
{
 QcdAssist::sendClearChache();
}

void DEditorPlugin::extensionsInitialized()
{
 // Retrieve objects from the plugin manager's object pool
 // In the extensionsInitialized method, a plugin can be sure that all
 // plugins that depend on it are completely initialized.

 m_searchResultWindow = Find::SearchResultWindow::instance();
// m_outlineFactory->setWidgetFactories(ExtensionSystem::PluginManager::getObjects<TextEditor::IOutlineWidgetFactory>());
 connect(m_settings, SIGNAL(fontSettingsChanged(TextEditor::FontSettings)),
         this, SLOT(updateSearchResultsFont(TextEditor::FontSettings)));

 updateSearchResultsFont(m_settings->fontSettings());

}

ExtensionSystem::IPlugin::ShutdownFlag DEditorPlugin::aboutToShutdown()
{
 // Save settings
 // Disconnect from signals that are not needed during shutdown
 // Hide UI (if you add UI that is not in the main window directly)

 disconnect(m_settings, SIGNAL(fontSettingsChanged(TextEditor::FontSettings)),
         this, SLOT(updateSearchResultsFont(TextEditor::FontSettings)));

 return SynchronousShutdown;
}

void DEditorPlugin::initializeEditor(DTextEditorWidget* editor)
{
 m_actionHandler->setupActions((BaseTextEditorWidget*)(editor));
 TextEditorSettings::instance()->initializeEditor((BaseTextEditorWidget*)editor);
}

void DEditorPlugin::updateSearchResultsFont(const FontSettings &settings)
{
 if (m_searchResultWindow)
 {
  m_searchResultWindow->setTextEditorFont(
     QFont(settings.family(), settings.fontSize() * settings.fontZoom() / 100),
           settings.formatFor(TextEditor::C_TEXT).foreground(),
           settings.formatFor(TextEditor::C_TEXT).background(),
           settings.formatFor(TextEditor::C_SEARCH_RESULT).foreground(),
           settings.formatFor(TextEditor::C_SEARCH_RESULT).background());
 }
}

Q_EXPORT_PLUGIN2(DEditor, DEditorPlugin)

