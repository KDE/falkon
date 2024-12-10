/* ============================================================
 * Falkon - Qt web browser
 * Copyright (C) 2024 Juraj Oravec <jurajoravec@mailo.com>
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

#include "idleinhibitor.h"

#include <QDBusInterface>
#include <QDBusReply>

IdleInhibitor::IdleInhibitor()
: m_dbusCookie(0)
, m_active(false)
{
}

void IdleInhibitor::inhibit()
{
    if (m_dbusCookie != 0) {
        return;
    }

#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
    QDBusInterface dbus(QSL("org.freedesktop.ScreenSaver"), QSL("/org/freedesktop/ScreenSaver"), QSL("org.freedesktop.ScreenSaver"), QDBusConnection::sessionBus());
    if (!dbus.isValid()) {
        qInfo() << "Dbus ionterface 'org.freedesktop.ScreenSaver' is not available.";
        return;
    }

    QDBusReply <quint32> reply = dbus.call(QSL("Inhibit"), QL1S(Qz::APPNAME), tr("Media playback"));
    if (reply.isValid()) {
        m_dbusCookie = reply.value();
        setActive(true);
    }
    else {
        qWarning() << "IdleInhibitor DBus error:" << reply.error();
        setActive(false);
    }
#endif /* Q_OS_UNIX && !DISABLE_DBUS */
}

void IdleInhibitor::unInhibit()
{
    if ((m_dbusCookie == 0) && !m_activeTabs.isEmpty()) {
        return;
    }

#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
    QDBusInterface dbus(QSL("org.freedesktop.ScreenSaver"), QSL("/org/freedesktop/ScreenSaver"), QSL("org.freedesktop.ScreenSaver"), QDBusConnection::sessionBus());
    if (!dbus.isValid()) {
        qInfo() << "IdleInhibitor: Dbus ionterface 'org.freedesktop.ScreenSaver' is not available.";
        setActive(false);
        return;
    }

    QDBusReply <quint32> reply = dbus.call(QSL("UnInhibit"), m_dbusCookie);
    QDBusError err = dbus.lastError();
    if (err.isValid()) {
        qWarning() << "IdleInhibitor DBus error:" << reply.error();
    }

    setActive(false);
#endif /* Q_OS_UNIX && !DISABLE_DBUS */
}

bool IdleInhibitor::active() const
{
    return m_active;
}

void IdleInhibitor::setActive(bool active)
{
    if (m_active == active) {
        return;
    }

    m_active = active;
    Q_EMIT activeChanged(m_active);
}
