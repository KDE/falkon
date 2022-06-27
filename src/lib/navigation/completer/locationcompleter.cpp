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
#include "locationcompleter.h"
#include "locationcompletermodel.h"
#include "locationcompleterview.h"
#include "locationcompleterrefreshjob.h"
#include "locationbar.h"
#include "mainapplication.h"
#include "browserwindow.h"
#include "tabbedwebview.h"
#include "tabwidget.h"
#include "history.h"
#include "bookmarks.h"
#include "bookmarkitem.h"
#include "qzsettings.h"
#include "opensearchengine.h"
#include "networkmanager.h"
#include "searchenginesdialog.h"

#include <QWindow>

LocationCompleterView* LocationCompleter::s_view = 0;
LocationCompleterModel* LocationCompleter::s_model = 0;

LocationCompleter::LocationCompleter(QObject* parent)
    : QObject(parent)
    , m_window(0)
    , m_locationBar(0)
    , m_lastRefreshTimestamp(0)
    , m_popupClosed(false)
{
    if (!s_view) {
        s_model = new LocationCompleterModel;
        s_view = new LocationCompleterView;
        s_view->setModel(s_model);
    }
}

void LocationCompleter::setMainWindow(BrowserWindow* window)
{
    m_window = window;
}

void LocationCompleter::setLocationBar(LocationBar* locationBar)
{
    m_locationBar = locationBar;
}

bool LocationCompleter::isVisible() const
{
    return s_view->isVisible();
}

void LocationCompleter::closePopup()
{
    s_view->close();
}

void LocationCompleter::complete(const QString &string)
{
    QString trimmedStr = string.trimmed();

    // Indicates that new completion was requested by user
    // Eg. popup was not closed yet this completion session
    m_popupClosed = false;

    Q_EMIT cancelRefreshJob();

    auto* job = new LocationCompleterRefreshJob(trimmedStr);
    connect(job, &LocationCompleterRefreshJob::finished, this, &LocationCompleter::refreshJobFinished);
    connect(this, SIGNAL(cancelRefreshJob()), job, SLOT(jobCancelled()));

    if (qzSettings->searchFromAddressBar && qzSettings->showABSearchSuggestions && trimmedStr.length() >= 2) {
        if (!m_openSearchEngine) {
            m_openSearchEngine = new OpenSearchEngine(this);
            m_openSearchEngine->setNetworkAccessManager(mApp->networkManager());
            connect(m_openSearchEngine, &OpenSearchEngine::suggestions, this, &LocationCompleter::addSuggestions);
        }
        m_openSearchEngine->setSuggestionsUrl(LocationBar::searchEngine().suggestionsUrl);
        m_openSearchEngine->setSuggestionsParameters(LocationBar::searchEngine().suggestionsParameters);
        m_suggestionsTerm = trimmedStr;
        m_openSearchEngine->requestSuggestions(m_suggestionsTerm);
    } else {
        m_oldSuggestions.clear();
    }

    // Add/update search/visit item
    QTimer::singleShot(0, this, [=]() {
        const QModelIndex index = s_model->index(0, 0);
        if (index.data(LocationCompleterModel::VisitSearchItemRole).toBool()) {
            s_model->setData(index, trimmedStr, Qt::DisplayRole);
            s_model->setData(index, trimmedStr, LocationCompleterModel::UrlRole);
            s_model->setData(index, m_locationBar->text(), LocationCompleterModel::SearchStringRole);
        } else {
            auto *item = new QStandardItem();
            item->setText(trimmedStr);
            item->setData(trimmedStr, LocationCompleterModel::UrlRole);
            item->setData(m_locationBar->text(), LocationCompleterModel::SearchStringRole);
            item->setData(true, LocationCompleterModel::VisitSearchItemRole);
            s_model->setCompletions({item});
            addSuggestions(m_oldSuggestions);
        }
        showPopup();
        if (!s_view->currentIndex().isValid()) {
            m_ignoreCurrentChanged = true;
            s_view->setCurrentIndex(s_model->index(0, 0));
            m_ignoreCurrentChanged = false;
        }
    });
}

void LocationCompleter::showMostVisited()
{
    m_locationBar->setFocus();
    complete(QString());
}

void LocationCompleter::refreshJobFinished()
{
    auto* job = qobject_cast<LocationCompleterRefreshJob*>(sender());
    Q_ASSERT(job);

    // Don't show results of older jobs
    // Also don't open the popup again when it was already closed
    if (!job->isCanceled() && job->timestamp() > m_lastRefreshTimestamp && !m_popupClosed) {
        s_model->setCompletions(job->completions());
        addSuggestions(m_oldSuggestions);
        showPopup();

        m_lastRefreshTimestamp = job->timestamp();

        if (!s_view->currentIndex().isValid() && s_model->index(0, 0).data(LocationCompleterModel::VisitSearchItemRole).toBool()) {
            m_ignoreCurrentChanged = true;
            s_view->setCurrentIndex(s_model->index(0, 0));
            m_ignoreCurrentChanged = false;
        }

        if (qzSettings->useInlineCompletion) {
            Q_EMIT showDomainCompletion(job->domainCompletion());
        }

        s_model->setData(s_model->index(0, 0), m_locationBar->text(), LocationCompleterModel::SearchStringRole);
    }

    job->deleteLater();
}

void LocationCompleter::slotPopupClosed()
{
    m_popupClosed = true;
    m_oldSuggestions.clear();

    disconnect(s_view, &LocationCompleterView::closed, this, &LocationCompleter::slotPopupClosed);
    disconnect(s_view, &LocationCompleterView::indexActivated, this, &LocationCompleter::indexActivated);
    disconnect(s_view, &LocationCompleterView::indexCtrlActivated, this, &LocationCompleter::indexCtrlActivated);
    disconnect(s_view, &LocationCompleterView::indexShiftActivated, this, &LocationCompleter::indexShiftActivated);
    disconnect(s_view, &LocationCompleterView::indexDeleteRequested, this, &LocationCompleter::indexDeleteRequested);
    disconnect(s_view, &LocationCompleterView::loadRequested, this, &LocationCompleter::loadRequested);
    disconnect(s_view, &LocationCompleterView::searchEnginesDialogRequested, this, &LocationCompleter::openSearchEnginesDialog);
    disconnect(s_view->selectionModel(), &QItemSelectionModel::currentChanged, this, &LocationCompleter::currentChanged);

    Q_EMIT popupClosed();
}

void LocationCompleter::addSuggestions(const QStringList &suggestions)
{
    const auto suggestionItems = s_model->suggestionItems();

    // Delete existing suggestions
    for (QStandardItem *item : suggestionItems) {
        s_model->takeRow(item->row());
        delete item;
    }

    // Add new suggestions
    QList<QStandardItem*> items;
    for (const QString &suggestion : suggestions) {
        auto* item = new QStandardItem();
        item->setText(suggestion);
        item->setData(suggestion, LocationCompleterModel::TitleRole);
        item->setData(suggestion, LocationCompleterModel::UrlRole);
        item->setData(m_suggestionsTerm, LocationCompleterModel::SearchStringRole);
        item->setData(true, LocationCompleterModel::SearchSuggestionRole);
        items.append(item);
    }

    s_model->addCompletions(items);
    m_oldSuggestions = suggestions;

    if (!m_popupClosed) {
        showPopup();
    }
}

void LocationCompleter::currentChanged(const QModelIndex &index)
{
    if (m_ignoreCurrentChanged) {
        return;
    }

    QString completion = index.data().toString();

    bool completeDomain = index.data(LocationCompleterModel::VisitSearchItemRole).toBool();

    const QString originalText = s_model->index(0, 0).data(LocationCompleterModel::SearchStringRole).toString();

    // Domain completion was dismissed
    if (completeDomain && completion == originalText) {
        completeDomain = false;
    }

    if (completion.isEmpty()) {
        completeDomain = true;
        completion = originalText;
    }

    Q_EMIT showCompletion(completion, completeDomain);
}

void LocationCompleter::indexActivated(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());

    closePopup();

    // Clear locationbar
    Q_EMIT clearCompletion();

    bool ok;
    const int tabPos = index.data(LocationCompleterModel::TabPositionTabRole).toInt(&ok);

    // Switch to tab with simple index activation
    if (ok && tabPos > -1) {
        BrowserWindow* window = static_cast<BrowserWindow*>(index.data(LocationCompleterModel::TabPositionWindowRole).value<void*>());
        Q_ASSERT(window);
        switchToTab(window, tabPos);
        return;
    }

    loadRequest(createLoadRequest(index));
}

void LocationCompleter::indexCtrlActivated(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(m_window);

    closePopup();

    // Clear locationbar
    Q_EMIT clearCompletion();

    // Load request in new tab
    m_window->tabWidget()->addView(createLoadRequest(index), Qz::NT_CleanSelectedTab);
}

void LocationCompleter::indexShiftActivated(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());

    closePopup();

    // Clear locationbar
    Q_EMIT clearCompletion();

    // Load request
    if (index.data(LocationCompleterModel::VisitSearchItemRole).toBool()) {
        loadRequest(LoadRequest(index.data(LocationCompleterModel::SearchStringRole).toUrl()));
    } else {
        loadRequest(createLoadRequest(index));
    }
}

void LocationCompleter::indexDeleteRequested(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    if (index.data(LocationCompleterModel::BookmarkRole).toBool()) {
        BookmarkItem* bookmark = static_cast<BookmarkItem*>(index.data(LocationCompleterModel::BookmarkItemRole).value<void*>());
        mApp->bookmarks()->removeBookmark(bookmark);
    } else if (index.data(LocationCompleterModel::HistoryRole).toBool()) {
        int id = index.data(LocationCompleterModel::IdRole).toInt();
        mApp->history()->deleteHistoryEntry(id);
    } else {
        return;
    }

    s_view->setUpdatesEnabled(false);
    s_model->removeRow(index.row(), index.parent());
    s_view->setUpdatesEnabled(true);

    showPopup();
}

LoadRequest LocationCompleter::createLoadRequest(const QModelIndex &index)
{
    LoadRequest request;
    BookmarkItem *bookmark = nullptr;

    if (index.data(LocationCompleterModel::HistoryRole).toBool()) {
        request = index.data(LocationCompleterModel::UrlRole).toUrl();
    } else if (index.data(LocationCompleterModel::BookmarkRole).toBool()) {
        bookmark = static_cast<BookmarkItem*>(index.data(LocationCompleterModel::BookmarkItemRole).value<void*>());
    } else if (index.data(LocationCompleterModel::SearchSuggestionRole).toBool()) {
        const QString text = index.data(LocationCompleterModel::TitleRole).toString();
        request = mApp->searchEnginesManager()->searchResult(LocationBar::searchEngine(), text);
    } else if (index.data(LocationCompleterModel::VisitSearchItemRole).toBool()) {
        const auto action = LocationBar::loadAction(index.data(LocationCompleterModel::SearchStringRole).toString());
        switch (action.type) {
        case LocationBar::LoadAction::Url:
        case LocationBar::LoadAction::Search:
            request = action.loadRequest;
            break;
        case LocationBar::LoadAction::Bookmark:
            bookmark = action.bookmark;
            break;
        default:
            break;
        }
    }

    if (bookmark) {
        bookmark->updateVisitCount();
        request = bookmark->url();
    }

    return request;
}

void LocationCompleter::switchToTab(BrowserWindow* window, int tab)
{
    Q_ASSERT(window);
    Q_ASSERT(tab >= 0);

    TabWidget* tabWidget = window->tabWidget();

    if (window->isActiveWindow() || tabWidget->currentIndex() != tab) {
        tabWidget->setCurrentIndex(tab);
        window->show();
        window->activateWindow();
        window->raise();
    }
    else {
        tabWidget->webTab()->setFocus();
    }
}

void LocationCompleter::loadRequest(const LoadRequest &request)
{
    closePopup();

    // Show url in locationbar
    Q_EMIT showCompletion(request.url().toString(), false);

    // Load request
    Q_EMIT loadRequested(request);
}

void LocationCompleter::openSearchEnginesDialog()
{
    // Clear locationbar
    Q_EMIT clearCompletion();

    auto *dialog = new SearchEnginesDialog(m_window);
    dialog->open();
}

void LocationCompleter::showPopup()
{
    Q_ASSERT(m_window);
    Q_ASSERT(m_locationBar);

    if (!m_locationBar->hasFocus() || s_model->rowCount() == 0) {
        s_view->close();
        return;
    }

    if (s_view->isVisible()) {
        adjustPopupSize();
        return;
    }

    QRect popupRect(m_locationBar->mapToGlobal(m_locationBar->pos()), m_locationBar->size());
    popupRect.setY(popupRect.bottom());

    s_view->setGeometry(popupRect);
    s_view->setFocusProxy(m_locationBar);
    s_view->setCurrentIndex(QModelIndex());

    connect(s_view, &LocationCompleterView::closed, this, &LocationCompleter::slotPopupClosed);
    connect(s_view, &LocationCompleterView::indexActivated, this, &LocationCompleter::indexActivated);
    connect(s_view, &LocationCompleterView::indexCtrlActivated, this, &LocationCompleter::indexCtrlActivated);
    connect(s_view, &LocationCompleterView::indexShiftActivated, this, &LocationCompleter::indexShiftActivated);
    connect(s_view, &LocationCompleterView::indexDeleteRequested, this, &LocationCompleter::indexDeleteRequested);
    connect(s_view, &LocationCompleterView::loadRequested, this, &LocationCompleter::loadRequested);
    connect(s_view, &LocationCompleterView::searchEnginesDialogRequested, this, &LocationCompleter::openSearchEnginesDialog);
    connect(s_view->selectionModel(), &QItemSelectionModel::currentChanged, this, &LocationCompleter::currentChanged);

    s_view->createWinId();
    s_view->windowHandle()->setTransientParent(m_window->windowHandle());

    adjustPopupSize();
}

void LocationCompleter::adjustPopupSize()
{
    s_view->adjustSize();
    s_view->show();
}
