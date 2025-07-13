/*
 * Falkon - Dialog for auto open protocols management
 * SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "autoopenprotocolsdialog.h"
#include "ui_autoopenprotocolsdialog.h"

#include "qzsettings.h"

#include <QInputDialog>

AutoOpenProtocolsDialog::AutoOpenProtocolsDialog(QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::AutoOpenProtocolsDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_ui->setupUi(this);

    reset();

    connect(m_ui->buttonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &AutoOpenProtocolsDialog::reset);
    connect(m_ui->allowedAddButton, &QAbstractButton::clicked, this, &AutoOpenProtocolsDialog::allowedAdd);
    connect(m_ui->allowedRemoveButton, &QAbstractButton::clicked, this, &AutoOpenProtocolsDialog::allowedRemove);
    connect(m_ui->blockedAddButton, &QAbstractButton::clicked, this, &AutoOpenProtocolsDialog::blockedAdd);
    connect(m_ui->blockedRemoveButton, &QAbstractButton::clicked, this, &AutoOpenProtocolsDialog::blockedRemove);
}

AutoOpenProtocolsDialog::~AutoOpenProtocolsDialog()
{
}

void AutoOpenProtocolsDialog::reset()
{
    Settings settings;
    settings.beginGroup(QSL("Web-Browser-Settings"));

    m_ui->allowedList->clear();
    m_ui->allowedList->addItems(settings.value(QSL("AutomaticallyOpenProtocols"), QStringList()).toStringList());

    m_ui->blockedList->clear();
    m_ui->blockedList->addItems(settings.value(QSL("BlockOpeningProtocols"), QStringList()).toStringList());

    settings.endGroup();
}

void AutoOpenProtocolsDialog::accept()
{
    QStringList list;
    Settings settings;
    settings.beginGroup(QSL("Web-Browser-Settings"));

    for (int i = 0; i < m_ui->allowedList->count(); ++i) {
        list.append(m_ui->allowedList->item(i)->text().toLower());
    }
    list.removeDuplicates();
    settings.setValue(QSL("AutomaticallyOpenProtocols"), list);

    list.clear();
    for (int i = 0; i < m_ui->blockedList->count(); ++i) {
        list.append(m_ui->blockedList->item(i)->text().toLower());
    }
    list.removeDuplicates();
    settings.setValue(QSL("BlockOpeningProtocols"), list);

    settings.endGroup();
    settings.sync();

    qzSettings->loadSettings();

    QDialog::close();
}

void AutoOpenProtocolsDialog::allowedAdd()
{
    const QString scheme = QInputDialog::getText(this, tr("Add allowed scheme"), tr("Scheme:"));

    if (scheme.isEmpty()) {
        return;
    }

    if (m_ui->allowedList->findItems(scheme, Qt::MatchFixedString).isEmpty()) {
        m_ui->allowedList->addItem(scheme);
    }
}

void AutoOpenProtocolsDialog::allowedRemove()
{
    int currentRow = m_ui->allowedList->currentRow();
    auto* item = m_ui->allowedList->item(currentRow);

    if ((item != nullptr) && (item->isSelected())) {
        m_ui->allowedList->takeItem(m_ui->allowedList->currentRow());
    }
}

void AutoOpenProtocolsDialog::blockedAdd()
{
    const QString scheme = QInputDialog::getText(this, tr("Add blocked scheme"), tr("Scheme:"));

    if (scheme.isEmpty()) {
        return;
    }

    if (m_ui->blockedList->findItems(scheme, Qt::MatchFixedString).isEmpty()) {
        m_ui->blockedList->addItem(scheme);
    }
}

void AutoOpenProtocolsDialog::blockedRemove()
{
    int currentRow = m_ui->blockedList->currentRow();
    auto* item = m_ui->blockedList->item(currentRow);

    if ((item != nullptr) && (item->isSelected())) {
        m_ui->blockedList->takeItem(m_ui->blockedList->currentRow());
    }
}
