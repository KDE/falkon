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

/**
 * @brief The class exposing Browser window to QML
 */
class QmlWindow : public QObject
{
    Q_OBJECT

    /**
     * @brief id of window
     */
    Q_PROPERTY(int id READ id CONSTANT)

    /**
     * @brief checks if the window is private
     */
    Q_PROPERTY(bool incognito READ incognito CONSTANT)

    /**
     * @brief title of window
     */
    Q_PROPERTY(QString title READ title CONSTANT)

    /**
     * @brief [window state](@ref QmlWindowState::WindowState) of window
     */
    Q_PROPERTY(QmlWindowState::WindowState state READ state CONSTANT)

    /**
     * @brief [window type](@ref QmlWindowType::WindowType) of window
     */
    Q_PROPERTY(QmlWindowType::WindowType type READ type CONSTANT)

    /**
     * @brief list of all tabs of window
     */
    Q_PROPERTY(QList<QObject*> tabs READ tabs CONSTANT)

    /**
     * @brief checks if the window is focussed
     */
    Q_PROPERTY(bool focussed READ focussed CONSTANT)

    /**
     * @brief height of window
     */
    Q_PROPERTY(int height READ height CONSTANT)

    /**
     * @brief width of window
     */
    Q_PROPERTY(int width READ width CONSTANT)
public:
    QmlWindow(BrowserWindow *window = nullptr, QObject *parent = nullptr);

private:
    BrowserWindow *m_window;

    int id() const;
    bool incognito() const;
    QString title() const;
    QmlWindowState::WindowState state() const;
    QmlWindowType::WindowType type() const;
    QList<QObject*> tabs() const;
    bool focussed() const;
    int height() const;
    int width() const;
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
