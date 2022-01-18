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
#ifndef TABCONTEXTMENU_H
#define TABCONTEXTMENU_H

#include <QMenu>

#include "qzcommon.h"

class BrowserWindow;
class TabWidget;

class FALKON_EXPORT TabContextMenu : public QMenu
{
    Q_OBJECT

public:
    enum Option {
        InvalidOption = 0,
        HorizontalTabs = 1 << 0,
        VerticalTabs = 1 << 1,
        ShowCloseOtherTabsActions = 1 << 2,
        ShowDetachTabAction = 1 << 3,

        DefaultOptions = HorizontalTabs | ShowCloseOtherTabsActions | ShowDetachTabAction
    };
    Q_DECLARE_FLAGS(Options, Option)

    explicit TabContextMenu(int index, BrowserWindow *window, Options options = DefaultOptions);

Q_SIGNALS:
    void reloadTab(int index);
    void stopTab(int index);
    void tabCloseRequested(int index);
    void closeAllButCurrent(int index);
    void closeToRight(int index);
    void closeToLeft(int index);
    void duplicateTab(int index);
    void detachTab(int index);
    void loadTab(int index);
    void unloadTab(int index);

private Q_SLOTS:
    void reloadTab() { Q_EMIT reloadTab(m_clickedTab); }
    void stopTab() { Q_EMIT stopTab(m_clickedTab); }
    void closeTab() { Q_EMIT tabCloseRequested(m_clickedTab); }
    void duplicateTab() { Q_EMIT duplicateTab(m_clickedTab); }
    void detachTab() { Q_EMIT detachTab(m_clickedTab); }
    void loadTab() { Q_EMIT loadTab(m_clickedTab); }
    void unloadTab() { Q_EMIT unloadTab(m_clickedTab); }

    void pinTab();
    void muteTab();

    void closeAllButCurrent();
    void closeToRight();
    void closeToLeft();

private:
    void init();

    int m_clickedTab;
    BrowserWindow *m_window;
    Options m_options = InvalidOption;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TabContextMenu::Options)

#endif // TABCONTEXTMENU_H
