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

#include "siteinfopermissiondefaultitem.h"
#include "ui_siteinfopermissiondefaultitem.h"
#include "mainapplication.h"
#include "sitesettingsmanager.h"


SiteInfoPermissionDefaultItem::SiteInfoPermissionDefaultItem(const SiteSettingsManager::Permission& a_permission, QWidget* parent)
: QWidget(parent)
, m_ui(new Ui::SiteInfoPermissionDefaultItem())
, m_hasOptionAsk(true)
{
    m_ui->setupUi(this);

    m_ui->permissionCombo->addItem(
        mApp->siteSettingsManager()->getPermissionName(SiteSettingsManager::Ask),
        SiteSettingsManager::Ask
    );
    m_ui->permissionCombo->addItem(
        mApp->siteSettingsManager()->getPermissionName(SiteSettingsManager::Allow),
        SiteSettingsManager::Allow
    );
    m_ui->permissionCombo->addItem(
        mApp->siteSettingsManager()->getPermissionName(SiteSettingsManager::Deny),
        SiteSettingsManager::Deny
    );

    setPermission(a_permission);

    if (mApp->isPrivate()) {
        m_ui->permissionCombo->setEnabled(false);
    }
}

SiteInfoPermissionDefaultItem::~SiteInfoPermissionDefaultItem()
{
}

bool SiteInfoPermissionDefaultItem::hasOptionAsk() const
{
    return m_hasOptionAsk;
}

void SiteInfoPermissionDefaultItem::setHasOptionAsk(bool hasOptionAsk)
{
    if (m_hasOptionAsk == hasOptionAsk) {
        return;
    }

    m_hasOptionAsk = hasOptionAsk;
    int index = m_ui->permissionCombo->findData(SiteSettingsManager::Ask);
    if (index != -1) {
        m_ui->permissionCombo->removeItem(index);
    }
}

void SiteInfoPermissionDefaultItem::setPermission(const SiteSettingsManager::Permission permission)
{
    int index = m_ui->permissionCombo->findData(permission);

    if (index == -1) {
        qWarning() << "Unknown permission" << permission;
        return;
    }

    m_ui->permissionCombo->setCurrentIndex(index);
}

SiteSettingsManager::Permission SiteInfoPermissionDefaultItem::permission() const
{
    QVariant data = m_ui->permissionCombo->currentData();
    return data.value<SiteSettingsManager::Permission>();
}

QString SiteInfoPermissionDefaultItem::sqlColumn()
{
    return m_sqlColumn;
}

void SiteInfoPermissionDefaultItem::setAttribute(const QWebEngineSettings::WebAttribute &attribute)
{
    m_sqlColumn = mApp->siteSettingsManager()->webAttributeToSqlColumn(attribute);
    m_ui->label->setText(mApp->siteSettingsManager()->getOptionName(attribute));
    setHasOptionAsk(false);
}

void SiteInfoPermissionDefaultItem::setFeature(const QWebEnginePage::Feature& feature)
{
    m_sqlColumn = mApp->siteSettingsManager()->featureToSqlColumn(feature);
    m_ui->label->setText(mApp->siteSettingsManager()->getOptionName(feature));
}

void SiteInfoPermissionDefaultItem::setOption(const SiteSettingsManager::PageOptions& option)
{
    m_sqlColumn = mApp->siteSettingsManager()->optionToSqlColumn(option);
    m_ui->label->setText(mApp->siteSettingsManager()->getOptionName(option));
    setHasOptionAsk(false);
}
