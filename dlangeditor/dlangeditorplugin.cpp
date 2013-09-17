#include "_dlangeditorconstants.h"
#include "dlangeditorplugin.h"
//#include "dlangeditor.h"
#include "dlangeditorfactory.h"
#include "dlangfilewizard.h"
//#include "dlanghoverhandler.h"
//#include "dlangcompletionassist.h"

#include <coreplugin/icore.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/id.h>
#include <coreplugin/fileiconprovider.h>
#include <utils/qtcassert.h>
#include <texteditor/generichighlighter/manager.h>

#include <QtPlugin>
#include <QCoreApplication>
#include <QShortcut>

using namespace Core;
using namespace TextEditor;
using namespace TextEditor::Internal;


using namespace DLangEditor;
using namespace DLangEditor::Internal;

DLangEditorPlugin *DLangEditorPlugin::m_instance = 0;

DLangEditorPlugin::DLangEditorPlugin()
 : m_editorFactory(0),
   m_settings(0),
   m_searchResultWindow(0)
{
 QTC_ASSERT(!m_instance, return);
 m_instance = this;
}
DLangEditorPlugin::~DLangEditorPlugin()
{
 //removeObject(m_editorFactory);
 m_instance = 0;
}

bool DLangEditorPlugin::initialize(const QStringList & arguments, QString *errorMessage)
{
 Q_UNUSED(arguments);

 if (!MimeDatabase::addMimeTypes(QLatin1String(":/dlangeditor/DLangEditor.mimetypes.xml"), errorMessage))
  return false;

 QObject *core = ICore::instance();
 BaseFileWizardParameters emptyWizardParameters(IWizard::FileWizard);
 emptyWizardParameters.setCategory(QLatin1String(Constants::WIZARD_CATEGORY_DLANG));
 emptyWizardParameters.setDisplayCategory(QCoreApplication::translate("DLangEditor", Constants::WIZARD_TR_CATEGORY_DLANG));
 emptyWizardParameters.setDescription(tr("Creates a empty D language file."));
 emptyWizardParameters.setDisplayName(tr("Empty D file"));
 emptyWizardParameters.setId(QLatin1String("F.DLang"));
 addAutoReleasedObject(new DLangFileWizard(emptyWizardParameters, DLangFileWizard::Empty, core));

 FileIconProvider *iconProvider = FileIconProvider::instance();
 iconProvider->registerIconOverlayForMimeType(QIcon(QLatin1String(":/dlangeditor/images/d.png")),
                                              MimeDatabase::findByType(QLatin1String(DLangEditor::Constants::DLANG_MIMETYPE)));
 m_settings = TextEditorSettings::instance();
 if(!m_settings)
  m_settings = new TextEditorSettings(this);

 m_editorFactory = new DLangEditorFactory(this);
 addAutoReleasedObject(m_editorFactory);

 m_editorFactory->actionHandler()->initializeActions();
 errorMessage->clear();
 return true;
// addAutoReleasedObject(new DLangCompletionAssistProvider);
// addAutoReleasedObject(new DLangHoverHandler(this));

}

void DLangEditorPlugin::initializeEditor(DLangTextEditorWidget* editor)
{
 // common actions
 m_editorFactory->actionHandler()->
   setupActions((BaseTextEditorWidget*)(editor));

 TextEditorSettings::instance()->initializeEditor((BaseTextEditorWidget*)editor);
}

void DLangEditorPlugin::extensionsInitialized()
{
 m_searchResultWindow = Find::SearchResultWindow::instance();

// m_outlineFactory->setWidgetFactories(ExtensionSystem::PluginManager::getObjects<TextEditor::IOutlineWidgetFactory>());

 connect(m_settings, SIGNAL(fontSettingsChanged(TextEditor::FontSettings)),
         this, SLOT(updateSearchResultsFont(TextEditor::FontSettings)));

 updateSearchResultsFont(m_settings->fontSettings());
}

void DLangEditorPlugin::updateSearchResultsFont(const FontSettings &settings)
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

Q_EXPORT_PLUGIN(DLangEditor::Internal::DLangEditorPlugin)


/*
#include "dlangeditorplugin.h"
#include "dlangeditor.h"
#include "dlangeditorconstants.h"
#include "dlangeditorfactory.h"
#include "dlangfilewizard.h"
#include "dlanghoverhandler.h"
#include "dlangcompletionassist.h"

#include <coreplugin/icore.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/mimedatabase.h>
#include <coreplugin/id.h>
#include <coreplugin/fileiconprovider.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/editormanager/editormanager.h>
#include <projectexplorer/taskhub.h>
#include <extensionsystem/pluginmanager.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/textfilewizard.h>
#include <texteditor/texteditoractionhandler.h>
#include <utils/qtcassert.h>

//#include <dlang/dlangengine.h>
//#include <dlang/dlangparser.h>
//#include <dlang/dlanglexer.h>

#include <QtPlugin>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QTimer>
#include <QMenu>
#include <QAction>

using namespace Core;
using namespace TextEditor;

namespace DLangEditor {
namespace Internal {

DLangEditorPlugin *DLangEditorPlugin::m_instance = 0;

//DLangEditorPlugin::InitFile::~InitFile()
//{
//    delete engine;
//}

DLangEditorPlugin::DLangEditorPlugin() :
    m_editor(0),
    m_actionHandler(0)//
    //,m_dlang_120_frag(0),
    //m_dlang_120_vert(0),
    //m_dlang_120_common(0),
    //m_dlang_es_100_frag(0),
    //m_dlang_es_100_vert(0),
    //m_dlang_es_100_common(0)
{
    m_instance = this;
}

DLangEditorPlugin::~DLangEditorPlugin()
{
    removeObject(m_editor);
    delete m_actionHandler;
//    delete m_dlang_120_frag;
//    delete m_dlang_120_vert;
//    delete m_dlang_120_common;
//    delete m_dlang_es_100_frag;
//    delete m_dlang_es_100_vert;
//    delete m_dlang_es_100_common;
    m_instance = 0;
}

bool DLangEditorPlugin::initialize(const QStringList & arguments, QString *errorMessage)
{
				if (!MimeDatabase::addMimeTypes(QLatin1String(":/dlangeditor/DLangEditor.mimetypes.xml"), errorMessage))
        return false;

				m_editor = new DLangEditorFactory(this);
    addObject(m_editor);

				addAutoReleasedObject(new DLangCompletionAssistProvider);
    addAutoReleasedObject(new DLangHoverHandler(this));

				m_actionHandler = new TextEditorActionHandler(Constants::C_DLANGEDITOR_ID,
						TextEditorActionHandler::Format
						| TextEditorActionHandler::UnCommentSelection
						| TextEditorActionHandler::UnCollapseAll);
    m_actionHandler->initializeActions();

				ActionContainer *contextMenu = ActionManager::createMenu(DLangEditor::Constants::M_CONTEXT);
				ActionContainer *dlangToolsMenu = ActionManager::createMenu(Id(Constants::M_TOOLS_DLang));
				dlangToolsMenu->setOnAllDisabledBehavior(ActionContainer::Hide);
				QMenu *menu = dlangToolsMenu->menu();
				//: DLang sub-menu in the Tools menu
				menu->setTitle(tr("DLang"));
				ActionManager::actionContainer(Core::Constants::M_TOOLS)->addMenu(dlangToolsMenu);

    Command *cmd = 0;

    // Insert marker for "Refactoring" menu:
    Context globalContext(Core::Constants::C_GLOBAL);
    Command *sep = contextMenu->addSeparator(globalContext);
    sep->action()->setObjectName(QLatin1String(Constants::M_REFACTORING_MENU_INSERTION_POINT));
    contextMenu->addSeparator(globalContext);

    cmd = ActionManager::command(TextEditor::Constants::UN_COMMENT_SELECTION);
    contextMenu->addAction(cmd);

    errorMessage->clear();

    FileIconProvider *iconProvider = FileIconProvider::instance();
				iconProvider->registerIconOverlayForMimeType(QIcon(QLatin1String(":/dlangeditor/images/d.png")),
					MimeDatabase::findByType(QLatin1String(DLangEditor::Constants::DLANG_MIMETYPE)));


    QObject *core = ICore::instance();
    BaseFileWizardParameters emptyWizardParameters(IWizard::FileWizard);
    emptyWizardParameters.setCategory(QLatin1String(Constants::WIZARD_CATEGORY_DLANG));
    emptyWizardParameters.setDisplayCategory(QCoreApplication::translate("DLangEditor", Constants::WIZARD_TR_CATEGORY_DLANG));
    emptyWizardParameters.setDescription(tr("Creates a empty D language file."));
    emptyWizardParameters.setDisplayName(tr("Empty D file"));
    emptyWizardParameters.setId(QLatin1String("F.DLang"));
    addAutoReleasedObject(new DLangFileWizard(emptyWizardParameters, DLangFileWizard::Empty, core));

    return true;
}

void DLangEditorPlugin::extensionsInitialized()
{
}

ExtensionSystem::IPlugin::ShutdownFlag DLangEditorPlugin::aboutToShutdown()
{
				// delete DLang::Icons::instance(); // delete object held by singleton
    return IPlugin::aboutToShutdown();
}

void DLangEditorPlugin::initializeEditor(DLangTextEditorWidget *editor)
{
    QTC_CHECK(m_instance);
    m_actionHandler->setupActions(editor);
    TextEditorSettings::instance()->initializeEditor(editor);
}

//DLangEditorPlugin::InitFile *DLangEditorPlugin::getInitFile(const QString &fileName, InitFile **initFile) const
//{
//    if (*initFile)
//        return *initFile;
//				*initFile = new DLangEditorPlugin::InitFile;
//    parseGlslFile(fileName, *initFile);
//    return *initFile;
//}

//QByteArray DLangEditorPlugin::dlangFile(const QString &fileName) const
//{
//    QFile file(ICore::resourcePath() + QLatin1String("/dlang/") + fileName);
//    if (file.open(QFile::ReadOnly))
//        return file.readAll();
//    return QByteArray();
//}

//void DLangEditorPlugin::parseGlslFile(const QString &fileName, InitFile *initFile) const
//{
//    // Parse the builtins for any langugage variant so we can use all keywords.
//				const int variant = DLang::Lexer::Variant_All;

//    const QByteArray code = dlangFile(fileName);
//				initFile->engine = new DLang::Engine();
//				DLang::Parser parser(initFile->engine, code.constData(), code.size(), variant);
//    initFile->ast = parser.parse();
//}

//const DLangEditorPlugin::InitFile *DLangEditorPlugin::fragmentShaderInit(int variant) const
//{
//				if (variant & DLang::Lexer::Variant_DLang_120)
//        return getInitFile(QLatin1String("dlang_120.frag"), &m_dlang_120_frag);
//    else
//        return getInitFile(QLatin1String("dlang_es_100.frag"), &m_dlang_es_100_frag);
//}

//const DLangEditorPlugin::InitFile *DLangEditorPlugin::vertexShaderInit(int variant) const
//{
//				if (variant & DLang::Lexer::Variant_DLang_120)
//        return getInitFile(QLatin1String("dlang_120.vert"), &m_dlang_120_vert);
//    else
//        return getInitFile(QLatin1String("dlang_es_100.vert"), &m_dlang_es_100_vert);
//}

//const DLangEditorPlugin::InitFile *DLangEditorPlugin::shaderInit(int variant) const
//{
//				if (variant & DLang::Lexer::Variant_DLang_120)
//        return getInitFile(QLatin1String("dlang_120_common.dlang"), &m_dlang_120_common);
//    else
//        return getInitFile(QLatin1String("dlang_es_100_common.dlang"), &m_dlang_es_100_common);
//}

} // namespace Internal
} // namespace DLangEditor

Q_EXPORT_PLUGIN(DLangEditor::Internal::DLangEditorPlugin)*/
