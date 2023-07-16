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
#ifndef ZOOM_LABEL_H
#define ZOOM_LABEL_H

#include "clickablelabel.h"

class LocationBar;
class WebView;
class BrowserWindow;

class ZoomLabel : public ClickableLabel
{
    Q_OBJECT

public:
    explicit ZoomLabel(LocationBar* parent);

    void setWebView(WebView* view);
    void requestShow();

protected:
    void paintEvent(QPaintEvent* e) override;

private Q_SLOTS:
    void valueChanged(int value);

private:
    LocationBar* m_locationBar;
    WebView* m_view;
};

#endif // ZOOM_LABEL_H
