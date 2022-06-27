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
#include "qztools.h"
#include "sidebar.h"
#include "qml/api/fileutils/qmlfileutils.h"
#include "qml/qmlengine.h"
#include "qml/qmlstaticdata.h"
#include <QAction>
#include <QQuickWidget>
#include <QQmlContext>

QmlSideBar::QmlSideBar(QObject *parent)
    : QObject(parent)
{
    m_sideBarHelper = new QmlSideBarHelper(this);

    connect(this, &QmlSideBar::titleChanged, m_sideBarHelper, &QmlSideBarHelper::setTitle);
    connect(this, &QmlSideBar::iconChanged, m_sideBarHelper, &QmlSideBarHelper::setIcon);
    connect(this, &QmlSideBar::shortcutChanged, m_sideBarHelper, &QmlSideBarHelper::setShortcut);
    connect(this, &QmlSideBar::checkableChanged, m_sideBarHelper, &QmlSideBarHelper::setCheckable);
    connect(this, &QmlSideBar::itemChanged, m_sideBarHelper, &QmlSideBarHelper::setItem);
}

QmlSideBar::~QmlSideBar()
{
    SideBarManager::removeSidebar(m_sideBarHelper);
}

void QmlSideBar::componentComplete()
{
    SideBarManager::addSidebar(name(), sideBar());
}

QString QmlSideBar::name() const
{
    return m_name;
}

SideBarInterface *QmlSideBar::sideBar() const
{
    return m_sideBarHelper;
}

void QmlSideBar::setName(const QString &name)
{
    m_name = name;
    Q_EMIT nameChanged(m_name);
}

QString QmlSideBar::title() const
{
    return m_title;
}

void QmlSideBar::setTitle(const QString &title)
{
    m_title = title;
    Q_EMIT titleChanged(title);
}

QString QmlSideBar::icon() const
{
    return m_iconUrl;
}

void QmlSideBar::setIcon(const QString &icon)
{
    m_iconUrl = icon;
    Q_EMIT iconChanged(m_iconUrl);
}

QString QmlSideBar::shortcut() const
{
    return m_shortcut;
}

void QmlSideBar::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
    Q_EMIT shortcutChanged(m_shortcut);
}

bool QmlSideBar::checkable()
{
    return m_checkable;
}

void QmlSideBar::setCheckable(bool checkable)
{
    m_checkable = checkable;
    Q_EMIT checkableChanged(m_checkable);
}

QQmlComponent *QmlSideBar::item() const
{
    return m_item;
}

void QmlSideBar::setItem(QQmlComponent *item)
{
    m_item = item;
    Q_EMIT itemChanged(m_item);
}

QmlSideBarHelper::QmlSideBarHelper(QObject *parent)
    : SideBarInterface(parent)
{
}

QString QmlSideBarHelper::title() const
{
    return m_title;
}

QAction *QmlSideBarHelper::createMenuAction()
{
    auto *action = new QAction(m_title);
    action->setShortcut(QKeySequence(m_shortcut));
    action->setCheckable(m_checkable);
    if (!m_item) {
        return action;
    }
    auto qmlEngine = qobject_cast<QmlEngine*>(m_item->creationContext()->engine());
    if (qmlEngine) {
        return action;
    }
    const QString pluginPath = qmlEngine->extensionPath();
    const QIcon icon = QmlStaticData::instance().getIcon(m_iconUrl, pluginPath);
    action->setIcon(icon);
    return action;
}

QWidget *QmlSideBarHelper::createSideBarWidget(BrowserWindow *mainWindow)
{
    Q_UNUSED(mainWindow)

    auto *widget = new QQuickWidget();
    widget->setContent(m_item->url(), m_item, m_item->create(m_item->creationContext()));
    widget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    return widget;
}

void QmlSideBarHelper::setTitle(const QString &title)
{
    m_title = title;
}

void QmlSideBarHelper::setIcon(const QString &icon)
{
    m_iconUrl = icon;
}

void QmlSideBarHelper::setShortcut(const QString &shortcut)
{
    m_shortcut = shortcut;
}

void QmlSideBarHelper::setCheckable(bool checkable)
{
    m_checkable = checkable;
}

void QmlSideBarHelper::setItem(QQmlComponent *item)
{
    m_item = item;
}
