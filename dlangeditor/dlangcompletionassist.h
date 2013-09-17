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

class DLangCompletionAssistInterface;

class DLangCompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
public:
    virtual bool supportsEditor(const Core::Id &editorId) const;
    virtual TextEditor::IAssistProcessor *createProcessor() const;

    virtual int activationCharSequenceLength() const;
    virtual bool isActivationCharSequence(const QString &sequence) const;
};

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
    QIcon m_typeIcon;
    QIcon m_constIcon;
    QIcon m_attributeIcon;
    QIcon m_uniformIcon;
    QIcon m_varyingIcon;
    QIcon m_otherIcon;
};

class DLangCompletionAssistInterface : public TextEditor::DefaultAssistInterface
{
public:
				DLangCompletionAssistInterface(QTextDocument *textDocument,
                                  int position, const QString &fileName,
                                  TextEditor::AssistReason reason,
                                  const QString &mimeType/*,
                                  const Document::Ptr &glslDoc*/);

    const QString &mimeType() const { return m_mimeType; }
    //const Document::Ptr &glslDocument() const { return m_glslDoc; }

private:
    QString m_mimeType;
    //Document::Ptr m_glslDoc;
};

} // DLangEditor

#endif // DLangCOMPLETIONASSIST_H
