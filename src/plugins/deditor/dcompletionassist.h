#ifndef DCOMPLETIONASSIST_H
#define DCOMPLETIONASSIST_H

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/iassistproposal.h>
#include <texteditor/codeassist/assistinterface.h>
#include <texteditor/codeassist/ifunctionhintproposalmodel.h>

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
	bool supportsEditor(Core::Id editorId) const override;
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

	void toUtf8(QByteArray& arr, QTextDocument* doc, int & charPosition);

private:
	IAssistProposal* createContentProposal() const;
	IAssistProposal* createHintProposal() const;
	bool acceptsIdleEditor() const;
	void addCompletion(const QString &text, const QIcon &icon, int order = 0);

	int m_startPosition;
	QScopedPointer<const DCompletionAssistInterface> m_interface;
	QList<TextEditor::AssistProposalItem *> m_completions;

	QIcon i_ClassName, i_InterfaceName, i_StructName, i_UnionName,
	i_VariableName, i_MemberVariableName, i_Keyword, i_FunctionName,
	i_EnumName, i_PackageName, i_ModuleName, i_Array,
	i_AssociativeArray, i_AliasName, i_TemplateName,i_MixinTemplateName, i_dIcon;

};
//**************************************************************************************
class DFunctionHintProposalModel : public TextEditor::IFunctionHintProposalModel
{
public:
	DFunctionHintProposalModel(const QList<AssistProposalItem *> items) :
			m_items(items),	m_currentArg(-1)
	{
	}

	virtual void reset() {  }
	virtual int size() const { return m_items.size(); }
	virtual QString text(int index) const;
	virtual int activeArgument(const QString &prefix) const;

private:
	const QList<AssistProposalItem *> m_items;
	mutable int m_currentArg;
};

} // DEditor

#endif // DCOMPLETIONASSIST_H
