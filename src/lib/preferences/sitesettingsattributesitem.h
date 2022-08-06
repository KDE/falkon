/*
 * Falkon - Permission item for web attributes for preferences
 * SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SITESETTINGSATTRIBUTESITEM_H
#define SITESETTINGSATTRIBUTESITEM_H

#include <QWidget>
#include <QScopedPointer>
#include <QWebEngineSettings>

namespace Ui
{
class SiteSettingsAttributesItem;
}

class SiteSettingsAttributesItem : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QWebEngineSettings::WebAttribute m_attribute READ attribute)

public:
    SiteSettingsAttributesItem(const QWebEngineSettings::WebAttribute a_attribute, QWidget* parent = nullptr);
    ~SiteSettingsAttributesItem();

    QWebEngineSettings::WebAttribute attribute() const;

public Q_SLOTS:
    void showBrowseDialog();

private:
    QWebEngineSettings::WebAttribute m_attribute;

private:
    QScopedPointer<Ui::SiteSettingsAttributesItem> m_ui;
};

#endif // SITESETTINGSATTRIBUTESITEM_H
