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
#pragma once

#include <QWebEngineView>

#include "qzcommon.h"

class WebView;

class FALKON_EXPORT WebInspector : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebInspector(QWidget *parent = nullptr);
    ~WebInspector() override;

    void setView(WebView *view);
    void inspectElement();

    QSize sizeHint() const override;

    static bool isEnabled();
    static void pushView(QWebEngineView *view);
    static void registerView(QWebEngineView *view);
    static void unregisterView(QWebEngineView *view);

private Q_SLOTS:
    void loadFinished();

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    static QList<QWebEngineView*> s_views;

    int m_height;
    QSize m_windowSize;
    bool m_inspectElement = false;
    WebView *m_view;
};
