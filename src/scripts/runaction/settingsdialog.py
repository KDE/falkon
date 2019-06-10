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
import os
from PySide2 import QtCore, QtWidgets, QtUiTools
from runaction.i18n import i18n


class SettingsDialog(QtWidgets.QDialog):
    def __init__(self, manager, parent=None):
        super().__init__(parent)

        self.manager = manager

        file = QtCore.QFile(os.path.join(os.path.dirname(__file__), "settings.ui"))
        file.open(QtCore.QFile.ReadOnly)
        self.ui = QtUiTools.QUiLoader().load(file, self)
        file.close()

        layout = QtWidgets.QVBoxLayout(self)
        layout.addWidget(self.ui)

        self.setMinimumSize(400, 250)
        self.setWindowTitle(i18n("Run Action Settings"))
        self.ui.label.setText(i18n("Available actions"))

        for action in self.manager.actions:
            item = QtWidgets.QListWidgetItem(self.ui.listWidget)
            item.setText(action.title)
            item.setIcon(action.icon)
            item.setData(QtCore.Qt.UserRole, action.id)
            item.setFlags(item.flags() | QtCore.Qt.ItemIsUserCheckable)
            item.setCheckState(QtCore.Qt.Unchecked if action.id in self.manager.disabledActions else QtCore.Qt.Checked)
            self.ui.listWidget.addItem(item)

        self.ui.buttonBox.accepted.connect(self.accept)
        self.ui.buttonBox.rejected.connect(self.reject)

    def accept(self):
        disabled = []
        for i in range(self.ui.listWidget.count()):
            item = self.ui.listWidget.item(i)
            if item.checkState() == QtCore.Qt.Unchecked:
                disabled.append(item.data(QtCore.Qt.UserRole))
        self.manager.disabledActions = disabled
        super().accept()
