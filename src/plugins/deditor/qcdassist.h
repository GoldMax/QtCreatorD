#ifndef QCDASSIST_H
#define QCDASSIST_H

#include "deditor_global.h"

#include <QtGlobal>
#include <QString>
#include <QList>
#include <string>

namespace QcdAssist
{
	DEDITORSHARED_EXPORT bool isDCDEnabled();
	void isDCDEnabled(bool value);
	QString dcdClient();

// struct AutocompleteRequest
// {
//  enum RequestKind
//  {
//   autocomplete,
//   clearCache,
//   addImport,
//   shutdown
//  };

//  // File name used for error reporting
//  std::string fileName;
//  // Command coming from the client
//  int kind;
//  // Paths to be searched for import files
//  std::string importPaths;
//  // The source code to auto complete
//  std::vector<char> sourceCode;
//  // The cursor position
//  size_t cursorPosition;

//  MSGPACK_DEFINE(fileName, kind, importPaths, sourceCode, cursorPosition)
// };

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
	DEDITORSHARED_EXPORT void sendClearChache();
	DEDITORSHARED_EXPORT void sendAddImportToDCD(QString path);
	DEDITORSHARED_EXPORT DCDCompletion sendRequestToDCD(QByteArray& filedata, uint pos);
	DEDITORSHARED_EXPORT DCDCompletion processCompletion(QByteArray dataArray);
}

#endif // QCDASSIST_H
