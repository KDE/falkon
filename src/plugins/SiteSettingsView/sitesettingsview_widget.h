/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2024 Juraj Oravec <jurajoravec@mailo.com>
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

#ifndef SITESETTINGSVIEW_WIDGET_H
#define SITESETTINGSVIEW_WIDGET_H

#include "sitesettingsview_plugin.h"
#include "sitesettingsmanager.h"

#include <QWidget>
#include <QWebEngineSettings>

class QTreeWidget;
class QTreeWidgetItem;

class BrowserWindow;

class SiteSettingsView_Widget : public QWidget
{
    Q_OBJECT
public:
    explicit SiteSettingsView_Widget(BrowserWindow *window);

    void loadFinished(WebPage* page);

private Q_SLOTS:
    void updateData(int index = 0);

private:
    QTreeWidget *m_attributes;
    QTreeWidget *m_features;
    QTreeWidget *m_options;
    BrowserWindow *m_window;
    QBrush m_brushGreen;
    QBrush m_brushYellow;
    QBrush m_brushOrange;
    QBrush m_brushRed;
    QBrush m_brushBlue;

    QLabel *m_isWebValue;

    QList<SiteSettingsManager::PageOptions> m_availableOptions;

    QHash<QWebEngineSettings::WebAttribute, QTreeWidgetItem*> m_attributeItems;
    QHash<QWebEnginePage::Feature, QTreeWidgetItem*> m_featureItems;
    QHash<SiteSettingsManager::PageOptions, QTreeWidgetItem*> m_optionsItems;

    QBrush permissionColor(SiteSettingsManager::Permission permission);
};

#endif /* SITESETTINGSVIEW_WIDGET_H */
