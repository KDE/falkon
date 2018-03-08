#include "ui_ytsettings.h"
#include "ytsettings.h"
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QToolButton>
#include <QSlider>

YtSettings::YtSettings(YtInterface *plugin, QWidget *parent) : QDialog(parent), ui(new Ui::YtSettings), m_plugin(plugin)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	m_plugin->loadSettings();

	ui->comboAudioExtention->setCurrentText(m_plugin->s_formataudio);
	ui->comboVideoExtention->setCurrentText(m_plugin->s_formatvideo);
	ui->lineExec->setText(m_plugin->s_executable);
	ui->lineDefaultDir->setText(m_plugin->s_defaultdir);
	ui->checkMetadata->setChecked(m_plugin->s_metadata);
	ui->checkSubtitle->setChecked(m_plugin->s_subtitle);
	ui->checkThumbnail->setChecked(m_plugin->s_thumbnail);
	ui->checkAskAlways->setChecked(m_plugin->s_askalways);
	ui->checkMetadata->setChecked(m_plugin->s_metadata);
	ui->checkDebug->setChecked(m_plugin->s_debug);
	ui->checkExtract->setChecked(m_plugin->s_extractaudio);
	ui->checkAskAlwaysFile->setChecked(m_plugin->s_askalwaysfile);
	ui->checkProxy->setChecked(m_plugin->s_useproxy);
	ui->sliderQuality->setValue(m_plugin->s_audioquality);

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(on_buttonBox_accepted()));
	connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(on_buttonBox_rejected()));
	connect(ui->toolExecutable, SIGNAL(clicked(bool)), this, SLOT(selectFile()));
	connect(ui->toolDir, SIGNAL(clicked(bool)), this, SLOT(selectDir()));
	connect(ui->lineExec, SIGNAL(textChanged(QString)), this, SLOT(changeExec(QString)));
}

YtSettings::~YtSettings()
{
	delete ui;
}

void YtSettings::on_buttonBox_accepted()
{
	if(!ui->lineExec->text().isEmpty())
		m_plugin->s_executable = ui->lineExec->text();

	m_plugin->s_metadata = ui->checkMetadata->isChecked();
	m_plugin->s_subtitle = ui->checkSubtitle->isChecked();
	m_plugin->s_thumbnail = ui->checkThumbnail->isChecked();
	m_plugin->s_askalways = ui->checkAskAlways->isChecked();
	m_plugin->s_debug = ui->checkDebug->isChecked();
	m_plugin->s_extractaudio = ui->checkExtract->isChecked();
	m_plugin->s_askalwaysfile = ui->checkAskAlwaysFile->isChecked();
	m_plugin->s_useproxy = ui->checkProxy->isChecked();

	m_plugin->s_formatvideo = ui->comboVideoExtention->currentText();
	m_plugin->s_formataudio = ui->comboAudioExtention->currentText();
	m_plugin->s_defaultdir = ui->lineDefaultDir->text();
	m_plugin->s_audioquality = ui->sliderQuality->value();

	m_plugin->saveSettings();
	accept();
}

void YtSettings::on_buttonBox_rejected()
{
	reject();
}

void YtSettings::selectFile()
{
	ui->lineExec->setText(QFileDialog::getOpenFileName(this, "Select youtube-dl executable file.", m_plugin->s_executable));
}

void YtSettings::selectDir()
{
	ui->lineDefaultDir->setText(QFileDialog::getExistingDirectory(this, "Select download directory.", m_plugin->s_defaultdir));
}

void YtSettings::changeExec(const QString& file)
{
	if(!YtProcess::test(file))
		ui->lineExec->setStyleSheet("QLineEdit { color : red; }");
	else
		ui->lineExec->setStyleSheet("");
}
