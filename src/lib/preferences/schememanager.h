/* ============================================================
 * Falkon - Qt web browser
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

#ifndef SCHEMEMANAGER_H
#define SCHEMEMANAGER_H

#include <QDialog>

namespace Ui {
class SchemeManager;
}

class SchemeManager : public QDialog
{
    Q_OBJECT

public:
    explicit SchemeManager(QWidget* parent = nullptr);
    ~SchemeManager() override;

public Q_SLOTS:
    void accept() override;

private Q_SLOTS:
    void reset();
    void blockScheme();

    void allowedAdd();
    void allowedRemove();
    void blockedAdd();
    void blockedRemove();

private:
    Ui::SchemeManager *m_ui;
};

#endif // SCHEMEMANAGER_H
