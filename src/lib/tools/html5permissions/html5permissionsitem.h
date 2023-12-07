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
#ifndef HTML5PERMISSIONSITEM_H
#define HTML5PERMISSIONSITEM_H


#include "qzcommon.h"
#include "sitesettingsmanager.h"
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWidget>

namespace Ui
{
class HTML5PermissionsItem;
}

class FALKON_EXPORT HTML5PermissionsItem : public QWidget
{
    Q_OBJECT
public:
    explicit HTML5PermissionsItem(const QWebEnginePage::Feature &feature, const SiteSettingsManager::Permission &permission, QWidget* parent = nullptr);
    ~HTML5PermissionsItem();

    QWebEnginePage::Feature getFeature() const;
    SiteSettingsManager::Permission getPermission() const;

private Q_SLOTS:
    void permissionIndexChanged();

private:
    void setLabel();
    void setCombo();

    Ui::HTML5PermissionsItem* ui;
    QWebEnginePage::Feature m_feature;
    SiteSettingsManager::Permission m_permission;
};

#endif // HTML5PERMISSIONSITEM_H
