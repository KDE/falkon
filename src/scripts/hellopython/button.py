# ============================================================
# HelloPython plugin for Falkon
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
from PySide2 import QtGui, QtWidgets

class HelloButton(Falkon.AbstractButtonInterface):
    def __init__(self):
        super().__init__()
        self.setIcon(QtGui.QIcon(":icons/other/about.svg"))
        self.setTitle("HelloPython")
        self.setToolTip("Hello Python")

        self.clicked.connect(self.onClicked)

    def id(self):
        return "hellopython-button"

    def name(self):
        return "HelloPython button"

    def onClicked(self, controller):
        self.menu = QtWidgets.QMenu()
        self.menu.addAction("Hello Python", lambda: print("clicked"))
        self.menu.popup(controller.callPopupPosition(self.menu.sizeHint()))
        self.menu.aboutToHide.connect(controller.callPopupClosed)
