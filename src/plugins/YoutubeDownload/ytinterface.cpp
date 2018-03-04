 
#include "ytinterface.h"
#include "ytsettings.h"
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

#include <QMenu>
#include <QTranslator>
#include <QPushButton>
#include <QToolBar>
#include <navigationbar.h>
#include <QProcess>
#include <QSettings>
#include <QDir>

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
	m_download = new YtIcon();
	m_download->setIcon(QIcon(QL1S(":ytdownload/data/icon.svg")));
	mApp->getWindow()->navigationBar()->addToolButton(m_download);
	connect(m_download, SIGNAL(clicked(ClickController*)), this, SLOT(actionSlot()));

	m_settingsPath = settingsPath + QL1S("/ytdownload/ytdownload.ini");
	qWarning() << m_settingsPath;
	loadSettings();
}

void YtInterface::unload()
{
	mApp->getWindow()->navigationBar()->removeToolButton(m_download);

    // Deleting settings dialog if opened
    delete m_settings.data();
}

bool YtInterface::testPlugin()
{
    // This function is called right after init()
    // There should be some testing if plugin is loaded correctly
    // If this function returns false, plugin is automatically unloaded

    return (Qz::VERSION == QLatin1String(FALKON_VERSION));
}


void YtInterface::showSettings(QWidget* parent)
{
	if (!m_settings)
		m_settings = new YtSettings(this, parent);

    m_settings.data()->show();
    m_settings.data()->raise();
}


void YtInterface::actionSlot()
{
	QProcess* exe = new QProcess;

	QStringList args;

	if(s_metadata)
		args.append(QL1S("--add-metadata"));
	if(s_subtitle)
		args.append(QL1S("--write-sub"));
	if(s_thumbnail)
		args.append(QL1S("--embed-thumbnail"));

	args.append("-f 'bestvideo[ext=" + s_formatvideo + "]+bestaudio[ext=" + s_formataudio + "]'");
	args.append("-o '" + s_outputformat + "'");
	args.append(mApp->getWindow()->tabWidget()->webTab(mApp->getWindow()->tabWidget()->currentIndex())->url().toString());
	qWarning() << s_executable << args;
	exe->execute(s_executable, args);
	qWarning() << exe->errorString() << exe->exitCode();
}

void YtInterface::saveSettings()
{
	QSettings settings(m_settingsPath, QSettings::IniFormat);
	settings.beginGroup("General");
	settings.setValue("AskAlways", s_askalways);
	settings.setValue("ExecutablePath", s_executable);
	settings.endGroup();
	settings.beginGroup("Output");
	settings.setValue("DefaultDir", s_defaultdir);
	settings.setValue("OutputFormat", s_outputformat);
	settings.endGroup();
	settings.beginGroup("Format");
	settings.setValue("Video", s_formatvideo);
	settings.setValue("Audio", s_formataudio);
	settings.endGroup();
	settings.beginGroup("Other");
	settings.setValue("Metadata", s_metadata);
	settings.setValue("Subtitle", s_subtitle);
	settings.setValue("Thumbnail", s_thumbnail);
	settings.endGroup();

}

void YtInterface::loadSettings()
{
	if(!QFileInfo::exists(m_settingsPath))
		return;

	QSettings settings(m_settingsPath, QSettings::IniFormat);
	settings.beginGroup("General");
	s_askalways = settings.value("AskAlways", false).toBool();
	s_executable = settings.value("ExecutablePath", "/bin/youtube-dl").toString();
	settings.endGroup();
	settings.beginGroup("Output");
	s_defaultdir = settings.value("DefaultDir", QDir::homePath()).toString();
	s_outputformat = settings.value("OutputFormat", "%(title)s.%(ext)s").toString();
	settings.endGroup();
	settings.beginGroup("Format");
	s_formatvideo = settings.value("Video", "mp4").toString();
	s_formataudio = settings.value("Audio", "m4a").toString();
	settings.endGroup();
	settings.beginGroup("Other");
	s_metadata = settings.value("Metadata", false).toBool();
	s_subtitle = settings.value("Subtitle", false).toBool();
	s_thumbnail = settings.value("Thumbnail", false).toBool();
	settings.endGroup();
}
