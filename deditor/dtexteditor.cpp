#include "deditorconstants.h"
#include "deditorplugin.h"
#include "dtexteditor.h"
//#include "dautocompleter.h"
//#include "dindenter.h"
#include "dcompletionassist.h"
#include "deditorhighlighter.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <extensionsystem/pluginmanager.h>
#include <texteditor/basetextdocument.h>
#include <texteditor/normalindenter.h>

#include <cpptools/cppqtstyleindenter.h>

#include <QFileInfo>

using namespace Core;
using namespace DEditor::Internal;
using namespace TextEditor;
using namespace TextEditor::Internal;

DTextEditor::DTextEditor(DTextEditorWidget* editor)
	: BaseTextEditor(editor)
{
	setId(DEditor::Constants::C_DEDITOR_ID);
	setContext(Core::Context(DEditor::Constants::C_DEDITOR_ID,
																										TextEditor::Constants::C_TEXTEDITOR));
}

Core::IEditor* DTextEditor::duplicate()
{
	DTextEditorWidget *newWidget = new DTextEditorWidget(
			qobject_cast<DTextEditorWidget*>(editorWidget()));
	DEditorPlugin::instance()->initializeEditor(newWidget);
	return newWidget->editor();
	//return NULL;
}

bool DTextEditor::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
	baseTextDocument()->setMimeType(Core::MimeDatabase::findByFile(QFileInfo(fileName)).type());
	bool b = TextEditor::BaseTextEditor::open(errorString, fileName, realFileName);
	return b;
	//return false;
}

TextEditor::CompletionAssistProvider* DTextEditor::completionAssistProvider()
{
	return ExtensionSystem::PluginManager::getObject<DCompletionAssistProvider>();
}
//-----------------------------
//- DTextEditorWidget
//-----------------------------
void DTextEditorWidget::init()
{
	setRevisionsVisible(true);
	setMarksVisible(true);
	setLineSeparatorsAllowed(true);
	setParenthesesMatchingEnabled(true);
	setCodeFoldingSupported(true);

	baseTextDocument()->setIndenter(new CppTools::CppQtStyleIndenter);
	// Indenter вызывает исключения в стандартном hightlighter
	//setIndenter(new DIndenter());

	new DEditorHighlighter(baseTextDocument());

	baseTextDocument()->setMimeType(QLatin1String(DEditor::Constants::D_MIMETYPE_SRC));
}
DTextEditorWidget::DTextEditorWidget(QWidget *parent)
		: BaseTextEditorWidget(parent)
{
	init();
}
DTextEditorWidget::DTextEditorWidget(DTextEditorWidget* other)
		: BaseTextEditorWidget(other)
{
	init();
}

TextEditor::IAssistInterface* DTextEditorWidget::createAssistInterface(
		TextEditor::AssistKind kind, TextEditor::AssistReason reason) const
{
	if (kind == TextEditor::Completion)
		return new DCompletionAssistInterface(document(),
																																												position(),
																																												editor()->document()->filePath(),
																																												reason);
	return BaseTextEditorWidget::createAssistInterface(kind, reason);
}

TextEditor::BaseTextEditor* DTextEditorWidget::createEditor()
{
	DTextEditor* edit = new DTextEditor(this);
	//createToolBar(edit);
	return edit;
}

void DTextEditorWidget::unCommentSelection()
{
	Utils::unCommentSelection(this);
}

