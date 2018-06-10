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
#include "qmlwindow.h"
#include "mainapplication.h"
#include "../tabs/qmltab.h"
#include "tabwidget.h"
#include <QQmlEngine>

QmlWindow::QmlWindow(BrowserWindow *window, QObject *parent)
    : QObject(parent)
    , m_window(window)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

/**
 * @brief Get the Id of the window
 * @return Integer representing the window id if window exists, else -1
 */
int QmlWindow::id() const
{
    if (!mApp->windowIdHash().contains(m_window)) {
        return -1;
    }

    return mApp->windowIdHash().value(m_window);
}

/**
 * @brief Checks if the window is private window
 * @return True if the window is private, else false
 */
bool QmlWindow::incognito() const
{
    return mApp->isPrivate();
}

/**
 * @brief Get the window title
 * @return String representing the window title
 */
QString QmlWindow::title() const
{
    if (!m_window) {
        return QString();
    }

    return m_window->windowTitle();
}

/**
 * @brief Gets the [window state](@ref QmlWindowState)
 * @return Window State
 */
QmlWindowState::WindowState QmlWindow::state() const
{
    if (!m_window) {
        return QmlWindowState::Invalid;
    }

    if (m_window->isFullScreen()) {
        return QmlWindowState::FullScreen;
    } else if (m_window->isMaximized()) {
        return QmlWindowState::Maximized;
    } else if (m_window->isMinimized()) {
        return QmlWindowState::Minimized;
    } else {
        return QmlWindowState::Normal;
    }
}

/**
 * @brief Gets the [window type](@ref QmlWindowType)
 * @return Window Type
 */
QmlWindowType::WindowType QmlWindow::type() const
{
    if (!m_window) {
        return QmlWindowType::OtherRestoredWindow;
    }

    switch (m_window->windowType()) {
    case Qz::BW_FirstAppWindow:
        return QmlWindowType::FirstAppWindow;
    case Qz::BW_MacFirstWindow:
        return QmlWindowType::MacFirstWindow;
    case Qz::BW_NewWindow:
        return QmlWindowType::NewWindow;
    default:
        return QmlWindowType::OtherRestoredWindow;
    }
}

/**
 * @brief Get the window tabs
 * @return List of tabs of type [QmlTab](@ref QmlTab)
 */
QList<QObject*> QmlWindow::tabs() const
{
    if (!m_window) {
        return QList<QObject*>();
    }

    QList<QObject*> list;

    for (WebTab *tab : m_window->tabWidget()->allTabs(true)) {
        list.append(new QmlTab(tab));
    }

    return list;
}

/**
 * @brief Checks if the window is under focus
 * @return True if the window is under focus, else false
 */
bool QmlWindow::focussed() const
{
    if (!m_window) {
        return false;
    }

    return m_window->isActiveWindow();
}

/**
 * @brief Get the window height
 * @return Integer representing the window height
 */
int QmlWindow::height() const
{
    if (!m_window) {
        return -1;
    }

    return m_window->height();
}

/**
 * @brief Get the window width
 * @return Integer representing the window width
 */
int QmlWindow::width() const
{
    if (!m_window) {
        return -1;
    }

    return m_window->width();
}

QmlWindowData::QmlWindowData()
{
}

QmlWindowData::~QmlWindowData()
{
    qDeleteAll(m_windows);
}

QmlWindow *QmlWindowData::get(BrowserWindow *window)
{
    QmlWindow *qmlWindow = m_windows.value(window);
    if (!qmlWindow) {
        qmlWindow = new QmlWindow(window);
        m_windows.insert(window, qmlWindow);
    }
    return qmlWindow;
}
