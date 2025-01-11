/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
* Copyright (C) 2025 Juraj Oravec <jurajoravec@mailo.com>
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
#include "adblocktreewidget.h"
#include "adblocktreemodel.h"
#include "adblocksubscription.h"

#include <QMenu>
#include <QKeyEvent>
#include <QClipboard>
#include <QApplication>
#include <QInputDialog>

AdBlockTreeWidget::AdBlockTreeWidget(AdBlockSubscription *subscription, QWidget *parent)
    : QTreeView(parent)
    , m_subscription(subscription)
    , m_subscriptionModel(new AdBlockTreeModel(subscription, this))
    , m_filterModel(new AdBlockFilterModel(m_subscriptionModel))
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAlternatingRowColors(true);
    setLayoutDirection(Qt::LeftToRight);
    setUniformRowHeights(true);

    setModel(m_filterModel);

    connect(this, &QWidget::customContextMenuRequested, this, &AdBlockTreeWidget::contextMenuRequested);
}

AdBlockSubscription* AdBlockTreeWidget::subscription() const
{
    return m_subscription;
}

void AdBlockTreeWidget::showRule(const AdBlockRule* rule)
{
    Q_UNUSED(rule)
#if 0
    if (!m_topItem && rule) {
        m_ruleToBeSelected = rule->filter();
    }
    else if (!m_ruleToBeSelected.isEmpty()) {
        QList<QTreeWidgetItem*> items = findItems(m_ruleToBeSelected, Qt::MatchRecursive);
        if (!items.isEmpty()) {
            QTreeWidgetItem* item = items.at(0);

            setCurrentItem(item);
            scrollToItem(item, QAbstractItemView::PositionAtCenter);
        }

        m_ruleToBeSelected.clear();
    }
#endif
}

void AdBlockTreeWidget::filterString(const QString& string)
{
    m_filterModel->setFilterFixedString(string);
}

void AdBlockTreeWidget::contextMenuRequested(const QPoint &pos)
{
    if (!m_subscription->canEditRules()) {
        return;
    }

    QModelIndex index = indexAt(pos);
    if (!index.isValid()) {
        return;
    }

    QMenu menu;
    menu.addAction(tr("Add Rule"), this, &AdBlockTreeWidget::addRule);
    menu.addSeparator();
    menu.addAction(tr("Remove Rule"), this, &AdBlockTreeWidget::removeRule);

    menu.exec(viewport()->mapToGlobal(pos));
}

void AdBlockTreeWidget::copyFilter()
{
    QModelIndex index = selectionModel()->currentIndex();
    if (!index.isValid()) {
        return;
    }
    QApplication::clipboard()->setText(m_subscriptionModel->data(index).toString());
}

void AdBlockTreeWidget::addRule()
{
    if (!m_subscription->canEditRules()) {
        return;
    }

    QString newRule = QInputDialog::getText(this, tr("Add Custom Rule"), tr("Please write your rule here:"));
    if (newRule.isEmpty()) {
        return;
    }

    auto* rule = new AdBlockRule(newRule, m_subscription);
    m_subscriptionModel->addRule(rule);
}

void AdBlockTreeWidget::removeRule()
{
    QModelIndex index = selectionModel()->currentIndex();
    if (!index.isValid() || !m_subscription->canEditRules()) {
        return;
    }
    m_subscriptionModel->removeRow(index.row());
}

void AdBlockTreeWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_C && event->modifiers() & Qt::ControlModifier) {
        copyFilter();
    }

    if (event->key() == Qt::Key_Delete) {
        removeRule();
    }

    QTreeView::keyPressEvent(event);
}
