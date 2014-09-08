#ifndef DCOMPLETIONASSIST_H
#define DCOMPLETIONASSIST_H

#include "dtexteditor.h"

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/defaultassistinterface.h>
#include <texteditor/codeassist/ifunctionhintproposalmodel.h>

#include <QScopedPointer>
#include <QIcon>

namespace TextEditor {
class BasicProposalItem;
}

namespace DEditor {
namespace Internal {

class DCompletionAssistInterface : public TextEditor::DefaultAssistInterface
{
public:
				DCompletionAssistInterface(QTextDocument *textDocument,
																																		int position, const QString &fileName,
																																		TextEditor::AssistReason reason);
};
//**************************************************************************************
class DCompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
	Q_OBJECT
public:
				virtual bool supportsEditor(const Core::Id &editorId) const;
				virtual TextEditor::IAssistProcessor *createProcessor() const;

				virtual int activationCharSequenceLength() const;
				virtual bool isActivationCharSequence(const QString &sequence) const;
};
//**************************************************************************************
class DCompletionAssistProcessor : public TextEditor::IAssistProcessor
{
public:
				DCompletionAssistProcessor();
				virtual ~DCompletionAssistProcessor();

				virtual TextEditor::IAssistProposal *perform(const TextEditor::IAssistInterface *interface);

				void toUtf8(QByteArray& arr, QTextDocument* doc, int & charPosition);

private:
				TextEditor::IAssistProposal *createContentProposal() const;
				TextEditor::IAssistProposal *createHintProposal() const;
				bool acceptsIdleEditor() const;
				void addCompletion(const QString &text, const QIcon &icon, int order = 0);

				int m_startPosition;
				QScopedPointer<const DCompletionAssistInterface> m_interface;
				QList<TextEditor::BasicProposalItem *> m_completions;

				QIcon i_ClassName, i_InterfaceName, i_StructName, i_UnionName,
										i_VariableName, i_MemberVariableName, i_Keyword, i_FunctionName,
										i_EnumName, i_PackageName, i_ModuleName, i_Array,
										i_AssociativeArray, i_AliasName, i_TemplateName,i_MixinTemplateName, i_dIcon;

};
//**************************************************************************************
class DFunctionHintProposalModel : public TextEditor::IFunctionHintProposalModel
{
public:
				DFunctionHintProposalModel(const QList<TextEditor::BasicProposalItem *> items)
								///*QVector<D::Function *> functionSymbols*/)
								: //m_items(functionSymbols),
									m_items(items),
									m_currentArg(-1)
				{}

				virtual void reset() {  }
				virtual int size() const { return m_items.size(); }
				virtual QString text(int index) const;
				virtual int activeArgument(const QString &prefix) const;

private:
				//QVector<D::Function *> m_items;
				const QList<TextEditor::BasicProposalItem *> m_items;
				mutable int m_currentArg;
};

} // Internal
} // DEditor

#endif // DCOMPLETIONASSIST_H
