#include "ytprocess.h"
#include <QDebug>

YtProcess::YtProcess(QObject* parent, bool debug) : QThread(parent)
{
	proc = new QProcess;
	if(debug)
		proc->setProcessChannelMode(QProcess::ForwardedChannels);
}

YtProcess::~YtProcess()
{
}

void YtProcess::run()
{
	if(exe.isEmpty() || args.isEmpty() || url.isEmpty())
	{
		qWarning() << "[YoutubeDownload][FAIL] program or arguments are not found";
		return;
	}

	QStringList temp = args;
	temp << url.toString();

	proc->start(exe, temp, QIODevice::ReadOnly);
	if(!proc->waitForStarted())
		return;

	proc->waitForReadyRead();
	while(proc->bytesAvailable())
	{
		emit readOutput(proc->readLine());
	}
}

QString YtProcess::getVideoTitle(const QString& _exe, const QString &_url)
{
	QProcess* p = new QProcess();
	QStringList a;
	QString offset;
	a << "--get-filename" << "-o" << "%(title)s" << _url;

	p->start(_exe, a, QIODevice::ReadOnly);
	if(!p->waitForStarted())
		return "";


	p->waitForReadyRead();
	offset = p->readAll();
	return offset;
}

bool YtProcess::test(const QString &e)
{
	QProcess* p = new QProcess();
	QStringList a;
	a << "--get-filename" << "--restrict-filenames" << "-o" << "%(title)s" << "BaW_jenozKc";

	p->start(e, a, QIODevice::ReadOnly);
	if(!p->waitForStarted())
		return false;

	QString b;
	while(p->waitForReadyRead())
		b.append(p->readAll());

	return b == "youtube-dl_test_video_a\n";
}

void YtProcess::setExecutable(const QString &_exe) { exe = _exe; }
void YtProcess::setArguments(const QStringList& _args) { args = _args; }
void YtProcess::setUrl(const QUrl &_url) { url = _url; }
