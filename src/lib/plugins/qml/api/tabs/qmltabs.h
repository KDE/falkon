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
#pragma once

#include <QObject>
#include "mainapplication.h"
#include "qmltab.h"

/**
 * @brief The class exposing Tabs API to QML
 */
class QmlTabs : public QObject
{
    Q_OBJECT
public:
    explicit QmlTabs(QObject *parent = nullptr);
    /**
     * @brief Sets the current tab in a window
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing new current index
     *        - windowId:
     *          The id of window containing the tab
     * @return True if success, else false
     */
    Q_INVOKABLE bool setCurrentIndex(const QVariantMap &map);
    /**
     * @brief Sets the next tab as current tab
     * @param Integer representing the window
     * @return True if success, else false
     */
    Q_INVOKABLE bool nextTab(int windowId = -1);
    /**
     * @brief Sets the previous tab as current tab
     * @param Integer representing the window
     * @return True if success, else false
     */
    Q_INVOKABLE bool previousTab(int windowId = -1);
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
    Q_INVOKABLE bool moveTab(const QVariantMap &map);
    /**
     * @brief Pins a tab
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing the tab to be pinned
     *        - windowId:
     *          The id of window containing the tab
     * @return True if success, else false
     */
    Q_INVOKABLE bool pinTab(const QVariantMap &map);
    /**
     * @brief Un-pins a tab
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing the tab to be unpinned
     *        - windowId:
     *          The id of window containing the tab
     * @return True if success, else false
     */
    Q_INVOKABLE bool unpinTab(const QVariantMap &map);
    /**
     * @brief Detaches a tab
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing the tab to be detached
     *        - windowId:
     *          The id of window containing the tab
     * @return True if tab is detached, else false
     */
    Q_INVOKABLE bool detachTab(const QVariantMap &map);
    /**
     * @brief Duplicates a tab
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing the tab to duplicate
     *        - windowId:
     *          The id of window containing the tab
     * @return True if success, else false
     */
    Q_INVOKABLE bool duplicate(const QVariantMap &map);
    /**
     * @brief Close a tab
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing the tab to be closed
     *        - windowId:
     *          The id of window containing the tab
     * @return True if success, else false
     */
    Q_INVOKABLE bool closeTab(const QVariantMap &map);
    /**
     * @brief Reloads a tab
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing the tab to be reloaded
     *        - windowId:
     *          The id of window containing the tab
     * @return True if success, else false
     */
    Q_INVOKABLE bool reloadTab(const QVariantMap &map);
    /**
     * @brief Stops a tab
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing the tab to be stopped
     *        - windowId:
     *          The id of window containing the tab
     * @return True if success, else false
     */
    Q_INVOKABLE bool stopTab(const QVariantMap &map);
    /**
     * @brief Gets a tab
     * @param A JavaScript object containing
     *        - index:
     *          Integer representing the index of the tab
     *        - windowId:
     *          The id of window containing the tab
     * @return Tab of type [QmlTab](@ref QmlTab) if exists, else null
     */
    Q_INVOKABLE QmlTab *get(const QVariantMap &map) const;
    /**
     * @brief Get the normal tabs count in a window
     * @param Integer representing the window
     * @return Number of normal tabs in the window
     */
    Q_INVOKABLE int normalTabsCount(int windowId = -1) const;
    /**
     * @brief Get the pinned tabs count in a window
     * @param Integer representing the window
     * @return Number of pinned tabs in the window
     */
    Q_INVOKABLE int pinnedTabsCount(int windowId = -1) const;
    /**
     * @brief Gets all the tabs of a window
     * @param A JavaScript object containing
     *        - windowId:
     *          The id of window containing the tab
     *        - withPinned:
     *          Bool representing if the searched tab can be pinned
     * @return List of tabs, each of type [QmlTab](@ref QmlTab)
     */
    Q_INVOKABLE QList<QObject*> getAll(const QVariantMap &map = QVariantMap()) const;
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
    Q_INVOKABLE QList<QObject*> search(const QVariantMap &map);
    /**
     * @brief Adds a tab
     * @param A JavaScript object containing
     *        - url:
     *          String representing the url of the tab
     *        - windowId:
     *          The id of window containing the tab
     * @return True if the tab is added, else false
     */
    Q_INVOKABLE bool addTab(const QVariantMap &map);
Q_SIGNALS:
    /**
     * @brief The signal emitted when tabs in the tab widget are changed
     * @param window id representing the window in which the change occurs
     */
    void changed(int windowId);

    /**
     * @brief The signal emitted when a tab is inserted
     * @param A JavaScript object containing
     *        - index:
     *          The index of the inserted tab
     *        - windowId:
     *          The id of window in which the tab is inserted
     */
    void tabInserted(const QVariantMap &map);

    /**
     * @brief The signal emitted when a tab is removed
     * @param A JavaScript object containing
     *        - index:
     *          The index of the removed tab
     *        - windowId:
     *          The id of window in which the tab is removed
     */
    void tabRemoved(const QVariantMap &map);

    /**
     * @brief The signal emitted when a tab is moved
     * @param A JavaScript object containing
     *        - from:
     *          The initial index of the moved tab
     *        - to:
     *          The final index of the moved tab
     *        - windowId:
     *          The id of window in which the tab is moved
     */
    void tabMoved(const QVariantMap &map);
private:
    BrowserWindow *getWindow(const QVariantMap &map) const;
    BrowserWindow *getWindow(int windowId) const;
    void windowCreated(BrowserWindow *window);
};
