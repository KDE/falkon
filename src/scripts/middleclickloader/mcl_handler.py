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
from PySide2 import QtCore, QtGui, QtWidgets

from middleclickloader.mcl_loadmode import MCL_LoadMode
from middleclickloader.mcl_settings import MCL_Settings


class MCL_Handler(QtCore.QObject):
    settingsFile = ""
    loaded = False

    onlyValidUrl = False
    loadMode = 0

    def __init__(self, settingsPath, parent=None):
        super().__init__(parent)

        self.settingsFile = settingsPath + "/extensions.ini"

    def loadSettings(self):
        settings = QtCore.QSettings(self.settingsFile, QtCore.QSettings.IniFormat)
        settings.beginGroup("MiddleClickLoader")
        self.loadMode = int(settings.value("LoadMode", MCL_LoadMode.NEW_TAB))
        self.onlyValidUrl = bool(settings.value("OnlyValidUrl", True))
        settings.endGroup()

        self.loaded = True

    def showSettings(self, parent=None):
        self.settings = MCL_Settings(self.settingsFile, parent)
        self.settings.accepted.connect(self.loadSettings)

        self.settings.exec_()

    def mousePress(self, view, event):
        if not self.loaded:
            self.loadSettings()

        if event.buttons() == QtCore.Qt.MiddleButton:
            res = view.page().hitTestContent(event.pos())

            if res.isContentEditable() or not res.linkUrl().isEmpty():
                return False

            selectionClipboard = QtWidgets.QApplication.clipboard().text(QtGui.QClipboard.Selection)

            if selectionClipboard:
                guessedUrl = QtCore.QUrl.fromUserInput(selectionClipboard)
                isValid = view.isUrlValid(guessedUrl)

                if self.onlyValidUrl and not isValid:
                    return False

                if not isValid:
                    searchManager = Falkon.MainApplication.instance().searchEnginesManager()
                    engine = searchManager.defaultEngine()
                    req = searchManager.searchResult(engine, selectionClipboard)
                    guessedUrl = req.url()

                return self.loadUrl(view, guessedUrl)
        return False

    def loadUrl(self, view, url):
        if self.loadMode == MCL_LoadMode.NEW_TAB:
            view.openUrlInNewTab(url, Falkon.Qz.NT_NotSelectedTab)
        elif self.loadMode == MCL_LoadMode.CURRENT_TAB:
            view.load(url)
        elif self.loadMode == MCL_LoadMode.NEW_WINDOW:
            Falkon.MainApplication.instance().createWindow(Falkon.Qz.BW_NewWindow, url)
        else:
            return False
        return True
