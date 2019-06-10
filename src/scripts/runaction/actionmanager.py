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
import subprocess
from PySide2 import QtCore
from runaction.action import Action
from runaction.settingsdialog import SettingsDialog


class ActionManager(QtCore.QObject):
    actions = []

    def __init__(self, settingsPath, parent=None):
        super().__init__(parent)

        self.settingsPath = settingsPath
        settings = QtCore.QSettings(self.settingsPath + "/extensions.ini", QtCore.QSettings.IniFormat)
        self._disabledActions = settings.value("RunAction/disabledActions") or []
        self.loadActions()

    def getActions(self, webView, r=None):
        out = []
        menus = {}

        for action in list(filter(lambda a: a.id not in self.disabledActions, self.actions)):
            url = webView.url()
            text = ""
            if r and webView.selectedText():
                cond = Action.TypeCondition.Text
                text = webView.selectedText()
            elif r and not r.linkUrl().isEmpty():
                cond = Action.TypeCondition.Link
                url = r.linkUrl()
            elif r and not r.imageUrl().isEmpty():
                cond = Action.TypeCondition.Image
                url = r.imageUrl()
            elif r and not r.mediaUrl().isEmpty():
                cond = Action.TypeCondition.Media
                url = r.mediaUrl()
            else:
                cond = Action.TypeCondition.Page

            if action.testAction(cond, url):
                act = Falkon.Action(action.icon, action.title, self)
                act.triggered.connect(lambda a=action, w=webView, u=url, t=text: self.execAction(a, w, u, t))
                if action.submenu:
                    if action.submenu not in menus:
                        menu = Falkon.Menu(action.menuTitle, webView)
                        menus[action.submenu] = menu
                        out.append(menu)
                    menus[action.submenu].addAction(act)
                else:
                    out.append(act)

        return out

    @property
    def disabledActions(self):
        return self._disabledActions

    @disabledActions.setter
    def disabledActions(self, value):
        settings = QtCore.QSettings(self.settingsPath + "/extensions.ini", QtCore.QSettings.IniFormat)
        settings.setValue("RunAction/disabledActions", value)
        self._disabledActions = value

    def showSettings(self, parent=None):
        dialog = SettingsDialog(self, parent)
        dialog.exec_()

    def execAction(self, action, webView, url, text=""):
        command = action.execAction(url, text)
        if action.actionType == Action.Type.Command:
            subprocess.Popen(command, shell=True)
        elif action.actionType == Action.Type.Url:
            webView.openUrlInNewTab(QtCore.QUrl(command), Falkon.Qz.NT_SelectedTab)

    def loadActions(self):
        self.actions = []

        paths = [
            os.path.join(os.path.dirname(__file__), "actions"),
            os.path.join(self.settingsPath, "runaction")
        ]

        for path in paths:
            if not os.path.exists(path):
                continue
            for file in os.listdir(path):
                if not file.endswith(".desktop"):
                    continue
                fileName = os.path.join(path, file)
                try:
                    action = Action(fileName)
                except Exception as e:
                    print("Failed to parse {}: {}".format(fileName, e))
                finally:
                    if action.supported:
                        self.actions.append(action)
