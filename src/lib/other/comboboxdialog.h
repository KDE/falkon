/* ============================================================
 * Falkon - Qt web browser
 * SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 * ============================================================ */

#ifndef COMBOBOXDIALOG_H
#define COMBOBOXDIALOG_H

#include <QAbstractItemModel>
#include <QDialog>
#include <QDialogButtonBox>

#include "qzcommon.h"

class QComboBox;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QSpacerItem;

class FALKON_EXPORT ComboBoxDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ComboBoxDialog(const QDialogButtonBox::StandardButtons buttons, QWidget* parent = nullptr);

    void setModel(QAbstractItemModel *model);
    void setLabel(const QString &label);
    void setDescription(const QString &description);

    int currentIndex();

private:
    QComboBox *m_comboBox;
    QLabel *m_label;
    QLabel *m_description;

    QGridLayout *m_gridLayout;
    QHBoxLayout *m_hLayout;
    QDialogButtonBox *m_buttonBox;
    QSpacerItem *m_hSpacerLeft;
    QSpacerItem *m_hSpacerRight;
};

#endif // COMBOBOXDIALOG_H

