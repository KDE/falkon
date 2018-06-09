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
#include <QWebEngineHistory>
#include <QQmlEngine>

QmlTab::QmlTab(WebTab *webTab, QObject *parent)
    : QObject(parent)
    , m_webTab(webTab)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    connect(m_webTab, &WebTab::titleChanged, this, [this](const QString &title){
        QVariantMap map;
        map.insert(QSL("title"), title);
        emit titleChanged(map);
    });

    connect(m_webTab, &WebTab::pinnedChanged, this, [this](bool pinned){
        QVariantMap map;
        map.insert(QSL("pinned"), pinned);
        emit pinnedChanged(map);
    });

    connect(m_webTab, &WebTab::loadingChanged, this, [this](bool loading){
        QVariantMap map;
        map.insert(QSL("loading"), loading);
        emit loadingChanged(map);
    });

    connect(m_webTab, &WebTab::mutedChanged, this, [this](bool muted){
        QVariantMap map;
        map.insert(QSL("muted"), muted);
        emit mutedChanged(map);
    });

    connect(m_webTab, &WebTab::restoredChanged, this, [this](bool restored){
        QVariantMap map;
        map.insert(QSL("restored"), restored);
        emit restoredChanged(map);
    });

    connect(m_webTab, &WebTab::playingChanged, this, [this](bool playing){
        QVariantMap map;
        map.insert(QSL("playing"), playing);
        emit playingChanged(map);
    });

    connect(m_webTab->webView(), &TabbedWebView::zoomLevelChanged, this, &QmlTab::zoomLevelChanged);
    connect(m_webTab->webView(), &TabbedWebView::backgroundActivityChanged, this, &QmlTab::backgroundActivityChanged);
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

    return new QmlWindow(m_webTab->browserWindow());
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

void QmlTab::detach()
{
    if (!m_webTab) {
        return;
    }

    m_webTab->detach();
}

void QmlTab::setZoomLevel(const QVariantMap &map)
{
    if (!m_webTab) {
        return;
    }

    if (!map.contains(QSL("zoomLevel"))) {
        qDebug() << "Unable to set zoomLevel:" << "zoomLevel not defined";
        return;
    }

    int zoomLevel = map.value(QSL("zoomLevel")).toInt();
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

void QmlTab::load(const QVariantMap &map)
{
    if (!m_webTab) {
        return;
    }

    if (!map.contains(QSL("url"))) {
        qDebug() << "Unable to load tab:" << "zoomLevel not defined";
        return;
    }

    QString url = map.value(QSL("url")).toString();
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

QmlTabData::QmlTabData()
{
}

QmlTabData::~QmlTabData()
{
    for (QmlTab *tab : m_tabs.values()) {
        tab->deleteLater();
    }
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
