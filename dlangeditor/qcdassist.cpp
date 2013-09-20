#include "qcdassist.h"

#include <QAbstractSocket>
#include <QHostAddress>
#include <QDebug>
#include <QProcess>
//#include <msgpack.hpp>

using namespace QcdAssist;

DCDCompletionItem::DCDCompletionItem(DCDCompletionItemType t, QString s) : type(t), name(s) {}
//--------------------
//--- Socket Funcs ---
//--------------------
//bool QcdAssist::openDcdSocket()
//{
// if(dcdSocket == 0)
//  dcdSocket = new QTcpSocket();
// if(dcdSocket->state() != QAbstractSocket::ConnectedState)
// {
//  dcdSocket->connectToHost(QHostAddress::LocalHost,dcdPort);
//  if(dcdSocket->waitForConnected(waitForConnectedTimeout))
//   qDebug("Connected to DCD ...");
//  else
//   qDebug("Connection to DCD fails!");
// }
// return dcdSocket->state() == QAbstractSocket::ConnectedState;
//}
//QByteArray QcdAssist::sendRequestToDCD(const char* data, qint64 len)
//{
// if(openDcdSocket() == false)
//  return QByteArray();
// dcdSocket->write(data, len);
// if(dcdSocket->waitForBytesWritten(waitForWriteTimeout) == false)
// {
//  qDebug("Write to DCD timeout!");
//  return QByteArray();
// }
// if(dcdSocket->waitForReadyRead(waitForReadyReadTimeout) == false)
// {
//  qDebug("waitForReadyRead timeout!");
//  return QByteArray();
// }
// return dcdSocket->readAll();
//}
//QByteArray QcdAssist::sendRequestToDCD(AutocompleteRequest& req)
//{
//// // Serialize it.
//// msgpack::sbuffer sbuf;  // simple buffer
//// msgpack::pack(&sbuf, req);

//// return sendRequestToDCD(sbuf.data(), sbuf.size());
//}
DCDCompletion QcdAssist::sendRequestToDCD(QByteArray& filedata, uint pos)
{
 QProcess proc;
 proc.setProcessChannelMode(QProcess::MergedChannels);
 proc.start(dcdClient,
  QStringList()
   << QString(QLatin1String("-p%1")).arg(dcdPort)
   << QString(QLatin1String("-c%1")).arg(pos)
 );
 proc.write(filedata);
 proc.closeWriteChannel();
 if(!proc.waitForFinished(waitForReadyReadTimeout))
 {
  qDebug() << "unable to complete: client didn't finish in time";
  proc.close();
 }
 else if(proc.exitCode() != 0)
 {
  qDebug() << "unable to complete:" << proc.exitCode();
  qDebug() << proc.readAll();
 }
 else
 {
  // everything Ok
  return processCompletion(proc.readAllStandardOutput());
 }

 return DCDCompletion();
}

DCDCompletion QcdAssist::processCompletion(QByteArray dataArray)
{
 DCDCompletion completion;

 QString data = QString::fromUtf8(dataArray.data(),dataArray.length());
 QStringList lines = data.split(QRegExp(QLatin1String("[\r\n]")), QString::SkipEmptyParts);
 if(lines.length() == 0)
  return completion;

 QString type = lines.front();
 if(type.startsWith(QLatin1String("WARNING:")))
 {
  lines.pop_front();
  type = lines.front();
 }
 if(type == QLatin1String("identifiers"))
  completion.type = Identifiers;
 else if(type == QLatin1String("calltips"))
  completion.type = Calltips;
 else
 {
  qDebug() << "Invalid type:" << type;
  return completion;
 }
 lines.pop_front();

 foreach(QString line, lines)
 {
  if(line.trimmed().length() == 0)
   continue;

  QStringList kv = line.split(QRegExp(QLatin1String("\\s+")), QString::SkipEmptyParts);
  if(kv.length() != 2 && completion.type != Calltips)
  {
   qDebug() << "invalid completion data:" << kv.length() << completion.type;
   continue;
  }

  if(completion.type == Identifiers)
  {
   completion.completions.append(DCDCompletionItem(
    (DCDCompletionItemType)kv[1].at(0).toLatin1(), kv[0]));
  }
  else
  {
   completion.completions.append(DCDCompletionItem(Calltip, line));
  }
 }

 return completion;
}
