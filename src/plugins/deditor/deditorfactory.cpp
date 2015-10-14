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
#include <texteditor/completionsettings.h>

#include <cppeditor/cppdocumentationcommenthelper.h>

#include <QCoreApplication>
#include <QFileInfo>
#include <QTextDocument>
#include <QTextBlock>

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
	//setEditorCreator([]() { return new DEditor; });
	setAutoCompleterCreator([]() { return new DCompleter; });
	setIndenterCreator([]() { return new DIndenter; });
	setSyntaxHighlighterCreator([]() { return new DEditorHighlighter; });
	setCommentStyle(Utils::CommentDefinition::CppStyle);
	setCompletionAssistProvider(new DCompletionAssistProvider);
	setParenthesesMatchingEnabled(true);
	setMarksVisible(true);
	setCodeFoldingSupported(true);

	setEditorActionHandlers(TextEditorActionHandler::Format
																							| TextEditorActionHandler::UnCommentSelection
																							| TextEditorActionHandler::UnCollapseAll
																							| TextEditorActionHandler::FollowSymbolUnderCursor);

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

void DEditorWidget::keyPressEvent(QKeyEvent *e)
{
	if(handleStringSplitting(e))
		return;

	if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
	{
		if(CppEditor::Internal::trySplitComment(this))
		{
			e->accept();
			return;
		}
	}

	TextEditorWidget::keyPressEvent(e);
}
bool DEditorWidget::handleStringSplitting(QKeyEvent *e) const
{
	if (!TextEditorSettings::completionSettings().m_autoSplitStrings)
		return false;

	if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
	{
		QTextCursor cursor = textCursor();

		if (autoCompleter()->isInString(cursor))
		{
			cursor.beginEditBlock();
			if (cursor.positionInBlock() > 0
							&& cursor.block().text().at(cursor.positionInBlock() - 1) == QLatin1Char('\\'))
			{
				// Already escaped: simply go back to line, but do not indent.
				cursor.insertText(QLatin1String("\n"));
			}
			else if (e->modifiers() & Qt::ShiftModifier)
			{
				// With 'shift' modifier, escape the end of line character
				// and start at beginning of next line.
				cursor.insertText(QLatin1String("\\\n"));
			}
			else
			{
				// End the current string, and start a new one on the line, properly indented.
				cursor.insertText(QLatin1String("\"\n\""));
				textDocument()->autoIndent(cursor);
			}
			cursor.endEditBlock();
			e->accept();
			return true;
		}
	}

	return false;
}
