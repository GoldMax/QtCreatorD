#include "deditorconstants.h"
#include "deditorplugin.h"
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
//#include <coreplugin/mimedatabase.h>
#include <coreplugin/id.h>
#include <coreplugin/fileiconprovider.h>
#include <utils/qtcassert.h>
//#include <texteditor/generichighlighter/manager.h>
#include <utils/mimetypes/mimedatabase.h>

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

using namespace DEditor;

DEditorPlugin* DEditorPlugin::m_instance = 0;

DEditorPlugin::DEditorPlugin()
{
	QTC_ASSERT(!m_instance, return);
	m_instance = this;
}
DEditorPlugin::~DEditorPlugin()
{
	m_instance = NULL;
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

	Utils::MimeDatabase::addMimeTypes(QLatin1String(":/deditor/DEditor.mimetypes.xml"));

	addAutoReleasedObject(new DEditorFactory);
	addAutoReleasedObject(new DCompletionAssistProvider);

	IWizardFactory::registerFactoryCreator([]()
	{
		return QList<IWizardFactory *>() <<
			new DFileWizardFactory(DFileWizardFactory::FileType::Source) <<
			new DFileWizardFactory(DFileWizardFactory::FileType::Header);
	});

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
void DEditorPlugin::extensionsInitialized()
{
	// Retrieve objects from the plugin manager's object pool
	// In the extensionsInitialized method, a plugin can be sure that all
	// plugins that depend on it are completely initialized.
	FileIconProvider::registerIconOverlayForMimeType(":/deditor/images/d.png", Constants::D_MIMETYPE_SRC);
	FileIconProvider::registerIconOverlayForMimeType(":/deditor/images/di.png", Constants::D_MIMETYPE_HDR);
}
ExtensionSystem::IPlugin::ShutdownFlag DEditorPlugin::aboutToShutdown()
{
	// Save settings
	// Disconnect from signals that are not needed during shutdown
	// Hide UI (if you add UI that is not in the main window directly)
	return IPlugin::aboutToShutdown();
}

void DEditorPlugin::toggleDCDAction()
{
	QcdAssist::isDCDEnabled(!QcdAssist::isDCDEnabled());
}
void DEditorPlugin::clearAssistCacheAction()
{
	QcdAssist::sendClearChache();
}


