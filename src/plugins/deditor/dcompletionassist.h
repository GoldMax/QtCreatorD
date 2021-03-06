#ifndef DCOMPLETIONASSIST_H
#define DCOMPLETIONASSIST_H

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/iassistproposal.h>
#include <texteditor/codeassist/assistinterface.h>
#include <texteditor/codeassist/ifunctionhintproposalmodel.h>
#include <texteditor/snippets/snippetassistcollector.h>

#include <QObject>
#include <QScopedPointer>
#include <QIcon>

using namespace TextEditor;

namespace DEditor {

class DCompletionAssistInterface : public AssistInterface
{
public:
	DCompletionAssistInterface(QTextDocument *textDocument,
																												int position, const QString &fileName,
																												AssistReason reason);
};
//**************************************************************************************
class DCompletionAssistProvider : public CompletionAssistProvider
{
	Q_OBJECT
public:
	DCompletionAssistProvider(QObject *parent = nullptr);

	//bool supportsEditor(Core::Id editorId) const;
	IAssistProcessor* createProcessor() const override;

	int activationCharSequenceLength() const override;
	bool isActivationCharSequence(const QString &sequence) const override;
};
//**************************************************************************************
class DCompletionAssistProcessor : public IAssistProcessor
{
public:
	DCompletionAssistProcessor();
	IAssistProposal *perform(const AssistInterface *interface) override;

private:
	IAssistProposal* createContentProposal();
	IAssistProposal* createHintProposal() const;
	bool acceptsIdleEditor() const;
	void addCompletion(const QString &text, const QIcon &icon, int order = 0);

	void addSnippets();

	int m_startPosition;
	QScopedPointer<const DCompletionAssistInterface> m_interface;
	QList<TextEditor::AssistProposalItemInterface *> m_completions;

	QIcon i_ClassName, i_InterfaceName, i_StructName, i_UnionName,
	i_VariableName, i_MemberVariableName, i_Keyword, i_FunctionName,
	i_EnumName, i_PackageName, i_ModuleName, i_Array,
	i_AssociativeArray, i_AliasName, i_TemplateName,i_MixinTemplateName, i_dIcon;

	TextEditor::SnippetAssistCollector m_snippetCollector;
};
//**************************************************************************************
class DFunctionHintProposalModel : public TextEditor::IFunctionHintProposalModel
{
public:
	DFunctionHintProposalModel(const QList<AssistProposalItemInterface *> items) :
			m_items(items),	m_currentArg(-1)
	{
	}

	virtual void reset() {  }
	virtual int size() const { return m_items.size(); }
	virtual QString text(int index) const;
	virtual int activeArgument(const QString &prefix) const;

private:
	const QList<AssistProposalItemInterface *> m_items;
	mutable int m_currentArg;
};

} // DEditor

#endif // DCOMPLETIONASSIST_H
