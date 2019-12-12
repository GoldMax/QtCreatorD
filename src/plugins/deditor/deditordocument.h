#pragma once

#include <texteditor/textdocument.h>

namespace DEditor
{



class DEditorDocument  : public TextEditor::TextDocument
{
	Q_OBJECT


public:
	DEditorDocument();

	//CppTools::CppCompletionAssistProvider *completionAssistProvider() const override;

};

} //namespace DEditor

