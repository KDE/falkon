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
#include "abstractbuttoninterface.h"
#include <QQmlComponent>

class QmlBrowserActionButton;

class QmlBrowserAction : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identity READ identity WRITE setIdentity NOTIFY identityChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString badgeText READ badgeText WRITE setBadgeText NOTIFY badgeTextChanged)
    Q_PROPERTY(QQmlComponent* popup READ popup WRITE setPopup NOTIFY popupChanged)
    Q_PROPERTY(Locations location READ location WRITE setLocation NOTIFY locationChanged)

public:
    enum Location {
        NavigationToolBar = 0x1,
        StatusBar = 0x2
    };
    Q_DECLARE_FLAGS(Locations, Location)
    Q_ENUMS(Locations)

    explicit QmlBrowserAction(QObject *parent = nullptr);
    QString identity() const;
    void setIdentity(const QString &identity);
    QString name() const;
    void setName(const QString &name);
    QString title() const;
    void setTitle(const QString &title);
    QString toolTip() const;
    void setToolTip(const QString &toolTip);
    QString icon() const;
    void setIcon(const QString &icon);
    QString badgeText() const;
    void setBadgeText(const QString &badgeText);
    QQmlComponent* popup() const;
    void setPopup(QQmlComponent* popup);
    Locations location() const;
    void setLocation(const Locations &locations);

    QmlBrowserActionButton *button() const;

Q_SIGNALS:
    void identityChanged(const QString &identity);
    void nameChanged(const QString &name);
    void titleChanged(const QString &title);
    void toolTipChanged(const QString &toolTip);
    void iconChanged(const QString &icon);
    void badgeTextChanged(const QString &badgeText);
    void popupChanged(QQmlComponent *popup);
    void locationChanged(const Locations &locations);
    void clicked();

private:
    QString m_identity;
    QString m_name;
    QString m_title;
    QString m_toolTip;
    QString m_icon;
    QString m_badgeText;
    QQmlComponent* m_popup;
    Locations m_locations;
    QmlBrowserActionButton *m_button;
};

class QmlBrowserActionButton : public AbstractButtonInterface
{
    Q_OBJECT
public:
    explicit QmlBrowserActionButton(QObject *parent = nullptr);
    QString id() const;
    void setId(const QString &id);
    QString name() const;
    void setName(const QString &name);
    void setTitle(const QString &title);
    void setToolTip(const QString &toolTip);
    void setIcon(const QString &icon);
    void setBadgeText(const QString &badgeText);
    void setPopup(QQmlComponent *popup);

    void positionPopup(ClickController *clickController);

private:
    QString m_id;
    QString m_name;
    QString m_iconUrl;
    QQmlComponent *m_popup;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QmlBrowserAction::Locations)
