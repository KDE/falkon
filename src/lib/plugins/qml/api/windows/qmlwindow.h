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
#include "browserwindow.h"
#include "qmlwindowstate.h"
#include "qmlwindowtype.h"

class QmlWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(bool incognito READ incognito CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QmlWindowState::WindowState state READ state CONSTANT)
    Q_PROPERTY(QmlWindowType::WindowType type READ type CONSTANT)
    Q_PROPERTY(QList<QObject*> tabs READ tabs CONSTANT)
    Q_PROPERTY(bool focussed READ focussed CONSTANT)
    Q_PROPERTY(int height READ height CONSTANT)
    Q_PROPERTY(int width READ width CONSTANT)
public:
    QmlWindow(BrowserWindow *window = nullptr, QObject *parent = nullptr);
    int id() const;
    bool incognito() const;
    QString title() const;
    QmlWindowState::WindowState state() const;
    QmlWindowType::WindowType type() const;
    QList<QObject*> tabs() const;
    bool focussed() const;
    int height() const;
    int width() const;
private:
    BrowserWindow *m_window;
};

class QmlWindowData
{
public:
    explicit QmlWindowData();
    ~QmlWindowData();
    QmlWindow *get(BrowserWindow *window);
private:
    QHash<BrowserWindow*, QmlWindow*> m_windows;
};
