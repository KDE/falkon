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
#include "qmltabs.h"
#include "tabwidget.h"
#include "pluginproxy.h"

QmlTabs::QmlTabs(QObject *parent)
    : QObject(parent)
{
    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, [this](BrowserWindow *window){
        // FIXME: make it more efficient
        for (int i = 0; i < mApp->windowCount(); i++) {
            windowIdHash[mApp->windows().at(i)] = i;
        }

        connect(window->tabWidget(), &TabWidget::changed, this, [this, window]{
            QVariantMap map;
            int windowId = windowIdHash.value(window);
            map.insert(QSL("windowId"), windowId);
            emit changed(map);
        });

        connect(window->tabWidget(), &TabWidget::tabInserted, this, [this, window](int index){
            QVariantMap map;
            int windowId = windowIdHash.value(window);
            map.insert(QSL("windowId"), windowId);
            map.insert(QSL("index"), index);
            emit tabInserted(map);
        });

        connect(window->tabWidget(), &TabWidget::tabRemoved, this, [this, window](int index){
            QVariantMap map;
            int windowId = windowIdHash.value(window);
            map.insert(QSL("windowId"), windowId);
            map.insert(QSL("index"), index);
            emit tabRemoved(map);
        });
    });

    connect(mApp->plugins(), &PluginProxy::mainWindowDeleted, this, [this](BrowserWindow *window){
        // FIXME: make it more efficient
        windowIdHash.remove(window);
        // If the window is not destroyed (which is almost always),
        // then remove bias(= 1) from the index of window in the list
        // for the correct index
        int bias = 0;
        for (int i = 0; i < mApp->windowCount(); i++) {
            BrowserWindow *windowAtI = mApp->windows().at(i);
            if (windowAtI == window) {
                bias = 1;
                continue;
            }
            windowIdHash[windowAtI] = i - bias;
        }
    });
}

void QmlTabs::setCurrentIndex(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to set current index:" << "index not defined";
        return;
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->setCurrentIndex(index);
}

void QmlTabs::nextTab(const QVariantMap &map)
{
    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->nextTab();
}

void QmlTabs::previousTab(const QVariantMap &map)
{
    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->previousTab();
}

void QmlTabs::moveTab(const QVariantMap &map)
{
    if (!map.contains(QSL("from"))) {
        qWarning() << "Unable to move tab:" << "from not defined";
        return;
    }
    if (!map.contains(QSL("to"))) {
        qWarning() << "Unable to move tab:" << "to not defined";
        return;
    }

    int from = map.value(QSL("from")).toInt();
    int to = map.value(QSL("to")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->moveTab(from, to);
}

bool QmlTabs::pinTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to pin tab:" << "index not defined";
        return false;
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }

    if (window->tabWidget()->webTab(index)->isPinned()) {
        return false;
    }

    window->tabWidget()->pinUnPinTab(index);
    return true;
}

bool QmlTabs::unpinTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to unpin tab:" << "index not defined";
        return false;
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }

    if (!window->tabWidget()->webTab(index)->isPinned()) {
        return false;
    }

    window->tabWidget()->pinUnPinTab(index);
    return true;
}

void QmlTabs::detachTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to detatch tab:" << "index not defined";
        return;
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->detachTab(index);
}

void QmlTabs::duplicate(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to duplicate:" << "index not defined";
        return;
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->duplicateTab(index);
}

void QmlTabs::closeTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to close tab:" << "index not defined";
        return;
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->closeTab(index);
}

void QmlTabs::reloadTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to reload tab:" << "index not defined";
        return;
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->reloadTab(index);
}

void QmlTabs::stopTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to close tab:" << "index not defined";
        return;
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return;
    }
    window->tabWidget()->stopTab(index);
}

QmlTab *QmlTabs::get(const QVariantMap &map) const
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to set current index:" << "index not defined";
        return new QmlTab();
    }

    int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return new QmlTab();
    }
    const auto webTab = window->tabWidget()->webTab(index);
    return new QmlTab(webTab);
}

int QmlTabs::normalTabsCount(const QVariantMap &map) const
{
    const auto window = getWindow(map);
    if (!window) {
        return -1;
    }
    return window->tabWidget()->normalTabsCount();
}

int QmlTabs::pinnedTabsCount(const QVariantMap &map) const
{
    const auto window = getWindow(map);
    if (!window) {
        return -1;
    }
    return window->tabWidget()->pinnedTabsCount();
}

QList<QObject*> QmlTabs::getAll(const QVariantMap &map) const
{
    const auto window = getWindow(map);
    if (!window) {
        return QList<QObject*>();
    }

    bool withPinned = map.value(QSL("withPinned")).toBool();
    const auto tabList = window->tabWidget()->allTabs(withPinned);

    QList<QObject*> list;
    for (const auto tab : tabList) {
        list.append(new QmlTab(tab));
    }

    return list;
}

BrowserWindow *QmlTabs::getWindow(const QVariantMap &map) const
{
    int windowId = map.value(QSL("windowId"), -1).toInt();

    if (windowId < -1 || windowId >= mApp->windowCount()) {
        qWarning() << "Unable to get window" << "windowId is out of bounds";
        return nullptr;
    }

    BrowserWindow *window;
    if (windowId == -1) {
        window = mApp->getWindow();
    } else {
        window = mApp->windows().at(windowId);
    }

    return window;
}
