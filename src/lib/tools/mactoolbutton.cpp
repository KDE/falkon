/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2017 David Rosca <nowrep@gmail.com>
* Copyright (C) 2013-2014 S. Razi Alavizadeh <s.r.alavizadeh@gmail.com>
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
#include "mactoolbutton.h"

#include <QStyle>
#include <QColorDialog>

#ifdef Q_OS_MACOS
MacToolButton::MacToolButton(QWidget* parent)
    : QPushButton(parent)
    , m_autoRise(false)
    , m_buttonFixedSize(18, 18)
    , m_iconColor()
    , m_defaultIconColor()
{
}

void MacToolButton::setIconSize(const QSize &size)
{
    QPushButton::setIconSize(size);
    m_buttonFixedSize = QSize(size.width() + 2, size.height() + 2);
}

void MacToolButton::setAutoRaise(bool enable)
{
    m_autoRise = enable;
    setFlat(enable);
    if (enable) {
        setFixedSize(m_buttonFixedSize);
    }
}

bool MacToolButton::autoRaise() const
{
    return m_autoRise;
}
#else
MacToolButton::MacToolButton(QWidget* parent)
    : QToolButton(parent)
    , m_iconColor()
    , m_defaultIconColor()
{
}
#endif

void MacToolButton::setIconColor(QColor color)
{
    if (color == m_iconColor) {
        return;
    }

    const int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    QPixmap pm(QSize(size, size));
    if (!color.isValid()) {
        color = m_defaultIconColor;
    }
    pm.fill(color);
    setIcon(pm);
    m_iconColor = color;
    Q_EMIT iconColorChanged(color);
}

const QColor MacToolButton::iconColor() const
{
    if (m_iconColor.isValid()) {
        return m_iconColor;
    }
    else if (m_defaultIconColor.isValid()) {
        return m_defaultIconColor;
    }
    else {
        return QColor(Qt::white);
    }
}

void MacToolButton::selectColorDialog()
{
    QColor newColor = QColorDialog::getColor(m_iconColor, this, tr("Select Color"));
    if (newColor.isValid()) {
        setIconColor(newColor);
    }
}

void MacToolButton::setDefaultIconColor(QColor color)
{
    if (color.isValid() && (m_defaultIconColor != color)) {
        m_defaultIconColor = color;
        Q_EMIT defaultIconColorChanged(color);
    }
}

const QColor MacToolButton::defaultIconColor() const
{
    if (m_defaultIconColor.isValid()) {
        return m_defaultIconColor;
    }
    else if (m_iconColor.isValid()) {
        return m_iconColor;
    }
    else {
        return QColor(Qt::white);
    }
}

void MacToolButton::resetIconColor()
{
    setIconColor(defaultIconColor());
}