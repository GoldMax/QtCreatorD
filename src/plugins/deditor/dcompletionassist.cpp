#include "deditorconstants.h"
#include "dcompletionassist.h"
//#include "deditorplugin.h"
#include "qcdassist.h"

#include <coreplugin/idocument.h>
#include <coreplugin/messagemanager.h>
#include <texteditor/completionsettings.h>
#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/genericproposalmodel.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/functionhintproposal.h>

#include <QIcon>
#include <QTextDocument>
#include <QDebug>

using namespace DEditor;
using namespace TextEditor;

namespace // Anonymous
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
	Q_UNUSED(x)
	Q_UNUSED(i)
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
	: AssistInterface(textDocument, position, fileName, reason)
{
}
// --------------------------------------------------------------------------------
// DCompletionAssistProvider
// --------------------------------------------------------------------------------
DCompletionAssistProvider::DCompletionAssistProvider(QObject *parent) :
		CompletionAssistProvider(parent)
{

}

bool DCompletionAssistProvider::supportsEditor(Core::Id editorId) const
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
	//-Core::MessageManager::write(QString(QLatin1String("text(%1)")).arg(index));
	if(m_currentArg == -1)
		return m_items.at(index)->text();
	return prettyPrint(m_items.at(index)->text(), m_currentArg);
}
int DFunctionHintProposalModel::activeArgument(const QString &prefix) const
{
	//-Core::MessageManager::write(QString(QLatin1String("activeArgument(%1)")).arg(prefix));
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
		//Core::MessageManager::write(str);
	} while(x > 0 && y > 0);

	m_currentArg = str.count(QLatin1Char(','));
	return m_currentArg;
}
// ----------------------------------------------------------------------------
// DCompletionAssistProcessor
// ----------------------------------------------------------------------------
DCompletionAssistProcessor::DCompletionAssistProcessor()
	: m_startPosition(0),
	i_ClassName(QLatin1String(":/deditor/images/class.png")),
	i_InterfaceName(QLatin1String(":/deditor/images/interface.png")),
	i_StructName(QLatin1String(":/deditor/images/struct.png")),
	i_UnionName(QLatin1String(":/deditor/images/union.png")),
	i_VariableName(QLatin1String(":/deditor/images/variable.png")),
	i_MemberVariableName(QLatin1String(":/deditor/images/field.png")),
	i_Keyword(QLatin1String(":/deditor/images/keyword.png")),
	i_FunctionName(QLatin1String(":/deditor/images/method.png")),
	i_EnumName(QLatin1String(":/deditor/images/enum.png")),
	i_PackageName(QLatin1String(":/deditor/images/package.png")),
	i_ModuleName(QLatin1String(":/deditor/images/module.png")),
	i_Array(QLatin1String(":/deditor/images/array.png")),
	i_AssociativeArray(QLatin1String(":/deditor/images/asarray.png")),
	i_AliasName(QLatin1String(":/deditor/images/alias.png")),
	i_TemplateName(QLatin1String(":/deditor/images/template.png")),
	i_MixinTemplateName(QLatin1String(":/deditor/images/mixin.png")),
	i_dIcon(QLatin1String(":/deditor/images/d.png")),
	m_snippetCollector(QLatin1String(Constants::D_SNIPPETS_GROUP_ID),
																				QIcon(QLatin1String(":/texteditor/images/snippet.png")))
{}

IAssistProposal *DCompletionAssistProcessor::perform(const AssistInterface *interface)
{
	using namespace QcdAssist;
	m_interface.reset(static_cast<const DCompletionAssistInterface *>(interface));

	if(QcdAssist::isDCDEnabled() == false)
		return nullptr;

	AssistReason reason = interface->reason();
	int pos = interface->position();
	if(pos == 0)
		return nullptr;

	// просто по нажатию кнопки
	if (reason == IdleEditor && !acceptsIdleEditor())
		return nullptr;
	else if(reason == ExplicitlyInvoked)
	{
		if(isActivationChar(interface->characterAt(pos-1)))
			m_startPosition = pos;
		else
			for(int x = pos-1; x >= 0;  x--)
			{
				auto ch = interface->characterAt(x);
				if(!isIdentifierChar(ch))
				{
					m_startPosition = x+1;
					break;
				}
			}
	}
	else if(reason == ActivationCharacter && isActivationChar(interface->characterAt(pos-1)))
		m_startPosition = pos;

	if(m_startPosition == 0)
		return nullptr;

	QTextDocument* doc = interface->textDocument();
	QByteArray arr;
	toUtf8(arr, doc, pos);
	DCDCompletion c = QcdAssist::sendRequestToDCD(arr, pos);
	//qDebug() <<  "DCD items: " << c.completions.length();
	foreach(DCDCompletionItem i, c.completions)
	{
		QIcon icon;
		switch(i.type)
		{
			case Calltip: icon = i_FunctionName; break;
			case ClassName: icon = i_ClassName; break;
			case InterfaceName: icon = i_InterfaceName; break;
			case StructName: icon = i_StructName; break;
			case UnionName: icon = i_UnionName; break;
			case VariableName: icon = i_VariableName; break;
			case MemberVariableName: icon = i_MemberVariableName; break;
			case Keyword: icon = i_Keyword; break;
			case FunctionName: icon = i_FunctionName; break;
			case EnumName: icon = i_EnumName; break;
			case EnumMember: icon = i_EnumName; break;
			case PackageName: icon = i_PackageName; break;
			case ModuleName: icon = i_ModuleName; break;
			case Array: icon = i_Array; break;
			case AssociativeArray: icon = i_AssociativeArray; break;
			case AliasName: icon = i_AliasName; break;
			case TemplateName: icon = i_TemplateName; break;
			case MixinTemplateName: icon = i_MixinTemplateName; break;
			default: icon = i_dIcon;
		}
		addCompletion(i.name,icon);
	}
	if(c.type == Calltips)
			return createHintProposal();
	else if(c.type == Identifiers || reason == ExplicitlyInvoked)
		return createContentProposal();
	else
		return nullptr;
}

void DCompletionAssistProcessor::toUtf8(QByteArray& arr, QTextDocument* doc, int& charPosition)
{
	const QString text = doc->toPlainText();
	arr.append(text.leftRef(charPosition).toUtf8());
	int pos = arr.length();
	arr.append(QStringRef(&text,charPosition, text.length()-charPosition).toUtf8());
	charPosition = pos;
}

IAssistProposal* DCompletionAssistProcessor::createContentProposal()
{
//	GenericProposalModel* model = new GenericProposalModel();
//	model->loadContent(m_completions);
//			//new BasicProposalItemListModel(m_completions);
//	IAssistProposal *proposal = new GenericProposal(m_startPosition, model);
//	return proposal;
	addSnippets();
	return new GenericProposal(m_startPosition, m_completions);
}

IAssistProposal* DCompletionAssistProcessor::createHintProposal() const
{
	FunctionHintProposalModelPtr model(new DFunctionHintProposalModel(m_completions));
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
	AssistProposalItem* item = new AssistProposalItem;
	item->setText(text);
	item->setIcon(icon);
	item->setOrder(order);
	m_completions.append(item);
}

void DCompletionAssistProcessor::addSnippets()
{
	m_completions.append(m_snippetCollector.collect());
}
