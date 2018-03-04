#ifndef YTSETTINGS_H
#define YTSETTINGS_H

#include <QDialog>
#include "ytinterface.h"

namespace Ui {
class YtSettings;
}

class YtSettings : public QDialog
{
	Q_OBJECT

public:
	explicit YtSettings(YtInterface *plugin, QWidget *parent = nullptr);
	~YtSettings();

private Q_SLOTS:

	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

private:

	Ui::YtSettings *ui;
	YtInterface *m_plugin;
};

#endif
