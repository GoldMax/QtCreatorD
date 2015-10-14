#include "deditorconstants.h"
#include "deditorplugin.h"
#include "deditorfactory.h"
#include "dcompletionassist.h"
#include "dhoverhandler.h"
#include "deditorhighlighter.h"
#include "dindenter.h"
#include "dautocompleter.h"

#include <coreplugin/icore.h>
#include <coreplugin/infobar.h>
#include <coreplugin/editormanager/editormanager.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <texteditor/refactoroverlay.h>
#include <texteditor/textdocument.h>
#include <texteditor/syntaxhighlighter.h>
#include <texteditor/texteditoractionhandler.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditorsettings.h>

#include <QCoreApplication>
#include <QFileInfo>

using namespace DEditor;
using namespace DEditor::Constants;
using namespace TextEditor;

DEditorFactory::DEditorFactory()
{
	setId(C_DEDITOR_ID);
	setDisplayName(qApp->translate("OpenWith::Editors", C_DEDITOR_DISPLAY_NAME));
	addMimeType(DEditor::Constants::D_MIMETYPE_SRC);
	addMimeType(DEditor::Constants::D_MIMETYPE_HDR);


	setDocumentCreator([]() { return new TextDocument(Constants::C_DEDITOR_ID); });
	setEditorWidgetCreator([]() { return new DEditorWidget; });
	setIndenterCreator([]() { return new DIndenter; });
	setSyntaxHighlighterCreator([]() { return new DEditorHighlighter; });
	setCommentStyle(Utils::CommentDefinition::CppStyle);
	setCompletionAssistProvider(new DCompletionAssistProvider);
	setParenthesesMatchingEnabled(true);
	setMarksVisible(true);
	setCodeFoldingSupported(true);

	setEditorActionHandlers(TextEditorActionHandler::Format
																							| TextEditorActionHandler::UnCommentSelection
																							| TextEditorActionHandler::UnCollapseAll);

	addHoverHandler(new DHoverHandler);

}

//-----------------------------
//- DEditorWidget
//-----------------------------
DEditorWidget::DEditorWidget()
{
	setAutoCompleter(new DCompleter);
}

AssistInterface* DEditorWidget::createAssistInterface(AssistKind kind,
																																																						AssistReason reason) const
{
	if (kind == Completion)
					return new DCompletionAssistInterface(document(),
																																											position(),
																																											textDocument()->filePath().toString(),
																																											reason);
	return TextEditorWidget::createAssistInterface(kind, reason);
}
