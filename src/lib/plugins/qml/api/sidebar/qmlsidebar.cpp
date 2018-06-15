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
#include "qmlsidebar.h"
#include "mainapplication.h"
#include <QAction>
#include <QQuickWindow>

QmlSideBar::QmlSideBar(QObject *parent)
    : SideBarInterface(parent)
    , m_item(nullptr)
{
}

QString QmlSideBar::name() const
{
    return m_name;
}

void QmlSideBar::setName(const QString &name)
{
    m_name = name;
}

QString QmlSideBar::title() const
{
    return m_title;
}

void QmlSideBar::setTitle(const QString &title)
{
    m_title = title;
}

QString QmlSideBar::icon() const
{
    return m_iconUrl;
}

void QmlSideBar::setIcon(const QString &icon)
{
    m_iconUrl = icon;
}

QString QmlSideBar::shortcut() const
{
    return m_shortcut;
}

void QmlSideBar::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
}

bool QmlSideBar::checkable()
{
    return m_checkable;
}

void QmlSideBar::setCheckable(bool checkable)
{
    m_checkable = checkable;
}

QQmlComponent *QmlSideBar::item() const
{
    return m_item;
}

void QmlSideBar::setItem(QQmlComponent *item)
{
    m_item = item;
}

QAction *QmlSideBar::createMenuAction()
{
    QAction *action = new QAction;
    action->setText(m_title);
    action->setCheckable(m_checkable);
    action->setShortcut(QKeySequence(m_shortcut));
    action->setIcon(QIcon(QUrl(m_iconUrl).toLocalFile()));
    return action;
}

QWidget *QmlSideBar::createSideBarWidget(BrowserWindow *mainWindow)
{
    Q_UNUSED(mainWindow)

    QQuickWindow *window = qobject_cast<QQuickWindow*>(m_item->create());
    if (!window) {
        qWarning() << "Unable to create QQuickWindow";
        return nullptr;
    }

    return QWidget::createWindowContainer(window);
}
