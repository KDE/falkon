/* ============================================================
 * ZoomLabel - Shows current zoom level in locationbar
 * Copyright (C) 2023 Juraj Oravec <jurajoravec@mailo.com>
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
#include "zoomlabel.h"
#include "locationbar.h"
#include "mainapplication.h"
#include "tabbedwebview.h"
#include "qzsettings.h"

#include <QApplication>

ZoomLabel::ZoomLabel(LocationBar* parent)
    : ClickableLabel(parent)
    , m_locationBar(parent)
    , m_view(nullptr)
{
    setObjectName(QSL("locationbar-zoomlabel"));
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setScaledContents(true);
    setToolTip(tr("Reset zoom level"));

    connect(mApp, &MainApplication::settingsReloaded, this, [this]() {
        if (this->m_view) {
            this->valueChanged(m_view->zoomLevel());
        }
    });
}

void ZoomLabel::setWebView(WebView* view)
{
    m_view = view;
    connect(view, &WebView::zoomLevelChanged, this, &ZoomLabel::valueChanged);
    connect(this, &ZoomLabel::clicked, view, &WebView::zoomReset);
    valueChanged(m_view->zoomLevel());
}

void ZoomLabel::requestShow()
{
    valueChanged(m_view->zoomLevel());
}

void ZoomLabel::valueChanged(int value)
{
    if ((m_view) && (value != qzSettings->defaultZoomLevel) && (qzSettings->showZoomLabel)) {
        setText(tr("%1%").arg(m_view->zoomFactor() * 100));
        show();
    }
    else {
        hide();
    }
}

void ZoomLabel::paintEvent(QPaintEvent* e)
{
    QPainter p(this);

    QFontMetrics fmNormalFont(font());
    QFont smallFont(font());
    smallFont.setPointSizeF(smallFont.pointSizeF() * 0.8);
    p.setFont(smallFont);

    QFontMetrics fmSmallFont(smallFont);
    int fontSizeDiff = fmNormalFont.height() - fmSmallFont.height();

    QRect rect = e->rect();
    rect.setY(rect.y() + (fontSizeDiff * 2));
    rect.setHeight(fmSmallFont.height());
    p.fillRect(rect, QApplication::palette().color(QPalette::Base));

    rect.setX(rect.x() + (fmNormalFont.horizontalAdvance(text()) - fmSmallFont.horizontalAdvance(text())) / 2);
    p.drawText(rect, text());
}
