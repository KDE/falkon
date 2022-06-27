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
#include "qml/qmlstaticdata.h"
#include <QQmlEngine>

QmlWindows::QmlWindows(QObject *parent)
    : QObject(parent)
{
    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, [this](BrowserWindow *window){
        QmlWindow *qmlWindow = QmlStaticData::instance().getWindow(window);
        Q_EMIT created(qmlWindow);
    });

    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, this, [this](BrowserWindow *window){
        QmlWindow *qmlWindow = QmlStaticData::instance().getWindow(window);
        Q_EMIT removed(qmlWindow);
    });
}

QmlWindow *QmlWindows::get(int id) const
{
    return QmlStaticData::instance().getWindow(getBrowserWindow(id));
}

QmlWindow *QmlWindows::getCurrent() const
{
    return QmlStaticData::instance().getWindow(mApp->getWindow());
}

QList<QObject*> QmlWindows::getAll() const
{
    QList<QObject*> list;
    const QList<BrowserWindow*> windows = mApp->windows();
    list.reserve(windows.size());
    for (BrowserWindow *window : windows) {
        list.append(QmlStaticData::instance().getWindow(window));
    }
    return list;
}

QmlWindow *QmlWindows::create(const QVariantMap &map) const
{
    const QUrl url = QUrl::fromEncoded(map.value(QSL("url")).toString().toUtf8());
    const Qz::BrowserWindowType type = Qz::BrowserWindowType(map.value(QSL("type"), QmlEnums::NewWindow).toInt());
    BrowserWindow *window = mApp->createWindow(type, url);
    return QmlStaticData::instance().getWindow(window);
}

void QmlWindows::remove(int windowId) const
{
    BrowserWindow *window = getBrowserWindow(windowId);
    window->close();
}

BrowserWindow *QmlWindows::getBrowserWindow(int windowId) const
{
    const QList<BrowserWindow*> windows = mApp->windows();
    for (BrowserWindow *window : windows) {
        if (QmlStaticData::instance().windowIdHash().value(window, -1) == windowId) {
            return window;
        }
    }

    qWarning() << "Unable to get window with id:" << windowId;
    return nullptr;
}
