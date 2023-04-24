/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2023 Javier Llorente <javier@opensuse.org>
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
#ifndef CERTIFICATEMANAGER_H
#define CERTIFICATEMANAGER_H

#include <QDialog>

#include "qzcommon.h"

namespace Ui
{
class CertificateManager;
}

class FALKON_EXPORT CertificateManager : public QDialog
{
    Q_OBJECT

public:
    explicit CertificateManager(QWidget* parent = nullptr);
    ~CertificateManager() override;

public Q_SLOTS:
    void accept() override;

private Q_SLOTS:
    void addException();
    void removeException();

private:
    Ui::CertificateManager* ui;
    QStringList m_ignoredSslHosts;
};

#endif // CERTIFICATEMANAGER_H
