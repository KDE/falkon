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
#ifndef ICONCHOOSER_H
#define ICONCHOOSER_H

#include <QDialog>
#include <QStyledItemDelegate>

#include "qzcommon.h"

class QIcon;

namespace Ui
{
class IconChooser;
}

class FALKON_EXPORT IconChooser : public QDialog
{
    Q_OBJECT

public:
    explicit IconChooser(QWidget* parent = nullptr);
    ~IconChooser();

    QIcon getIcon();

private Q_SLOTS:
    void chooseFile();
    void searchIcon(const QString &string);

private:
    Ui::IconChooser* ui;
};

class FALKON_EXPORT IconChooserDelegate : public QStyledItemDelegate
{
public:
    explicit IconChooserDelegate(QWidget* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // ICONCHOOSER_H
