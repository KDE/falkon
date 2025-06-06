/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
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
#ifndef ADBLOCKTREEWIDGET_H
#define ADBLOCKTREEWIDGET_H

#include "qzcommon.h"

#include <QTreeView>

class AdBlockSubscription;
class AdBlockTreeModel;
class AdBlockFilterModel;
class AdBlockRule;

class FALKON_EXPORT AdBlockTreeWidget : public QTreeView
{
    Q_OBJECT
public:
    explicit AdBlockTreeWidget(AdBlockSubscription *subscription, QWidget *parent = nullptr);

    AdBlockSubscription* subscription() const;

    void showRule(const AdBlockRule *rule);
    void filterString(const QString &string);

public Q_SLOTS:
    void addRule();
    void removeRule();

private Q_SLOTS:
    void contextMenuRequested(const QPoint &pos);
    void copyFilter();

private:
    void keyPressEvent(QKeyEvent* event) override;

    AdBlockSubscription* m_subscription;
    AdBlockTreeModel *m_subscriptionModel;
    AdBlockFilterModel *m_filterModel;
};

#endif /* ADBLOCKTREEWIDGET_H */
