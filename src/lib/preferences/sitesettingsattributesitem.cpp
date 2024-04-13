/*
 * Falkon - Permission item for web attributes for preferences
 * SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sitesettingsattributesitem.h"
#include "ui_sitesettingsattributesitem.h"

#include "mainapplication.h"
#include "sitesettingsmanager.h"

SiteSettingsAttributesItem::SiteSettingsAttributesItem(const QWebEngineSettings::WebAttribute a_attribute, QWidget* parent)
    : QWidget(parent)
    , m_attribute(a_attribute)
    , m_ui(new Ui::SiteSettingsAttributesItem)
{
    m_ui->setupUi(this);
    m_ui->label->setText(mApp->siteSettingsManager()->getOptionName(m_attribute));
}

SiteSettingsAttributesItem::~SiteSettingsAttributesItem()
{
}

QWebEngineSettings::WebAttribute SiteSettingsAttributesItem::attribute() const
{
    return m_attribute;
}
