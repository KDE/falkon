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
import os
from PySide2 import QtGui, QtWidgets
from runaction.i18n import i18n


class RunActionButton(Falkon.AbstractButtonInterface):
    def __init__(self, manager):
        super().__init__()
        self.manager = manager

        self.setIcon(QtGui.QIcon(os.path.join(os.path.dirname(__file__), "icon.svg")))
        self.setTitle(i18n("Run Action"))
        self.setToolTip(i18n("Run action on current page"))

        self.clicked.connect(self.onClicked)

    def id(self):
        return "runaction-button"

    def name(self):
        return i18n("RunAction button")

    def onClicked(self, controller):
        self.menu = QtWidgets.QMenu()

        for action in self.manager.getActions(self.webView()):
            self.menu.addAction(action)

        self.menu.addSeparator()
        self.menu.addAction(QtGui.QIcon.fromTheme("configure"), i18n("Configure..."), self.manager.showSettings)

        self.menu.popup(controller.callPopupPosition(self.menu.sizeHint()))
        self.menu.aboutToHide.connect(controller.callPopupClosed)
