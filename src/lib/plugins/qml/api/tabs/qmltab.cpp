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
#include "qml/qmlstaticdata.h"
#include <QWebEngineHistory>
#include <QQmlEngine>

QmlTab::QmlTab(WebTab *webTab, QObject *parent)
    : QObject(parent)
    , m_webTab(webTab)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (!m_webTab) {
        return;
    }

    createConnections();
}

void QmlTab::detach()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->detach();
}

void QmlTab::setZoomLevel(int zoomLevel)
{
    if (!m_webTab) {
        return;
    }

    m_webTab->setZoomLevel(zoomLevel);
}

void QmlTab::stop()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->stop();
}

void QmlTab::reload()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->reload();
}

void QmlTab::unload()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->unload();
}

void QmlTab::load(const QString &url)
{
    if (!m_webTab) {
        return;
    }

    LoadRequest req;
    req.setUrl(QUrl::fromEncoded(url.toUtf8()));
    m_webTab->load(req);
}

void QmlTab::zoomIn()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->zoomIn();
}

void QmlTab::zoomOut()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->zoomOut();
}

void QmlTab::zoomReset()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->zoomReset();
}

void QmlTab::undo()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->editUndo();
}

void QmlTab::redo()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->editRedo();
}

void QmlTab::selectAll()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->editSelectAll();
}

void QmlTab::reloadBypassCache()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->reloadBypassCache();
}

void QmlTab::back()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->back();
}

void QmlTab::forward()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->forward();
}

void QmlTab::printPage()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->printPage();
}

void QmlTab::showSource()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->showSource();
}

void QmlTab::sendPageByMail()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->webView()->sendPageByMail();
}

QVariant QmlTab::execJavaScript(const QJSValue &value)
{
    if (!m_webPage && !m_webTab) {
        return {};
    }
    WebPage *webPage = m_webPage;
    if (!m_webPage) {
        webPage = m_webTab->webView()->page();
    }
    return webPage->execJavaScript(value.toString());
}

QmlWebHitTestResult *QmlTab::hitTestContent(const QPoint &point)
{
    if (!m_webPage && !m_webTab) {
        return nullptr;
    }
    WebPage *webPage = m_webPage;
    if (!m_webPage) {
        webPage = m_webTab->webView()->page();
    }
    const WebHitTestResult result = webPage->hitTestContent(point);
    return new QmlWebHitTestResult(result);
}

QString QmlTab::url() const
{
    if (!m_webTab) {
        return {};
    }

    return QString::fromUtf8(m_webTab->url().toEncoded());
}

QString QmlTab::title() const
{
    if (!m_webTab) {
        return {};
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

    return QmlStaticData::instance().getWindow(m_webTab->browserWindow());
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
    if (m_webPage) {
        removeConnections();
    }
    m_webPage = webPage;
    auto *tabbedWebView = qobject_cast<TabbedWebView*>(m_webPage->view());
    m_webTab = tabbedWebView->webTab();
    if (m_webTab) {
        createConnections();
    }
}

void QmlTab::createConnections()
{
    Q_ASSERT(m_lambdaConnections.length() == 0);

    auto titleChangedConnection = connect(m_webTab, &WebTab::titleChanged, this, [this](const QString &title){
        Q_EMIT titleChanged(title);
    });
    m_lambdaConnections.append(titleChangedConnection);

    auto pinnedChangedConnection = connect(m_webTab, &WebTab::pinnedChanged, this, [this](bool pinned){
        Q_EMIT pinnedChanged(pinned);
    });
    m_lambdaConnections.append(pinnedChangedConnection);

    auto loadingChangedConnection = connect(m_webTab, &WebTab::loadingChanged, this, [this](bool loading){
        Q_EMIT loadingChanged(loading);
    });
    m_lambdaConnections.append(loadingChangedConnection);

    auto mutedChangedConnection = connect(m_webTab, &WebTab::mutedChanged, this, [this](bool muted){
        Q_EMIT mutedChanged(muted);
    });
    m_lambdaConnections.append(mutedChangedConnection);

    auto restoredChangedConnection = connect(m_webTab, &WebTab::restoredChanged, this, [this](bool restored){
        Q_EMIT restoredChanged(restored);
    });
    m_lambdaConnections.append(restoredChangedConnection);

    auto playingChangedConnection = connect(m_webTab, &WebTab::playingChanged, this, [this](bool playing){
        Q_EMIT playingChanged(playing);
    });
    m_lambdaConnections.append(playingChangedConnection);

    connect(m_webTab->webView(), &TabbedWebView::zoomLevelChanged, this, &QmlTab::zoomLevelChanged);
    connect(m_webTab->webView(), &TabbedWebView::backgroundActivityChanged, this, &QmlTab::backgroundActivityChanged);

    if (m_webPage) {
        connect(m_webPage, &WebPage::navigationRequestAccepted, this, &QmlTab::navigationRequestAccepted);
    }
}

void QmlTab::removeConnections()
{
    disconnect(this);
}
