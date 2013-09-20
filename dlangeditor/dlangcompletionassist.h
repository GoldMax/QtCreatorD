#ifndef DLANGCOMPLETIONASSIST_H
#define DLANGCOMPLETIONASSIST_H

#include "dlangtexteditor.h"

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/defaultassistinterface.h>
#include <texteditor/codeassist/ifunctionhintproposalmodel.h>

#include <QScopedPointer>
#include <QIcon>

namespace TextEditor {
class BasicProposalItem;
}

namespace DLangEditor {

class DLangCompletionAssistInterface : public TextEditor::DefaultAssistInterface
{
public:
    DLangCompletionAssistInterface(QTextDocument *textDocument,
                                  int position, const QString &fileName,
                                  TextEditor::AssistReason reason);
};
//**************************************************************************************
class DLangCompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
public:
    virtual bool supportsEditor(const Core::Id &editorId) const;
    virtual TextEditor::IAssistProcessor *createProcessor() const;

    virtual int activationCharSequenceLength() const;
    virtual bool isActivationCharSequence(const QString &sequence) const;
};
//**************************************************************************************
class DLangCompletionAssistProcessor : public TextEditor::IAssistProcessor
{
public:
				DLangCompletionAssistProcessor();
				virtual ~DLangCompletionAssistProcessor();

    virtual TextEditor::IAssistProposal *perform(const TextEditor::IAssistInterface *interface);

private:
    TextEditor::IAssistProposal *createContentProposal() const;
//				TextEditor::IAssistProposal *createHintProposal(const QVector<DLang::Function *> &symbols);
    bool acceptsIdleEditor() const;
    void addCompletion(const QString &text, const QIcon &icon, int order = 0);

    int m_startPosition;
				QScopedPointer<const DLangCompletionAssistInterface> m_interface;
    QList<TextEditor::BasicProposalItem *> m_completions;

    QIcon m_keywordIcon;
    QIcon m_varIcon;
    QIcon m_functionIcon;
    QIcon m_classIcon;
    QIcon m_namespaceIcon;
    QIcon m_enumIcon;
    QIcon m_enumMemberIcon;

    QIcon m_constIcon;
    QIcon m_attributeIcon;
    QIcon m_uniformIcon;
    QIcon m_varyingIcon;
    QIcon m_otherIcon;
};
//**************************************************************************************
class DLangFunctionHintProposalModel : public TextEditor::IFunctionHintProposalModel
{
public:
    DLangFunctionHintProposalModel(/*QVector<DLang::Function *> functionSymbols*/)
        : //m_items(functionSymbols),
         m_currentArg(-1)
    {}

    virtual void reset() {}
    virtual int size() const { return 1; /*m_items.size();*/ }
    virtual QString text(int index) const;
    virtual int activeArgument(const QString &prefix) const;

private:
    //QVector<DLang::Function *> m_items;
    mutable int m_currentArg;
};
} // DLangEditor

#endif // DLangCOMPLETIONASSIST_H
