# ============================================================
# Falkon - Qt web browser
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
import gettext
from PySide2 import QtCore

locale = QtCore.QLocale.system()
languages = [ locale.name(), locale.bcp47Name() ]
i = locale.name().find('_')
if i > 0: languages.append(locale.name()[:i])
localedir = QtCore.QStandardPaths.locate(QtCore.QStandardPaths.GenericDataLocation, 'locale', QtCore.QStandardPaths.LocateDirectory)

t = gettext.translation('falkon_' + __package__, localedir, languages, fallback=True)
i18n = t.gettext
i18np = t.ngettext
