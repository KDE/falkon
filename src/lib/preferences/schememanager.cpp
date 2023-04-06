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

#include "ui_schememanager.h"
#include "schememanager.h"
#include "qzsettings.h"

#include <QInputDialog>
#include <QMessageBox>

SchemeManager::SchemeManager(QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::SchemeManager)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_ui->setupUi(this);

    reset();

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &SchemeManager::accept);
    connect(m_ui->buttonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked, this, &SchemeManager::reset);
    connect(m_ui->blockButton, &QAbstractButton::clicked, this, &SchemeManager::blockScheme);
    connect(m_ui->allowedAddButton, &QAbstractButton::clicked, this, &SchemeManager::allowedAdd);
    connect(m_ui->allowedRemoveButton, &QAbstractButton::clicked, this, &SchemeManager::allowedRemove);
    connect(m_ui->blockedAddButton, &QAbstractButton::clicked, this, &SchemeManager::blockedAdd);
    connect(m_ui->blockedRemoveButton, &QAbstractButton::clicked, this, &SchemeManager::blockedRemove);
}

void SchemeManager::reset()
{
    Settings settings;
    settings.beginGroup(QSL("Web-Browser-Settings"));

    m_ui->allowedList->clear();
    m_ui->allowedList->addItems(settings.value(QSL("AllowedSchemes"), QStringList()).toStringList());

    m_ui->blockedList->clear();
    m_ui->blockedList->addItems(settings.value(QSL("BlockedSchemes"), QStringList()).toStringList());

    settings.endGroup();
}

void SchemeManager::accept()
{
    QStringList list;
    Settings settings;
    settings.beginGroup(QSL("Web-Browser-Settings"));

    for (int i = 0; i < m_ui->allowedList->count(); ++i) {
        list.append(m_ui->allowedList->item(i)->text().toLower());
    }
    list.removeDuplicates();
    settings.setValue(QSL("AllowedSchemes"), list);

    list.clear();
    for (int i = 0; i < m_ui->blockedList->count(); ++i) {
        list.append(m_ui->blockedList->item(i)->text().toLower());
    }
    list.removeDuplicates();
    settings.setValue(QSL("BlockedSchemes"), list);
    settings.endGroup();

    QDialog::close();
}

void SchemeManager::allowedAdd()
{
    const QString scheme = QInputDialog::getText(this, tr("Add allowed scheme"), tr("Scheme:"));

    if (scheme.isEmpty()) {
        return;
    }

    if (m_ui->allowedList->findItems(scheme, Qt::MatchFixedString).isEmpty()) {
        m_ui->allowedList->addItem(scheme);
    }
}

void SchemeManager::allowedRemove()
{
    int currentRow = m_ui->allowedList->currentRow();
    auto* item = m_ui->allowedList->item(currentRow);

    if ((item != nullptr) && (item->isSelected())) {
        m_ui->allowedList->takeItem(m_ui->allowedList->currentRow());
    }
}

void SchemeManager::blockScheme()
{
    int currentRow = m_ui->allowedList->currentRow();
    auto* item = m_ui->allowedList->item(currentRow);

    if ((item != nullptr) && (item->isSelected())) {
        if (m_ui->blockedList->findItems(item->text(), Qt::MatchFixedString).isEmpty()) {
            m_ui->blockedList->addItem(item->text());
        }
    }
}

void SchemeManager::blockedAdd()
{
    const QString scheme = QInputDialog::getText(this, tr("Add blocked scheme"), tr("Scheme:"));

    if (scheme.isEmpty()) {
        return;
    }

    if (m_ui->blockedList->findItems(scheme, Qt::MatchFixedString).isEmpty()) {
        m_ui->blockedList->addItem(scheme);
    }
}

void SchemeManager::blockedRemove()
{
    int currentRow = m_ui->blockedList->currentRow();
    auto* item = m_ui->blockedList->item(currentRow);

    if ((item != nullptr) && (item->isSelected())) {
        m_ui->blockedList->takeItem(m_ui->blockedList->currentRow());
    }
}

SchemeManager::~SchemeManager()
{
    delete m_ui;
}
