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
#include "mainapplication.h"


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
            qWarning() << "Unknown permission index" << ui->comboBox->currentIndex();
            break;
    }
}

void HTML5PermissionsItem::setLabel()
{
    ui->label->setText(mApp->siteSettingsManager()->getOptionName(m_feature));
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
            ui->comboBox->setCurrentIndex(2);
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
