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
#ifndef SITESETTINGSVIEW_CONTROLLER_H
#define SITESETTINGSVIEW_CONTROLLER_H

#include "sidebarinterface.h"

class WebPage;
class SiteSettingsView;
class SiteSettingsView_Widget;

class SiteSettingsView_Controller : public SideBarInterface
{
    Q_OBJECT
public:
    explicit SiteSettingsView_Controller(QObject* parent = nullptr);

    QString title() const override;
    QAction* createMenuAction() override;

    QWidget* createSideBarWidget(BrowserWindow* mainWindow) override;

public Q_SLOTS:
    void webPageCreated(WebPage* page);
    void mainWindowDeleted(BrowserWindow* window);

private:
    QHash<BrowserWindow*, QPointer<SiteSettingsView_Widget>> m_widgets;
};

#endif // SITESETTINGSVIEW_CONTROLLER_H
