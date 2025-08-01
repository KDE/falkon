/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2014  David Rosca <nowrep@gmail.com>
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
#ifndef SSLERRORDIALOG_H
#define SSLERRORDIALOG_H

#include <QDialog>
#include <QSslCertificate>

namespace Ui
{
class SslErrorDialog;
}

class QAbstractButton;

#include "qzcommon.h"

class FALKON_EXPORT SslErrorDialog : public QDialog
{
    Q_OBJECT

public:
    enum Result { Yes, No, OnlyForThisSession, NoForThisSession };

    explicit SslErrorDialog(QWidget* parent = nullptr);
    ~SslErrorDialog();

    void setText(const QString &text);
    void setCertificateChain(const QList<QSslCertificate> &certificateChain);

    Result result();

private Q_SLOTS:
    void buttonClicked(QAbstractButton* button);
    void toggleCertificateDetails();

private:
    Ui::SslErrorDialog* ui;

    Result m_result;
};

#endif // SSLERRORDIALOG_H
