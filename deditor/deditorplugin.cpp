#include "deditorconstants.h"
#include "deditorplugin.h"
#include "dtexteditor.h"
#include "deditorfactory.h"
#include "dfilewizard.h"
#include "dhoverhandler.h"
#include "dcompletionassist.h"
#include "deditorhighlighter.h"
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
//#include <texteditor/generichighlighter/manager.h>

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
   m_settings(0)
{
 QTC_ASSERT(!m_instance, return);
 m_instance = this;
}
DEditorPlugin::~DEditorPlugin()
{
 removeObject(m_editorFactory);
 removeObject(m_settings);

 //delete m_actionHandler;
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
 FileIconProvider::registerIconOverlayForMimeType(":/deditor/images/d.png", Constants::D_MIMETYPE_SRC);
 FileIconProvider::registerIconOverlayForMimeType(":/deditor/images/di.png", Constants::D_MIMETYPE_HDR);

 m_settings = TextEditorSettings::instance();
 if(!m_settings)
  m_settings = new TextEditorSettings(this);

 m_editorFactory = new DEditorFactory(this);
 addObject(m_editorFactory);

 addAutoReleasedObject(new DCompletionAssistProvider);
 addAutoReleasedObject(new DHoverHandler(this));
	addAutoReleasedObject(new DEditorHighlighterFactory);

 //QObject *core = ICore::instance();
 DFileWizard* wizard = new DFileWizard(DFileWizard::Source);
 wizard->setWizardKind(IWizard::FileWizard);
 wizard->setCategory(QLatin1String(Constants::WIZARD_CATEGORY_D));
 wizard->setDisplayCategory(QCoreApplication::translate("DEditor", Constants::WIZARD_TR_CATEGORY_D));
 wizard->setDescription(tr("Creates a D source file."));
 wizard->setDisplayName(tr("D Source File"));
 wizard->setId(QLatin1String("A.Source"));
 addAutoReleasedObject(wizard);

 wizard = new DFileWizard(DFileWizard::Header);
 wizard->setWizardKind(IWizard::FileWizard);
 wizard->setCategory(QLatin1String(Constants::WIZARD_CATEGORY_D));
 wizard->setDisplayCategory(QCoreApplication::translate("DEditor", Constants::WIZARD_TR_CATEGORY_D));
 wizard->setDescription(tr("Creates a D header file."));
 wizard->setDisplayName(tr("D Header File"));
 wizard->setId(QLatin1String("B.Header"));
 addAutoReleasedObject(wizard);

 QAction *action;
 Core::Command *cmd;

 //*** Tools submenu *****************
 Core::ActionContainer *menu = Core::ActionManager::createMenu(Constants::M_TOOLS_D);
 menu->menu()->setTitle(tr("D"));
 //-- Enable/disable DCD
 action = new QAction(tr("Enable DCD"), this);
 cmd = Core::ActionManager::registerAction(action, Constants::D_ACTION_TOGGLE_DCD_ID,
                                           Core::Context(Core::Constants::C_GLOBAL));
 action->setCheckable(true);
 action->setChecked(true);
 connect(action, SIGNAL(triggered()), this, SLOT(toggleDCDAction()));
 menu->addAction(cmd);
 //-- Clear code assist cache
 action = new QAction(tr("Clear DCD cache"), this);
 cmd = Core::ActionManager::registerAction(action, Constants::D_ACTION_CLEARASSISTCACHE_ID,
                                           Core::Context(Core::Constants::C_GLOBAL));
 connect(action, SIGNAL(triggered()), this, SLOT(clearAssistCacheAction()));
 menu->addAction(cmd);
 //--
 Core::ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(menu);
 //*** Tools submenu *****************

 //Context context(Constants::C_DEDITOR_ID);
 ActionContainer *contextMenu =
   ActionManager::createMenu(Constants::M_CONTEXT);

 //contextMenu->addSeparator(context);
 cmd = ActionManager::command(TextEditor::Constants::AUTO_INDENT_SELECTION);
 contextMenu->addAction(cmd);
 cmd = ActionManager::command(TextEditor::Constants::UN_COMMENT_SELECTION);
 contextMenu->addAction(cmd);
 //***

 errorString->clear();
 return true;
}

void DEditorPlugin::toggleDCDAction()
{
 QcdAssist::isDCDEnabled(!QcdAssist::isDCDEnabled());
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
 TextEditorSettings::initializeEditor(editor);
}

Q_EXPORT_PLUGIN2(DEditor, DEditorPlugin)

