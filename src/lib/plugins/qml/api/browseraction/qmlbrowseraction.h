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
#include "mainapplication.h"
#include <QQmlComponent>
#include <QQmlParserStatus>

class QmlBrowserActionButton;

/**
 * @brief The class exposing BrowserAction API to QML
 */
class QmlBrowserAction : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    /**
     * @brief identity for the button. This is a required property.
     */
    Q_PROPERTY(QString identity READ identity WRITE setIdentity NOTIFY identityChanged)

    /**
     * @brief name of the button. This is a required property.
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    /**
     * @brief title of the button.
     */
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

    /**
     * @brief tool tip of the button.
     */
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)

    /**
     * @brief icon path of button
     *
     * The icon path will be search in the following order
     * - Theme icon: if the icon is found as a theme icon, then it will
     *               be used even if the icon file with same name is present
     *               in the plugin directory
     * - Falkon resource: for the icons starting with ":", they are searched in
     *               falkon resource file
     * - Files in plugin directory: All other paths will be resolved relative to
     *               the plugin directory. If the icon path is outside the
     *               plugin directory, then it will be resolved as empty path.
     */
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)

    /**
     * @brief badge text of the button
     */
    Q_PROPERTY(QString badgeText READ badgeText WRITE setBadgeText NOTIFY badgeTextChanged)

    /**
     * @brief the popup shown when the button is clicked. This must be a QML Window.
     */
    Q_PROPERTY(QQmlComponent* popup READ popup WRITE setPopup NOTIFY popupChanged)

    /**
     * @brief represents locations where the button is to be added.
     */
    Q_PROPERTY(Locations location READ location WRITE setLocation NOTIFY locationChanged)

public:
    /**
     * @brief The Location enum
     */
    enum Location {
        NavigationToolBar = 0x1, //!< to add the button in navigation tool bar
        StatusBar = 0x2          //!< to add the button in status bar
    };
    Q_DECLARE_FLAGS(Locations, Location)
    Q_ENUM(Locations)

    explicit QmlBrowserAction(QObject *parent = nullptr);
    ~QmlBrowserAction() override;
    void classBegin() override {}
    void componentComplete() override;
    QmlBrowserActionButton *button() const;
    Locations location() const;

Q_SIGNALS:
    /**
     * @brief This signal is emitted when identity property is changed
     * @param QString representing identity
     */
    void identityChanged(const QString &identity);

    /**
     * @brief This signal is emitted when name property is changed
     * @param QString representing name
     */
    void nameChanged(const QString &name);

    /**
     * @brief This signal is emitted when title property is changed
     * @param QString representing title
     */
    void titleChanged(const QString &title);

    /**
     * @brief This signal is emitted when the toolTip property is changed
     * @param QString representing toolTip
     */
    void toolTipChanged(const QString &toolTip);

    /**
     * @brief This signal is emitted when the icon property is changed
     * @param QString representing icon
     */
    void iconChanged(const QString &icon);

    /**
     * @brief This signal is emitted when the badgeText property is changed
     * @param QString representing badgeText
     */
    void badgeTextChanged(const QString &badgeText);

    /**
     * @brief This signal is emitted when the popup property is changed
     * @param QQmComponent representing popup
     */
    void popupChanged(QQmlComponent *popup);

    /**
     * @brief This signal is emitted when the locations property is changed
     * @param locations
     */
    void locationChanged(const Locations &locations);

    /**
     * @brief This signal is emitted when the button is clicked
     */
    void clicked();

private:
    QString m_identity;
    QString m_name;
    QString m_title;
    QString m_toolTip;
    QString m_icon;
    QString m_badgeText;
    QQmlComponent *m_popup = nullptr;
    Locations m_locations = NavigationToolBar;
    QmlBrowserActionButton *m_button = nullptr;

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
    QQmlComponent *popup() const;
    void setPopup(QQmlComponent *popup);
    void setLocation(const Locations &locations);

    void addButton(BrowserWindow *window);
    void removeButton(BrowserWindow *window);
};

class QmlBrowserActionButton : public AbstractButtonInterface
{
    Q_OBJECT
public:
    explicit QmlBrowserActionButton(QObject *parent = nullptr);
    QString id() const override;
    void setId(const QString &id);
    QString name() const override;
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
    QQmlComponent *m_popup = nullptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QmlBrowserAction::Locations)
