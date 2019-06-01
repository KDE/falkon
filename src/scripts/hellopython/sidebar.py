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
from hellopython.i18n import i18n

class HelloSidebar(Falkon.SideBarInterface):
    def title(self):
        return i18n("Hello Python Sidebar")

    def createMenuAction(self):
        act = QtWidgets.QAction(i18n("Hello Python Sidebar"))
        act.setCheckable(True)
        return act

    def createSideBarWidget(self, window):
        w = QtWidgets.QWidget()
        b = QtWidgets.QPushButton("Hello Python v0.0.1")
        label = QtWidgets.QLabel()
        label.setPixmap(QtGui.QPixmap(":icons/other/about.svg"))
        l = QtWidgets.QVBoxLayout(w)
        l.addWidget(label)
        l.addWidget(b)
        w.setLayout(l)
        return w
