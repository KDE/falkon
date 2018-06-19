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

class QmlSideBarHelper;

class QmlSideBar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable NOTIFY checkableChanged)
    Q_PROPERTY(QQmlComponent* item READ item WRITE setItem NOTIFY itemChanged)
    Q_CLASSINFO("DefaultProperty", "item")

public:
    explicit QmlSideBar(QObject *parent = nullptr);
    QString name() const;
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

    SideBarInterface *sideBar() const;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void titleChanged(const QString &title);
    void iconChanged(const QString &icon);
    void shortcutChanged(const QString &shortcut);
    void checkableChanged(bool checkable);
    void itemChanged(QQmlComponent *item);

private:
    QString m_name;
    QString m_title;
    QString m_iconUrl;
    QString m_shortcut;
    bool m_checkable;
    QQmlComponent *m_item;

    QmlSideBarHelper *m_sideBarHelper;
};

class QmlSideBarHelper : public SideBarInterface
{
    Q_OBJECT
public:
    explicit QmlSideBarHelper(QObject *parent = nullptr);
    QString title() const;
    QAction *createMenuAction();
    QWidget *createSideBarWidget(BrowserWindow *mainWindow);

    void setTitle(const QString &title);
    void setIcon(const QString &icon);
    void setShortcut(const QString &shortcut);
    void setCheckable(bool checkable);
    void setItem(QQmlComponent *item);

private:
    QString m_title;
    QString m_iconUrl;
    QString m_shortcut;
    bool m_checkable;
    QQmlComponent *m_item;
};
