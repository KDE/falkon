/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2014  David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "sslerrordialog.h"
#include "ui_sslerrordialog.h"
#include "iconprovider.h"

#include <QPushButton>
#include <QPlainTextEdit>

SslErrorDialog::SslErrorDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SslErrorDialog)
    , m_result(No)
{
    ui->setupUi(this);
    ui->icon->setPixmap(IconProvider::standardIcon(QStyle::SP_MessageBoxCritical).pixmap(52));
    ui->buttonBox->addButton(tr("Only for this session"), QDialogButtonBox::ApplyRole);
    ui->buttonBox->button(QDialogButtonBox::No)->setFocus();

    ui->certTabWidget->hide();
    ui->certDetailsButton->hide();
    adjustSize();

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SslErrorDialog::buttonClicked);
    connect(ui->certDetailsButton, &QPushButton::clicked, this, &SslErrorDialog::toggleCertificateDetails);
}

SslErrorDialog::~SslErrorDialog()
{
    delete ui;
}

void SslErrorDialog::setText(const QString &text)
{
    ui->text->setText(text);
}

void SslErrorDialog::setCertificateChain(const QList<QSslCertificate> &certificateChain)
{
    if (certificateChain.isEmpty()) {
        return;
    }

    ui->certDetailsButton->show();

    for (auto &certificate : certificateChain) {
        auto *certDescription = new QPlainTextEdit(ui->certTabWidget);
        certDescription->setPlainText(certificate.toText());
        certDescription->setReadOnly(true);
        ui->certTabWidget->indexOf(certDescription);
        ui->certTabWidget->addTab(certDescription, certificate.subjectDisplayName());
    }
}

SslErrorDialog::Result SslErrorDialog::result()
{
    return m_result;
}

void SslErrorDialog::buttonClicked(QAbstractButton* button)
{
    switch (ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::YesRole:
        m_result = Yes;
        accept();
        break;

    case QDialogButtonBox::ApplyRole:
        m_result = OnlyForThisSession;
        accept();
        break;

    case QDialogButtonBox::NoRole:
        m_result = NoForThisSession;
        reject();
        break;

    default:
        m_result = No;
        reject();
        break;
    }
}

void SslErrorDialog::toggleCertificateDetails()
{
    if (ui->certTabWidget->isVisible()) {
        ui->certTabWidget->hide();
    }
    else {
        ui->certTabWidget->show();
    }
}
