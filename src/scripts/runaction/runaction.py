# ============================================================
# RunAction plugin for Falkon
# Copyright (C) 2018 David Rosca <nowrep@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ============================================================
import Falkon
from PySide2 import QtCore
from runaction import actionmanager, button


class RunActionPlugin(Falkon.PluginInterface, QtCore.QObject):
    buttons = {}
    manager = None

    def init(self, state, settingsPath):
        plugins = Falkon.MainApplication.instance().plugins()
        plugins.mainWindowCreated.connect(self.mainWindowCreated)
        plugins.mainWindowDeleted.connect(self.mainWindowDeleted)

        self.manager = actionmanager.ActionManager(settingsPath)

        if state == Falkon.PluginInterface.LateInitState:
            for window in Falkon.MainApplication.instance().windows():
                self.mainWindowCreated(window)

    def unload(self):
        for window in Falkon.MainApplication.instance().windows():
            self.mainWindowDeleted(window)

        self.manager = None

    def testPlugin(self):
        return True

    def populateWebViewMenu(self, menu, view, r):
        for action in self.manager.getActions(view, r):
            if action.inherits("QMenu"):
                menu.addMenu(action).setParent(menu)
            else:
                action.setParent(menu)
                menu.addAction(action)

    def showSettings(self, parent):
        self.manager.showSettings(parent)

    def mainWindowCreated(self, window):
        b = button.RunActionButton(self.manager)
        window.statusBar().addButton(b)
        window.navigationBar().addToolButton(b)
        self.buttons[window] = b

    def mainWindowDeleted(self, window):
        if window not in self.buttons:
            return
        b = self.buttons[window]
        window.statusBar().removeButton(b)
        window.navigationBar().removeToolButton(b)
        del self.buttons[window]


Falkon.registerPlugin(RunActionPlugin())
