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
#include "sidebarinterface.h"
#include <QQmlComponent>
#include <QQmlParserStatus>

class QmlSideBarHelper;

/**
 * @brief The class exposing SideBar API to QML
 */
class QmlSideBar : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    /**
     * @brief name of the sidebar. This is required property.
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    /**
     * @brief title of the sidebar action.
     */
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

    /**
     * @brief icon path of the sidebar action.
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
     * @brief shortcut for the sidebar action.
     */
    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)

    /**
     * @brief represents whether the sidebar action is checkable
     */
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable NOTIFY checkableChanged)

    /**
     * @brief the GUI of the sidebar. This must be provided as QML Window.
     *        This is a default property.
     */
    Q_PROPERTY(QQmlComponent* item READ item WRITE setItem NOTIFY itemChanged)
    Q_CLASSINFO("DefaultProperty", "item")

public:
    explicit QmlSideBar(QObject *parent = nullptr);
    ~QmlSideBar() override;
    void classBegin() override {}
    void componentComplete() override;
    QString name() const;
    SideBarInterface *sideBar() const;

Q_SIGNALS:
    /**
     * @brief This signal is emitted when name property is changed.
     * @param QString represening name
     */
    void nameChanged(const QString &name);

    /**
     * @brief This signal is emitted when title property is changed
     * @param QString representing title
     */
    void titleChanged(const QString &title);

    /**
     * @brief This signal is emitted when icon property is changed
     * @param QString representing icon path url
     */
    void iconChanged(const QString &icon);

    /**
     * @brief This signal is emitted when shortcut property is changed
     * @param QString representing shortcut
     */
    void shortcutChanged(const QString &shortcut);

    /**
     * @brief This signal is emitted when checkable property is changed
     * @param checkable
     */
    void checkableChanged(bool checkable);
    void itemChanged(QQmlComponent *item);

private:
    QString m_name;
    QString m_title;
    QString m_iconUrl;
    QString m_shortcut;
    bool m_checkable = false;
    QQmlComponent *m_item = nullptr;

    QmlSideBarHelper *m_sideBarHelper = nullptr;

    void setName(const QString &name);
    QString title() const;
    void setTitle(const QString &title);
    QString icon() const;
    void setIcon(const QString &icon);
    QString shortcut() const;
    void setShortcut(const QString &shortcut);
    bool checkable();
    void setCheckable(bool checkable);
    QQmlComponent *item() const;
    void setItem(QQmlComponent *item);
};

class QmlSideBarHelper : public SideBarInterface
{
    Q_OBJECT
public:
    explicit QmlSideBarHelper(QObject *parent = nullptr);
    QString title() const override;
    QAction *createMenuAction() override;
    QWidget *createSideBarWidget(BrowserWindow *mainWindow) override;

    void setTitle(const QString &title);
    void setIcon(const QString &icon);
    void setShortcut(const QString &shortcut);
    void setCheckable(bool checkable);
    void setItem(QQmlComponent *item);

private:
    QString m_title;
    QString m_iconUrl;
    QString m_shortcut;
    bool m_checkable = false;
    QQmlComponent *m_item = nullptr;
};
