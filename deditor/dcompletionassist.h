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

private:
    TextEditor::IAssistProposal *createContentProposal() const;
//				TextEditor::IAssistProposal *createHintProposal(const QVector<D::Function *> &symbols);
    bool acceptsIdleEditor() const;
    void addCompletion(const QString &text, const QIcon &icon, int order = 0);

    int m_startPosition;
    QScopedPointer<const DCompletionAssistInterface> m_interface;
    QList<TextEditor::BasicProposalItem *> m_completions;

    QIcon m_keywordIcon;
    QIcon m_varIcon;
    QIcon m_functionIcon;
    QIcon m_classIcon;
    QIcon m_namespaceIcon;
    QIcon m_enumIcon;
    QIcon m_enumMemberIcon;
    QIcon m_dIcon;

};
//**************************************************************************************
class DFunctionHintProposalModel : public TextEditor::IFunctionHintProposalModel
{
public:
    DFunctionHintProposalModel(/*QVector<D::Function *> functionSymbols*/)
        : //m_items(functionSymbols),
         m_currentArg(-1)
    {}

    virtual void reset() {}
    virtual int size() const { return 1; /*m_items.size();*/ }
    virtual QString text(int index) const;
    virtual int activeArgument(const QString &prefix) const;

private:
    //QVector<D::Function *> m_items;
    mutable int m_currentArg;
};

} // Internal
} // DEditor

#endif // DCOMPLETIONASSIST_H
