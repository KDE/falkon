#ifndef YTINTERFACE_H
#define YTINTERFACE_H

// Include plugininterface.h for your version of Falkon
#include "plugininterface.h"
#include "yticon.h"
#include "browserwindow.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

class YtInterface : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.YtInterface")

public:
	explicit YtInterface();

    DesktopFile metaData() const override;
    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;
	void showSettings(QWidget *parent) override;
	void saveSettings();
	void loadSettings();

	QString getOutputFile(const QString &url);

	bool s_debug;
	bool s_askalways;
	bool s_metadata;
	bool s_subtitle;
	bool s_thumbnail;
	bool s_extractaudio;
	bool s_useproxy;
	bool s_askalwaysfile;
	QString s_formataudio;
	QString s_formatvideo;
	QString s_defaultdir;

	int s_audioquality;

	QString s_executable;

private slots:
    void actionSlot();
	void downloadFinished(const QString &file);
	void windowCreated(BrowserWindow* window);

private:

	int dialogSettings(QWidget* parent = nullptr);
	YtIcon* m_download;
	WebView* m_view;
	QString m_settingsPath;
};

#endif // YTINTERFACE_H
