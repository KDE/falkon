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

#ifndef SITEINFOPERMISSIONDEFAULTITEM_H
#define SITEINFOPERMISSIONDEFAULTITEM_H

#include "sitesettingsmanager.h"
#include <qwidget.h>
#include <QScopedPointer>

namespace Ui
{
class SiteInfoPermissionDefaultItem;
}

/**
 * @todo write docs
 */
class SiteInfoPermissionDefaultItem : public QWidget
{
    Q_OBJECT

public:
    explicit SiteInfoPermissionDefaultItem(const SiteSettingsManager::Permission &a_permission, QWidget* parent = nullptr);
    ~SiteInfoPermissionDefaultItem();

    bool hasOptionAsk() const;

    SiteSettingsManager::Permission permission() const;
    QString sqlColumn();

    void setAttribute(const QWebEngineSettings::WebAttribute& attribute);
    void setFeature(const QWebEnginePage::Feature &feature);
    void setOption(const SiteSettingsManager::PageOptions &option);

private:
    void setPermission(const SiteSettingsManager::Permission permission);
    void setHasOptionAsk(bool hasAsk);

    bool m_hasOptionAsk;
    QString m_sqlColumn;

    QScopedPointer<Ui::SiteInfoPermissionDefaultItem> m_ui;
};

#endif // SITEINFOPERMISSIONDEFAULTITEM_H
