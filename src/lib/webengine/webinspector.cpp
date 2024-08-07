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
#include "webinspector.h"
#include "mainapplication.h"
#include "networkmanager.h"
#include "settings.h"
#include "webview.h"
#include "webpage.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QWebEngineSettings>
#include <QtWebEngineWidgetsVersion>

QList<QWebEngineView*> WebInspector::s_views;

WebInspector::WebInspector(QWidget *parent)
    : QWebEngineView(parent)
    , m_view(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName(QSL("web-inspector"));
    setMinimumHeight(80);

    m_height = Settings().value(QSL("Web-Inspector/height"), 80).toInt();
    m_windowSize = Settings().value(QSL("Web-Inspector/windowSize"), QSize(640, 480)).toSize();

    registerView(this);

    connect(page(), &QWebEnginePage::windowCloseRequested, this, &WebInspector::deleteLater);
    connect(page(), &QWebEnginePage::loadFinished, this, &WebInspector::loadFinished);
}

WebInspector::~WebInspector()
{
    if (m_view && hasFocus()) {
        m_view->setFocus();
    }

    unregisterView(this);

    if (isWindow()) {
        Settings().setValue(QSL("Web-Inspector/windowSize"), size());
    } else {
        Settings().setValue(QSL("Web-Inspector/height"), height());
    }
}

void WebInspector::setView(WebView *view)
{
    m_view = view;
    Q_ASSERT(isEnabled());

    page()->setInspectedPage(m_view->page());
    connect(m_view, &WebView::pageChanged, this, &WebInspector::deleteLater);
}

void WebInspector::inspectElement()
{
    m_inspectElement = true;
}

bool WebInspector::isEnabled()
{
    return true;
}

void WebInspector::pushView(QWebEngineView *view)
{
    s_views.removeOne(view);
    s_views.prepend(view);
}

void WebInspector::registerView(QWebEngineView *view)
{
    s_views.prepend(view);
}

void WebInspector::unregisterView(QWebEngineView *view)
{
    s_views.removeOne(view);
}

void WebInspector::loadFinished()
{
    // Inspect element
    if (m_inspectElement) {
        m_view->triggerPageAction(QWebEnginePage::InspectElement);
        m_inspectElement = false;
    }
}

QSize WebInspector::sizeHint() const
{
    if (isWindow()) {
        return m_windowSize;
    }
    QSize s = QWebEngineView::sizeHint();
    s.setHeight(m_height);
    return s;
}

void WebInspector::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
    // Stop propagation
}

void WebInspector::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event)
    // Stop propagation
}
