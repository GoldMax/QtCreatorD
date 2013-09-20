#ifndef QCDASSIST_H
#define QCDASSIST_H

#include <QString>
#include <QTcpSocket>

#include <string>
//#include <msgpack.hpp>

namespace QcdAssist
{
 /// Port of DCD server
 static quint16 dcdPort = 9166;
 static QString dcdClient(QLatin1String("/opt/DCD/dcd-client"));
 static QTcpSocket* dcdSocket = 0;
 static int waitForConnectedTimeout = 1000;
 static int waitForWriteTimeout = 5000;
 static int waitForReadyReadTimeout = 10000;

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
  PackageName         = 'p',
  ModuleName          = 'M'
 };
 struct DCDCompletionItem
 {
  DCDCompletionItem(DCDCompletionItemType t, QString s);

  DCDCompletionItemType type;
  QString name;

  //QIcon icon() const;
 };
 struct DCDCompletion
 {
  DCDCompletionType type;
  QList<DCDCompletionItem> completions;
 };

 //--------------------
 //--- Socket Funcs ---
 //--------------------
 bool openDcdSocket();
 DCDCompletion sendRequestToDCD(QByteArray& filedata, uint pos);
 //QByteArray sendRequestToDCD(AutocompleteRequest& req);
 QByteArray sendRequestToDCD(const char* data, qint64 len);
 DCDCompletion processCompletion(QByteArray dataArray);
// {
//  ubyte[] message = msgpack.pack(request);
//  ubyte[] messageBuffer = new ubyte[message.length + message.length.sizeof];
//  auto messageLength = message.length;
//  messageBuffer[0 .. size_t.sizeof] = (cast(ubyte*) &messageLength)[0 .. size_t.sizeof];
//  messageBuffer[size_t.sizeof .. $] = message[];
//  return socket.send(messageBuffer) == messageBuffer.length;
// }

// /**
//  * Gets the response from the server
//  */
// AutocompleteResponse getResponse(TcpSocket socket)
// {
//  ubyte[1024 * 16] buffer;
//  auto bytesReceived = socket.receive(buffer);
//  if (bytesReceived == Socket.ERROR)
//   throw new Exception("Incorrect number of bytes received");
//  AutocompleteResponse response;
//  msgpack.unpack(buffer[0..bytesReceived], response);
//  return response;
// }


}

#endif // QCDASSIST_H
