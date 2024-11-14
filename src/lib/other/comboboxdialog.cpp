/* ============================================================
 * Falkon - Qt web browser
 * SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 * ============================================================ */

#include "comboboxdialog.h"

#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>

ComboBoxDialog::ComboBoxDialog(const QDialogButtonBox::StandardButtons buttons, QWidget* parent)
    : QDialog(parent)
{
    m_gridLayout = new QGridLayout();
    m_label = new QLabel();
    m_description = new QLabel();
    m_comboBox = new QComboBox();

    m_hLayout = new QHBoxLayout();
    m_buttonBox = new QDialogButtonBox(buttons);
    m_hSpacerLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
    m_hSpacerRight = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

    m_hLayout->addItem(m_hSpacerLeft);
    m_hLayout->addWidget(m_buttonBox);
    m_hLayout->addItem(m_hSpacerRight);

    m_gridLayout->addWidget(m_description, 0, 0, 1, 2);
    m_gridLayout->addWidget(m_label, 1, 0);
    m_gridLayout->addWidget(m_comboBox, 1, 1);
    m_gridLayout->addLayout(m_hLayout, 2, 0, 1, 2);

    setLayout(m_gridLayout);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ComboBoxDialog::setLabel(const QString& label)
{
    m_label->setText(label);
}

void ComboBoxDialog::setModel(QAbstractItemModel* model)
{
    m_comboBox->setModel(model);
}

int ComboBoxDialog::currentIndex()
{
    return m_comboBox->currentIndex();
}

void ComboBoxDialog::setDescription(const QString& description)
{
    m_description->setText(description);
}
