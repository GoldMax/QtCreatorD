#include "deditorplugin.h"
#include "deditorconstants.h"
#include "deditor.h"
#include "deditordocument.h"
#include "dfilewizard.h"
#include "dcompletionassist.h"
#include "dindenter.h"
#include "dautocompleter.h"
#include "dhoverhandler.h"
#include "qcdassist.h"

#include <coreplugin/icore.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/fileiconprovider.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/command.h>

#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditor.h>
#include <texteditor/textdocument.h>
#include <texteditor/texteditoractionhandler.h>
#include <texteditor/snippets/snippetprovider.h>

//#include <utils/qtcassert.h>
//#include <utils/mimetypes/mimedatabase.h>

#include <QCoreApplication>
#include <QAction>
#include <QMenu>
#include <QList>
//#include <QMessageBox>
//#include <QMainWindow>
//#include <QtPlugin>
//#include <QShortcut>

using namespace Core;
using namespace TextEditor;
//using namespace TextEditor::Internal;

using namespace DEditor;

namespace DEditor
{
	//---- DEditorFactory ----------------------------
	class DEditorFactory : public TextEditorFactory
	{
	public:
		DEditorFactory()
		{
			setId(DEditor::Constants::C_DEDITOR_ID);
			setDisplayName(QCoreApplication::translate("OpenWith::Editors",
																																		DEditor::Constants::C_DEDITOR_DISPLAY_NAME));
			addMimeType(DEditor::Constants::D_MIMETYPE_SRC);
			addMimeType(DEditor::Constants::D_MIMETYPE_HDR);


			setDocumentCreator([]() { return new DEditorDocument; });
			setEditorCreator([]() { return new DTextEditor; });
			setEditorWidgetCreator([]() { return new DEditorWidget; });
			setCommentDefinition(Utils::CommentDefinition::CppStyle);
			setCodeFoldingSupported(true);
			setParenthesesMatchingEnabled(true);
			setMarksVisible(true);

			setIndenterCreator([](QTextDocument * d) { return new DIndenter(d); });
			setCompletionAssistProvider(new DCompletionAssistProvider);
			setAutoCompleterCreator([]() { return new DAutoCompleter; });

			setEditorActionHandlers(TextEditorActionHandler::Format
																									| TextEditorActionHandler::UnCommentSelection
																									| TextEditorActionHandler::UnCollapseAll
																									| TextEditorActionHandler::FollowSymbolUnderCursor);

			addHoverHandler(new DHoverHandler);
		}
	};

	//--- DEditorPluginPrivate -----------------------
	class DEditorPluginPrivate : public QObject
	{
	public:
		DEditorFactory m_editorFactory;
	};
}

DEditorPlugin* DEditorPlugin::m_instance = nullptr;

DEditorPlugin::DEditorPlugin()
{
	m_instance = this;
}
DEditorPlugin::~DEditorPlugin()
{
	delete d;
	d = nullptr;
	m_instance = nullptr;
}

bool DEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	Q_UNUSED(arguments)

	d = new DEditorPluginPrivate;

	SnippetProvider::registerGroup(Constants::D_SNIPPETS_GROUP_ID, tr("D", "SnippetProvider"),
																																&DTextEditor::decorateEditor);

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


