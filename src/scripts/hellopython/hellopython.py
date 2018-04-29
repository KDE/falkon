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
from PySide2 import QtCore, QtGui, QtWidgets
from hellopython import sidebar, button
from hellopython.i18n import i18n

class HelloPlugin(Falkon.PluginInterface, QtCore.QObject):
    buttons = {}

    def init(self, state, settingsPath):
        print("{} {}".format(state, settingsPath))

        plugins = Falkon.MainApplication.instance().plugins()
        plugins.registerAppEventHandler(Falkon.PluginProxy.MousePressHandler, self)

        plugins.mainWindowCreated.connect(self.mainWindowCreated)
        plugins.mainWindowDeleted.connect(self.mainWindowDeleted)

        self.sidebar = sidebar.HelloSidebar()
        Falkon.SideBarManager.addSidebar("hellopython-sidebar", self.sidebar)

        self.schemeHandler = HelloSchemeHandler()
        Falkon.MainApplication.instance().networkManager().registerExtensionSchemeHandler("hello", self.schemeHandler)

        if state == Falkon.PluginInterface.LateInitState:
            for window in Falkon.MainApplication.instance().windows():
                self.mainWindowCreated(window)

    def unload(self):
        print("unload")

        Falkon.SideBarManager.removeSidebar(self.sidebar)

        for window in Falkon.MainApplication.instance().windows():
            self.mainWindowDeleted(window)

    def testPlugin(self):
        return True

    def populateWebViewMenu(self, menu, view, r):
        self.view = view

        title = ""
        if not r.imageUrl().isEmpty():
            title += " on image"

        if not r.linkUrl().isEmpty():
            title += " on link"

        if r.isContentEditable():
            title += " on input"

        menu.addAction(i18n("My first plugin action") + title, self.actionSlot)

    def mousePress(self, type, obj, event):
        print("mousePress {} {} {}".format(type, obj, event))
        return False

    def actionSlot(self):
        QtWidgets.QMessageBox.information(self.view, i18n("Hello"), i18n("First plugin action works :-)"))

    def showSettings(self, parent):
        self.settings = QtWidgets.QDialog(parent)
        b = QtWidgets.QPushButton("Hello Python v0.0.1")
        closeButton = QtWidgets.QPushButton(i18n("Close"))
        label = QtWidgets.QLabel()
        label.setPixmap(QtGui.QPixmap(":icons/other/about.svg"))

        l = QtWidgets.QVBoxLayout(self.settings)
        l.addWidget(label)
        l.addWidget(b)
        l.addWidget(closeButton)

        self.settings.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.settings.setWindowTitle(i18n("Hello Python Settings"))
        self.settings.setWindowIcon(QtGui.QIcon(":icons/falkon.svg"))
        closeButton.clicked.connect(self.settings.close)

        self.settings.show()

    def mainWindowCreated(self, window):
        b = button.HelloButton()
        window.statusBar().addButton(b)
        window.navigationBar().addToolButton(b)
        self.buttons[window] = b

    def mainWindowDeleted(self, window):
        if not window in self.buttons: return
        b = self.buttons[window]
        window.statusBar().removeButton(b)
        window.navigationBar().removeToolButton(b)
        del self.buttons[window]

Falkon.registerPlugin(HelloPlugin())

class HelloSchemeHandler(Falkon.ExtensionSchemeHandler):
    def requestStarted(self, job):
        print("req {}".format(job.requestUrl()))
        self.setReply(job, "text/html", "<h1>TEST</h1>{}".format(job.requestUrl()))
