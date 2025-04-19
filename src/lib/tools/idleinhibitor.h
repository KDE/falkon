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

#ifndef IDLEINHIBITOR_H
#define IDLEINHIBITOR_H

#include "qzcommon.h"

#include <QObject>
#include <QList>

class WebTab;

/**
 * @brief ScreenSaver inhibitor
 * @details Tracks the audio playback on the tabs and user request to inhibit the ScreenSaver.
 */
class FALKON_EXPORT IdleInhibitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(IdleInhibitor::UserOverride userOverride READ userOverride WRITE setUserOverride NOTIFY userOverrideChanged)
    Q_PROPERTY(IdleInhibitor::InhibitorState state READ state NOTIFY stateChanged)

public:
    /**
     * @brief User override state enum
     */
    enum class UserOverride {
        Idle,       //!< No request from user
        Inhibit,    //!< User requested to inhibit the screen saver
        Uninhibit   //!< User requested to NOT inhibit the screen saver
    };
    Q_ENUM(UserOverride);

    /**
     * @brief IdleInhibitor state enum
     */
    enum class InhibitorState {
        Idle,       //!< No request from user nor tab, doing nothing, screen saver is NOT inhibited
        Inhibit,    //!< Send a request to inhibit the screen saver
        Inhibited,  //!< Screen saver is already inhibited
        Uninhibit,  //!< Send a request to Uninhibit screen saver
    };
    Q_ENUM(InhibitorState);

    /**
     * @brief IdleInhibitor constructor
     * @param parent The parent object
     */
    explicit IdleInhibitor(QObject* parent = nullptr);
    /**
     * @brief IdleInhibitor destructor
     */
    ~IdleInhibitor();

    /**
     * @brief Provides access to user override state
     * @return IdleInhibitor::UserOverride user override state
     */
    IdleInhibitor::UserOverride userOverride() const;
    /**
     * @brief Provides access to the IdleInhibitor state
     * @return IdleInhibitor::InhibitorState IdleInhibitor state
     */
    IdleInhibitor::InhibitorState state() const;

public Q_SLOTS:
    /**
     * @brief Sets user override state
     * @param[in] userState User override state
     */
    void setUserOverride(IdleInhibitor::UserOverride userState);
    /**
     * @brief Handles changes of playing state of a tab
     * @param[in] tab Tab for which the playing state changed
     * @param[in] playing Current playing state of a tab
     */
    void playingChanged(WebTab *tab, bool playing);
    /**
     * @brief Handles removal of a tab
     * @param[in] tab Tab which was removed
     */
    void tabRemoved(WebTab *tab);

Q_SIGNALS:
    /**
     * @brief Signal emitted when user override state changed
     * @param[in] userState User override state
     */
    void userOverrideChanged(IdleInhibitor::UserOverride userState);
    /**
     * @brief Signal emitted when IdleInhibitor state changed
     * @param[in] state IdleInhibitor state
     */
    void stateChanged(IdleInhibitor::InhibitorState state);

private:
    QList<WebTab*> m_activeTabs;                        //!< @brief List of tabs with active Audio

    InhibitorState m_state {InhibitorState::Idle};      //!< @brief Current IdleInhibitor state
    InhibitorState m_stateLast {InhibitorState::Idle};  //!< @brief Previous IndleInhibitor state
    UserOverride m_userOverride {UserOverride::Idle};   //!< @brief User override state

    /**
     * @brief IdleInhibitor state machine
     */
    void stateMachine();

#if defined(Q_OS_UNIX) && !defined(DISABLE_DBUS)
    const QString dbusService {QSL("org.freedesktop.ScreenSaver")};     //!< @brief DBus ScreenSaver service name
    const QString dbusPath {QSL("/org/freedesktop/ScreenSaver")};       //!< @brief DBus ScreenSaver path
    const QString dbusInterface {QSL("org.freedesktop.ScreenSaver")};   //!< @brief DBus ScreenSaver interface
    quint32 m_dbusCookie {0U};          //!< @brief DBus request identification cookie

    /**
     * @brief Send an Inhibit request over DBus
     */
    void dbusSendInhibitRequest();
    /**
     * @brief Send an Uninhibit request over DBus
     */
    void dbusSendUninhibitRequest();
#endif /* Q_OS_UNIX && !DISABLE_DBUS */
};

#endif /* IDLEINHIBITOR_H */
