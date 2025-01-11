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
#ifndef ADBLOCKTREEMODEL_H
#define ADBLOCKTREEMODEL_H

#include "qzcommon.h"

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

class AdBlockRule;
class AdBlockSubscription;

class QTimer;

// #define ADBLOCKTREEMODEL_DEBUG
#ifdef ADBLOCKTREEMODEL_DEBUG
class QAbstractItemModelTester;
#endif

class FALKON_EXPORT AdBlockTreeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    AdBlockTreeModel(AdBlockSubscription *subscription, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void addRule(AdBlockRule *rule);

private Q_SLOTS:
    void subscriptionError(const QString &message);

private:
    AdBlockSubscription *m_subscription;
    QString m_errorString;

#ifdef ADBLOCKTREEMODEL_DEBUG
    QAbstractItemModelTester *m_tester;
#endif
};


class FALKON_EXPORT AdBlockFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit AdBlockFilterModel(QAbstractItemModel *parent);

public Q_SLOTS:
    void setFilterFixedString(const QString &pattern);

private Q_SLOTS:
    void startFiltering();

private:
    QString m_pattern;
    QTimer *m_filterTimer;
};

#endif /* ADBLOCKTREEMODEL_H */

