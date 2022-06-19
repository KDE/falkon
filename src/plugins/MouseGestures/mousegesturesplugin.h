/* ============================================================
* Mouse Gestures plugin for Falkon
* Copyright (C) 2012-2014  David Rosca <nowrep@gmail.com>
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
#ifndef MOUSEGESTURESPLUGIN_H
#define MOUSEGESTURESPLUGIN_H

#include "plugininterface.h"

class MouseGestures;
class MouseGesturesPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "Falkon.Browser.plugin.MouseGestures" FILE "mousegestures.json")

public:
    MouseGesturesPlugin();

    void init(InitState state, const QString &settingsPath) override;
    void unload() override;
    bool testPlugin() override;
    void showSettings(QWidget *parent) override;

    bool mousePress(Qz::ObjectName type, QObject *obj, QMouseEvent *event) override;
    bool mouseRelease(Qz::ObjectName type, QObject *obj, QMouseEvent *event) override;
    bool mouseMove(Qz::ObjectName type, QObject *obj, QMouseEvent *event) override;

private:
    MouseGestures* m_gestures = nullptr;

};

#endif // MOUSEGESTURESPLUGIN_H
