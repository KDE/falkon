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
#include "qmlbrowseraction.h"
#include "qztools.h"
#include "navigationbar.h"
#include "statusbar.h"
#include "pluginproxy.h"
#include "qml/api/fileutils/qmlfileutils.h"
#include "qml/qmlengine.h"
#include "qml/qmlstaticdata.h"
#include <QQuickWidget>
#include <QQmlContext>

QmlBrowserAction::QmlBrowserAction(QObject *parent)
    : QObject(parent)
{
    m_button = new QmlBrowserActionButton();

    connect(this, &QmlBrowserAction::identityChanged, m_button, &QmlBrowserActionButton::setId);
    connect(this, &QmlBrowserAction::nameChanged, m_button, &QmlBrowserActionButton::setName);
    connect(this, &QmlBrowserAction::titleChanged, m_button, &QmlBrowserActionButton::setTitle);
    connect(this, &QmlBrowserAction::toolTipChanged, m_button, &QmlBrowserActionButton::setToolTip);
    connect(this, &QmlBrowserAction::iconChanged, m_button, &QmlBrowserActionButton::setIcon);
    connect(this, &QmlBrowserAction::badgeTextChanged, m_button, &QmlBrowserActionButton::setBadgeText);
    connect(this, &QmlBrowserAction::popupChanged, m_button, &QmlBrowserActionButton::setPopup);

    connect(m_button, &QmlBrowserActionButton::clicked, this, &QmlBrowserAction::clicked);

    connect(mApp->plugins(), &PluginProxy::mainWindowCreated, this, &QmlBrowserAction::addButton);
}

void QmlBrowserAction::componentComplete()
{
    const QList<BrowserWindow*> windows = mApp->windows();
    for (BrowserWindow *window : windows) {
        addButton(window);
    }
}

QmlBrowserAction::~QmlBrowserAction()
{
    const QList<BrowserWindow*> windows = mApp->windows();
    for (BrowserWindow *window : windows) {
        removeButton(window);
    }
}

QmlBrowserActionButton *QmlBrowserAction::button() const
{
    return m_button;
}

QmlBrowserAction::Locations QmlBrowserAction::location() const
{
    return m_locations;
}

QString QmlBrowserAction::identity() const
{
    return m_identity;
}

void QmlBrowserAction::setIdentity(const QString &identity)
{
    m_identity = identity;
    Q_EMIT identityChanged(m_identity);
}

QString QmlBrowserAction::name() const
{
    return m_name;
}

void QmlBrowserAction::setName(const QString &name)
{
    m_name = name;
    Q_EMIT nameChanged(m_name);
}

QString QmlBrowserAction::title() const
{
    return m_title;
}

void QmlBrowserAction::setTitle(const QString &title)
{
    m_title = title;
    Q_EMIT titleChanged(m_title);
}

QString QmlBrowserAction::toolTip() const
{
    return m_toolTip;
}

void QmlBrowserAction::setToolTip(const QString &toolTip)
{
    m_toolTip = toolTip;
    Q_EMIT toolTipChanged(m_toolTip);
}

QString QmlBrowserAction::icon() const
{
    return m_icon;
}

void QmlBrowserAction::setIcon(const QString &icon)
{
    m_icon = icon;
    Q_EMIT iconChanged(m_icon);
}

QString QmlBrowserAction::badgeText() const
{
    return m_badgeText;
}

void QmlBrowserAction::setBadgeText(const QString &badgeText)
{
    m_badgeText = badgeText;
    Q_EMIT badgeTextChanged(m_badgeText);
}

QQmlComponent *QmlBrowserAction::popup() const
{
    return m_popup;
}

void QmlBrowserAction::setPopup(QQmlComponent *popup)
{
    m_popup = popup;
    Q_EMIT popupChanged(m_popup);
}

void QmlBrowserAction::setLocation(const Locations &locations)
{
    m_locations = locations;
    Q_EMIT locationChanged(m_locations);
}

void QmlBrowserAction::addButton(BrowserWindow *window)
{
    if (location().testFlag(NavigationToolBar)) {
        window->navigationBar()->addToolButton(button());
    }

    if (location().testFlag(StatusBar)) {
        window->statusBar()->addButton(button());
    }
}

void QmlBrowserAction::removeButton(BrowserWindow *window)
{
    if (location().testFlag(NavigationToolBar)) {
        window->navigationBar()->removeToolButton(button());
    }

    if (location().testFlag(StatusBar)) {
        window->statusBar()->removeButton(button());
    }
}

QmlBrowserActionButton::QmlBrowserActionButton(QObject *parent)
    : AbstractButtonInterface(parent)
{
    connect(this, &AbstractButtonInterface::clicked, this, &QmlBrowserActionButton::positionPopup);
}

QString QmlBrowserActionButton::id() const
{
    return m_id;
}

void QmlBrowserActionButton::setId(const QString &id)
{
    m_id = id;
}

QString QmlBrowserActionButton::name() const
{
    return m_name;
}

void QmlBrowserActionButton::setName(const QString &name)
{
    m_name = name;
}

void QmlBrowserActionButton::setTitle(const QString &title)
{
    AbstractButtonInterface::setTitle(title);
}

void QmlBrowserActionButton::setToolTip(const QString &toolTip)
{
    AbstractButtonInterface::setToolTip(toolTip);
}

void QmlBrowserActionButton::setIcon(const QString &icon)
{
    m_iconUrl = icon;
    if (!m_popup) {
        return;
    }
    auto qmlEngine = qobject_cast<QmlEngine*>(m_popup->creationContext()->engine());
    if (!qmlEngine) {
        return;
    }
    const QString pluginPath = qmlEngine->extensionPath();
    QIcon qicon = QmlStaticData::instance().getIcon(m_iconUrl, pluginPath);
    AbstractButtonInterface::setIcon(qicon);
}

void QmlBrowserActionButton::setBadgeText(const QString &badgeText)
{
    AbstractButtonInterface::setBadgeText(badgeText);
}

void QmlBrowserActionButton::setPopup(QQmlComponent *popup)
{
    m_popup = popup;
}

void QmlBrowserActionButton::positionPopup(ClickController *clickController)
{
    if (!m_popup) {
        qWarning() << "No popup to show";
        return;
    }

    auto *quickWidget = new QQuickWidget();
    quickWidget->setContent(m_popup->url(), m_popup, m_popup->create(m_popup->creationContext()));

    auto *widget = new QWidget();
    quickWidget->setParent(widget);

    widget->setWindowFlag(Qt::Popup);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->move(clickController->callPopupPosition(quickWidget->size()));

    connect(quickWidget, &QQuickWidget::destroyed, this, [clickController]{
        clickController->callPopupClosed();
    });

    widget->show();
}
