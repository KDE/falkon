/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "adblockdialog.h"
#include "adblockmanager.h"
#include "adblocksubscription.h"
#include "adblocktreewidget.h"
#include "adblockaddsubscriptiondialog.h"
#include "mainapplication.h"
#include "qztools.h"

#include <QMenu>
#include <QTimer>
#include <QMessageBox>
#include <QInputDialog>

AdBlockDialog::AdBlockDialog(QWidget* parent)
    : QDialog(parent)
    , m_manager(AdBlockManager::instance())
    , m_currentTreeWidget(nullptr)
    , m_currentSubscription(nullptr)
    , m_loaded(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);

    QzTools::centerWidgetOnScreen(this);

#ifdef Q_OS_MACOS
    tabWidget->setDocumentMode(false);
#endif
    adblockCheckBox->setChecked(m_manager->isEnabled());

    auto* menu = new QMenu(buttonOptions);
    m_actionAddRule = menu->addAction(tr("Add Rule"), this, &AdBlockDialog::addRule);
    m_actionRemoveRule = menu->addAction(tr("Remove Rule"), this, &AdBlockDialog::removeRule);
    menu->addSeparator();
    m_actionAddSubscription = menu->addAction(tr("Add Subscription"), this, &AdBlockDialog::addSubscription);
    m_actionRemoveSubscription = menu->addAction(tr("Remove Subscription"), this, &AdBlockDialog::removeSubscription);
    menu->addAction(tr("Update Subscriptions"), m_manager, &AdBlockManager::updateAllSubscriptions);
    menu->addSeparator();
    menu->addAction(tr("Learn about writing rules..."), this, &AdBlockDialog::learnAboutRules);

    buttonOptions->setMenu(menu);
    connect(menu, &QMenu::aboutToShow, this, &AdBlockDialog::aboutToShowMenu);

    connect(adblockCheckBox, &QAbstractButton::toggled, this, &AdBlockDialog::enableAdBlock);
    connect(search, &QLineEdit::textChanged, this, &AdBlockDialog::filterString);
    connect(tabWidget, &QTabWidget::currentChanged, this, &AdBlockDialog::currentChanged);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QWidget::close);

    load();

    buttonBox->setFocus();
}

void AdBlockDialog::showRule(const AdBlockRule* rule) const
{
    AdBlockSubscription* subscription = rule->subscription();
    if (!subscription) {
        return;
    }

    for (int i = 0; i < tabWidget->count(); ++i) {
        auto* treeWidget = qobject_cast<AdBlockTreeWidget*>(tabWidget->widget(i));

        if (subscription == treeWidget->subscription()) {
            treeWidget->showRule(rule);
            tabWidget->setCurrentIndex(i);
            break;
        }
    }
}

void AdBlockDialog::addRule()
{
    m_currentTreeWidget->addRule();
}

void AdBlockDialog::removeRule()
{
    m_currentTreeWidget->removeRule();
}

void AdBlockDialog::addSubscription()
{
    AdBlockAddSubscriptionDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString title = dialog.title();
    QString url = dialog.url();

    if (AdBlockSubscription* subscription = m_manager->addSubscription(title, url)) {
        auto *tree = new AdBlockTreeWidget(subscription, tabWidget);
        int index = tabWidget->insertTab(tabWidget->count() - 1, tree, subscription->title());

        tabWidget->setCurrentIndex(index);
    }
}

void AdBlockDialog::removeSubscription()
{
    if (m_manager->removeSubscription(m_currentSubscription)) {
        delete m_currentTreeWidget;
    }
}

void AdBlockDialog::currentChanged(int index)
{
    if (index != -1) {
        m_currentTreeWidget = qobject_cast<AdBlockTreeWidget*>(tabWidget->widget(index));
        m_currentSubscription = m_currentTreeWidget->subscription();
        filterString(search->text());
    }
}

void AdBlockDialog::filterString(const QString &string)
{
    if (m_currentTreeWidget && adblockCheckBox->isChecked()) {
        m_currentTreeWidget->filterString(string);
    }
}

void AdBlockDialog::enableAdBlock(bool state)
{
    m_manager->setEnabled(state);

    if (state) {
        load();
    }
}

void AdBlockDialog::aboutToShowMenu()
{
    bool subscriptionEditable = m_currentSubscription && m_currentSubscription->canEditRules();
    bool subscriptionRemovable = m_currentSubscription && m_currentSubscription->canBeRemoved();

    m_actionAddRule->setEnabled(subscriptionEditable);
    m_actionRemoveRule->setEnabled(subscriptionEditable);
    m_actionRemoveSubscription->setEnabled(subscriptionRemovable);
}

void AdBlockDialog::learnAboutRules()
{
    mApp->addNewTab(QUrl(QSL("http://adblockplus.org/en/filters")));
}

void AdBlockDialog::load()
{
    if (m_loaded || !adblockCheckBox->isChecked()) {
        return;
    }

    const auto subscriptions = m_manager->subscriptions();
    for (AdBlockSubscription* subscription : subscriptions) {
        auto *tree = new AdBlockTreeWidget(subscription, tabWidget);
        tabWidget->addTab(tree, subscription->title());
    }

    m_loaded = true;
}
