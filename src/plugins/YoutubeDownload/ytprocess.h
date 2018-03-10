#ifndef YTPROCESS_H
#define YTPROCESS_H

#include <QThread>
#include <QProcess>
#include <QUrl>

class YtProcess : public QThread
{
    Q_OBJECT
public:
	YtProcess(QObject* parent = nullptr, bool debug = false);
    ~YtProcess();

	void run();
	static bool test(const QString& e);

	static QString getVideoTitle(const QString& _exe, const QString& _url);

	void setExecutable(const QString& _exe);
	void setArguments(const QStringList& _args);
	void setUrl(const QUrl &_url);
	void setOutputFile(const QString &out);

	QString executable();
	QStringList arguments();
	QUrl url();
	QString outputfile();

signals:
	void readOutput(const QString& out);
	void downloadFinished(const QString& file);
private:
	QProcess* proc;
	QString exe;
	QStringList args;
	QUrl u;
	QString of;
};

#endif //YTPROCESS_H
