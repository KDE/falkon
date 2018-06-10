/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2018 Anmol Gautam <tarptaeya@gmail.com>
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
#pragma once

#include <QObject>
#include <QHash>
#include <QPair>

/**
 * @brief The class exposing MostVisitedUrl type to QML
 */
class QmlMostVisitedUrl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString url READ url CONSTANT)
public:
    explicit QmlMostVisitedUrl(const QString &title = QString(), const QString &url = QString(), QObject *parent = nullptr);
    QString title() const;
    QString url() const;
private:
    QString m_title;
    QString m_url;
};

class QmlMostVisitedUrlData
{
public:
    explicit QmlMostVisitedUrlData();
    ~QmlMostVisitedUrlData();
    QmlMostVisitedUrl *get(const QString &title = QString(), const QString &url = QString());
private:
    QHash<QPair<QString, QString>, QmlMostVisitedUrl*> m_urls;
};
