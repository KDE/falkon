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
    : AbstractButtonInterface(parent)
    , m_popup(nullptr)
{
    connect(this, &AbstractButtonInterface::clicked, this, &QmlBrowserAction::clicked);
}

QString QmlBrowserAction::id() const
{
    return m_id;
}

void QmlBrowserAction::setId(const QString &id)
{
    m_id = id;
}

QString QmlBrowserAction::name() const
{
    return m_name;
}

void QmlBrowserAction::setName(const QString &name)
{
    m_name = name;
}

QString QmlBrowserAction::iconUrl() const
{
    return m_iconUrl;
}

void QmlBrowserAction::setIconUrl(const QString &iconUrl)
{
    m_iconUrl = iconUrl;
    QString iconPath = QzTools::getPathFromUrl(QUrl(m_iconUrl));
    setIcon(QIcon(iconPath));
}

QQmlComponent* QmlBrowserAction::popup() const
{
    return m_popup;
}

void QmlBrowserAction::setPopup(QQmlComponent* popup)
{
    m_popup = popup;
}

QmlBrowserAction::LocationFlags QmlBrowserAction::location() const
{
    return m_displayFlags;
}

void QmlBrowserAction::setLocation(const LocationFlags &locationFlags)
{
    m_displayFlags = locationFlags;
    emit locationChanged();
}

void QmlBrowserAction::clicked(ClickController *clickController)
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
