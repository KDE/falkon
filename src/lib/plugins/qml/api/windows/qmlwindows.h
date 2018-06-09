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
#include "qmlwindow.h"

class QmlWindows : public QObject
{
    Q_OBJECT
public:
    QmlWindows(QObject *parent = nullptr);
    Q_INVOKABLE QmlWindow *get(const QVariantMap &map) const;
    Q_INVOKABLE QmlWindow *getCurrent() const;
    Q_INVOKABLE QList<QObject*> getAll() const;
    Q_INVOKABLE QmlWindow *create(const QVariantMap &map) const;
    Q_INVOKABLE void remove(int windowId) const;
Q_SIGNALS:
    void created(QmlWindow *window);
    void removed(QmlWindow *window);
private:
    BrowserWindow *getBrowserWindow(int windowId) const;
};
