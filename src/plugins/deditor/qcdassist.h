#ifndef QCDASSIST_H
#define QCDASSIST_H

#include <texteditor/textdocument.h>
#include <utils/link.h>

#include "deditor_global.h"

#include <QtGlobal>
#include <QString>
#include <QList>

class QTextDocument;

namespace QcdAssist
{
	void isDCDEnabled(bool value);
	QString dcdClient();

	// Записывает содержимое документа в массив байт UTF-8 кодировки
	// возвращает преобразованное значение символьной позиции в байтовое смещение
	int toUtf8(QByteArray& arr, QTextDocument* doc, int charPosition);
	// Отправляет запрос к DCD
	QString sendRequestToDCD(QByteArray& filedata, QString command, uint bytePosition);


	enum DCDCompletionType { Identifiers, Calltips };
	enum DCDCompletionItemType
	{
		Invalid             = 0,
		Calltip             = 1,
		ClassName           = 'c',
		InterfaceName       = 'i',
		StructName          = 's',
		UnionName           = 'u',
		VariableName        = 'v',
		MemberVariableName  = 'm',
		Keyword             = 'k',
		FunctionName        = 'f',
		EnumName            = 'g',
		EnumMember          = 'e',
		PackageName         = 'P',
		ModuleName          = 'M',
		Array               = 'a',
		AssociativeArray    = 'A',
		AliasName           = 'l',
		TemplateName        = 't',
		MixinTemplateName   = 'T'
	};
	struct DCDCompletionItem
	{
		DCDCompletionItem(DCDCompletionItemType t, QString s);

		DCDCompletionItemType type;
		QString name;
	};
	struct DCDCompletion
	{
		DCDCompletionType type;
		QList<DCDCompletionItem> completions;
	};

	//-------------------
	//--- Client Func ---
	//-------------------
	DEDITORSHARED_EXPORT bool isDCDEnabled();
	DEDITORSHARED_EXPORT void sendClearChache();
	DEDITORSHARED_EXPORT void sendAddImportToDCD(QString path);
	DEDITORSHARED_EXPORT DCDCompletion autocompletion(QTextDocument* document, int charPosition);
	DEDITORSHARED_EXPORT Utils::Link symbolLocation(QTextDocument* document, int charPosition);
}

#endif // QCDASSIST_H
