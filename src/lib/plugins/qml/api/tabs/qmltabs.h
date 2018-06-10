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
    Q_INVOKABLE bool setCurrentIndex(const QVariantMap &map);
    Q_INVOKABLE bool nextTab(int windowId = -1);
    Q_INVOKABLE bool previousTab(int windowId = -1);
    Q_INVOKABLE bool moveTab(const QVariantMap &map);
    Q_INVOKABLE bool pinTab(const QVariantMap &map);
    Q_INVOKABLE bool unpinTab(const QVariantMap &map);
    Q_INVOKABLE bool detachTab(const QVariantMap &map);
    Q_INVOKABLE bool duplicate(const QVariantMap &map);
    Q_INVOKABLE bool closeTab(const QVariantMap &map);
    Q_INVOKABLE bool reloadTab(const QVariantMap &map);
    Q_INVOKABLE bool stopTab(const QVariantMap &map);
    Q_INVOKABLE QmlTab *get(const QVariantMap &map) const;
    Q_INVOKABLE int normalTabsCount(int windowId = -1) const;
    Q_INVOKABLE int pinnedTabsCount(int windowId = -1) const;
    Q_INVOKABLE QList<QObject*> getAll(const QVariantMap &map = QVariantMap()) const;
    Q_INVOKABLE QList<QObject*> search(const QVariantMap &map);
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
