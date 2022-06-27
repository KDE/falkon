/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include <QHeaderView>

class QContextMenuEvent;

#include "qzcommon.h"

class FALKON_EXPORT HeaderView : public QHeaderView
{
    Q_OBJECT
public:
    explicit HeaderView(QAbstractItemView* parent);

    void setDefaultSectionSizes(const QList<double> &sizes);
    QList<double> defaultSectionSizes() const;

    bool restoreState(const QByteArray &state);

private Q_SLOTS:
    void toggleSectionVisibility();

private:
    void showEvent(QShowEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

    QAbstractItemView* m_parent = nullptr;
    QMenu* m_menu = nullptr;

    bool m_resizeOnShow;
    QList<double> m_sectionSizes;
    QByteArray m_restoreData;
};

#endif // HEADERVIEW_H
