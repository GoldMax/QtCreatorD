#ifndef DEDITORFACTORY_H
#define DEDITORFACTORY_H

#include <texteditor/texteditor.h>

using namespace TextEditor;

namespace DEditor {

class DEditorFactory : public TextEditorFactory
{
	Q_OBJECT

public:
	DEditorFactory();
};

class DEditorWidget : public TextEditorWidget
{
	Q_OBJECT

public:
	DEditorWidget();

	AssistInterface *createAssistInterface(AssistKind assistKind,
																																								AssistReason reason) const override;
};


} // namespace DEditor

#endif // DEDITORFACTORY_H
