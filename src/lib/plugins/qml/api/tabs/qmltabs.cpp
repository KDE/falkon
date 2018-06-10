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

Q_GLOBAL_STATIC(QmlTabData, tabData)

QmlTabs::QmlTabs(QObject *parent)
    : QObject(parent)
{
    for (BrowserWindow *window : mApp->windows()) {
        windowCreated(window);
    }

    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &QmlTabs::windowCreated);
}

/**
 * @brief Sets the current tab in a window
 * @param A JavaScript object containing
 *        - index:
 *          Integer representing new current index
 *        - windowId:
 *          The id of window containing the tab
 * @return True if success, else false
 */
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

/**
 * @brief Sets the next tab as current tab
 * @param Integer representing the window
 * @return True if success, else false
 */
bool QmlTabs::nextTab(int windowId)
{
    const auto window = getWindow(windowId);
    if (!window) {
        return false;
    }
    window->tabWidget()->nextTab();
    return true;
}

/**
 * @brief Sets the prvious tab as current tab
 * @param Integer representing the window
 * @return True if success, else false
 */
bool QmlTabs::previousTab(int windowId)
{
    const auto window = getWindow(windowId);
    if (!window) {
        return false;
    }
    window->tabWidget()->previousTab();
    return true;
}

/**
 * @brief Moves a tab
 * @param A JavaScript object containing
 *        - from:
 *          The initial index of the tab
 *        - to:
 *          The final index of the tab
 *        - windowId:
 *          The id of window containing the tab
 * @return True if tab is moved, else false
 */
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

/**
 * @brief Pins a tab
 * @param A JavaScript object containing
 *        - index:
 *          Integer representing the tab to be pinned
 *        - windowId:
 *          The id of window containing the tab
 * @return True if success, else false
 */
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

/**
 * @brief Un-pins a tab
 * @param A JavaScript object containing
 *        - index:
 *          Integer representing the tab to be unpinned
 *        - windowId:
 *          The id of window containing the tab
 * @return True if success, else false
 */
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

/**
 * @brief Detaches a tab
 * @param A JavaScript object containing
 *        - index:
 *          Integer representing the tab to be detached
 *        - windowId:
 *          The id of window containing the tab
 * @return True if tab is detached, else false
 */
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

/**
 * @brief Duplicates a tab
 * @param A JavaScript object containing
 *        - index:
 *          Integer representing the tab to duplicate
 *        - windowId:
 *          The id of window containing the tab
 * @return True if success, else false
 */
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

/**
 * @brief Close a tab
 * @param A JavaScript object containing
 *        - index:
 *          Integer representing the tab to be closed
 *        - windowId:
 *          The id of window containing the tab
 * @return True if success, else false
 */
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

/**
 * @brief Reloads a tab
 * @param A JavaScript object containing
 *        - index:
 *          Integer representing the tab to be reloaded
 *        - windowId:
 *          The id of window containing the tab
 * @return True if success, else false
 */
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

/**
 * @brief Stops a tab
 * @param A JavaScript object containing
 *        - index:
 *          Integer representing the tab to be stoped
 *        - windowId:
 *          The id of window containing the tab
 * @return True if success, else false
 */
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

/**
 * @brief Gets a tab
 * @param A JavaScript object contining
 *        - index:
 *          Integer representign the index of the tab
 *        - windowId:
 *          The id of window containing the tab
 * @return Tab of type [QmlTab](@ref QmlTab) if exists, else null
 */
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
    return tabData->get(webTab);
}

/**
 * @brief Get the normal tabs count in a window
 * @param Integer representing the window
 * @return Number of normal tabs in the window
 */
int QmlTabs::normalTabsCount(int windowId) const
{
    const auto window = getWindow(windowId);
    if (!window) {
        return -1;
    }
    return window->tabWidget()->normalTabsCount();
}

/**
 * @brief Get the pinned tabs count in a window
 * @param Integer representing the window
 * @return Number of pinned tabs in the window
 */
int QmlTabs::pinnedTabsCount(int windowId) const
{
    const auto window = getWindow(windowId);
    if (!window) {
        return -1;
    }
    return window->tabWidget()->pinnedTabsCount();
}

/**
 * @brief Gets all the tabs of a window
 * @param A JavaScript object containing
 *        - windowId:
 *          The id of window containing the tab
 *        - withPinned:
 *          Bool representing if the searched tab can be pinned
 * @return List of tabs, each of type [QmlTab](@ref QmlTab)
 */
QList<QObject*> QmlTabs::getAll(const QVariantMap &map) const
{
    const auto window = getWindow(map);
    if (!window) {
        return QList<QObject*>();
    }

    const bool withPinned = map.value(QSL("withPinned")).toBool();
    const auto tabList = window->tabWidget()->allTabs(withPinned);

    QList<QObject*> list;
    for (auto tab : tabList) {
        list.append(tabData->get(tab));
    }

    return list;
}

/**
 * @brief Searches tabs against a criteria
 * @param A JavaScript object containing
 *        - title:
 *          String representing the title to be searched
 *        - url:
 *          String representing the url to be searched
 *        - withPinned:
 *          Bool representing if the searched tab can be pinned
 * @return List of tabs, each of type [QmlTab](@ref QmlTab), which are
 *         matched against the criteria
 */
QList<QObject*> QmlTabs::search(const QVariantMap &map)
{
    const QString title = map.value(QSL("title")).toString();
    const QString url = map.value(QSL("url")).toString();
    const bool withPinned = map.value(QSL("withPinned")).toBool();
    QList<QObject*> list;
    foreach (BrowserWindow *window, mApp->windows()) {
        foreach (WebTab *webTab, window->tabWidget()->allTabs(withPinned)) {
            if (webTab->title().contains(title, Qt::CaseInsensitive)
                    || QString::fromUtf8(webTab->url().toEncoded()).contains(url, Qt::CaseInsensitive)) {
                list.append(tabData->get(webTab));
            }
        }
    }
    return list;
}

/**
 * @brief Adds a tab
 * @param A JavaScript object containing
 *        - url:
 *          String representing the url of the tab
 *        - windowId:
 *          The id of window containing the tab
 * @return True if the tab is added, else false
 */
bool QmlTabs::addTab(const QVariantMap &map)
{
    const QString urlString = map.value(QSL("url")).toString();
    const auto window = getWindow(map);
    if (!window) {
        qDebug() << "Unable to add tab:" << "window not found";
        return false;
    }
    LoadRequest req;
    req.setUrl(QUrl::fromEncoded(urlString.toUtf8()));
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

    for(BrowserWindow *window : mApp->windowIdHash().keys()) {
        if (mApp->windowIdHash().value(window) == windowId) {
            return window;
        }
    }

    qWarning() << "Unable to get window with given windowId";
    return nullptr;
}

void QmlTabs::windowCreated(BrowserWindow *window)
{
    const int windowId = mApp->windowIdHash().value(window);

    connect(window->tabWidget(), &TabWidget::changed, this, [this, windowId]{
        emit changed(windowId);
    });

    connect(window->tabWidget(), &TabWidget::tabInserted, this, [this, windowId](int index){
        QVariantMap map;
        map.insert(QSL("windowId"), windowId);
        map.insert(QSL("index"), index);
        emit tabInserted(map);
    });

    connect(window->tabWidget(), &TabWidget::tabRemoved, this, [this, windowId](int index){
        QVariantMap map;
        map.insert(QSL("windowId"), windowId);
        map.insert(QSL("index"), index);
        emit tabRemoved(map);
    });

    connect(window->tabWidget(), &TabWidget::tabMoved, this, [this, windowId](int from, int to){
        QVariantMap map;
        map.insert(QSL("windowId"), windowId);
        map.insert(QSL("from"), from);
        map.insert(QSL("to"), to);
        emit tabMoved(map);
    });
}
