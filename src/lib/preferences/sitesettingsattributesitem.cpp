/*
 * Falkon - Permission item for web attributes for preferences
 * SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "sitesettingsattributesitem.h"
#include "ui_sitesettingsattributesitem.h"

#include "mainapplication.h"
#include "sitesettingsmanager.h"
#include "sitesettingsbrowsedialog.h"

SiteSettingsAttributesItem::SiteSettingsAttributesItem(const QWebEngineSettings::WebAttribute a_attribute, QWidget* parent)
    : QWidget(parent)
    , m_attribute(a_attribute)
    , m_ui(new Ui::SiteSettingsAttributesItem)
{
    m_ui->setupUi(this);
    m_ui->label->setText(mApp->siteSettingsManager()->getOptionName(m_attribute));

    connect(m_ui->browseButton, &QPushButton::clicked, this, &SiteSettingsAttributesItem::showBrowseDialog);
}

SiteSettingsAttributesItem::~SiteSettingsAttributesItem()
{
}

void SiteSettingsAttributesItem::showBrowseDialog()
{
    QString sqlColumn = mApp->siteSettingsManager()->webAttributeToSqlColumn(m_attribute);
    QString name = m_ui->label->text();

    auto* dialog = new SiteSettingsBrowseDialog(name, sqlColumn, this);
    dialog->hideAskButton();
    dialog->open();
}

QWebEngineSettings::WebAttribute SiteSettingsAttributesItem::attribute() const
{
    return m_attribute;
}
