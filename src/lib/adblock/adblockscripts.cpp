/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2025 Juraj Oravec <jurajoravec@mailo.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */

#include "adblockscripts.h"

QString AdBlockScripts::removeRulesScript(const QString &rules)
{
    QString source = QL1S(
        "(function() {"
        "var rules = [%1];"
        "rules.forEach(function myFunction(rule) {"
            "var elements = document.querySelectorAll(rule);"
            "elements.forEach(function(element){element.remove();});"
        "})"
        "})()"
    );

    return source.arg(rules);
}
