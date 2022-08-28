/* ============================================================
 * Falkon - Qt web browser
 * Copyright (C) 2022  Juraj Oravec <jurajoravec@mailo.com>
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

#include "siteinfopermissionitem.h"
#include "ui_siteinfopermissionitem.h"
#include "mainapplication.h"
#include "sitesettingsmanager.h"


SiteInfoPermissionItem::SiteInfoPermissionItem(const SiteSettingsManager::PageOptions &a_option, const SiteSettingsManager::Permission &a_permission, QWidget* parent)
: QWidget(parent)
, m_option(a_option)
, m_ui(new Ui::SiteInfoPermissionItem())
{
    m_ui->setupUi(this);

    setPermission(a_permission);
    m_ui->label->setText(mApp->siteSettingsManager()->getOptionName(a_option));
}

SiteInfoPermissionItem::~SiteInfoPermissionItem()
{
}

bool SiteInfoPermissionItem::hasOptionAsk() const
{
    return m_hasOptionAsk;
}

void SiteInfoPermissionItem::setHasOptionAsk(bool hasOptionAsk)
{
    if (m_hasOptionAsk == hasOptionAsk) {
        return;
    }

    m_hasOptionAsk = hasOptionAsk;
    m_ui->radioAsk->setVisible(hasOptionAsk);
}

bool SiteInfoPermissionItem::hasOptionDefault() const
{
    return m_hasOptionDefault;
}

void SiteInfoPermissionItem::setHasOptionDefault(bool hasOptionDefault)
{
    if (m_hasOptionDefault == hasOptionDefault) {
        return;
    }

    m_hasOptionDefault = hasOptionDefault;
    m_ui->radioDefault->setVisible(hasOptionDefault);
}

void SiteInfoPermissionItem::setPermission(SiteSettingsManager::Permission permission)
{
    m_ui->radioAllow->setChecked(false);
    m_ui->radioAsk->setChecked(false);
    m_ui->radioDeny->setChecked(false);
    m_ui->radioDefault->setChecked(false);

    switch (permission) {
        case SiteSettingsManager::Allow:
            m_ui->radioAllow->setChecked(true);
            break;
        case SiteSettingsManager::Ask:
            m_ui->radioAsk->setChecked(true);
            break;
        case SiteSettingsManager::Deny:
            m_ui->radioDeny->setChecked(true);
            break;
        case SiteSettingsManager::Default:
            m_ui->radioDefault->setChecked(true);
            break;
        default:
            qWarning() << "Unknown permission" << permission;
            m_ui->radioDefault->setChecked(true);
    }

    m_ui->labelDefaultPermission->setText(mApp->siteSettingsManager()->getPermissionName(mApp->siteSettingsManager()->getDefaultPermission(m_option)));
}

SiteSettingsManager::Permission SiteInfoPermissionItem::permission() const
{
    if (m_ui->radioAllow->isChecked()) {
        return SiteSettingsManager::Allow;
    }
    else if (m_ui->radioAsk->isChecked()) {
        return SiteSettingsManager::Ask;
    }
    else if (m_ui->radioDeny->isChecked()) {
        return SiteSettingsManager::Deny;
    }
    else if (m_ui->radioDefault->isChecked()) {
        return SiteSettingsManager::Default;
    }
    else {
        qWarning() << "No permission is selected";
        return SiteSettingsManager::Default;
    }
}

SiteSettingsManager::PageOptions SiteInfoPermissionItem::option() const
{
    return m_option;
}
