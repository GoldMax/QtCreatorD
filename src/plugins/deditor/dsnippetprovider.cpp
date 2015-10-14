#include "dsnippetprovider.h"

#include "deditorhighlighter.h"
#include "dautocompleter.h"
#include "dindenter.h"
#include "deditorconstants.h"

#include <cpptools/cppqtstyleindenter.h>

#include <texteditor/snippets/snippeteditor.h>
#include <texteditor/textdocument.h>

#include <QLatin1String>
#include <QCoreApplication>

using namespace DEditor;

DSnippetProvider::DSnippetProvider() :
				TextEditor::ISnippetProvider()
{}

DSnippetProvider::~DSnippetProvider()
{}

QString DSnippetProvider::groupId() const
{
				return QLatin1String(Constants::D_SNIPPETS_GROUP_ID);
}

QString DSnippetProvider::displayName() const
{
				return QCoreApplication::translate("DEditor::DSnippetProvider", "D");
}

void DSnippetProvider::decorateEditor(TextEditor::SnippetEditorWidget *editor) const
{
				editor->textDocument()->setSyntaxHighlighter(new DEditorHighlighter);
				editor->textDocument()->setIndenter(new DIndenter);
				editor->setAutoCompleter(new DCompleter);
}
