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

#include "sbi_idleinhibitoricon.h"

#include "mainapplication.h"

#include <QMenu>

SBI_IdleInhibitorIcon::SBI_IdleInhibitorIcon(BrowserWindow* window, const QString& settingsPath)
: SBI_Icon(window, settingsPath)
{
    setObjectName(QSL("sbi_networkicon"));
    setCursor(Qt::PointingHandCursor);

    userOverrideChanged(mApp->idleInhibitor()->userOverride());

    connect(mApp->idleInhibitor(), &IdleInhibitor::userOverrideChanged, this, &SBI_IdleInhibitorIcon::userOverrideChanged);
    connect(this, &ClickableLabel::clicked, this, &SBI_IdleInhibitorIcon::showMenu);
}

void SBI_IdleInhibitorIcon::userOverrideChanged(IdleInhibitor::UserOverride userState)
{
    switch (userState) {
    case IdleInhibitor::UserOverride::Idle:
        setPixmap(QIcon::fromTheme(QSL("user-idle")).pixmap(16));
        setToolTip(tr("Screensaver user override is not active."));
        break;
    case IdleInhibitor::UserOverride::Inhibit:
        setPixmap(QIcon::fromTheme(QSL("user-busy")).pixmap(16));
        setToolTip(tr("Screensaver user override is active: Inhibit"));
        break;
    case IdleInhibitor::UserOverride::Uninhibit:
        setPixmap(QIcon::fromTheme(QSL("user-away")).pixmap(16));
        setToolTip(tr("Screensaver user override is active: UnInhibit"));
        break;
    }
}

void SBI_IdleInhibitorIcon::showMenu(const QPoint& point)
{
    QFont boldFont = font();
    boldFont.setBold(true);

    QMenu menu;
    menu.addAction(tr("Screensaver user override"))->setFont(boldFont);
    menu.addSeparator();
    menu.addAction(tr("Idle"), this, &SBI_IdleInhibitorIcon::menu_idle);
    menu.addAction(tr("Inhibit"), this, &SBI_IdleInhibitorIcon::menu_inhibit);
    menu.addAction(tr("UnInhibit"), this, &SBI_IdleInhibitorIcon::menu_uninhibit);

    menu.exec(point);
}

void SBI_IdleInhibitorIcon::menu_idle()
{
    mApp->idleInhibitor()->setUserOverride(IdleInhibitor::UserOverride::Idle);
}

void SBI_IdleInhibitorIcon::menu_inhibit()
{
    mApp->idleInhibitor()->setUserOverride(IdleInhibitor::UserOverride::Inhibit);
}

void SBI_IdleInhibitorIcon::menu_uninhibit()
{
    mApp->idleInhibitor()->setUserOverride(IdleInhibitor::UserOverride::Uninhibit);
}
