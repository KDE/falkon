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

#ifndef SITEINFOPERMISSIONITEM_H
#define SITEINFOPERMISSIONITEM_H

#include "sitesettingsmanager.h"
#include <qwidget.h>
#include <QScopedPointer>

namespace Ui
{
class SiteInfoPermissionItem;
}

/**
 * @todo write docs
 */
class SiteInfoPermissionItem : public QWidget
{
    Q_OBJECT

public:
    explicit SiteInfoPermissionItem(const SiteSettingsManager::PageOptions &a_option, const SiteSettingsManager::Permission &a_permission, QWidget* parent = nullptr);
    ~SiteInfoPermissionItem();

    bool hasOptionAsk() const;
    bool hasOptionDefault() const;

    SiteSettingsManager::Permission permission() const;
    SiteSettingsManager::PageOptions option() const;

    void setHasOptionAsk(bool hasAsk);
    void setHasOptionDefault(bool hasDefault);
    void setPermission(SiteSettingsManager::Permission permission);

private:
    bool m_hasOptionAsk;
    bool m_hasOptionDefault;
    SiteSettingsManager::PageOptions m_option;

private:
    QScopedPointer<Ui::SiteInfoPermissionItem> m_ui;
};

#endif // SITEINFOPERMISSIONITEM_H
