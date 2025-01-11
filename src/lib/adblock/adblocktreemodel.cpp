/* ============================================================
* Falkon - Qt web browser
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
#include "adblocktreemodel.h"

#include "adblocksubscription.h"
#include "mainapplication.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QTimer>

#ifdef ADBLOCKTREEMODEL_DEBUG
#include <QAbstractItemModelTester>
#endif

AdBlockTreeModel::AdBlockTreeModel(AdBlockSubscription* subscription, QObject* parent)
    : QAbstractListModel(parent)
    , m_subscription(subscription)
{
#ifdef ADBLOCKTREEMODEL_DEBUG
    m_tester = new QAbstractItemModelTester(this, QAbstractItemModelTester::FailureReportingMode::Fatal, this);
#endif

    connect(m_subscription, &AdBlockSubscription::subscriptionUpdateStarted, this, &AdBlockTreeModel::beginResetModel);
    connect(m_subscription, &AdBlockSubscription::subscriptionUpdated, this, &AdBlockTreeModel::endResetModel);
    connect(m_subscription, &AdBlockSubscription::subscriptionError, this, &AdBlockTreeModel::subscriptionError);
}

int AdBlockTreeModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_subscription->allRules().count();
}

QVariant AdBlockTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const AdBlockRule *rule = m_subscription->allRules().at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return rule->filter();
        break;
    case Qt::FontRole:
        if (!rule->isEnabled() && !rule->isComment()) {
            QFont font;
            font.setItalic(true);
            return font;
        }
        break;
    case Qt::ForegroundRole:
        if (rule->isEnabled()) {
            if (rule->isUnsupportedRule()) {
                return QBrush(QColor(Qt::gray));
            } else if (rule->isException()) {
                return QBrush(QColor(Qt::darkGreen));
            }
            else if (rule->isCssRule()) {
                return QBrush(QColor(Qt::darkBlue));
            }
        }
        else {
            return QBrush(QColor(Qt::gray));
        }
        break;
    case Qt::CheckStateRole:
        if (rule->isEnabled()) {
            return Qt::Checked;
        }
        else if (!rule->isComment()) {
            return Qt::Unchecked;
        }
        break;
    }
    return QVariant();
}

QVariant AdBlockTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)

    if (role == Qt::DisplayRole) {
        if (m_errorString.isEmpty()) {
            return m_subscription->title();
        }
        else {
            return tr("%1 - %2").arg(m_subscription->title(), m_errorString);
        }
    }
    return QVariant();
}

bool AdBlockTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    AdBlockRule *rule = m_subscription->allRules().at(index.row());
    int offset = index.row();

    switch (role) {
    case Qt::EditRole:
        if (m_subscription->canEditRules()){
            auto *newRule = new AdBlockRule(value.toString(), m_subscription);
            m_subscription->replaceRule(newRule, offset);
            return true;
        }
        break;
    case Qt::CheckStateRole:
        if (!rule->isComment()) {
            if (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked) {
                m_subscription->enableRule(offset);
            }
            else {
                m_subscription->disableRule(offset);
            }
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

Qt::ItemFlags AdBlockTreeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    AdBlockRule *rule = m_subscription->allRules().at(index.row());
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (m_subscription->canEditRules()) {
        flags |= Qt::ItemIsEditable;
    }

    if (!rule->isComment()) {
        flags |= Qt::ItemIsUserCheckable;
    }

    return flags;
}

bool AdBlockTreeModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (!m_subscription->canEditRules()) {
        return false;
    }

    int lastRow = row + count - 1;
    beginRemoveRows(parent, row, lastRow);
    for (int i = 0; i < count; ++i) {
        m_subscription->removeRule(row);
    }
    endRemoveRows();

    return true;
}

void AdBlockTreeModel::addRule(AdBlockRule* rule)
{
    if (!m_subscription->canEditRules()) {
        return;
    }

    int row = rowCount(QModelIndex());

    beginInsertRows(QModelIndex(), row, row);
    m_subscription->addRule(rule);
    endInsertRows();
}

void AdBlockTreeModel::subscriptionError(const QString& message)
{
    m_errorString = message;
}

/* AdBlockFilterModel */
AdBlockFilterModel::AdBlockFilterModel(QAbstractItemModel* parent)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(parent);
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_filterTimer = new QTimer(this);
    m_filterTimer->setSingleShot(true);
    m_filterTimer->setInterval(200);

    connect(m_filterTimer, &QTimer::timeout, this, &AdBlockFilterModel::startFiltering);
}

void AdBlockFilterModel::setFilterFixedString(const QString &pattern)
{
    m_pattern = pattern;

    m_filterTimer->start();
}

void AdBlockFilterModel::startFiltering()
{
    QSortFilterProxyModel::setFilterFixedString(m_pattern);
}
