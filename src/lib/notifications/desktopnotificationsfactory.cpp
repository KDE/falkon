/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "desktopnotificationsfactory.h"
#include "desktopnotification.h"
#include "datapaths.h"
#include "settings.h"
#include "mainapplication.h"
#include "browserwindow.h"
#include "../config.h"

#include <QFile>
#include <QDir>

#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
#include <QDBusInterface>
#endif

DesktopNotificationsFactory::DesktopNotificationsFactory(QObject* parent)
    : QObject(parent)
    , m_uint(0)
{
    loadSettings();
}

void DesktopNotificationsFactory::loadSettings()
{
    Settings settings;
    settings.beginGroup(QSL("Notifications"));
    m_enabled = settings.value(QSL("Enabled"), true).toBool();
    m_timeout = settings.value(QSL("Timeout"), 6000).toInt();
#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
    m_notifType = settings.value(QSL("UseNativeDesktop"), true).toBool() ? DesktopNative : PopupWidget;
#else
    m_notifType = PopupWidget;
#endif
    m_position = settings.value(QSL("Position"), QPoint(10, 10)).toPoint();
    settings.endGroup();
}

bool DesktopNotificationsFactory::supportsNativeNotifications() const
{
#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
    return true;
#else
    return false;
#endif
}

void DesktopNotificationsFactory::showNotification(const QString &heading, const QString &text)
{
    showNotification(QPixmap(), heading, text);
}

void DesktopNotificationsFactory::showNotification(const QPixmap &icon, const QString &heading, const QString &text)
{
    if (!m_enabled) {
        return;
    }

    switch (m_notifType) {
    case PopupWidget:
        if (!m_desktopNotif) {
            m_desktopNotif = new DesktopNotification();
        }
        m_desktopNotif.data()->setPixmap(icon);
        m_desktopNotif.data()->setHeading(heading);
        m_desktopNotif.data()->setText(text);
        m_desktopNotif.data()->setTimeout(m_timeout);
        m_desktopNotif.data()->move(m_position);
        m_desktopNotif.data()->show();
        break;
    case DesktopNative:
#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
        QFile tmp(DataPaths::path(DataPaths::Temp) + QLatin1String("/falkon_notif.png"));
        tmp.open(QFile::WriteOnly);
        icon.save(tmp.fileName());

        const QVariantMap hints {
            {QStringLiteral("desktop-entry"), QGuiApplication::desktopFileName()}
        };

        QDBusInterface dbus(QSL("org.freedesktop.Notifications"), QSL("/org/freedesktop/Notifications"), QSL("org.freedesktop.Notifications"), QDBusConnection::sessionBus());
        QVariantList args;
        args.append(QLatin1String("Falkon"));
        args.append(m_uint);
        args.append(tmp.fileName());
        args.append(heading);
        args.append(text);
        args.append(QStringList());
        args.append(hints);
        args.append(m_timeout);
        dbus.callWithCallback(QSL("Notify"), args, this, SLOT(updateLastId(QDBusMessage)), SLOT(error(QDBusError)));
#endif
        break;
    }
}

void DesktopNotificationsFactory::nativeNotificationPreview()
{
    Type type = m_notifType;

    m_notifType = DesktopNative;
    const QPixmap icon = mApp->getWindow()->windowIcon().pixmap(64);
    showNotification(icon, QObject::tr("Native System Notification"), tr("Preview"));
    m_notifType = type;
}

void DesktopNotificationsFactory::updateLastId(const QDBusMessage &msg)
{
    Q_UNUSED(msg)
#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
    QVariantList list = msg.arguments();
    if (list.count() > 0) {
        m_uint = list.at(0).toInt();
    }
#endif
}

void DesktopNotificationsFactory::error(const QDBusError &error)
{
    Q_UNUSED(error)
#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
    qWarning() << "QDBusError:" << error.message();
#endif
}
