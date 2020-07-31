#include "qcdassist.h"

#include <coreplugin/messagemanager.h>
#include <utils/environment.h>

#include <QProcess>
#include <QTextDocument>

namespace QcdAssist
{
	static bool _DCDEnabled = true;
	static int waitForReadyReadTimeout = 10000;
}

using namespace QcdAssist;

bool QcdAssist::isDCDEnabled() { return _DCDEnabled; }
void QcdAssist::isDCDEnabled(bool value) { _DCDEnabled = value; }

QString QcdAssist::dcdClient()
{
	static Utils::FilePath dcd;
	if(dcd.isEmpty())
	{
		dcd = Utils::Environment::systemEnvironment().searchInPath(QLatin1String("dcd-client"));
		if(dcd.isEmpty())
			dcd = Utils::FilePath::fromString(QLatin1String("dcd-client"));
	}
	return dcd.toString();
}

DCDCompletionItem::DCDCompletionItem(DCDCompletionItemType t, QString s) : type(t), name(s) {}
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

// Записывает содержимое документа в массив байт UTF-8 кодировки
// возвращает преобразованное значение символьной позиции в байтовое смещение
int QcdAssist::toUtf8(QByteArray& arr, QTextDocument* doc, int charPosition)
{
	const QString text = doc->toPlainText();
	arr.append(text.leftRef(charPosition).toUtf8());
	int pos = arr.length();
	arr.append(QStringRef(&text,charPosition, text.length()-charPosition).toUtf8());
	return pos;
}
QString QcdAssist::sendRequestToDCD(QByteArray& filedata, QString command, uint bytePosition)
{
	QProcess proc;
	proc.setProcessChannelMode(QProcess::MergedChannels);
	QList<QString> args;
	if(command.isNull() == false)
		args.append(command);
	args.append(QString("-c%2").arg(bytePosition));
	proc.start(QcdAssist::dcdClient(), args	);
	proc.write(filedata);
	proc.closeWriteChannel();

	QByteArray res;

	if(!proc.waitForFinished(QcdAssist::waitForReadyReadTimeout))
	{
		Core::MessageManager::write(QLatin1String("qcdassist error: unable to complete: client didn't finish in time"));
		proc.close();
		return QString();
	}
	else if(proc.exitCode() != 0)
	{
		Core::MessageManager::write(QString("qcdassist error code: %1").arg(proc.exitCode()));
		res = proc.readAll();
		QString err = QString::fromUtf8(res.data(), res.length());
		Core::MessageManager::write(QString("qcdassist error text: %1").arg(err));
		return QString();
	}
	// everything Ok
	res = proc.readAllStandardOutput();
	return QString::fromUtf8(res.data(), res.length());
}

DCDCompletion QcdAssist::autocompletion(QTextDocument* document, int charPosition)
{
	DCDCompletion completion;
	if(document == nullptr || charPosition == 0)
		return completion;

	QByteArray dataArray;
	int bytePosition = toUtf8(dataArray, document, charPosition);
	QString data = sendRequestToDCD(dataArray, QString(), bytePosition);

	QStringList lines = data.split(QRegExp(QLatin1String("[\r\n]")), Qt::SkipEmptyParts);
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

		QStringList kv = line.split(QRegExp(QLatin1String("\\s+")), Qt::SkipEmptyParts);
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

Utils::Link QcdAssist::symbolLocation(QTextDocument* document, int charPosition)
{
	if(document == nullptr || charPosition == 0)
		return QString();

	QByteArray dataArray;
	int bytePosition = toUtf8(dataArray, document, charPosition);
	QString data = sendRequestToDCD(dataArray, QLatin1String("--symbolLocation"), bytePosition);

	data = data.trimmed();
	//Core::MessageManager::write(QLatin1String("qcdassist: %1").arg(data));

	if(data.toLower() == "not found")
		return Utils::Link();

	QStringList lines = data.split(QRegExp(QLatin1String("[\t]")), Qt::SkipEmptyParts);
	if(lines.length() != 2)
		return Utils::Link();

	bytePosition = lines[1].toInt();

	Utils::Link link;
	link.targetFileName = lines[0];
	link.targetLine = 0;
	link.targetColumn = 0;

	if(link.targetFileName != "stdin")
	{
		QFile file(link.targetFileName);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return Utils::Link();
		dataArray = file.read(bytePosition);
		if(dataArray.length() < bytePosition)
			return Utils::Link();
	}
	data = QString::fromUtf8(dataArray.data(), bytePosition);

	link.targetLine = data.count(QChar('\n'));
	if(link.targetLine > 0)
		link.targetColumn = data.length() - data.lastIndexOf(QChar('\n'));

	return link;
}
