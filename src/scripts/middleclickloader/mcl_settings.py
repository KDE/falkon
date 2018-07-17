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

import os
from PySide2 import QtCore, QtWidgets, QtUiTools
from middleclickloader.i18n import i18n
from middleclickloader.mcl_loadmode import MCL_LoadMode


class MCL_Settings(QtWidgets.QDialog):
    settingsFile = ""
    ui = None

    def __init__(self, settingsFile, parent=None):
        super().__init__(parent)

        self.settingsFile = settingsFile

        file = QtCore.QFile(os.path.join(os.path.dirname(__file__), "mcl_settings.ui"))
        file.open(QtCore.QFile.ReadOnly)
        self.ui = QtUiTools.QUiLoader().load(file, self)
        file.close()

        layout = QtWidgets.QVBoxLayout(self)
        layout.addWidget(self.ui)
        self.setLayout(layout)

        self.setWindowTitle(i18n("MiddleClickLoader Setting"))
        self.ui.label_header.setText("<h2>{}</h2>".format(i18n("MiddleClickLoader")))
        self.ui.label_loadMode.setText(i18n("Open url in:"))
        self.ui.onlyValidUrl.setText(i18n("Use only valid url"))

        self.ui.loadMode.addItem(i18n("New Tab"), MCL_LoadMode.NEW_TAB)
        self.ui.loadMode.addItem(i18n("Current Tab"), MCL_LoadMode.CURRENT_TAB)
        self.ui.loadMode.addItem(i18n("New Window"), MCL_LoadMode.NEW_WINDOW)

        settings = QtCore.QSettings(self.settingsFile, QtCore.QSettings.IniFormat)
        settings.beginGroup("MiddleClickLoader")
        self.ui.loadMode.setCurrentIndex(int(settings.value("LoadMode", MCL_LoadMode.NEW_TAB)))
        self.ui.onlyValidUrl.setChecked(bool(settings.value("OnlyValidUrl", True)))
        settings.endGroup()

        self.ui.buttonBox.accepted.connect(self.accept)
        self.ui.buttonBox.rejected.connect(self.reject)

    def accept(self):
        settings = QtCore.QSettings(self.settingsFile, QtCore.QSettings.IniFormat)
        settings.beginGroup("MiddleClickLoader")
        settings.setValue("LoadMode", self.ui.loadMode.currentIndex())
        settings.setValue("OnlyValidUrl", self.ui.onlyValidUrl.isChecked())
        settings.endGroup()

        super().accept()
        self.close()
