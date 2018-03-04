#include "ui_ytsettings.h"
#include "ytsettings.h"
#include <QDialogButtonBox>
#include <QCheckBox>

YtSettings::YtSettings(YtInterface *plugin, QWidget *parent) : QDialog(parent), ui(new Ui::YtSettings)
{
	ui->setupUi(this);

	m_plugin->loadSettings();

	ui->lineExecuteble->setText(m_plugin->s_executable);
	ui->lineTitleFormat->setText(m_plugin->s_defaultdir + "/" + m_plugin->s_outputformat);
	ui->checkMetadata->setChecked(m_plugin->s_metadata);
	ui->checkSubtitle->setChecked(m_plugin->s_subtitle);
	ui->checkThumbnail->setChecked(m_plugin->s_thumbnail);
	ui->checkAskAlways->setChecked(m_plugin->s_askalways);
	ui->checkMetadata->setChecked(m_plugin->s_metadata);

	m_plugin = plugin;
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(on_buttonBox_accepted()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(on_buttonBox_rejected()));
}

YtSettings::~YtSettings()
{
	delete ui;
}

void YtSettings::on_buttonBox_accepted()
{
	if(!ui->lineExecuteble->text().isEmpty())
		m_plugin->s_executable = ui->lineExecuteble->text();

	m_plugin->s_metadata = ui->checkMetadata->isChecked();
	m_plugin->s_subtitle = ui->checkSubtitle->isChecked();
	m_plugin->s_thumbnail = ui->checkThumbnail->isChecked();
	m_plugin->s_askalways = ui->checkAskAlways->isChecked();

	m_plugin->s_formatvideo = ui->comboVideoExtention->currentText();
	m_plugin->s_formataudio = ui->comboAudioExtention->currentText();
	m_plugin->s_outputformat = ui->lineTitleFormat->text();

	m_plugin->saveSettings();
	accept();
}

void YtSettings::on_buttonBox_rejected()
{
	reject();
}
