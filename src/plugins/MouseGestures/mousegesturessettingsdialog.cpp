/* ============================================================
* Mouse Gestures plugin for Falkon
* Copyright (C) 2012-2014  David Rosca <nowrep@gmail.com>
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
#include "mousegesturessettingsdialog.h"
#include "ui_mousegesturessettingsdialog.h"
#include "licenseviewer.h"
#include "mousegestures.h"

#include <QLabel>

MouseGesturesSettingsDialog::MouseGesturesSettingsDialog(MouseGestures* gestures, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::MouseGesturesSettingsDialog)
    , m_gestures(gestures)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    if (QApplication::isRightToLeft()) {
        ui->label_5->setPixmap(QPixmap(":/mousegestures/data/right.gif"));
        ui->label_6->setPixmap(QPixmap(":/mousegestures/data/left.gif"));
        ui->label_18->setPixmap(QPixmap(":/mousegestures/data/up-right.gif"));
        ui->label_20->setPixmap(QPixmap(":/mousegestures/data/up-left.gif"));
    }

    m_gestures->loadSettings();
    ui->mouseButtonComboBox->setCurrentIndex(m_gestures->buttonToIndex());
    ui->enableRockerNavigation->setChecked(m_gestures->rockerNavigationEnabled());

    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accepted()));
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);
    connect(ui->licenseButton, &QAbstractButton::clicked, this, &MouseGesturesSettingsDialog::showLicense);
}

MouseGesturesSettingsDialog::~MouseGesturesSettingsDialog()
{
    delete ui;
}

void MouseGesturesSettingsDialog::showLicense()
{
    auto* v = new LicenseViewer(this);
    v->setLicenseFile(":mousegestures/data/copyright");

    v->show();
}

void MouseGesturesSettingsDialog::accepted()
{
    m_gestures->setGestureButtonByIndex(ui->mouseButtonComboBox->currentIndex());
    m_gestures->setRockerNavigationEnabled(ui->enableRockerNavigation->isChecked());
    m_gestures->saveSettings();
    close();
}
