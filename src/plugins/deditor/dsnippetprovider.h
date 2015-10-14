#ifndef DSNIPPETPROVIDER
#define DSNIPPETPROVIDER

#include <texteditor/snippets/isnippetprovider.h>

namespace DEditor {

class DSnippetProvider : public TextEditor::ISnippetProvider
{
public:
				DSnippetProvider();
				virtual ~DSnippetProvider();

public:
				virtual QString groupId() const;
				virtual QString displayName() const;
				virtual void decorateEditor(TextEditor::SnippetEditorWidget *editor) const;
};

} // DEditor

#endif // DSNIPPETPROVIDER

