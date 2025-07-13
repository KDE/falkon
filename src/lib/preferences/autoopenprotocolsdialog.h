/*
 * Falkon - Dialog for auto open protocols management
 * SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef AUTOOPENPROTOCOLSDIALOG_H
#define AUTOOPENPROTOCOLSDIALOG_H

#include <qdialog.h>
#include <QScopedPointer>

namespace Ui
{
class AutoOpenProtocolsDialog;
}

class AutoOpenProtocolsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoOpenProtocolsDialog(QWidget* parent = nullptr);
    ~AutoOpenProtocolsDialog();

public Q_SLOTS:
    void accept() override;

private Q_SLOTS:
    void reset();

    void allowedAdd();
    void allowedRemove();

    void blockedAdd();
    void blockedRemove();

private:
    QScopedPointer<Ui::AutoOpenProtocolsDialog> m_ui;
};

#endif // AUTOOPENPROTOCOLSDIALOG_H
