#include "deditorconstants.h"
#include "dcompletionassist.h"
#include "deditorplugin.h"
#include "qcdassist.h"

#include <coreplugin/idocument.h>
#include <texteditor/completionsettings.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/basicproposalitem.h>
#include <texteditor/codeassist/basicproposalitemlistmodel.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/functionhintproposal.h>
#include <utils/faketooltip.h>
#include <coreplugin/messagemanager.h>

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
QString prettyPrint(QString str, int currentArgument)
{
 static QLatin1Char ob('(');
 static QLatin1Char cb(')');
 int left = str.indexOf(ob);
 int right = str.lastIndexOf(cb);
 int x=0,y,i;
 Q_UNUSED(x);
 Q_UNUSED(i);
 for(int i = 0,x = y = left+1; y <= right-1; i++)
 {
  y = str.indexOf(QLatin1Char(','),y);
  if(y == -1)
   y = right;
  int cob = str.midRef(x,y-x).count(ob);
  int ccb = str.midRef(x,y-x).count(cb);
  if(cob == ccb)
  {
   if(i == currentArgument)
    return str.leftRef(x) + QLatin1String("<b>") +
           str.midRef(x,y-x) + QLatin1String("</b>") +
           str.rightRef(str.length()-y);
   else
    x = ++y;
  }
  else
   i--;
 }
 return str;
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
 //Core::MessageManager::write(QString(QLatin1String("text(%1)")).arg(index));
 if(m_currentArg == -1)
  return m_items.at(index)->text();
 return prettyPrint(m_items.at(index)->text(), m_currentArg);
}
int DFunctionHintProposalModel::activeArgument(const QString &prefix) const
{
 //Core::MessageManager::write(QString(QLatin1String("activeArgument(%1)")).arg(prefix));
 static QLatin1Char ob('(');
 static QLatin1Char cb(')');
 if(prefix.count(cb) > prefix.count(ob))
  return -1;

 QString str(prefix);
 int x =0, y = 0;
 do
 {
  if((x = str.indexOf(ob)) == -1)
   break;
  if((y = str.indexOf(cb)) == -1)
   y = str.size()-1;
  str = str.remove(x, y - x+1);
  Core::MessageManager::write(str);
 }while(x > 0 && y > 0);

 m_currentArg = str.count(QLatin1Char(','));
 return m_currentArg;
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
 using namespace QcdAssist;
 m_interface.reset(static_cast<const DCompletionAssistInterface *>(interface));

 if(QcdAssist::isDCDEnabled() == false)
  return 0;

 AssistReason reason = interface->reason();
 int pos = interface->position();
 if(pos == 0)
  return 0;

 // просто по нажатию кнопки
 if (reason == IdleEditor) // && !acceptsIdleEditor())
  return 0;
 else if(reason == ExplicitlyInvoked)
 {
  int x = pos-1;
  for(; isIdentifierChar(interface->characterAt(x)); x--)
   m_startPosition = x;
 }
 else if(reason == ActivationCharacter && isActivationChar(interface->characterAt(pos-1)))
  m_startPosition = pos;

 if(m_startPosition == 0)
  return 0;

 QTextDocument* doc = interface->textDocument();
 QByteArray arr;
 toUtf8(arr, doc, pos);
 DCDCompletion c = QcdAssist::sendRequestToDCD(arr, pos);
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
 if(c.type == Identifiers)
  return createContentProposal();
 else if(c.type == Calltips)
  return createHintProposal();
 else
  return 0;
}

void DCompletionAssistProcessor::toUtf8(QByteArray& arr, QTextDocument* doc, int& charPosition)
{
 const QString text = doc->toPlainText();
 arr.append(text.leftRef(charPosition).toUtf8());
 int pos = arr.length();
 arr.append(QStringRef(&text,charPosition, text.length()-charPosition).toUtf8());
 charPosition = pos;
}

IAssistProposal *DCompletionAssistProcessor::createContentProposal() const
{
 IGenericProposalModel *model = new BasicProposalItemListModel(m_completions);
 IAssistProposal *proposal = new GenericProposal(m_startPosition, model);
 return proposal;
}

IAssistProposal* DCompletionAssistProcessor::createHintProposal() const
{
 IFunctionHintProposalModel *model = new DFunctionHintProposalModel(m_completions);
 //IGenericProposalModel *model = new BasicProposalItemListModel(m_completions);
 IAssistProposal *proposal = new FunctionHintProposal(m_startPosition, model);
 return proposal;
}

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


