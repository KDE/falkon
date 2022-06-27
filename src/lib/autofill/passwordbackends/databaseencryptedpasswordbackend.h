/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2014  S. Razi Alavizadeh <s.r.alavizadeh@gmail.com>
* Copyright (C) 2013-2018  David Rosca <nowrep@gmail.com>
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
#ifndef DATABASEENCRYPTEDPASSWORDBACKEND_H
#define DATABASEENCRYPTEDPASSWORDBACKEND_H

#include "passwordbackend.h"
#include "qzcommon.h"

#include <QDialog>

class AesInterface;
class MasterPasswordDialog;

class FALKON_EXPORT DatabaseEncryptedPasswordBackend : public PasswordBackend
{
public:
    enum MasterPasswordState {
        PasswordIsSetted,
        PasswordIsNotSetted,
        UnKnownState = -1
    };

    explicit DatabaseEncryptedPasswordBackend();

    ~DatabaseEncryptedPasswordBackend() override;

    QStringList getUsernames(const QUrl &url) override;
    QVector<PasswordEntry> getEntries(const QUrl &url) override;
    QVector<PasswordEntry> getAllEntries() override;

    void setActive(bool active) override;

    void addEntry(const PasswordEntry &entry) override;
    bool updateEntry(const PasswordEntry &entry) override;
    void updateLastUsed(PasswordEntry &entry) override;

    void removeEntry(const PasswordEntry &entry) override;
    void removeAll() override;

    QString name() const override;

    bool hasSettings() const override;
    void showSettings(QWidget* parent) override;

    bool isMasterPasswordSetted();

    QByteArray masterPassword() const;

    bool hasPermission();
    bool isPasswordVerified(const QByteArray &password);

    bool decryptPasswordEntry(PasswordEntry &entry, AesInterface* aesInterface);
    bool encryptPasswordEntry(PasswordEntry &entry, AesInterface* aesInterface);

    void tryToChangeMasterPassword(const QByteArray &newPassword);
    void removeMasterPassword();

    void setAskMasterPasswordState(bool ask);

    void encryptDataBaseTableOnFly(const QByteArray &decryptorPassword,
                                   const QByteArray &encryptorPassword);

    void updateSampleData(const QByteArray &password);

    void showMasterPasswordDialog();

private:
    QByteArray someDataFromDatabase();

    MasterPasswordState m_stateOfMasterPassword;
    QByteArray m_someDataStoredOnDataBase;

    bool m_askPasswordDialogVisible;
    bool m_askMasterPassword;
    QByteArray m_masterPassword;
};

namespace Ui
{
class MasterPasswordDialog;
}

class MasterPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MasterPasswordDialog(DatabaseEncryptedPasswordBackend* backend, QWidget* parent = nullptr);
    ~MasterPasswordDialog() override;

    void delayedExec();

public Q_SLOTS:
    void accept() override;
    void reject() override;
    void showSettingPage();
    void showSetMasterPasswordPage();
    void clearMasterPasswordAndConvert(bool forcedAskPass = true);
    bool samePasswordEntry(const PasswordEntry &entry1, const PasswordEntry &entry2);

private:
    Ui::MasterPasswordDialog* ui;
    DatabaseEncryptedPasswordBackend* m_backend;
};

class QDialogButtonBox;
class QLineEdit;
class QLabel;

class AskMasterPassword : public QDialog
{
    Q_OBJECT

public:
    explicit AskMasterPassword(DatabaseEncryptedPasswordBackend* backend, QWidget* parent = nullptr);

private Q_SLOTS:
    void verifyPassword();

private:
    DatabaseEncryptedPasswordBackend* m_backend;
    QDialogButtonBox* m_buttonBox;
    QLineEdit* m_lineEdit;
    QLabel* m_labelWarning;
};
#endif // DATABASEENCRYPTEDPASSWORDBACKEND_H
