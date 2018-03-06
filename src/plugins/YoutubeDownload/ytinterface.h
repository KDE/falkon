#ifndef YTINTERFACE_H
#define YTINTERFACE_H

// Include plugininterface.h for your version of Falkon
#include "plugininterface.h"

#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QPointer>
#include "yticon.h"

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

	bool s_debug;
	bool s_askalways;
	bool s_metadata;
	bool s_subtitle;
	bool s_thumbnail;
	QString s_formataudio;
	QString s_formatvideo;
	QString s_defaultdir;
	QString s_outputformat;
	QString s_executable;

private slots:
    void actionSlot();

private:
	QPointer<QDialog> m_settings;
	YtIcon* m_download;
	WebView* m_view;
	QString m_settingsPath;
};

#endif // YTINTERFACE_H
