/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#include "qmltab.h"
#include "loadrequest.h"
#include "tabbedwebview.h"
#include "webpage.h"
#include <QWebEngineHistory>
#include <QQmlEngine>

Q_GLOBAL_STATIC(QmlWindowData, windowData)

QmlTab::QmlTab(WebTab *webTab, QObject *parent)
    : QObject(parent)
    , m_webTab(webTab)
    , m_webPage(nullptr)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (!m_webTab) {
        return;
    }

    createConnections();
}

/**
 * @brief Detaches the tab
 */
void QmlTab::detach()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->detach();
}

/**
 * @brief Set the zoom level of the tab
 * @param Integer representing the zoom level
 */
void QmlTab::setZoomLevel(int zoomLevel)
{
    if (!m_webTab) {
        return;
    }

    m_webTab->setZoomLevel(zoomLevel);
}

/**
 * @brief Stops webview associated with the tab
 */
void QmlTab::stop()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->stop();
}

/**
 * @brief Reloads webview associated with the tab
 */
void QmlTab::reload()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->reload();
}

/**
 * @brief Unloads the tab
 */
void QmlTab::unload()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->unload();
}

/**
 * @brief Loads webview associated with the tab
 * @param String representing the url to load
 */
void QmlTab::load(const QString &url)
{
    if (!m_webTab) {
        return;
    }

    LoadRequest req;
    req.setUrl(QUrl::fromEncoded(url.toUtf8()));
    m_webTab->load(req);
}

/**
 * @brief Decreases the zoom level of the tab
 */
void QmlTab::zoomIn()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->zoomIn();
}

/**
 * @brief Increases the zoom level of the tab
 */
void QmlTab::zoomOut()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->zoomOut();
}

/**
 * @brief Resets the tab zoom level
 */
void QmlTab::zoomReset()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->zoomReset();
}

/**
 * @brief Performs edit undo on the tab
 */
void QmlTab::undo()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->editUndo();
}

/**
 * @brief Performs edit redo on the tab
 */
void QmlTab::redo()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->editRedo();
}

/**
 * @brief Performs edit select-all on the tab
 */
void QmlTab::selectAll()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->editSelectAll();
}

/**
 * @brief Reloads the tab by bypassing the cache
 */
void QmlTab::reloadBypassCache()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->reloadBypassCache();
}

/**
 * @brief Loads the previous page
 */
void QmlTab::back()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->back();
}

/**
 * @brief Loads the next page
 */
void QmlTab::forward()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->forward();
}

/**
 * @brief Prints the page
 */
void QmlTab::printPage()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->printPage();
}

/**
 * @brief Shows the page source
 */
void QmlTab::showSource()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->showSource();
}

/**
 * @brief Sends page by mail
 */
void QmlTab::sendPageByMail()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->sendPageByMail();
}

/**
 * @brief execute JavaScript function in a page
 * @param value, representing JavaScript function
 * @return QVariant, the return value of executed javascript
 */
QVariant QmlTab::execJavaScript(const QJSValue &value)
{
    if (!m_webPage && !m_webTab) {
        return QVariant();
    }
    WebPage *webPage = m_webPage;
    if (!m_webPage) {
        webPage = m_webTab->webView()->page();
    }
    return webPage->execJavaScript(value.toString());
}

/**
 * @brief Gets result of web hit test at a given point
 * @param point
 * @return result of web hit test
 */
QmlWebHitTestResult *QmlTab::hitTestContent(const QPoint &point)
{
    if (!m_webPage && !m_webTab) {
        return nullptr;
    }
    WebPage *webPage = m_webPage;
    if (!m_webPage) {
        webPage = m_webTab->webView()->page();
    }
    WebHitTestResult result = webPage->hitTestContent(point);
    return new QmlWebHitTestResult(result);
}

QString QmlTab::url() const
{
    if (!m_webTab) {
        return QString();
    }

    return QString::fromUtf8(m_webTab->url().toEncoded());
}

QString QmlTab::title() const
{
    if (!m_webTab) {
        return QString();
    }

    return m_webTab->title();
}


int QmlTab::zoomLevel() const
{
    if (!m_webTab) {
        return -1;
    }

    return m_webTab->zoomLevel();
}

int QmlTab::index() const
{
    if (!m_webTab) {
        return -1;
    }

    return m_webTab->tabIndex();
}

bool QmlTab::pinned() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isPinned();
}

bool QmlTab::muted() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isMuted();
}

bool QmlTab::restored() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isRestored();
}

bool QmlTab::current() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isCurrentTab();
}

bool QmlTab::playing() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isPlaying();
}

QmlWindow *QmlTab::browserWindow() const
{
    if (!m_webTab) {
        return nullptr;
    }

    return windowData->get(m_webTab->browserWindow());
}

bool QmlTab::loading() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->webView()->isLoading();
}

int QmlTab::loadingProgress() const
{
    if (!m_webTab) {
        return -1;
    }

    return m_webTab->webView()->loadingProgress();
}

bool QmlTab::backgroundActivity() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->webView()->backgroundActivity();
}

bool QmlTab::canGoBack() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->webView()->history()->canGoBack();
}

bool QmlTab::canGoForward() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->webView()->history()->canGoForward();
}

void QmlTab::setWebPage(WebPage *webPage)
{
    m_webPage = webPage;
    TabbedWebView *tabbedWebView = qobject_cast<TabbedWebView*>(m_webPage->view());
    m_webTab = tabbedWebView->webTab();
    if (m_webTab) {
        createConnections();
    }
}

void QmlTab::createConnections()
{
    connect(m_webTab, &WebTab::titleChanged, this, [this](const QString &title){
        emit titleChanged(title);
    });

    connect(m_webTab, &WebTab::pinnedChanged, this, [this](bool pinned){
        emit pinnedChanged(pinned);
    });

    connect(m_webTab, &WebTab::loadingChanged, this, [this](bool loading){
        emit loadingChanged(loading);
    });

    connect(m_webTab, &WebTab::mutedChanged, this, [this](bool muted){
        emit mutedChanged(muted);
    });

    connect(m_webTab, &WebTab::restoredChanged, this, [this](bool restored){
        emit restoredChanged(restored);
    });

    connect(m_webTab, &WebTab::playingChanged, this, [this](bool playing){
        emit playingChanged(playing);
    });

    connect(m_webTab->webView(), &TabbedWebView::zoomLevelChanged, this, &QmlTab::zoomLevelChanged);
    connect(m_webTab->webView(), &TabbedWebView::backgroundActivityChanged, this, &QmlTab::backgroundActivityChanged);

    if (m_webPage) {
        connect(m_webPage, &WebPage::navigationRequestAccepted, this, &QmlTab::navigationRequestAccepted);
    }
}

QmlTabData::QmlTabData()
{
}

QmlTabData::~QmlTabData()
{
    qDeleteAll(m_tabs);
}

QmlTab *QmlTabData::get(WebTab *webTab)
{
    QmlTab *tab = m_tabs.value(webTab);
    if (!tab) {
        tab = new QmlTab(webTab);
        m_tabs.insert(webTab, tab);
    }
    return tab;
}
