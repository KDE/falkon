/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
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
#ifndef SEARCHTOOLBAR_H
#define SEARCHTOOLBAR_H

#include <QWebEnginePage>
#include <QtWebEngineCoreVersion>

#include <QWebEngineFindTextResult>

#include "qzcommon.h"

namespace Ui
{
class SearchToolbar;
}

class QLineEdit;

class WebView;
class LineEdit;

class FALKON_EXPORT SearchToolBar : public QWidget
{
    Q_OBJECT
public:
    explicit SearchToolBar(WebView* view, QWidget* parent = nullptr);
    ~SearchToolBar() override;

    void showMinimalInPopupWindow();

    void focusSearchLine();
    bool eventFilter(QObject* obj, QEvent* event) override;

public Q_SLOTS:
    void setText(const QString &text);
    void searchText(const QString &text);
    void updateFindFlags();
    void caseSensitivityChanged();

    void findNext();
    void findPrevious();

    void showSearchResults(const QWebEngineFindTextResult &result);

    void close();

private:
    Ui::SearchToolbar* ui;
    WebView* m_view;

    QWebEnginePage::FindFlags m_findFlags;
    int m_searchRequests = 0;
};

#endif // SEARCHTOOLBAR_H
