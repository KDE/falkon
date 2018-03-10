
#include "ytinterface.h"
#include "ytsettings.h"
#include "ytprocess.h"
#include "browserwindow.h"
#include "webview.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "sidebar.h"
#include "webhittestresult.h"
#include "../config.h"
#include "desktopfile.h"
#include "ytsettings.h"
#include "tabwidget.h"
#include "tabbar.h"
#include "desktopnotificationsfactory.h"
#include "networkmanager.h"

#include <QMenu>
#include <QTranslator>
#include <QPushButton>
#include <QToolBar>
#include <navigationbar.h>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QNetworkProxy>

YtInterface::YtInterface()
    : QObject()
{
    // Don't do anything expensive in constructor!
    // It will be called even if user doesn't have the plugin allowed
}

DesktopFile YtInterface::metaData() const
{
	return DesktopFile(QSL(":ytdownload/metadata.desktop"));
}

void YtInterface::init(InitState state, const QString &settingsPath)
{
	Q_UNUSED(state)

	m_download = new YtIcon();
	m_download->setIcon(QIcon(QL1S(":ytdownload/data/icon.svg")));
	connect(m_download, SIGNAL(clicked(ClickController*)), this, SLOT(actionSlot()));
	connect(mApp, SIGNAL(windowCreated(BrowserWindow*)), this, SLOT(windowCreated(BrowserWindow*)));

	m_settingsPath = settingsPath + QL1S("/ytdownload/ytdownload.ini");
	loadSettings();
}

void YtInterface::unload()
{
	mApp->getWindow()->navigationBar()->removeToolButton(m_download);
}

bool YtInterface::testPlugin()
{
    // This function is called right after init()
    // There should be some testing if plugin is loaded correctly
    // If this function returns false, plugin is automatically unloaded

    return (Qz::VERSION == QLatin1String(FALKON_VERSION));
}

int YtInterface::dialogSettings(QWidget* parent)
{
	YtSettings* settings = new YtSettings(this, parent);
	return settings->exec();
}

void YtInterface::showSettings(QWidget* parent)
{
	dialogSettings(parent);
}

void YtInterface::windowCreated(BrowserWindow *window)
{
	window->navigationBar()->addToolButton(m_download);
}

void YtInterface::actionSlot()
{
	YtProcess* exe = new YtProcess(nullptr, s_debug);
	QStringList args;
	QString outputfile;
	QUrl url = mApp->getWindow()->tabWidget()->webTab(mApp->getWindow()->tabWidget()->currentIndex())->url();

	connect(exe, SIGNAL(downloadFinished(QString)), this, SLOT(downloadFinished(QString)));

	if(s_askalways)
		if(dialogSettings() == QDialog::Rejected)
			return;
	if(s_askalwaysfile)
		outputfile = QFileDialog::getSaveFileName(nullptr, "Save Video", s_defaultdir + "/" + YtProcess::getVideoTitle(s_executable, url.toString()));
	else
		outputfile = s_defaultdir + "/" + YtProcess::getVideoTitle(s_executable, url.toString());

	if(outputfile.isEmpty())
		return;

	if(s_useproxy)
		args << "--proxy" << "HTTP://" + mApp->networkManager()->proxy().hostName() + ":" + QString::number(mApp->networkManager()->proxy().port());
	if(s_debug)
		args.append("--verbose");
	if(s_metadata)
		args.append(QL1S("--add-metadata"));
	if(s_subtitle)
		args.append(QL1S("--write-sub"));
	if(s_thumbnail)
		args.append(QL1S("--embed-thumbnail"));
	if(s_extractaudio)
		args.append(QL1S("--extract-audio"));

	args << "--audio-format" <<  s_formataudio;
	args << "--audio-quality" << QString::number(s_audioquality);
	if(!s_extractaudio)
		args << "--recode-video" << s_formatvideo;

	exe->setExecutable(s_executable);
	exe->setArguments(args);
	exe->setUrl(url);
	exe->setOutputFile(outputfile);
	exe->start();
}

void YtInterface::saveSettings()
{
	QSettings settings(m_settingsPath, QSettings::IniFormat);
	settings.beginGroup("General");
	settings.setValue("Debug", s_debug);
	settings.setValue("AskFilename", s_askalwaysfile);
	settings.setValue("AskAlways", s_askalways);
	settings.setValue("ExecutablePath", s_executable);
	settings.endGroup();
	settings.beginGroup("Output");
	settings.setValue("DefaultDir", s_defaultdir);
	settings.setValue("ExtractAudio", s_extractaudio);
	settings.endGroup();
	settings.beginGroup("Format");
	settings.setValue("Video", s_formatvideo);
	settings.setValue("Audio", s_formataudio);
	settings.setValue("AudioQuality", s_audioquality);
	settings.endGroup();
	settings.beginGroup("Other");
	settings.setValue("Metadata", s_metadata);
	settings.setValue("Subtitle", s_subtitle);
	settings.setValue("Thumbnail", s_thumbnail);
	settings.endGroup();

}

void YtInterface::loadSettings()
{
	QSettings settings(m_settingsPath, QSettings::IniFormat);
	settings.beginGroup("General");
	s_debug = settings.value("Debug", false).toBool();
	s_askalwaysfile = settings.value("AskFilename", true).toBool();
	s_askalways = settings.value("AskAlways", false).toBool();
	s_executable = settings.value("ExecutablePath", "/bin/youtube-dl").toString();
	settings.endGroup();
	settings.beginGroup("Output");
	s_defaultdir = settings.value("DefaultDir", QDir::homePath()).toString();
	s_extractaudio = settings.value("ExtractAudio", false).toBool();
	settings.endGroup();
	settings.beginGroup("Format");
	s_formatvideo = settings.value("Video", "mp4").toString();
	s_formataudio = settings.value("Audio", "m4a").toString();
	s_audioquality = settings.value("AudioQuality", 5).toInt();
	settings.endGroup();
	settings.beginGroup("Other");
	s_metadata = settings.value("Metadata", false).toBool();
	s_subtitle = settings.value("Subtitle", false).toBool();
	s_thumbnail = settings.value("Thumbnail", false).toBool();
	settings.endGroup();
}

void YtInterface::downloadFinished(const QString &file)
{
	DesktopNotificationsFactory* notify = new DesktopNotificationsFactory();
	QString f = file + "." + (s_extractaudio ? s_formataudio : s_formatvideo);
	if(QFile::exists(f))
		notify->showNotification(QPixmap(QL1S(":ytdownload/data/icon-white.svg")),"Youtube video downloaded", "The youtube video has been downloaded!");
	else
		notify->showNotification(QPixmap(QL1S(":ytdownload/data/icon-white.svg")),"Download failed", "The youtube video cannot be downloaded!");
}
