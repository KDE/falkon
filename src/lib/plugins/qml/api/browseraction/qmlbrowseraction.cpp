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
#include "mainapplication.h"
#include "qztools.h"
#include <QQuickWindow>

QmlBrowserAction::QmlBrowserAction(QObject *parent)
    : QObject(parent)
    , m_popup(nullptr)
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
    emit identityChanged(m_identity);
}

QString QmlBrowserAction::name() const
{
    return m_name;
}

void QmlBrowserAction::setName(const QString &name)
{
    m_name = name;
    emit nameChanged(m_name);
}

QString QmlBrowserAction::title() const
{
    return m_title;
}

void QmlBrowserAction::setTitle(const QString &title)
{
    m_title = title;
    emit titleChanged(m_title);
}

QString QmlBrowserAction::toolTip() const
{
    return m_toolTip;
}

void QmlBrowserAction::setToolTip(const QString &toolTip)
{
    m_toolTip = toolTip;
    emit toolTipChanged(m_toolTip);
}

QString QmlBrowserAction::icon() const
{
    return m_icon;
}

void QmlBrowserAction::setIcon(const QString &icon)
{
    m_icon = icon;
    emit iconChanged(m_icon);
}

QString QmlBrowserAction::badgeText() const
{
    return m_badgeText;
}

void QmlBrowserAction::setBadgeText(const QString &badgeText)
{
    m_badgeText = badgeText;
    emit badgeTextChanged(m_badgeText);
}

QQmlComponent* QmlBrowserAction::popup() const
{
    return m_popup;
}

void QmlBrowserAction::setPopup(QQmlComponent* popup)
{
    m_popup = popup;
    emit popupChanged(m_popup);
}

void QmlBrowserAction::setLocation(const Locations &locations)
{
    m_locations = locations;
    emit locationChanged(m_locations);
}

QmlBrowserActionButton::QmlBrowserActionButton(QObject *parent)
    : AbstractButtonInterface(parent)
    , m_popup(nullptr)
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
    AbstractButtonInterface::setIcon(QIcon(QzTools::getPathFromUrl(QUrl(m_iconUrl))));
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

    QQuickWindow *quickWindow = dynamic_cast<QQuickWindow*>(m_popup->create());
    if (!quickWindow) {
        qWarning() << "Cannot create QQuickWindow from popup";
        return;
    }
    quickWindow->setFlags(Qt::Popup);
    quickWindow->setPosition(clickController->callPopupPosition(quickWindow->size()));

    connect(quickWindow, &QQuickWindow::activeChanged, this, [quickWindow, clickController]{
        if (!quickWindow->isActive()) {
            quickWindow->destroy();
            clickController->callPopupClosed();
        }
    });

    quickWindow->show();
    quickWindow->requestActivate();
}
