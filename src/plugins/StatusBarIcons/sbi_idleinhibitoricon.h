/* ============================================================
* StatusBarIcons - Extra icons in statusbar for Falkon
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
#ifndef SBI_IDLEINHIBITORICON_H
#define SBI_IDLEINHIBITORICON_H

#include <QIcon>

#include "sbi_icon.h"
#include "idleinhibitor.h"

/**
 * @brief An indicator for IdleInhibitor UserOverride option with menu to change it.
 */
class SBI_IdleInhibitorIcon : public SBI_Icon
{
    Q_OBJECT

public:
    /**
     * @brief SBI_IdleInhibitorIcon COnstructor
     * @param[in] window Current Falkon window where
     * @param[in] settingsPath Extension settings path
     */
    explicit SBI_IdleInhibitorIcon(BrowserWindow* window, const QString &settingsPath);

private Q_SLOTS:
    /**
     * @brief Handle user override changes from IdleInhibitor
     * @param[in] userState User override state
     */
    void userOverrideChanged(IdleInhibitor::UserOverride userState);
    /**
     * @brief Display popup menu after clicking on the icon
     * @param[in] point Position at which menu should be shown
     */
    void showMenu(const QPoint &point);

    /**
     * @brief Handle menu "Idle" option
     */
    void menu_idle();
    /**
     * @brief Handle menu "Inhibit" option
     */
    void menu_inhibit();
    /**
     * @brief Handle menu "Uninhibit" option
     */
    void menu_uninhibit();
};

#endif /* SBI_IDLEINHIBITORICON_H */
