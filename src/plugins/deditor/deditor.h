#pragma once

#include <texteditor/texteditor.h>

using namespace TextEditor;

namespace DEditor {

class DTextEditor : public TextEditor::BaseTextEditor
{
	Q_OBJECT

public:
	DTextEditor();

	static void decorateEditor(TextEditor::TextEditorWidget *editor);
};

class DEditorWidget : public TextEditor::TextEditorWidget
{
	Q_OBJECT

public:
	DEditorWidget();

	AssistInterface *createAssistInterface(AssistKind assistKind,
																																								AssistReason reason) const override;

protected:
	void keyPressEvent(QKeyEvent *e) override;
	void contextMenuEvent(QContextMenuEvent *) override;
	void findLinkAt(const QTextCursor &,
																									Utils::ProcessLinkCallback &&processLinkCallback,
																									bool resolveTarget = true,
																									bool inNextSplit = false) override;

private:
	bool handleStringSplitting(QKeyEvent *e) const;
};


} // namespace DEditor


