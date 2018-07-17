# ============================================================
# MiddleClickLoader - plugin for Falkon
# Copyright (C) 2018 Juraj Oravec <sgd.orava@gmail.com>
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
from middleclickloader.mcl_handler import MCL_Handler


class MCL_Plugin(Falkon.PluginInterface, QtCore.QObject):
    handler = None

    def init(self, state, settingsPath):
        plugins = Falkon.MainApplication.instance().plugins()
        plugins.registerAppEventHandler(Falkon.PluginProxy.MousePressHandler, self)

        self.handler = MCL_Handler(settingsPath)

    def unload(self):
        del self.handler
        self.handler = None

    def testPlugin(self):
        return True

    def mousePress(self, type, obj, event):
        if type == Falkon.Qz.ON_WebView:
            return self.handler.mousePress(obj, event)

        return False

    def showSettings(self, parent):
        pass
        self.handler.showSettings(parent)


Falkon.registerPlugin(MCL_Plugin())
