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

Q_GLOBAL_STATIC(QmlWindowData, windowData)

QmlTab::QmlTab(WebTab *webTab, QObject *parent)
    : QObject(parent)
    , m_webTab(webTab)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

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
}

/**
 * @brief Get the url of the tab
 * @return String representing the url of the tab
 */
QString QmlTab::url() const
{
    if (!m_webTab) {
        return QString();
    }

    return QString::fromUtf8(m_webTab->url().toEncoded());
}

/**
 * @brief Get the title of the tab
 * @return String representing the title of the tab
 */
QString QmlTab::title() const
{
    if (!m_webTab) {
        return QString();
    }

    return m_webTab->title();
}

/**
 * @brief Get the zoom level of the tab
 *
 * Zoom level will have the following values
 * <table>
 * <caption>Zoom Levels</caption>
 * <tr><th>Zoom Level</th><th>Zoom Percentage</th></tr>
 * <tr><td>0</td><td>30</td></tr>
 * <tr><td>1</td><td>40</td></tr>
 * <tr><td>2</td><td>50</td></tr>
 * <tr><td>3</td><td>67</td></tr>
 * <tr><td>4</td><td>80</td></tr>
 * <tr><td>5</td><td>90</td></tr>
 * <tr><td>6</td><td>100</td></tr>
 * <tr><td>7</td><td>110</td></tr>
 * <tr><td>8</td><td>120</td></tr>
 * <tr><td>9</td><td>133</td></tr>
 * <tr><td>10</td><td>150</td></tr>
 * <tr><td>11</td><td>170</td></tr>
 * <tr><td>12</td><td>200</td></tr>
 * <tr><td>13</td><td>220</td></tr>
 * <tr><td>14</td><td>233</td></tr>
 * <tr><td>15</td><td>250</td></tr>
 * <tr><td>16</td><td>270</td></tr>
 * <tr><td>17</td><td>285</td></tr>
 * <tr><td>18</td><td>300</td></tr>
 * </table>
 *
 * @return Integer representing the zoom level of the tab
 */
int QmlTab::zoomLevel() const
{
    if (!m_webTab) {
        return -1;
    }

    return m_webTab->zoomLevel();
}

/**
 * @brief Get the index of the tab
 * @return Integer representing index if the tab exists, else -1
 */
int QmlTab::index() const
{
    if (!m_webTab) {
        return -1;
    }

    return m_webTab->tabIndex();
}

/**
 * @brief Checks if the tab is pinned
 * @return True if the tab is pinned, else false
 */
bool QmlTab::pinned() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isPinned();
}

/**
 * @brief Checks if the tab is muted
 * @return True if the tab is muted, else false
 */
bool QmlTab::muted() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isMuted();
}

/**
 * @brief Checks if the tab is restored
 * @return True if the tab is restored, else false
 */
bool QmlTab::restored() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isRestored();
}

/**
 * @brief Checks if the tab is current tab
 * @return True if the tab is current tab, else false
 */
bool QmlTab::current() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isCurrentTab();
}

/**
 * @brief Checks if the tab is in playing state
 * @return True if the tab is in playing state, else false
 */
bool QmlTab::playing() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->isPlaying();
}

/**
 * @brief Get the browser window of the tab
 * @return Browser window of type [QmlWindow](@ref QmlWindow), representing
 *         the window associated with the tab
 */
QmlWindow *QmlTab::browserWindow() const
{
    if (!m_webTab) {
        return nullptr;
    }

    return windowData->get(m_webTab->browserWindow());
}

/**
 * @brief Checks if the tab is loading
 * @return True if the tab is loading, else false
 */
bool QmlTab::loading() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->webView()->isLoading();
}

/**
 * @brief Get the loading progress of the tab
 * @return Integer representing the loading progress of the tab
 *         if the tab exists, else -1
 */
int QmlTab::loadingProgress() const
{
    if (!m_webTab) {
        return -1;
    }

    return m_webTab->webView()->loadingProgress();
}

/**
 * @brief Checks if there is a background activity associated
 *        with the tab
 * @return True if the background activity is associated, else false
 */
bool QmlTab::backgroundActivity() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->webView()->backgroundActivity();
}

/**
 * @brief Checks if webview associated with the tab can go back
 * @return True if can go back, else false
 */
bool QmlTab::canGoBack() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->webView()->history()->canGoBack();
}

/**
 * @brief Checks if webview associated with the tab can go forward
 * @return True if can go forward, else false
 */
bool QmlTab::canGoForward() const
{
    if (!m_webTab) {
        return false;
    }

    return m_webTab->webView()->history()->canGoForward();
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
