/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2023 Javier Llorente <javier@opensuse.org>
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
#include "certificatemanager.h"
#include "ui_certificatemanager.h"
#include "mainapplication.h"
#include "networkmanager.h"
#include "settings.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>

CertificateManager::CertificateManager(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::CertificateManager)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);
    ui->listWidget->setLayoutDirection(Qt::LeftToRight);

    Settings settings;
    settings.beginGroup("Web-Browser-Settings");
    m_ignoredSslHosts = settings.value("IgnoredSslHosts", QStringList()).toStringList();
    settings.endGroup();
    ui->listWidget->addItems(m_ignoredSslHosts);

    connect(ui->add, &QAbstractButton::clicked, this, &CertificateManager::addException);
    connect(ui->remove, &QAbstractButton::clicked, this, &CertificateManager::removeException);
}

void CertificateManager::addException()
{    
    auto *dialog = new QDialog(this);
    auto *layout = new QFormLayout(dialog);
    auto *lineEdit = new QLineEdit(dialog);

    auto *buttonBox = new QDialogButtonBox(dialog);
    buttonBox->addButton(QDialogButtonBox::Ok);
    buttonBox->addButton(QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

    layout->addRow(new QLabel(tr("Host: ")), lineEdit);
    layout->addRow(buttonBox);
    lineEdit->setFocus();

    dialog->setWindowTitle(tr("Add certificate exception"));
    dialog->setMinimumSize(400, 100);
    dialog->setMaximumHeight(100);
    
    if (dialog->exec()) {
        QString host = lineEdit->text();
        if (m_ignoredSslHosts.contains(host)) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(tr("Host %1 already in the list").arg(host));
            if (msgBox.exec()) {
                addException();
            }
            return;
        }
        if (host.isEmpty()) {
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(tr("Empty host"));
            if (msgBox.exec()) {
                addException();
            }
            return;
        }
        
        m_ignoredSslHosts.append(host);
        ui->listWidget->addItem(host);
    }
}

void CertificateManager::removeException()
{
    m_ignoredSslHosts.removeOne(ui->listWidget->currentItem()->text());
    delete ui->listWidget->currentItem();
}

void CertificateManager::accept()
{
    Settings settings;
    settings.beginGroup("Web-Browser-Settings");
    settings.setValue("IgnoredSslHosts", m_ignoredSslHosts);
    settings.endGroup();

    mApp->networkManager()->loadSettings();

    QDialog::close();
}

CertificateManager::~CertificateManager()
{
    delete ui;
}
