/* ============================================================
* AutoScroll - Autoscroll for Falkon
* Copyright (C) 2014  David Rosca <nowrep@gmail.com>
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
#include "framescroller.h"
#include "webpage.h"

#include <QTimer>
#include <QtMath>

FrameScroller::FrameScroller(QObject* parent)
    : QObject(parent)
    , m_page(nullptr)
    , m_lengthX(0)
    , m_lengthY(0)
    , m_divider(8.0)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(10);
    connect(m_timer, &QTimer::timeout, this, &FrameScroller::scrollStep);
}

void FrameScroller::setPage(WebPage *page)
{
    m_page = page;
}

double FrameScroller::scrollDivider() const
{
    return m_divider;
}

void FrameScroller::setScrollDivider(double divider)
{
    m_divider = divider;
}

void FrameScroller::startScrolling(int lengthX, int lengthY)
{
    m_lengthX = lengthX;
    m_lengthY = lengthY;

    if (m_lengthX == 0 && m_lengthY == 0) {
        m_timer->stop();
    }
    else if (!m_timer->isActive()) {
        m_timer->start();
    }
}

void FrameScroller::stopScrolling()
{
    m_timer->stop();
}

void FrameScroller::scrollStep()
{
    m_page->scroll(qCeil(m_lengthX / m_divider), qCeil(m_lengthY / m_divider));
}
