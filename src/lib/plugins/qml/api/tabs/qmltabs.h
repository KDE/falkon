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

class QmlTabs : public QObject
{
    Q_OBJECT
public:
    explicit QmlTabs(QObject *parent = 0);
    Q_INVOKABLE void setCurrentIndex(const QVariantMap &map);
    Q_INVOKABLE void nextTab(const QVariantMap &map = QVariantMap());
    Q_INVOKABLE void previousTab(const QVariantMap &map = QVariantMap());
    Q_INVOKABLE void moveTab(const QVariantMap &map);
    Q_INVOKABLE bool pinTab(const QVariantMap &map);
    Q_INVOKABLE bool unpinTab(const QVariantMap &map);
    Q_INVOKABLE void detachTab(const QVariantMap &map);
    Q_INVOKABLE void duplicate(const QVariantMap &map);
    Q_INVOKABLE void closeTab(const QVariantMap &map);
    Q_INVOKABLE void reloadTab(const QVariantMap &map);
    Q_INVOKABLE void stopTab(const QVariantMap &map);
    Q_INVOKABLE QmlTab *get(const QVariantMap &map) const;
    Q_INVOKABLE int normalTabsCount(const QVariantMap &map = QVariantMap()) const;
    Q_INVOKABLE int pinnedTabsCount(const QVariantMap &map = QVariantMap()) const;
    Q_INVOKABLE QList<QObject*> getAll(const QVariantMap &map = QVariantMap()) const;
Q_SIGNALS:
    void changed(const QVariantMap &map);
    void tabInserted(const QVariantMap &map);
    void tabRemoved(const QVariantMap &map);
private:
    BrowserWindow *getWindow(const QVariantMap &map) const;
    QHash<BrowserWindow*, int> windowIdHash;
};
