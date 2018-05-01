// ==UserScript==
// @name     _falkon_restore
// @run-at   document-end
// @include  falkon:restore
// ==/UserScript==

(function() {

var scriptData = {};
var selectedRow = null;

function selectRow(row)
{
    if (selectedRow) {
        selectedRow.className = selectedRow.className.replace(/\bselected\b/, "");
    }

    row.className = row.className + " selected";
    selectedRow = row;
}

function forEachInput(f)
{
    var inputs = document.getElementsByTagName("input");
    for (var i = 0; i < inputs.length; ++i) {
        f(inputs[i]);
    }
}

function toggleWindow(e)
{
    var win = e.getAttribute("data-window");

    forEachInput(function(input) {
        if (input.getAttribute("data-window") == win) {
            input.checked = e.checked;
        }
    });
}

function toggleTab(e)
{
    var win = e.getAttribute("data-window");
    var winElement = null;
    var checked = 0;
    var total = 0;

    forEachInput(function(input) {
        if (input.getAttribute("data-window") != win) {
            return;
        }
        if (!input.hasAttribute("data-tab")) {
            winElement = input;
            return;
        }
        if (input.checked) {
            ++checked;
        }
        ++total;
    });

    if (checked == total) {
        winElement.checked = true;
        winElement.indeterminate = false;
    } else if (checked > 0) {
        winElement.indeterminate = true;
    } else {
        winElement.checked = false;
        winElement.indeterminate = false;
    }
}

function startNewSession()
{
    document.getElementById("start-new-session-button").disabled = true;
    external.recovery.startNewSession();
}

function restoreSession()
{
    document.getElementById("restore-session-button").disabled = true;

    var excludeWin = [];
    var excludeTab = [];

    forEachInput(function(input) {
        if (input.checked || input.indeterminate || !input.hasAttribute("data-tab")) {
            return;
        }
        excludeWin.unshift(input.getAttribute("data-window"));
        excludeTab.unshift(input.getAttribute("data-tab"));
    });

    external.recovery.restoreSession(excludeWin, excludeTab);
}

function addWindow(winId)
{
    var tr = document.createElement("tr");
    tr.className = "window";
    tr.onclick = function() { selectRow(tr); };
    var td = document.createElement("td");
    var input = document.createElement("input");
    input.type = "checkbox";
    input.checked = true;
    input.setAttribute("data-window", winId);
    input.onclick = function() { toggleWindow(input); };
    var span = document.createElement("span");
    span.innerText = scriptData.window + " " + (winId + 1);

    tr.appendChild(td);
    td.appendChild(input);
    td.appendChild(span);

    document.getElementById("recovery-items").appendChild(tr);
}

function addTab(winId, tab)
{
    var tr = document.createElement("tr");
    tr.className = "tab";
    tr.title = tab.url;
    tr.onclick = function() { selectRow(tr); };
    var td = document.createElement("td");
    var input = document.createElement("input");
    input.type = "checkbox";
    input.checked = true;
    input.setAttribute("data-window", winId);
    input.setAttribute("data-tab", tab.tab);
    input.onclick = function() { toggleTab(input); };
    var img = document.createElement("img");
    img.src = tab.icon;
    var span = document.createElement("span");
    span.innerText = tab.title;

    if (tab.pinned) {
        span.innerText = "🖈 " + span.innerText;
    }
    if (tab.current) {
        span.style.fontStyle = 'italic';
    }

    tr.appendChild(td);
    td.appendChild(input);
    td.appendChild(img);
    td.appendChild(span);

    document.getElementById("recovery-items").appendChild(tr);
}

function init()
{
    scriptData = document.getElementById("script-data").dataset;

    document.getElementById("start-new-session-button").onclick = function() {
        startNewSession();
        return false;
    };

    document.getElementById("restore-session-button").onclick = function() {
        restoreSession();
        return false;
    };

    var data = external.recovery.restoreData;
    for (var i = 0; i < data.length; ++i) {
        var win = data[i];
        addWindow(win.window);
        for (var j = 0; j < win.tabs.length; ++j) {
            var tab = win.tabs[j];
            addTab(win.window, tab);
        }
    }
}

// Initialize
if (window._falkon_external) {
    init();
} else {
    document.addEventListener("_falkon_external_created", init);
}

})();
