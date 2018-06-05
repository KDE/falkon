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
#include "webtab.h"
#include "../windows/qmlwindow.h"

class QmlTab : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(int zoomLevel READ zoomLevel CONSTANT)
    Q_PROPERTY(int index READ index CONSTANT)
    Q_PROPERTY(bool pinned READ pinned CONSTANT)
    Q_PROPERTY(bool muted READ muted CONSTANT)
    Q_PROPERTY(bool restored READ restored CONSTANT)
    Q_PROPERTY(bool current READ current CONSTANT)
    Q_PROPERTY(bool playing READ playing CONSTANT)
    Q_PROPERTY(QmlWindow* browserWindow READ browserWindow CONSTANT)
public:
    explicit QmlTab(WebTab *webTab = 0, QObject *parent = 0);
    QString url() const;
    QString title() const;
    int zoomLevel() const;
    int index() const;
    bool pinned() const;
    bool muted() const;
    bool restored() const;
    bool current() const;
    bool playing() const;
    QmlWindow *browserWindow() const;

    Q_INVOKABLE void detach();
    Q_INVOKABLE void setZoomLevel(const QVariantMap &map);
    Q_INVOKABLE void stop();
    Q_INVOKABLE void reload();
    Q_INVOKABLE void unload();
    Q_INVOKABLE void load(const QVariantMap &map);

Q_SIGNALS:
    void titleChanged(const QVariantMap &map);
    void pinnedChanged(const QVariantMap &map);
    void loadingChanged(const QVariantMap &map);
    void mutedChanged(const QVariantMap &map);
    void restoredChanged(const QVariantMap &map);
    void playingChanged(const QVariantMap &map);
private:
    WebTab *m_webTab;
};
