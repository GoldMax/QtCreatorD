#include "deditor.h"
#include "deditorconstants.h"
#include "dhighlighter.h"
#include "dindenter.h"
#include "dcompletionassist.h"
#include "dautocompleter.h"

#include <texteditor/textdocument.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/completionsettings.h>

#include <cplusplus/MatchingText.h>
#include <cplusplus/CppDocument.h>
#include <cpptools/cppsemanticinfo.h>

#include <QDebug>

using namespace DEditor;
using namespace TextEditor;
using namespace CPlusPlus;

//-----------------------------
//- DEditor
//-----------------------------

DTextEditor::DTextEditor()
{
	addContext(Constants::D_LANGUAGE_ID);
}

void DTextEditor::decorateEditor(TextEditor::TextEditorWidget *editor)
{
	editor->textDocument()->setSyntaxHighlighter(new DHighlighter);
	editor->textDocument()->setIndenter(new DIndenter(editor->textDocument()->document()));
	editor->setAutoCompleter(new DAutoCompleter);

}
//-----------------------------
//- DEditorWidget
//-----------------------------
DEditorWidget::DEditorWidget()
{
	setAutoCompleter(new DAutoCompleter);
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

// TODO : split comment
//	if(e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
//	{
//		if(CppEditor::Internal::trySplitComment(this, semanticInfo().snapshot))
//		{
//			e->accept();
//			return;
//		}
//	}

	TextEditorWidget::keyPressEvent(e);
}

bool DEditorWidget::handleStringSplitting(QKeyEvent *e) const
{
	if (!TextEditorSettings::completionSettings().m_autoSplitStrings)
		return false;

	if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
	{
		QTextCursor cursor = textCursor();
//		qDebug() <<  cursor.block().text();
//		qDebug() <<  cursor.block().text().count(QLatin1Char('"'));

		const Kind stringKind = MatchingText::stringKindAtCursor(cursor);
		bool isString = stringKind >= T_FIRST_STRING_LITERAL && stringKind < T_FIRST_RAW_STRING_LITERAL;
		if(isString == false)
		{
			int all = cursor.block().text().count(QLatin1Char('"'));
			int esced = cursor.block().text().count(QLatin1String("\\\""));
			isString = (all - esced)%2 > 0;
		}

		//if (autoCompleter()->isInString(cursor))  --- не работает
		if(isString)
		{
			cursor.beginEditBlock();
			if (cursor.positionInBlock() > 0 &&
							cursor.block().text().at(cursor.positionInBlock() - 1) == QLatin1Char('\\'))
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
				cursor.insertText(QLatin1String("\" ~\n\""));
				textDocument()->autoIndent(cursor);
			}
			cursor.endEditBlock();
			e->accept();
			return true;
		}
	}
	return false;
}
