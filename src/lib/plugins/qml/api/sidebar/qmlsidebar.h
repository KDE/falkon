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

class QmlSideBar : public SideBarInterface
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString icon READ icon WRITE setIcon)
    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut)
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable)
    Q_PROPERTY(QQmlComponent* item READ item WRITE setItem)
    Q_CLASSINFO("DefaultProperty", "item")

public:
    QmlSideBar(QObject *parent = nullptr);
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

    QAction *createMenuAction();
    QWidget *createSideBarWidget(BrowserWindow *mainWindow);

private:
    QString m_name;
    QString m_title;
    QString m_iconUrl;
    QString m_shortcut;
    bool m_checkable;
    QQmlComponent *m_item;
};
