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

#include "sitesettingsview_controller.h"
#include "sitesettingsview_widget.h"

#include "sitesettingsmanager.h"
#include "mainapplication.h"
#include "webpage.h"
#include "webview.h"

#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTreeWidget>

SiteSettingsView_Controller::SiteSettingsView_Controller(QObject* parent)
    : SideBarInterface(parent)
{
}

QString SiteSettingsView_Controller::title() const
{
    return tr("Site Settings");
}

QAction* SiteSettingsView_Controller::createMenuAction()
{
    // The action must be parented to some object from plugin, otherwise
    // there may be a crash when unloading the plugin.

    auto* act = new QAction(tr("Site Settings View"), this);
    act->setCheckable(true);

    return act;
}

QWidget* SiteSettingsView_Controller::createSideBarWidget(BrowserWindow* mainWindow)
{
    auto *widget = new SiteSettingsView_Widget(mainWindow);
    m_widgets[mainWindow] = widget;
    return widget;
}

void SiteSettingsView_Controller::webPageCreated(WebPage* page)
{
    connect(page, &WebPage::loadFinished, this, [=]() {
        QHash<BrowserWindow*, QPointer<SiteSettingsView_Widget>>::iterator it;
        for (it = m_widgets.begin(); it != m_widgets.end(); ++it) {
            it.value()->loadFinished(page);
        }
    });
    connect(page->view(), &WebView::zoomLevelChanged, this, [=](){
        QHash<BrowserWindow*, QPointer<SiteSettingsView_Widget>>::iterator it;
        for (it = m_widgets.begin(); it != m_widgets.end(); ++it) {
            it.value()->loadFinished(page);
        }
    });
}

void SiteSettingsView_Controller::mainWindowDeleted(BrowserWindow* window)
{
    m_widgets.remove(window);
}
