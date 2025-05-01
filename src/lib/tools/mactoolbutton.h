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
#ifndef MACTOOLBUTTON_H
#define MACTOOLBUTTON_H

#include "qzcommon.h"

#ifdef Q_OS_MACOS
#include <QPushButton>

class FALKON_EXPORT MacToolButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(bool autoRaise READ autoRaise WRITE setAutoRaise)
    Q_PROPERTY(QColor iconColor READ iconColor WRITE setIconColor NOTIFY iconColorChanged)
    Q_PROPERTY(QColor defaultIconColor READ defaultIconColor WRITE setDefaultIconColor NOTIFY defaultIconColorChanged)

public:
    explicit MacToolButton(QWidget* parent = nullptr);

    void setIconSize(const QSize &size);

    void setAutoRaise(bool enable);
    bool autoRaise() const;

    void setIconColor(QColor color = QColor());
    QColor iconColor();

    void setDefaultIconColor(QColor color = QColor());
    const QColor defaultIconColor() const;

Q_SIGNALS:
    void iconColorChanged(QColor color);
    void defaultIconColorChanged(QColor color);

public Q_SLOTS:
    void selectColorDialog();

private:
    bool m_autoRise;
    QSize m_buttonFixedSize;
    QColor m_iconColor;
    QColor m_defaultIconColor;
};
#else
#include <QToolButton>

class FALKON_EXPORT MacToolButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(QColor iconColor READ iconColor WRITE setIconColor NOTIFY iconColorChanged)
    Q_PROPERTY(QColor defaultIconColor READ defaultIconColor WRITE setDefaultIconColor NOTIFY defaultIconColorChanged)

public:
    explicit MacToolButton(QWidget* parent = nullptr);

    void setIconColor(QColor color = QColor());
    const QColor iconColor() const;

    void setDefaultIconColor(QColor color = QColor());
    const QColor defaultIconColor() const;

Q_SIGNALS:
    void iconColorChanged(QColor color);
    void defaultIconColorChanged(QColor color);

public Q_SLOTS:
    void selectColorDialog();
    void resetIconColor();

private:
    QColor m_iconColor;
    QColor m_defaultIconColor;
};
#endif
#endif // MACTOOLBUTTON_H
