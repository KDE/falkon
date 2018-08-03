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
#pragma once

#include <QObject>
#include "webtab.h"
#include "../windows/qmlwindow.h"
#include <QJSValue>
#include "qml/api/menus/qmlwebhittestresult.h"
#include <QWebEnginePage>

/**
 * @brief The class exposing a browser tab to QML
 */
class QmlTab : public QObject
{
    Q_OBJECT

    /**
     * @brief url of the tab
     */
    Q_PROPERTY(QString url READ url CONSTANT)

    /**
     * @brief title of the tab
     */
    Q_PROPERTY(QString title READ title CONSTANT)

    /**
     * @brief zoom level of the tab
     *
     * Zoom levels are from 0 to 18
     */
    Q_PROPERTY(int zoomLevel READ zoomLevel CONSTANT)

    /**
     * @brief index of the tab
     */
    Q_PROPERTY(int index READ index CONSTANT)

    /**
     * @brief checks if the tab is pinned
     */
    Q_PROPERTY(bool pinned READ pinned CONSTANT)

    /**
     * @brief checks if the tab is muted
     */
    Q_PROPERTY(bool muted READ muted CONSTANT)

    /**
     * @brief checks if the tab is restored
     */
    Q_PROPERTY(bool restored READ restored CONSTANT)

    /**
     * @brief checks if the tab is the current tab
     */
    Q_PROPERTY(bool current READ current CONSTANT)

    /**
     * @brief checks if the tab is playing
     */
    Q_PROPERTY(bool playing READ playing CONSTANT)

    /**
     * @brief window of the tab
     */
    Q_PROPERTY(QmlWindow* browserWindow READ browserWindow CONSTANT)

    /**
     * @brief checks if the tab is loading
     */
    Q_PROPERTY(bool loading READ loading CONSTANT)

    /**
     * @brief get the loading progress of the tab
     */
    Q_PROPERTY(int loadingProgress READ loadingProgress CONSTANT)

    /**
     * @brief checks if the tab has associated background activity
     */
    Q_PROPERTY(bool backgroundActivity READ backgroundActivity CONSTANT)

    /**
     * @brief checks if the tab is can go back
     */
    Q_PROPERTY(bool canGoBack READ canGoBack CONSTANT)

    /**
     * @brief checks if the tab is can go forward
     */
    Q_PROPERTY(bool canGoForward READ canGoForward CONSTANT)
public:
    explicit QmlTab(WebTab *webTab = nullptr, QObject *parent = nullptr);

    Q_INVOKABLE void detach();
    Q_INVOKABLE void setZoomLevel(int zoomLevel);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void reload();
    Q_INVOKABLE void unload();
    Q_INVOKABLE void load(const QString &url);
    Q_INVOKABLE void zoomIn();
    Q_INVOKABLE void zoomOut();
    Q_INVOKABLE void zoomReset();
    Q_INVOKABLE void undo();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void selectAll();
    Q_INVOKABLE void reloadBypassCache();
    Q_INVOKABLE void back();
    Q_INVOKABLE void forward();
    Q_INVOKABLE void printPage();
    Q_INVOKABLE void showSource();
    Q_INVOKABLE void sendPageByMail();
    Q_INVOKABLE QVariant execJavaScript(const QJSValue &value);
    Q_INVOKABLE QmlWebHitTestResult *hitTestContent(const QPoint &point);

    void setWebPage(WebPage *webPage);

Q_SIGNALS:
    /**
     * @brief The signal emitted when the tab title is changed
     * @param String representing the new title
     */
    void titleChanged(const QString &title);

    /**
     * @brief The signal emitted when pinned state of the tab is changed
     * @param Bool representing if a tab is pinned
     */
    void pinnedChanged(bool pinned);

    /**
     * @brief The signal emitted when loading state of the tab is changed
     * @param Bool representing if the tab is loading
     */
    void loadingChanged(bool loading);

    /**
     * @brief The signal emitted when muted state of the tab is changed
     * @param Bool representing if the tab is muted
     */
    void mutedChanged(bool muted);

    /**
     * @brief The signal emitted when restored state of the tab is changed
     * @param Bool representing if the tab is restored
     */
    void restoredChanged(bool restored);

    /**
     * @brief The signal emitted when playing state of the tab is changed
     * @param Bool representing if the tab is in playing state
     */
    void playingChanged(bool playing);

    /**
     * @brief The signal emitted when zoom level of the tab is changed
     * @param Integer representing the zoom level
     */
    void zoomLevelChanged(int zoomLevel);

    /**
     * @brief The signal emitted when background activity of the tab is changed
     * @param Bool representing if there is background activity attached to the tab
     */
    void backgroundActivityChanged(int backgroundActivityChanged);

    /**
     * @brief The signal emitted when navigation request is accepted
     * @param url, representing requested url
     * @param type of navigation
     * @param isMainFrame, represents if navigation is requested for a top level page.
     */
    void navigationRequestAccepted(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame);

private:
    WebTab *m_webTab;
    WebPage *m_webPage;

    QString url() const;
    QString title() const;
    int zoomLevel() const;
    int index() const;
    bool pinned() const;
    bool muted() const;
    bool restored() const;
    bool current() const;
    bool playing() const;
    QmlWindow *browserWindow() const;
    bool loading() const;
    int loadingProgress() const;
    bool backgroundActivity() const;
    bool canGoBack() const;
    bool canGoForward() const;

    void createConnections();
};

class QmlTabData
{
public:
    explicit QmlTabData();
    ~QmlTabData();
    QmlTab *get(WebTab *webTab);
private:
    QHash<WebTab*, QmlTab*> m_tabs;
};
