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
#ifndef LOCATIONBAR_H
#define LOCATIONBAR_H

#include "qzcommon.h"
#include "lineedit.h"
#include "searchenginesmanager.h"
#include "loadrequest.h"

class QStringListModel;

class BrowserWindow;
class LocationCompleter;
class ClickableLabel;
class TabbedWebView;
class BookmarksIcon;
class SiteIcon;
class GoIcon;
class AutoFillIcon;
class BookmarkItem;

class FALKON_EXPORT LocationBar : public LineEdit
{
    Q_OBJECT

public:
    explicit LocationBar(QWidget *parent = nullptr);

    struct LoadAction {
        enum Type {
            Invalid = 0,
            Search,
            Bookmark,
            Url
        };
        Type type = Invalid;
        SearchEngine searchEngine;
        BookmarkItem *bookmark = nullptr;
        LoadRequest loadRequest;
    };

    // BrowserWindow can be null!
    BrowserWindow *browserWindow() const;
    void setBrowserWindow(BrowserWindow *window);

    TabbedWebView* webView() const;
    void setWebView(TabbedWebView* view);

    static QString convertUrlToText(const QUrl &url);
    static SearchEngine searchEngine();
    static LoadAction loadAction(const QString &text);

public Q_SLOTS:
    void setText(const QString &text);
    void showUrl(const QUrl &url);
    void loadRequest(const LoadRequest &request);

private Q_SLOTS:
    void textEdited(const QString &text);
    void requestLoadUrl();
    void pasteAndGo();

    void updateSiteIcon();
    void updatePlaceHolderText();

    void setPrivacyState(bool state);
    void setGoIconVisible(bool state);

    void showCompletion(const QString &completion, bool completeDomain);
    void showDomainCompletion(const QString &completion);
    void clearCompletion();

    void loadStarted();
    void loadProgress(int progress);
    void loadFinished();
    void hideProgress();

    void loadSettings();

private:
    enum ProgressStyle {
        ProgressFilled,
        ProgressBottom,
        ProgressTop
    };

    void contextMenuEvent(QContextMenuEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void refreshTextFormat();

    LocationCompleter* m_completer;
    QStringListModel* m_domainCompleterModel;

    BookmarksIcon* m_bookmarkIcon;
    GoIcon* m_goIcon;
    SiteIcon* m_siteIcon;
    AutoFillIcon* m_autofillIcon;

    BrowserWindow* m_window;
    TabbedWebView* m_webView;

    bool m_holdingAlt;
    int m_oldTextLength;
    int m_currentTextLength;

    int m_loadProgress;
    bool m_progressVisible;
    ProgressStyle m_progressStyle;
    QColor m_progressColor;
    QTimer *m_progressTimer;
};

#endif // LOCATIONBAR_H
