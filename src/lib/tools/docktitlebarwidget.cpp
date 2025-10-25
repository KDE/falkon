/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#include "docktitlebarwidget.h"

#include <QActionGroup>
#include <QMenu>

#include "iconprovider.h"
#include "mainapplication.h"
#include "sidebar.h"

DockTitleBarWidget::DockTitleBarWidget(const QString &title, QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    closeButton->setIcon(QIcon(IconProvider::standardIcon(QStyle::SP_DialogCloseButton).pixmap(16)));

    m_menuTitle = new QMenu(this);

    titleButton->setText(title);
    titleButton->setMenu(m_menuTitle);
    titleButton->setShowMenuInside(true);

    connect(closeButton, &QAbstractButton::clicked, parent, &QWidget::close);
    connect(titleButton, &ToolButton::aboutToShowMenu, this, &DockTitleBarWidget::aboutToShowTitleMenu);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void DockTitleBarWidget::setTitle(const QString &title)
{
    titleButton->setText(title);
}

void DockTitleBarWidget::aboutToShowTitleMenu()
{
    m_menuTitle->clear();

    mApp->getWindow()->sideBarManager()->createMenu(m_menuTitle, false);

    m_menuTitle->addSeparator();

    m_menuTitle->addAction(tr("&Close SideBar"), parentWidget(), &QWidget::close);
}

DockTitleBarWidget::~DockTitleBarWidget()
= default;
