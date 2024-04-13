/*
 *  Falkon - Permission item for HTML5 features for preferences
 *  SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "sitesettingshtml5item.h"
#include "ui_sitesettingshtml5item.h"

#include "mainapplication.h"
#include "sitesettingsmanager.h"

SiteSettingsHtml5Item::SiteSettingsHtml5Item(const QWebEnginePage::Feature a_feature, QWidget* parent)
    : QWidget(parent)
    , m_feature(a_feature)
    , m_ui(new Ui::SiteSettingsHtml5Item)
{
    m_ui->setupUi(this);

    m_ui->defaultPermissionCombo->addItem(
        mApp->siteSettingsManager()->getPermissionName(SiteSettingsManager::Ask),
                                   SiteSettingsManager::Ask
    );
    m_ui->defaultPermissionCombo->addItem(
        mApp->siteSettingsManager()->getPermissionName(SiteSettingsManager::Allow),
                                   SiteSettingsManager::Allow
    );
    m_ui->defaultPermissionCombo->addItem(
        mApp->siteSettingsManager()->getPermissionName(SiteSettingsManager::Deny),
                                   SiteSettingsManager::Deny
    );

    setPermission();
    m_ui->label->setText(mApp->siteSettingsManager()->getOptionName(m_feature));
}

SiteSettingsHtml5Item::~SiteSettingsHtml5Item()
{
}

QWebEnginePage::Feature SiteSettingsHtml5Item::feature() const
{
    return m_feature;
}

SiteSettingsManager::Permission SiteSettingsHtml5Item::permission() const
{
    QVariant data = m_ui->defaultPermissionCombo->currentData();
    return data.value<SiteSettingsManager::Permission>();
}

void SiteSettingsHtml5Item::setPermission()
{
    auto defaultPermission = mApp->siteSettingsManager()->getDefaultPermission(m_feature);
    int index = m_ui->defaultPermissionCombo->findData(defaultPermission);

    if (index == -1) {
        qWarning() << "Unknown permission" << defaultPermission;
        return;
    }

    m_ui->defaultPermissionCombo->setCurrentIndex(index);
}
