/* ============================================================
 * Falkon - Qt web browser
 * Copyright (C) 2022 Juraj Oravec <jurajoravec@mailo.com>
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
#include "html5permissionsitem.h"
#include "ui_html5permissionsitem.h"


HTML5PermissionsItem::HTML5PermissionsItem(const QWebEnginePage::Feature& feature, const SiteSettingsManager::Permission& permission, QWidget* parent)
: QWidget(parent)
, ui(new Ui::HTML5PermissionsItem())
, m_feature(feature)
, m_permission(permission)
{
    ui->setupUi(this);

    setLabel();
    setCombo();

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(permissionIndexChanged()));
}

HTML5PermissionsItem::~HTML5PermissionsItem() noexcept
{
    delete ui;
}

void HTML5PermissionsItem::permissionIndexChanged()
{
    switch (ui->comboBox->currentIndex()) {
        case 0:
            m_permission = SiteSettingsManager::Allow;
            break;
        case 1:
            m_permission = SiteSettingsManager::Deny;
            break;
        case 2:
            m_permission = SiteSettingsManager::Ask;
            break;
        default:
            Q_UNREACHABLE();
            qWarning() << "Unknown permission index" << ui->comboBox->currentIndex();
            break;
    }
}

void HTML5PermissionsItem::setLabel()
{
    switch (m_feature) {
        case QWebEnginePage::Notifications:
            ui->label->setText("Notifications");
            break;

        case QWebEnginePage::Geolocation:
            ui->label->setText("Location");
            break;

        case QWebEnginePage::MediaAudioCapture:
            ui->label->setText("Microphone");
            break;

        case QWebEnginePage::MediaVideoCapture:
            ui->label->setText("Camera");
            break;

        case QWebEnginePage::MediaAudioVideoCapture:
            ui->label->setText("Microphone and Camera");
            break;

        case QWebEnginePage::MouseLock:
            ui->label->setText("Hide mouse pointer");
            break;

        case QWebEnginePage::DesktopVideoCapture:
            ui->label->setText("Screen capture");
            break;

        case QWebEnginePage::DesktopAudioVideoCapture:
            ui->label->setText("Screen capture with audio");
            break;

        default:
            Q_UNREACHABLE();
            qWarning() << "Unknown feature" << m_feature;
            break;
    }
}

void HTML5PermissionsItem::setCombo()
{
    switch (m_permission) {
        case SiteSettingsManager::Allow:
            ui->comboBox->setCurrentIndex(0);
            break;
        case SiteSettingsManager::Deny:
            ui->comboBox->setCurrentIndex(1);
            break;
        case SiteSettingsManager::Ask:
        case SiteSettingsManager::Default:
            ui->comboBox->setCurrentIndex(2);
            break;
        default:
            Q_UNREACHABLE();
            qWarning() << "Unknown permission" << m_permission;
            break;
    }
}

QWebEnginePage::Feature HTML5PermissionsItem::getFeature() const
{
    return m_feature;
}

SiteSettingsManager::Permission HTML5PermissionsItem::getPermission() const
{
    return m_permission;
}
