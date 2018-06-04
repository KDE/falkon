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

QmlWindow::QmlWindow(BrowserWindow *window, QObject *parent)
    : QObject(parent)
    , m_window(window)
{
}

int QmlWindow::id() const
{
    if (!mApp->windowIdHash().contains(m_window)) {
        return -1;
    }

    return mApp->windowIdHash().value(m_window);
}

bool QmlWindow::incognito() const
{
    return mApp->isPrivate();
}

QString QmlWindow::title() const
{
    if (!m_window) {
        return QString();
    }

    return m_window->windowTitle();
}

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

bool QmlWindow::focussed() const
{
    if (!m_window) {
        return false;
    }

    return m_window->isActiveWindow();
}

int QmlWindow::height() const
{
    if (!m_window) {
        return -1;
    }

    return m_window->height();
}

int QmlWindow::width() const
{
    if (!m_window) {
        return -1;
    }

    return m_window->width();
}
