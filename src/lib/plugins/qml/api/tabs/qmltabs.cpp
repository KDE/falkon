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
#include "qml/qmlstaticdata.h"
#include <QQmlEngine>

QmlTabs::QmlTabs(QObject *parent)
    : QObject(parent)
{
    const QList<BrowserWindow*> windows = mApp->windows();
    for (BrowserWindow *window : windows) {
        windowCreated(window);
    }

    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &QmlTabs::windowCreated);
}

bool QmlTabs::setCurrentIndex(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to set current index:" << "index not defined";
        return false;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }
    window->tabWidget()->setCurrentIndex(index);
    return true;
}

bool QmlTabs::nextTab(int windowId)
{
    const auto window = getWindow(windowId);
    if (!window) {
        return false;
    }
    window->tabWidget()->nextTab();
    return true;
}

bool QmlTabs::previousTab(int windowId)
{
    const auto window = getWindow(windowId);
    if (!window) {
        return false;
    }
    window->tabWidget()->previousTab();
    return true;
}

bool QmlTabs::moveTab(const QVariantMap &map)
{
    if (!map.contains(QSL("from"))) {
        qWarning() << "Unable to move tab:" << "from not defined";
        return false;
    }
    if (!map.contains(QSL("to"))) {
        qWarning() << "Unable to move tab:" << "to not defined";
        return false;
    }

    const int from = map.value(QSL("from")).toInt();
    const int to = map.value(QSL("to")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }
    window->tabWidget()->moveTab(from, to);
    return true;
}

bool QmlTabs::pinTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to pin tab:" << "index not defined";
        return false;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }

    WebTab *webTab = window->tabWidget()->webTab(index);

    if (webTab->isPinned()) {
        return false;
    }

    webTab->togglePinned();
    return true;
}

bool QmlTabs::unpinTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to unpin tab:" << "index not defined";
        return false;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }

    WebTab *webTab = window->tabWidget()->webTab(index);

    if (!webTab->isPinned()) {
        return false;
    }

    webTab->togglePinned();
    return true;
}

bool QmlTabs::detachTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to detatch tab:" << "index not defined";
        return false;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }
    window->tabWidget()->detachTab(index);
    return true;
}

bool QmlTabs::duplicate(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to duplicate:" << "index not defined";
        return false;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }
    window->tabWidget()->duplicateTab(index);
    return true;
}

bool QmlTabs::closeTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to close tab:" << "index not defined";
        return false;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }
    window->tabWidget()->closeTab(index);
    return true;
}

bool QmlTabs::reloadTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to reload tab:" << "index not defined";
        return false;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }
    window->tabWidget()->reloadTab(index);
    return true;
}

bool QmlTabs::stopTab(const QVariantMap &map)
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to close tab:" << "index not defined";
        return false;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return false;
    }
    window->tabWidget()->stopTab(index);
    return true;
}

QmlTab *QmlTabs::get(const QVariantMap &map) const
{
    if (!map.contains(QSL("index"))) {
        qWarning() << "Unable to set current index:" << "index not defined";
        return nullptr;
    }

    const int index = map.value(QSL("index")).toInt();

    const auto window = getWindow(map);
    if (!window) {
        return nullptr;
    }
    const auto webTab = window->tabWidget()->webTab(index);
    return QmlStaticData::instance().getTab(webTab);
}

int QmlTabs::normalTabsCount(int windowId) const
{
    const auto window = getWindow(windowId);
    if (!window) {
        return -1;
    }
    return window->tabWidget()->normalTabsCount();
}

int QmlTabs::pinnedTabsCount(int windowId) const
{
    const auto window = getWindow(windowId);
    if (!window) {
        return -1;
    }
    return window->tabWidget()->pinnedTabsCount();
}

QList<QObject*> QmlTabs::getAll(const QVariantMap &map) const
{
    const auto window = getWindow(map);
    if (!window) {
        return {};
    }

    const bool withPinned = map.value(QSL("withPinned")).toBool();
    const QList<WebTab*> tabList = window->tabWidget()->allTabs(withPinned);

    QList<QObject*> list;
    list.reserve(tabList.size());
    for (WebTab *tab : tabList) {
        list.append(QmlStaticData::instance().getTab(tab));
    }

    return list;
}

QList<QObject*> QmlTabs::search(const QVariantMap &map)
{
    const QString title = map.value(QSL("title")).toString();
    const QString url = map.value(QSL("url")).toString();
    const bool withPinned = map.value(QSL("withPinned")).toBool();
    QList<QObject*> list;
    for (BrowserWindow *window : mApp->windows()) {
        for (WebTab *webTab : window->tabWidget()->allTabs(withPinned)) {
            if (webTab->title().contains(title, Qt::CaseInsensitive)
                    || QString::fromUtf8(webTab->url().toEncoded()).contains(url, Qt::CaseInsensitive)) {
                list.append(QmlStaticData::instance().getTab(webTab));
            }
        }
    }
    return list;
}

bool QmlTabs::addTab(const QVariantMap &map)
{
    const QString urlString = map.value(QSL("url")).toString();
    const auto window = getWindow(map);
    if (!window) {
        qDebug() << "Unable to add tab:" << "window not found";
        return false;
    }
    LoadRequest req(QUrl::fromEncoded(urlString.toUtf8()));
    const int ret = window->tabWidget()->addView(req);
    return ret != -1 ? true : false;
}

BrowserWindow *QmlTabs::getWindow(const QVariantMap &map) const
{
    const int windowId = map.value(QSL("windowId"), -1).toInt();
    return getWindow(windowId);
}

BrowserWindow *QmlTabs::getWindow(int windowId) const
{
    if (windowId == -1) {
        return mApp->getWindow();
    }

    auto windowIdHash = QmlStaticData::instance().windowIdHash();
    for (auto it = windowIdHash.cbegin(); it != windowIdHash.cend(); it++) {
        BrowserWindow *window = it.key();
        if (QmlStaticData::instance().windowIdHash().value(window, -1) == windowId) {
            return window;
        }
    }
    qWarning() << "Unable to get window with given windowId";
    return nullptr;
}

void QmlTabs::windowCreated(BrowserWindow *window)
{
    const int windowId = QmlStaticData::instance().windowIdHash().value(window, -1);

    connect(window->tabWidget(), &TabWidget::changed, this, [this, windowId]{
        Q_EMIT changed(windowId);
    });

    connect(window->tabWidget(), &TabWidget::tabInserted, this, [this, windowId](int index){
        QVariantMap map;
        map.insert(QSL("windowId"), windowId);
        map.insert(QSL("index"), index);
        Q_EMIT tabInserted(map);
    });

    connect(window->tabWidget(), &TabWidget::tabRemoved, this, [this, windowId](int index){
        QVariantMap map;
        map.insert(QSL("windowId"), windowId);
        map.insert(QSL("index"), index);
        Q_EMIT tabRemoved(map);
    });

    connect(window->tabWidget(), &TabWidget::tabMoved, this, [this, windowId](int from, int to){
        QVariantMap map;
        map.insert(QSL("windowId"), windowId);
        map.insert(QSL("from"), from);
        map.insert(QSL("to"), to);
        Q_EMIT tabMoved(map);
    });
}
