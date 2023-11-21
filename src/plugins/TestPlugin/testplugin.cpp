/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "testplugin.h"
#include "testplugin_sidebar.h"
#include "browserwindow.h"
#include "webview.h"
#include "pluginproxy.h"
#include "mainapplication.h"
#include "sidebar.h"
#include "webhittestresult.h"
#include "../config.h"

#include <QMenu>
#include <QPushButton>

TestPlugin::TestPlugin()
    : QObject()
    , m_view(nullptr)
{
}

void TestPlugin::init(InitState state, const QString &settingsPath)
{
    qDebug() << __FUNCTION__ << "called";

    // This function is called right after plugin is loaded
    // it will be called even if we return false from testPlugin()
    // so it is recommended not to call any Falkon function here

    // Settings path is PROFILE/extensions (without trailign slash),
    // in this directory you can use global .ini file for QSettings
    // named "extensions.ini" or create new folder for your plugin
    // and save in it anything you want
    m_settingsPath = settingsPath;

    // State can be either StartupInitState or LateInitState, and it
    // indicates when the plugin have been loaded.
    // Currently, it can be from preferences, or automatically at startup.
    // Plugins are loaded before first BrowserWindow is created.
    Q_UNUSED(state)

    // Registering this plugin as a MousePressHandler.
    // Otherwise mousePress() function will never be called
    mApp->plugins()->registerAppEventHandler(PluginProxy::MousePressHandler, this);

    // Adding new sidebar into application
    m_sideBar = new TestPlugin_Sidebar(this);
    SideBarManager::addSidebar(QSL("testplugin-sidebar"), m_sideBar);
}

void TestPlugin::unload()
{
    qDebug() << __FUNCTION__ << "called";

    // This function will be called when unloading plugin
    // it will be also called if we return false from testPlugin()

    // Removing sidebar from application
    SideBarManager::removeSidebar(m_sideBar);
    delete m_sideBar;

    // Deleting settings dialog if opened
    delete m_settings.data();
}

bool TestPlugin::testPlugin()
{
    // This function is called right after init()
    // There should be some testing if plugin is loaded correctly
    // If this function returns false, plugin is automatically unloaded

    return (QString::fromLatin1(Qz::VERSION) == QLatin1String(FALKON_VERSION));
}

void TestPlugin::showSettings(QWidget* parent)
{
    // This function will be called from Preferences after clicking on Settings button.
    // Settings button will be enabled if PluginSpec.hasSettings == true

    if (!m_settings) {
        m_settings = new QDialog(parent);
        auto* b = new QPushButton(QSL("Example Plugin v0.0.1"));
        auto* closeButton = new QPushButton(tr("Close"));
        auto* label = new QLabel();
        label->setPixmap(QPixmap(QSL(":icons/other/about.svg")));

        auto* l = new QVBoxLayout(m_settings.data());
        l->addWidget(label);
        l->addWidget(b);
        l->addWidget(closeButton);
        m_settings.data()->setLayout(l);

        m_settings.data()->setAttribute(Qt::WA_DeleteOnClose);
        m_settings.data()->setWindowTitle(tr("Example Plugin Settings"));
        m_settings.data()->setWindowIcon(QIcon(QSL(":icons/falkon.svg")));
        connect(closeButton, SIGNAL(clicked()), m_settings.data(), SLOT(close()));
    }

    m_settings.data()->show();
    m_settings.data()->raise();
}

void TestPlugin::populateWebViewMenu(QMenu* menu, WebView* view, const WebHitTestResult &r)
{
    Q_UNUSED(r)

    // Called from WebView when creating context menu

    m_view = view;

    QString title;
    if (!r.imageUrl().isEmpty()) {
        title += QSL(" on image");
    }

    if (!r.linkUrl().isEmpty()) {
        title += QSL(" on link");
    }

    if (r.isContentEditable()) {
        title += QSL(" on input");
    }

    menu->addAction(tr("My first plugin action") + title, this, SLOT(actionSlot()));
}

bool TestPlugin::mousePress(Qz::ObjectName type, QObject* obj, QMouseEvent* event)
{
    qDebug() << "mousePress" << type << obj << event;

    // Returning false means, that we don't want to block propagating this event
    // Returning true may affect behaviour of Falkon, so make sure you know what
    // you are doing!
    return false;
}

void TestPlugin::actionSlot()
{
    QMessageBox::information(m_view, tr("Hello"), tr("First plugin action works :-)"));
}
