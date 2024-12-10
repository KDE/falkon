/* ============================================================
 * Falkon - Qt web browser
 * Copyright (C) 2024 Juraj Oravec <jurajoravec@mailo.com>
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

#ifndef IDLEINHIBITOR_H
#define IDLEINHIBITOR_H

#include "qzcommon.h"

#include <QObject>
#include <QList>

class WebTab;

class FALKON_EXPORT IdleInhibitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    IdleInhibitor();

    bool active() const;

Q_SIGNALS:
    void activeChanged(bool active);

private:
    void inhibit();
    void unInhibit();

    void setActive(bool active);

    QList<WebTab*> m_activeTabs;
    quint32 m_dbusCookie;
    bool m_active;
};

#endif /* IDLEINHIBITOR_H */
