/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#include "autofillnotification.h"
#include "ui_autofillnotification.h"
#include "autofill.h"
#include "mainapplication.h"
#include "animatedwidget.h"
#include "iconprovider.h"

#include <KLocalizedString>

AutoFillNotification::AutoFillNotification(const QUrl &url, const PageFormData &formData, const PasswordEntry &updateData)
    : AnimatedWidget(AnimatedWidget::Down, 300, 0)
    , ui(new Ui::AutoFillNotification)
    , m_url(url)
    , m_formData(formData)
    , m_updateData(updateData)
{
    setAutoFillBackground(true);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(widget());
    ui->closeButton->setIcon(IconProvider::standardIcon(QStyle::SP_DialogCloseButton));

    QString hostPart;
    QString userPart;

    if (!url.host().isEmpty()) {
        hostPart = i18n("on %1", url.host());
    }

    if (!m_formData.username.isEmpty()) {
        userPart = i18n("for <b>%1</b>", m_formData.username);
    }

    if (m_updateData.isValid()) {
        ui->label->setText(i18n("Do you want Falkon to update saved password %1?", userPart));

        ui->remember->setVisible(false);
        ui->never->setVisible(false);
    }
    else {
        ui->label->setText(i18n("Do you want Falkon to remember the password %1 %2?", userPart, hostPart));

        ui->update->setVisible(false);
    }

    connect(ui->update, SIGNAL(clicked()), this, SLOT(update()));
    connect(ui->remember, &QAbstractButton::clicked, this, &AutoFillNotification::remember);
    connect(ui->never, &QAbstractButton::clicked, this, &AutoFillNotification::never);
    connect(ui->notnow, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(hide()));

    startAnimation();
}

void AutoFillNotification::update()
{
    mApp->autoFill()->updateEntry(m_updateData);
    hide();
}

void AutoFillNotification::never()
{
    mApp->autoFill()->blockStoringforUrl(m_url);
    hide();
}

void AutoFillNotification::remember()
{
    mApp->autoFill()->addEntry(m_url, m_formData);
    hide();
}

AutoFillNotification::~AutoFillNotification()
{
    delete ui;
}
