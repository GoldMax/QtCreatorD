#include "deditorconstants.h"
#include "dcompletionassist.h"
#include "deditorplugin.h"
#include "qcdassist/qcdassist.h"

#include <coreplugin/idocument.h>
#include <texteditor/completionsettings.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/basicproposalitem.h>
#include <texteditor/codeassist/basicproposalitemlistmodel.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/functionhintproposal.h>
#include <utils/faketooltip.h>

#include <QIcon>
#include <QPainter>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

using namespace DEditor::Internal;
using namespace TextEditor;
using namespace TextEditor::Internal;

namespace
{
enum CompletionOrder { SpecialMemberOrder = -5 };
bool isActivationChar(const QChar &ch)
{
 return ch == QLatin1Char('(') || ch == QLatin1Char('.') || ch == QLatin1Char(',');
}
bool isIdentifierChar(QChar ch)
{
 return ch.isLetterOrNumber() || ch == QLatin1Char('_');
}
bool isDelimiter(QChar ch)
{
 switch (ch.unicode()) {
 case '{':
 case '}':
 case '[':
 case ']':
 case ')':
 case '?':
 case '!':
 case ':':
 case ';':
 case ',':
 case '+':
 case '-':
 case '*':
 case '/':
  return true;

 default:
  return false;
 }
}
bool checkStartOfIdentifier(const QString &word)
{
 if (! word.isEmpty())
 {
  const QChar ch = word.at(0);
  if (ch.isLetter() || ch == QLatin1Char('_'))
   return true;
 }
 return false;
}

} // Anonymous
// --------------------------------------------------------------------------------------
// DCompletionAssistInterface
// --------------------------------------------------------------------------------------
DCompletionAssistInterface::DCompletionAssistInterface(QTextDocument *textDocument,
                                                       int position,
                                                       const QString &fileName,
                                                       TextEditor::AssistReason reason)
 : DefaultAssistInterface(textDocument, position, fileName, reason)
{
}
// --------------------------------------------------------------------------------
// DCompletionAssistProvider
// --------------------------------------------------------------------------------
bool DCompletionAssistProvider::supportsEditor(const Core::Id &editorId) const
{
 return editorId == Constants::C_DEDITOR_ID;
}
IAssistProcessor* DCompletionAssistProvider::createProcessor() const
{
 return new DCompletionAssistProcessor;
}
int DCompletionAssistProvider::activationCharSequenceLength() const
{
 return 1;
}
bool DCompletionAssistProvider::isActivationCharSequence(const QString &sequence) const
{
 return isActivationChar(sequence.at(0));
}
// --------------------------------------------------------------------------------
// DFunctionHintProposalModel
// --------------------------------------------------------------------------------
QString DFunctionHintProposalModel::text(int index) const
{
 Q_UNUSED(index);
 return QLatin1String("DFunctionHintProposalModel::text"); //m_items.at(index)->prettyPrint(m_currentArg);
}

int DFunctionHintProposalModel::activeArgument(const QString &prefix) const
{
 Q_UNUSED(prefix);
 //    const QByteArray &str = prefix.toLatin1();
 //    int argnr = 0;
 //    int parcount = 0;
 //				D::Lexer lexer(0, str.constData(), str.length());
 //				D::Token tk;
 //				QList<D::Token> tokens;
 //    do {
 //        lexer.yylex(&tk);
 //        tokens.append(tk);
 //				} while (tk.isNot(D::Parser::EOF_SYMBOL));
 //    for (int i = 0; i < tokens.count(); ++i) {
 //								const D::Token &tk = tokens.at(i);
 //								if (tk.is(D::Parser::T_LEFT_PAREN))
 //            ++parcount;
 //								else if (tk.is(D::Parser::T_RIGHT_PAREN))
 //            --parcount;
 //								else if (! parcount && tk.is(D::Parser::T_COMMA))
 //            ++argnr;
 //    }

 //    if (parcount < 0)
 //        return -1;

 //    if (argnr != m_currentArg)
 //        m_currentArg = argnr;

 //    return argnr;
 return 0;
}
// ----------------------------------------------------------------------------
// DCompletionAssistProcessor
// ----------------------------------------------------------------------------
DCompletionAssistProcessor::DCompletionAssistProcessor()
 : m_startPosition(0)
 , m_keywordIcon(QLatin1String(":/deditor/images/keyword.png"))
 , m_varIcon(QLatin1String(":/deditor/images/var.png"))
 , m_functionIcon(QLatin1String(":/deditor/images/func.png"))
 , m_classIcon(QLatin1String(":/deditor/images/class.png"))
 , m_namespaceIcon(QLatin1String(":/deditor/images/namespace.png"))
 , m_enumIcon(QLatin1String(":/deditor/images/enum.png"))
 , m_enumMemberIcon(QLatin1String(":/deditor/images/enumerator.png"))
 , m_dIcon(QLatin1String(":/deditor/images/d.png"))
{}

DCompletionAssistProcessor::~DCompletionAssistProcessor(){}
IAssistProposal *DCompletionAssistProcessor::perform(const IAssistInterface *interface)
{
 m_interface.reset(static_cast<const DCompletionAssistInterface *>(interface));
 // просто по нажатию кнопки
 //if (interface->reason() == IdleEditor && !acceptsIdleEditor())
 // return 0;
 bool needCompletion = false;
 if(interface->reason() == ExplicitlyInvoked)
  needCompletion = true;
 else if( m_interface->characterAt(m_interface->position() - 1) == QLatin1Char('.'))
  needCompletion = true;


 if(needCompletion)
 {
  m_startPosition = m_interface->position();
  using namespace QcdAssist;
  QTextDocument* doc = interface->textDocument();
  QByteArray arr = doc->toPlainText().toUtf8();
  DCDCompletion c = QcdAssist::sendRequestToDCD(arr, m_startPosition);
  foreach(DCDCompletionItem i, c.completions)
  {
   QIcon icon;
   switch(i.type)
   {
   case Calltip: icon = m_functionIcon; break;
   case ClassName: icon = m_classIcon; break;
   case InterfaceName: icon = m_classIcon; break;
   case StructName: icon = m_classIcon; break;
   case UnionName: icon = m_classIcon; break;
   case VariableName: icon = m_varIcon; break;
   case MemberVariableName: icon = m_varIcon; break;
   case Keyword: icon = m_keywordIcon; break;
   case FunctionName: icon = m_functionIcon; break;
   case EnumName: icon = m_enumIcon; break;
   case EnumMember: icon = m_enumMemberIcon; break;
   case PackageName: icon = m_namespaceIcon; break;
   case ModuleName: icon = m_namespaceIcon; break;
   default: icon = m_dIcon;
   }
   addCompletion(i.name,icon);
  }
 }
 return createContentProposal();
}

IAssistProposal *DCompletionAssistProcessor::createContentProposal() const
{
 IGenericProposalModel *model = new BasicProposalItemListModel(m_completions);
 IAssistProposal *proposal = new GenericProposal(m_startPosition, model);
 return proposal;
}

//IAssistProposal *DCompletionAssistProcessor::createHintProposal(
//				const QVector<D::Function *> &symbols)
//{
//				IFunctionHintProposalModel *model = new DFunctionHintProposalModel(symbols);
//    IAssistProposal *proposal = new FunctionHintProposal(m_startPosition, model);
//    return proposal;
//}

bool DCompletionAssistProcessor::acceptsIdleEditor() const
{
 const int cursorPosition = m_interface->position();
 const QChar ch = m_interface->characterAt(cursorPosition - 1);

 const QChar characterUnderCursor = m_interface->characterAt(cursorPosition);

 if (isIdentifierChar(ch) && (characterUnderCursor.isSpace() ||
                              characterUnderCursor.isNull() ||
                              isDelimiter(characterUnderCursor))) {
  int pos = m_interface->position() - 1;
  for (; pos != -1; --pos) {
   if (! isIdentifierChar(m_interface->characterAt(pos)))
    break;
  }
  ++pos;

  const QString word = m_interface->textAt(pos, cursorPosition - pos);
  if (word.length() > 2 && checkStartOfIdentifier(word)) {
   for (int i = 0; i < word.length(); ++i) {
    if (! isIdentifierChar(word.at(i)))
     return false;
   }
   return true;
  }
 }

 return isActivationChar(ch);
}

void DCompletionAssistProcessor::addCompletion(const QString &text,
                                               const QIcon &icon,
                                               int order)
{
 BasicProposalItem *item = new BasicProposalItem;
 item->setText(text);
 item->setIcon(icon);
 item->setOrder(order);
 m_completions.append(item);
}


