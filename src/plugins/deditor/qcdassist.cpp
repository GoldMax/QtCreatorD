#include "qcdassist.h"

#include <coreplugin/messagemanager.h>
#include <utils/environment.h>
//#include <QAbstractSocket>
//#include <QHostAddress>
//#include <QDebug>
#include <QProcess>
//#include <msgpack.hpp>

namespace QcdAssist
{
	static bool _DCDEnabled = true;
//static QTcpSocket* dcdSocket = 0;
//static int waitForConnectedTimeout = 1000;
//static int waitForWriteTimeout = 5000;
static int waitForReadyReadTimeout = 10000;
}

using namespace QcdAssist;

bool QcdAssist::isDCDEnabled() { return _DCDEnabled; }
void QcdAssist::isDCDEnabled(bool value) { _DCDEnabled = value; }

QString QcdAssist::dcdClient()
{
	static Utils::FileName dcd;
	if(dcd.isEmpty())
	{
		dcd = Utils::Environment::systemEnvironment().searchInPath(QLatin1String("dcd-client"));
		if(dcd.isEmpty())
			dcd = Utils::FileName::fromString(QLatin1String("dcd-client"));
	}
	return dcd.toString();
}

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
void QcdAssist::sendClearChache()
{
	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(QcdAssist::dcdClient(), QStringList() << QString(QLatin1String("--clearCache")));

	if(!proc.waitForFinished(QcdAssist::waitForReadyReadTimeout))
	{
		//Core::MessageManager::write(QLatin1String("qcdassist error: unable to clear cache: client didn't finish in time"));
		proc.close();
	}
	else if(proc.exitCode() != 0)
	{
		//Core::MessageManager::write(QLatin1String("qcdassist error: unable to clear cache - exitCode=")
		//                            + QString::number(proc.exitCode()));
		QByteArray arr = proc.readAll();
		//Core::MessageManager::write(QString::fromUtf8(arr.data(),arr.length()));
	}
}
void QcdAssist::sendAddImportToDCD(QString path)
{
	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(QcdAssist::dcdClient(), QStringList() << QString(QLatin1String("-I%1")).arg(path));

	if(!proc.waitForFinished(QcdAssist::waitForReadyReadTimeout))
	{
		//Core::MessageManager::write(QLatin1String("qcdassist error: unable to add import: client didn't finish in time"));
		proc.close();
	}
	else if(proc.exitCode() != 0)
	{
		//Core::MessageManager::write(QLatin1String("qcdassist error: unable to complete - exitCode=")
		//                            + QString::number(proc.exitCode()));
		QByteArray arr = proc.readAll();
		//Core::MessageManager::write(QString::fromUtf8(arr.data(),arr.length()));
	}
}

DCDCompletion QcdAssist::sendRequestToDCD(QByteArray& filedata, uint pos)
{
	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	proc.start(QcdAssist::dcdClient(),
		QStringList()
			//<< QString(QLatin1String("-p%1")).arg(QcdAssist::dcdPort)
			<< QString(QLatin1String("-c%1")).arg(pos)
	);
	proc.write(filedata);
	proc.closeWriteChannel();
	if(!proc.waitForFinished(QcdAssist::waitForReadyReadTimeout))
	{
		//Core::MessageManager::write(QLatin1String("qcdassist error: unable to complete: client didn't finish in time"));
		proc.close();
	}
	else if(proc.exitCode() != 0)
	{
		//Core::MessageManager::write(QString(QLatin1String("qcdassist error: unable to complete: %1")).arg(proc.exitCode()));
		QByteArray arr = proc.readAll();
		//Core::MessageManager::write(QString::fromUtf8(arr.data(),arr.length()));
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
		if(lines.length() == 0)
			return completion;
		type = lines.front();
	}
	if(type == QLatin1String("identifiers"))
		completion.type = Identifiers;
	else if(type == QLatin1String("calltips"))
		completion.type = Calltips;
	else
	{
		//Core::MessageManager::write(QString(QLatin1String("qcdassist error: Invalid typ=:")).arg(type));
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
			//Core::MessageManager::write(QString(QLatin1String("qcdassist error: invalid completion data:")).arg(kv.length()).arg(completion.type));
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
