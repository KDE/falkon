/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmlwindows.h"
#include "mainapplication.h"
#include "pluginproxy.h"

Q_GLOBAL_STATIC(QmlWindowData, windowData)

QmlWindows::QmlWindows(QObject *parent)
    : QObject(parent)
{
    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, [this](BrowserWindow *window){
        QmlWindow *qmlWindow = windowData->get(window);
        emit created(qmlWindow);
    });

    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, this, [this](BrowserWindow *window){
        QmlWindow *qmlWindow = windowData->get(window);
        emit removed(qmlWindow);
    });
}

QmlWindow *QmlWindows::get(const QVariantMap &map) const
{
    if (!map.contains(QSL("id"))) {
        qWarning() << "Unable to get window:" << "id not defined";
        return nullptr;
    }

    const int id = map.value(QSL("id")).toInt();
    return windowData->get(getBrowserWindow(id));
}

QmlWindow *QmlWindows::getCurrent() const
{
    return windowData->get(mApp->getWindow());
}

QList<QObject*> QmlWindows::getAll() const
{
    QList<QObject*> list;
    for (BrowserWindow *window : mApp->windows()) {
        list.append(windowData->get(window));
    }
    return list;
}

QmlWindow *QmlWindows::create(const QVariantMap &map) const
{
    const QUrl url = QUrl::fromEncoded(map.value(QSL("url")).toString().toUtf8());
    const Qz::BrowserWindowType type = Qz::BrowserWindowType(map.value(QSL("type"), QmlWindowType::NewWindow).toInt());
    BrowserWindow *window = mApp->createWindow(type, url);
    return windowData->get(window);
}

void QmlWindows::remove(int windowId) const
{
    BrowserWindow *window = getBrowserWindow(windowId);
    window->close();
}

BrowserWindow *QmlWindows::getBrowserWindow(int windowId) const
{
    for (BrowserWindow *window : mApp->windows()) {
        if (mApp->windowIdHash().value(window) == windowId) {
            return window;
        }
    }

    qWarning() << "Unable to get window with given id";
    return nullptr;
}
