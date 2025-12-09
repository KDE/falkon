/* ============================================================
* KDEFrameworksIntegration - KDE support plugin for Falkon
* Copyright (C) 2013-2014  David Rosca <nowrep@gmail.com>
* Copyright (C) 2021-2025 Juraj Oravec <jurajoravec@mailo.com>
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
#ifndef KWALLETPASSWORDBACKEND_H
#define KWALLETPASSWORDBACKEND_H

#include <QVector>
#include <QWindow>

#include "passwordbackends/passwordbackend.h"
#include "passwordmanager.h"

class KDEFrameworksIntegrationPlugin;

namespace KWallet {
class Wallet;
}

class FALKON_EXPORT KWalletPasswordBackend : public PasswordBackend
{
public:
    explicit KWalletPasswordBackend(KDEFrameworksIntegrationPlugin *plugin);
    ~KWalletPasswordBackend() override;

    QString name() const override;

    QStringList getUsernames(const QUrl &url) override;
    QVector<PasswordEntry> getEntries(const QUrl &url) override;
    QVector<PasswordEntry> getAllEntries() override;

    void addEntry(const PasswordEntry &entry) override;
    bool updateEntry(const PasswordEntry &entry) override;
    void updateLastUsed(PasswordEntry &entry) override;

    void removeEntry(const PasswordEntry &entry) override;
    void removeAll() override;

private:
    void initialize();
    void showErrorNotification();

    void folderMigration();
    void updateVersion();
    void loadEntries();
    void openWallet();

    void addExistFlag(const QString &host);
    void removeExistFlag(const QString &host);

    KWallet::Wallet* m_wallet;
    QVector<PasswordEntry> m_allEntries;
    KDEFrameworksIntegrationPlugin *m_plugin;
    bool m_entriesLoaded;
    bool m_openingFailed;
};

#endif // KWALLETPASSWORDBACKEND_H
