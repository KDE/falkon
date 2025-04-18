/* ============================================================
 * Falkon - Qt web browser
 * Copyright (C) 2025 Juraj Oravec <jurajoravec@mailo.com>
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

#include "webtab.h"

#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
#include <QDBusInterface>
#include <QDBusReply>
#endif /* Q_OS_UNIX && !DISABLE_DBUS */

IdleInhibitor::IdleInhibitor(QObject* parent)
: QObject(parent)
{
}

IdleInhibitor::~IdleInhibitor() = default;

IdleInhibitor::InhibitorState IdleInhibitor::state() const
{
    return m_state;
}

IdleInhibitor::UserOverride IdleInhibitor::userOverride() const
{
    return m_userOverride;
}

void IdleInhibitor::setUserOverride(IdleInhibitor::UserOverride userState)
{
    if (m_userOverride == userState) {
        return;
    }

    m_userOverride = userState;
    stateMachine();

    Q_EMIT(userOverrideChanged(userState));
}

void IdleInhibitor::playingChanged(WebTab* tab, bool playing)
{
    const bool containsTab = m_activeTabs.contains(tab);

    if (containsTab && !playing) {
        m_activeTabs.remove(m_activeTabs.indexOf(tab));
        qDebug() << "IdleInhibitor: Removing tab:" << tab;
    }
    else if (!containsTab && playing) {
        m_activeTabs.append(tab);
        qDebug() << "IdleInhibitor: Adding tab:" << tab;
    }
    else {
        qDebug() << "IdleInhibitor: Doing nothing with tab:" << tab;
    }

    stateMachine();
}

void IdleInhibitor::tabRemoved(WebTab* tab)
{
    playingChanged(tab, false);
}

void IdleInhibitor::stateMachine()
{
    InhibitorState nextState;

    do {
        nextState = m_state;

        switch (m_state) {
        case InhibitorState::Idle:
            if (m_userOverride == UserOverride::Inhibit) {
                m_state = InhibitorState::Inhibit;
            }
            else if (!m_activeTabs.isEmpty()) {
                m_state = InhibitorState::Inhibit;
            }
            else {
                m_state = InhibitorState::Idle;
            }
            break;
        case InhibitorState::Inhibit:
#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
            dbusSendInhibitRequest();
#endif /* Q_OS_UNIX && !DISABLE_DBUS */
            m_state = InhibitorState::Inhibited;
            break;
        case InhibitorState::Inhibited:
            if (m_userOverride == UserOverride::Uninhibit) {
                m_state = InhibitorState::Uninhibit;
            }
            else if (m_activeTabs.isEmpty()) {
                m_state = InhibitorState::Uninhibit;
            }
            else {
                m_state = InhibitorState::Inhibited;
            }
            break;
        case InhibitorState::Uninhibit:
#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
            dbusSendUninhibitRequest();
#endif /* Q_OS_UNIX && !DISABLE_DBUS */
            m_state = InhibitorState::Idle;
            break;
        default:
            qWarning() << "IdleInhibitor: Got to an unknown state:" << m_state;
            break;
        }
    } while (nextState != m_state);

    if (m_state != m_stateLast) {
        qDebug() << "IdleInhibitor: State changed to:" << m_state;
        m_stateLast = m_state;
        Q_EMIT stateChanged(m_state);
    }
}

#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
void IdleInhibitor::dbusSendInhibitRequest()
{
    if (m_dbusCookie != 0U) {
        return;
    }

    QDBusInterface dbus(dbusService, dbusPath, dbusInterface, QDBusConnection::sessionBus());
    if (!dbus.isValid()) {
        qInfo() << "DBus interface " << dbusInterface << " is not available.";
        return;
    }

    QDBusReply <quint32> reply = dbus.call(QSL("Inhibit"), QL1S(Qz::APPNAME), tr("Media playback"));
    if (reply.isValid()) {
        m_dbusCookie = reply.value();
    }
    else {
        qWarning() << "IdleInhibitor DBus error:" << reply.error();
        m_dbusCookie = 0U;
    }
}

void IdleInhibitor::dbusSendUninhibitRequest()
{
    if (m_dbusCookie == 0U) {
        return;
    }

    QDBusInterface dbus(dbusService, dbusPath, dbusInterface, QDBusConnection::sessionBus());
    if (!dbus.isValid()) {
        qInfo() << "IdleInhibitor: DBus ionterface " << dbusInterface << " is not available.";
        m_dbusCookie = 0U;
        return;
    }

    QDBusReply <quint32> reply = dbus.call(QSL("UnInhibit"), m_dbusCookie);
    QDBusError err = dbus.lastError();
    if (err.isValid()) {
        qWarning() << "IdleInhibitor DBus error:" << reply.error();
        m_dbusCookie = 0U;
    }
}
#endif /* Q_OS_UNIX && !DISABLE_DBUS */
