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
#include "browsinglibrary.h"
#include "ui_browsinglibrary.h"
#include "bookmarksimport/bookmarksimportdialog.h"
#include "bookmarksexport/bookmarksexportdialog.h"
#include "historymanager.h"
#include "bookmarksmanager.h"
#include "mainapplication.h"
#include "qztools.h"
#include "settings.h"

#include <QMenu>
#include <QCloseEvent>

BrowsingLibrary::BrowsingLibrary(BrowserWindow* window, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::BrowsingLibrary)
    , m_historyManager(new HistoryManager(window))
    , m_bookmarksManager(new BookmarksManager(window))
{
    ui->setupUi(this);

    Settings settings;
    settings.beginGroup(QSL("BrowsingLibrary"));
    resize(settings.value(QSL("size"), QSize(760, 470)).toSize());
    m_historyManager->restoreState(settings.value(QSL("historyState"), QByteArray()).toByteArray());
    settings.endGroup();

    QzTools::centerWidgetOnScreen(this);

    QIcon historyIcon;
    historyIcon.addFile(QSL(":/icons/other/bighistory.svg"), QSize(), QIcon::Normal);
    historyIcon.addFile(QSL(":/icons/other/bighistory-selected.svg"), QSize(), QIcon::Selected);

    QIcon bookmarksIcon;
    bookmarksIcon.addFile(QSL(":/icons/other/bigstar.svg"), QSize(), QIcon::Normal);
    bookmarksIcon.addFile(QSL(":/icons/other/bigstar-selected.svg"), QSize(), QIcon::Selected);

    ui->tabs->AddTab(m_historyManager, historyIcon, tr("History"));
    ui->tabs->AddTab(m_bookmarksManager, bookmarksIcon, tr("Bookmarks"));
    ui->tabs->SetMode(FancyTabWidget::Mode_LargeSidebar);
    ui->tabs->setFocus();

    auto* m = new QMenu(this);
    m->addAction(tr("Import Bookmarks..."), this, &BrowsingLibrary::importBookmarks);
    m->addAction(tr("Export Bookmarks..."), this, &BrowsingLibrary::exportBookmarks);
    ui->importExport->setMenu(m);

    if (mApp->isPrivate()) {
        ui->searchLine->setInputMethodHints(Qt::InputMethodHint::ImhSensitiveData);
    }

    connect(ui->tabs, &FancyTabWidget::CurrentChanged, ui->searchLine, &QLineEdit::clear);
    connect(ui->searchLine, &QLineEdit::textChanged, this, &BrowsingLibrary::search);

    QzTools::setWmClass(QSL("Browsing Library"), this);
}

void BrowsingLibrary::search()
{
    if (ui->tabs->current_index() == 0) {
        m_historyManager->search(ui->searchLine->text());
    }
    else {
        m_bookmarksManager->search(ui->searchLine->text());
    }
}

void BrowsingLibrary::importBookmarks()
{
    auto* d = new BookmarksImportDialog(this);
    d->open();
}

void BrowsingLibrary::exportBookmarks()
{
    auto* d = new BookmarksExportDialog(this);
    d->open();
}

void BrowsingLibrary::showHistory(BrowserWindow* window)
{
    ui->tabs->SetCurrentIndex(0);
    show();
    m_historyManager->setMainWindow(window);

    raise();
    activateWindow();
}

void BrowsingLibrary::showBookmarks(BrowserWindow* window)
{
    ui->tabs->SetCurrentIndex(1);
    show();
    m_bookmarksManager->setMainWindow(window);

    raise();
    activateWindow();
}

void BrowsingLibrary::closeEvent(QCloseEvent* e)
{
    Settings settings;
    settings.beginGroup(QSL("BrowsingLibrary"));
    settings.setValue(QSL("size"), size());
    settings.setValue(QSL("historyState"), m_historyManager->saveState());
    settings.endGroup();
    e->accept();
}

void BrowsingLibrary::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape
        || (e->key() == Qt::Key_W && e->modifiers() == Qt::ControlModifier)) {
        close();
    }

    QWidget::keyPressEvent(e);
}

BrowsingLibrary::~BrowsingLibrary()
{
    delete ui;
}
