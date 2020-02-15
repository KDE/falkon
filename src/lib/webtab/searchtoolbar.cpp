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
#include "searchtoolbar.h"
#include "webview.h"
#include "webpage.h"
#include "lineedit.h"
#include "ui_searchtoolbar.h"
#include "iconprovider.h"

#include <QKeyEvent>
#include <QShortcut>

SearchToolBar::SearchToolBar(WebView* view, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::SearchToolbar)
    , m_view(view)
    , m_findFlags(0)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    ui->closeButton->setIcon(IconProvider::instance()->standardIcon(QStyle::SP_DialogCloseButton));
    ui->next->setShortcut(QKeySequence("Ctrl+G"));
    ui->previous->setShortcut(QKeySequence("Ctrl+Shift+G"));

    ui->resultsInfo->hide();
#if QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    connect(view->page(), &QWebEnginePage::findTextFinished, this, &SearchToolBar::showSearchResults);
#endif

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->lineEdit, &QLineEdit::textEdited, this, &SearchToolBar::findNext);
    connect(ui->next, &QAbstractButton::clicked, this, &SearchToolBar::findNext);
    connect(ui->previous, &QAbstractButton::clicked, this, &SearchToolBar::findPrevious);
    connect(ui->caseSensitive, &QAbstractButton::clicked, this, &SearchToolBar::caseSensitivityChanged);

    QShortcut* findNextAction = new QShortcut(QKeySequence("F3"), this);
    connect(findNextAction, &QShortcut::activated, this, &SearchToolBar::findNext);

    QShortcut* findPreviousAction = new QShortcut(QKeySequence("Shift+F3"), this);
    connect(findPreviousAction, &QShortcut::activated, this, &SearchToolBar::findPrevious);

    parent->installEventFilter(this);
}

void SearchToolBar::showMinimalInPopupWindow()
{
    // Show only essentials widget + set minimum width
    ui->caseSensitive->hide();
    ui->horizontalLayout->setSpacing(2);
    ui->horizontalLayout->setContentsMargins(2, 6, 2, 6);
    setMinimumWidth(260);
}

void SearchToolBar::focusSearchLine()
{
    ui->lineEdit->setFocus();
}

void SearchToolBar::close()
{
    hide();
    searchText(QString());
    m_view->setFocus();
    deleteLater();
}

void SearchToolBar::findNext()
{
    m_findFlags = 0;
    updateFindFlags();

    searchText(ui->lineEdit->text());
}

void SearchToolBar::findPrevious()
{
    m_findFlags = QWebEnginePage::FindBackward;
    updateFindFlags();

    searchText(ui->lineEdit->text());
}

void SearchToolBar::updateFindFlags()
{
    if (ui->caseSensitive->isChecked()) {
        m_findFlags = m_findFlags | QWebEnginePage::FindCaseSensitively;
    }
    else {
        m_findFlags = m_findFlags & ~QWebEnginePage::FindCaseSensitively;
    }
}

void SearchToolBar::caseSensitivityChanged()
{
    updateFindFlags();

    searchText(QString());
    searchText(ui->lineEdit->text());
}

void SearchToolBar::setText(const QString &text)
{
    ui->lineEdit->setText(text);
}

void SearchToolBar::searchText(const QString &text)
{
    m_searchRequests++;
    QPointer<SearchToolBar> guard = this;
    m_view->findText(text, m_findFlags, [=](bool found) {
        if (!guard) {
            return;
        }
        if (--m_searchRequests != 0) {
            return;
        }
        if (ui->lineEdit->text().isEmpty())
            found = true;

        ui->lineEdit->setProperty("notfound", QVariant(!found));
        ui->lineEdit->style()->unpolish(ui->lineEdit);
        ui->lineEdit->style()->polish(ui->lineEdit);

        // Clear selection
        m_view->page()->runJavaScript(QSL("window.getSelection().empty();"), WebPage::SafeJsWorld);
    });
}

#if QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(5, 14, 0)
void SearchToolBar::showSearchResults(const QWebEngineFindTextResult &result)
{
    if (result.numberOfMatches() == 0) {
        ui->resultsInfo->hide();
        return;
    }

    ui->resultsInfo->setText(tr("%1 of %2").arg(
        QString::number(result.activeMatch()), QString::number(result.numberOfMatches())));
    ui->resultsInfo->show();
}
#endif

bool SearchToolBar::eventFilter(QObject* obj, QEvent* event)
{
    Q_UNUSED(obj);

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        switch (ke->key()) {
        case Qt::Key_Escape:
            close();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (ke->modifiers() & Qt::ShiftModifier) {
                findPrevious();
            } else {
                findNext();
            }
            break;
        default:
            break;
        }
    }

    return false;
}

SearchToolBar::~SearchToolBar()
{
    delete ui;
}
