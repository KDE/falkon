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
import re
import enum
import shlex
from PySide2 import QtCore, QtGui


class Action():
    class Type(enum.Enum):
        Invalid, Url, Command = range(3)

    class TypeCondition(enum.Enum):
        Page, Link, Image, Media, Text = range(5)

    id = ""
    title = ""
    menuTitle = ""
    icon = QtGui.QIcon()
    actionType = Type.Invalid
    typeCondition = [ TypeCondition.Page, TypeCondition.Link, TypeCondition.Image, TypeCondition.Media ]
    urlCondition = ".*"
    submenu = ""
    normalExec = ""
    textExec = ""
    supported = False

    def __init__(self, fileName):
        data = Falkon.DesktopFile(fileName)
        self.id = os.path.splitext(os.path.basename(fileName))[0]
        self.title = data.name()
        self.menuTitle = data.comment()
        self.icon = QtGui.QIcon.fromTheme(data.icon(), QtGui.QIcon(os.path.join(os.path.dirname(fileName), data.icon())))
        self.actionType = Action.Type[data.value("X-RunAction-Type")]
        self.typeCondition = list(map(lambda s: Action.TypeCondition[s], data.value("X-RunAction-TypeCondition").split(";")))
        self.urlCondition = data.value("X-RunAction-UrlCondition") or self.urlCondition
        self.submenu = data.value("X-RunAction-Submenu") or self.submenu
        self.normalExec = data.value("X-RunAction-Exec") or self.normalExec
        self.textExec = data.value("X-RunAction-TextExec") or self.normalExec
        self.supported = data.tryExec()

    def testAction(self, condition, url):
        if not self.supported:
            return False
        if condition not in self.typeCondition:
            return False
        if not re.match(self.urlCondition, url.toString()):
            return False
        return True

    def execAction(self, url, text=""):
        url = str(url.toEncoded(), "utf-8")
        if self.actionType == Action.Type.Command:
            url = shlex.quote(url)
            text = shlex.quote(text)
        elif self.actionType == Action.Type.Url:
            url = str(QtCore.QUrl.toPercentEncoding(url), "utf-8")
            text = str(QtCore.QUrl.toPercentEncoding(text), "utf-8")
        command = self.normalExec if text == "" else self.textExec
        command = command.replace("{url}", url)
        command = command.replace("{text}", text)
        command = command.replace("{lang}", QtCore.QLocale.system().name()[:2])
        return command
