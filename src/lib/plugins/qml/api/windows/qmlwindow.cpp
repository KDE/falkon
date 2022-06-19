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
#include "qml/qmlstaticdata.h"
#include <QQmlEngine>

QmlWindow::QmlWindow(BrowserWindow *window, QObject *parent)
    : QObject(parent)
    , m_window(window)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

int QmlWindow::id() const
{
    if (!QmlStaticData::instance().windowIdHash().contains(m_window)) {
        return -1;
    }

    return QmlStaticData::instance().windowIdHash().value(m_window, -1);
}

bool QmlWindow::incognito() const
{
    return mApp->isPrivate();
}

QString QmlWindow::title() const
{
    if (!m_window) {
        return {};
    }

    return m_window->windowTitle();
}

QmlEnums::WindowState QmlWindow::state() const
{
    if (!m_window) {
        return QmlEnums::Invalid;
    }

    if (m_window->isFullScreen()) {
        return QmlEnums::FullScreen;
    } else if (m_window->isMaximized()) {
        return QmlEnums::Maximized;
    } else if (m_window->isMinimized()) {
        return QmlEnums::Minimized;
    } else {
        return QmlEnums::Normal;
    }
}

QmlEnums::WindowType QmlWindow::type() const
{
    if (!m_window) {
        return QmlEnums::OtherRestoredWindow;
    }

    switch (m_window->windowType()) {
    case Qz::BW_FirstAppWindow:
        return QmlEnums::FirstAppWindow;
    case Qz::BW_MacFirstWindow:
        return QmlEnums::MacFirstWindow;
    case Qz::BW_NewWindow:
        return QmlEnums::NewWindow;
    default:
        return QmlEnums::OtherRestoredWindow;
    }
}

QList<QObject*> QmlWindow::tabs() const
{
    if (!m_window) {
        return {};
    }

    QList<QObject*> list;
    const QList<WebTab*> allTabs = m_window->tabWidget()->allTabs(true);
    list.reserve(allTabs.size());
    for (WebTab *tab : allTabs) {
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
