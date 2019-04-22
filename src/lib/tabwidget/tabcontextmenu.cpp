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
#include "tabcontextmenu.h"
#include "tabbar.h"
#include "tabwidget.h"
#include "browserwindow.h"
#include "webtab.h"
#include "settings.h"
#include "tabbedwebview.h"
#include "mainapplication.h"
#include "iconprovider.h"
#include "checkboxdialog.h"

#include <KLocalizedString>

TabContextMenu::TabContextMenu(int index, BrowserWindow *window, Options options)
    : QMenu()
    , m_clickedTab(index)
    , m_window(window)
    , m_options(options)
{
    setObjectName("tabcontextmenu");

    TabWidget *tabWidget = m_window->tabWidget();
    connect(this, &TabContextMenu::tabCloseRequested, tabWidget->tabBar(), &ComboTabBar::tabCloseRequested);
    connect(this, SIGNAL(reloadTab(int)), tabWidget, SLOT(reloadTab(int)));
    connect(this, SIGNAL(stopTab(int)), tabWidget, SLOT(stopTab(int)));
    connect(this, SIGNAL(closeAllButCurrent(int)), tabWidget, SLOT(closeAllButCurrent(int)));
    connect(this, SIGNAL(closeToRight(int)), tabWidget, SLOT(closeToRight(int)));
    connect(this, SIGNAL(closeToLeft(int)), tabWidget, SLOT(closeToLeft(int)));
    connect(this, SIGNAL(duplicateTab(int)), tabWidget, SLOT(duplicateTab(int)));
    connect(this, SIGNAL(detachTab(int)), tabWidget, SLOT(detachTab(int)));
    connect(this, SIGNAL(loadTab(int)), tabWidget, SLOT(loadTab(int)));
    connect(this, SIGNAL(unloadTab(int)), tabWidget, SLOT(unloadTab(int)));

    init();
}

static bool canCloseTabs(const QString &settingsKey, const QString &title, const QString &description)
{
    Settings settings;
    bool ask = settings.value("Browser-Tabs-Settings/" + settingsKey, true).toBool();

    if (ask) {
        CheckBoxDialog dialog(QMessageBox::Yes | QMessageBox::No, mApp->activeWindow());
        dialog.setDefaultButton(QMessageBox::No);
        dialog.setWindowTitle(title);
        dialog.setText(description);
        dialog.setCheckBoxText(i18n("Don't ask again"));
        dialog.setIcon(QMessageBox::Question);

        if (dialog.exec() != QMessageBox::Yes) {
            return false;
        }

        if (dialog.isChecked()) {
            settings.setValue("Browser-Tabs-Settings/" + settingsKey, false);
        }
    }

    return true;
}

void TabContextMenu::closeAllButCurrent()
{
    if (canCloseTabs(QLatin1String("AskOnClosingAllButCurrent"), i18n("Close Tabs"), i18n("Do you really want to close other tabs?"))) {
        emit closeAllButCurrent(m_clickedTab);
    }
}

void TabContextMenu::closeToRight()
{
    const QString label = m_options & HorizontalTabs
            ? i18n("Do you really want to close all tabs to the right?")
            : i18n("Do you really want to close all tabs to the bottom?");

    if (canCloseTabs(QLatin1String("AskOnClosingToRight"), i18n("Close Tabs"), label)) {
        emit closeToRight(m_clickedTab);
    }
}

void TabContextMenu::closeToLeft()
{
    const QString label = m_options & HorizontalTabs
            ? i18n("Do you really want to close all tabs to the left?")
            : i18n("Do you really want to close all tabs to the top?");

    if (canCloseTabs(QLatin1String("AskOnClosingToLeft"), i18n("Close Tabs"), label)) {
        emit closeToLeft(m_clickedTab);
    }
}

void TabContextMenu::init()
{
    TabWidget *tabWidget = m_window->tabWidget();
    if (m_clickedTab != -1) {
        WebTab* webTab = tabWidget->webTab(m_clickedTab);
        if (!webTab) {
            return;
        }

        if (m_window->weView(m_clickedTab)->isLoading()) {
            addAction(QIcon::fromTheme(QSL("process-stop")), i18n("&Stop Tab"), this, SLOT(stopTab()));
        }
        else {
            addAction(QIcon::fromTheme(QSL("view-refresh")), i18n("&Reload Tab"), this, SLOT(reloadTab()));
        }

        addAction(QIcon::fromTheme("tab-duplicate"), i18n("&Duplicate Tab"), this, SLOT(duplicateTab()));

        if (m_options & ShowDetachTabAction && (mApp->windowCount() > 1 || tabWidget->count() > 1)) {
            addAction(QIcon::fromTheme("tab-detach"), i18n("D&etach Tab"), this, SLOT(detachTab()));
        }

        addAction(webTab->isPinned() ? i18n("Un&pin Tab") : i18n("&Pin Tab"), this, &TabContextMenu::pinTab);
        addAction(webTab->isMuted() ? i18n("Un&mute Tab") : i18n("&Mute Tab"), this, &TabContextMenu::muteTab);

        if (!webTab->isRestored()) {
            addAction(i18n("Load Tab"), this, SLOT(loadTab()));
        } else {
            addAction(i18n("Unload Tab"), this, SLOT(unloadTab()));
        }

        addSeparator();
        addAction(i18n("Re&load All Tabs"), tabWidget, &TabWidget::reloadAllTabs);
        addAction(i18n("Bookmark &All Tabs"), m_window, &BrowserWindow::bookmarkAllTabs);
        addSeparator();

        if (m_options & ShowCloseOtherTabsActions) {
            addAction(i18n("Close Ot&her Tabs"), this, SLOT(closeAllButCurrent()));
            addAction(m_options & HorizontalTabs ? i18n("Close Tabs To The Right") : i18n("Close Tabs To The Bottom"), this, SLOT(closeToRight()));
            addAction(m_options & HorizontalTabs ? i18n("Close Tabs To The Left") : i18n("Close Tabs To The Top"), this, SLOT(closeToLeft()));
            addSeparator();
        }

        addAction(m_window->action(QSL("Other/RestoreClosedTab")));
        addAction(QIcon::fromTheme("window-close"), i18n("Cl&ose Tab"), this, &TabContextMenu::closeTab);
    } else {
        addAction(IconProvider::newTabIcon(), i18n("&New tab"), m_window, &BrowserWindow::addTab);
        addSeparator();
        addAction(i18n("Reloa&d All Tabs"), tabWidget, &TabWidget::reloadAllTabs);
        addAction(i18n("Bookmark &All Tabs"), m_window, &BrowserWindow::bookmarkAllTabs);
        addSeparator();
        addAction(m_window->action(QSL("Other/RestoreClosedTab")));
    }

    m_window->action(QSL("Other/RestoreClosedTab"))->setEnabled(tabWidget->canRestoreTab());
    connect(this, &QMenu::aboutToHide, this, [this]() {
        m_window->action(QSL("Other/RestoreClosedTab"))->setEnabled(true);
    });
}

void TabContextMenu::pinTab()
{
    WebTab* webTab = m_window->tabWidget()->webTab(m_clickedTab);
    if (webTab) {
        webTab->togglePinned();
    }
}

void TabContextMenu::muteTab()
{
    WebTab* webTab = m_window->tabWidget()->webTab(m_clickedTab);
    if (webTab) {
        webTab->toggleMuted();
    }
}
