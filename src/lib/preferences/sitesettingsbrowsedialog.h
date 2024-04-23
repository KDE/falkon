// Falkon - SiteSettings permission browser dialog
// SPDX-FileCopyrightText: 2024 Juraj Oravec <jurajoravec@mailo.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SITESETTINGSBROWSEDIALOG_H
#define SITESETTINGSBROWSEDIALOG_H

#include "sitesettingsmanager.h"

#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
class SiteSettingsBrowseDialog;
}

class SiteSettingsBrowseDialog : public QDialog
{
    Q_OBJECT

public:
    SiteSettingsBrowseDialog(QString &name, QString &sqlColumn, QWidget* parent = nullptr);
    ~SiteSettingsBrowseDialog();

    void setPermission(const SiteSettingsManager::Permission permission);
    void hideAskButton();

public Q_SLOTS:
    void storeChanges();

private:
    void loadItems();

    QScopedPointer<Ui::SiteSettingsBrowseDialog> m_ui;
    QHash<QString, int> m_listModifications;
    QString m_sqlColumn;
};

#endif // SITESETTINGSBROWSEDIALOG_H
