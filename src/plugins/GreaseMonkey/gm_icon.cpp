/* ============================================================
* GreaseMonkey plugin for Falkon
* Copyright (C) 2013-2018 David Rosca <nowrep@gmail.com>
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
#include "gm_icon.h"
#include "gm_manager.h"

#include "webview.h"

#include <QMenu>

GM_Icon::GM_Icon(GM_Manager *manager)
    : AbstractButtonInterface(manager)
    , m_manager(manager)
{
    setIcon(QIcon(QSL(":gm/data/icon.svg")));
    setTitle(tr("GreaseMonkey"));

    updateState(m_manager->isEnabled());

    connect(this, &AbstractButtonInterface::clicked, this, &GM_Icon::handleClicked);
    connect(m_manager, &GM_Manager::enabledChanged, this, &GM_Icon::updateState);
}

QString GM_Icon::id() const
{
    return QSL("greasemonkey-icon");
}

QString GM_Icon::name() const
{
    return tr("GreaseMonkey Icon");
}

void GM_Icon::openSettings()
{
    m_manager->showSettings(webView());
}

void GM_Icon::handleClicked(AbstractButtonInterface::ClickController *controller)
{
    auto *menu = new QMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);

    QAction *act = menu->addAction(tr("&Enable GreaseMonkey"));
    act->setCheckable(true);
    act->setChecked(m_manager->isEnabled());
    connect(act, &QAction::triggered, m_manager, &GM_Manager::setEnabled);

    menu->addAction(tr("Show GreaseMonkey &Settings"), this, &GM_Icon::openSettings);
    menu->addSeparator();

    connect(menu, &QMenu::aboutToHide, this, [=]() {
        controller->popupClosed();
    });

    menu->popup(controller->popupPosition(menu->sizeHint()));
}

void GM_Icon::updateState(const bool enabled)
{
    setActive(enabled);

    if (enabled) {
        setToolTip(tr("GreaseMonkey is active"));
    }
    else {
        setToolTip(tr("GreaseMonkey is disabled"));
    }
}
