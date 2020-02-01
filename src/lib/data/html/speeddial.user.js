// ==UserScript==
// @name     _falkon_speeddial
// @run-at   document-end
// @include  falkon:speeddial
// ==/UserScript==

var scriptData = {};
var editingId = -1;
var ignoreNextChanged = false;

function b64DecodeUnicode(str) {
    return decodeURIComponent(atob(str).split('').map(function(c) {
        return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2);
    }).join(''));
}

function emitChanged(pages)
{
    ignoreNextChanged = true;
    external.speedDial.changed(pages);
}

function addSpeedDial()
{
    onEditClick(addBox('', scriptData.newPage , ''));
    alignPage();
}

function configureSpeedDial()
{
    // ====================== LOAD SETTINGS ========================= //
    $('#PgInRow').val(scriptData.maxPagesRow);
    $('#sliderValuePg').html(scriptData.maxPagesRow);
    $('#SdSize').val(scriptData.dialWidth);
    $('#SdSizeToggle').prop('checked', scriptData.dialWidth != 240);
    $('#sliderValueSd').html(scriptData.dialWidth);
    $('#BgImgSelSiz').val(scriptData.bSize).attr('selected', 'selected');
    $('#BgImgToggle').prop('checked', scriptData.imgBackground != "");
    $('#SdCntrToggle').prop('checked', scriptData.sdCenter == "true");
    $('#BgImgToggle').is(':checked') ? $('#BgImgSel').removeAttr('disabled') : $('#BgImgSel').attr('disabled', 'disabled');
    $('#BgImgToggle').is(':checked') ? $('#BgImgSelSiz').removeAttr('disabled') : $('#BgImgSelSiz').attr('disabled', 'disabled');
    $('#SdSizeToggle').is(':checked') ? $('#SdSize').removeAttr('disabled') : $('#SdSize').attr('disabled', 'disabled');
    if ($('#BgImgToggle').prop('checked') != true) {
        $('#ImgSelectorMenu').css({'color' : 'rgba(0,0,0, 0.0)', 'text-shadow' : 'none'});
        $('#BgImgSel').css({'color' : 'rgba(0,0,0, 0.0)', 'text-shadow' : 'none'});
        $('#BgImgSelSiz').css('visibility', 'hidden');
        }
    if ($('#SdSizeToggle').prop('checked') != true)
        $('#SdSizeStateColor').css('color', 'rgba(0,0,0, 0.0)');
    // ======================== SHOW DIALOG ======================== //
    $('#fadeOverlay2').css({'filter' : 'alpha(opacity=100)'}).fadeIn();
    $('#fadeOverlay2').click(function() { $(this).fadeOut('slow'); });
    $('#settingsBox').click(function(event) { event.stopPropagation(); });
}

function escapeTitle(title) {
    title = title.replace(/"/g, '&quot;');
    title = title.replace(/'/g, '&apos;');
    return title;
}

function unescapeTitle(title) {
    title = title.replace(/&quot;/g, '"');
    title = title.replace(/&apos;/g, '\'');
    return title;
}

function escapeUrl(url) {
    url = url.replace(/"/g, '');
    url = url.replace(/'/g, '');
    return url;
}

function onRemoveClick(box) {
    removeBox($(box).index());
}

function onFetchTitleClick(checkbox) {
    var displayStyle;
    checkbox.checked ? displayStyle = 'hidden' : displayStyle = 'visible';
    $('#titleLine').css({'visibility' : displayStyle });
}

function hideEditBox() {
    $('#fadeOverlay').fadeOut("slow", function() {$("#fadeOverlay").remove();});
}

function onEditClick(box) {
    editingId = $(box).index();
    var boxUrl = $(box).children('a').first().attr('href');
    var boxTitle = escapeTitle($(box).children('span').first().text());
    if (boxUrl === '')
        boxUrl = 'http://';

    $('body').append('<div id="fadeOverlay" class="overlay" style="display:none;">' +
        '<div id="overlay-edit">' +
        '<img src="' + $(box).children('img').first().attr('src') + '"> ' +
        '<table><tr><td>' + scriptData.url + ': </td><td>' +
        '<input type="text" id="formUrl" value="' + boxUrl + '"></td></tr>' +
        '<tr id="titleLine"><td>' + scriptData.title + ': </td><td>' +
        '<input type="text" id="formTitle" value="' + boxTitle + '"></td></tr>' +
        '<tr><td></td><td><div class="checkbox"><input type="checkbox" id="fetchTitle">' +
        '<label for="fetchTitle"></label>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  ' + scriptData.titleFetchTitle + ' </div></td></tr>' +
        '</table><p class="buttonbox"><input id="cancelEditBox" type="button" value=" ' + scriptData.cancel + ' ">&nbsp;&nbsp;&nbsp;' +
        '<input id="acceptEditBox" type="button" value="   ' + scriptData.edit + '   "></p>' +
        '</div></div>');

    $('#fetchTitle').click(function() {
        onFetchTitleClick(this);
    });

    $('#cancelEditBox').click(function() {
        hideEditBox();
        location.reload();
    });

    $('#acceptEditBox').click(boxEdited);

    $('#fadeOverlay').css({'filter' : 'alpha(opacity=100)'}).fadeIn();
    $('#fadeOverlay').click(function() {hideEditBox()});
    $('#overlay-edit').click(function(event) { event.stopPropagation(); });

    var temp = $('#formUrl').val();
    $('#formUrl').focus().val('').val(temp); // focus and move cursor to end
}

function onReloadClick(box) {
    var url = $(box).children('a').first().attr('href');
    var img = $(box).children('img').first();

    if (url === '')
        return;

    $(img).attr('src', scriptData.loadingImage);
    external.speedDial.loadThumbnail(url, false);
}

function boxEdited() {
    if (editingId == -1)
        return;

    external.speedDial.urlFromUserInput($('#formUrl').val(), function(newUrl) {
        var box = document.getElementById('quickdial').getElementsByTagName('div')[editingId];
        var a = box.getElementsByTagName('a')[0];
        var originalUrl = a.getAttribute('href');
        setBoxUrl(editingId, newUrl);
        setBoxTitle(editingId, $('#formTitle').val());
        var changedUrl = a.getAttribute('href');
        var fetchTitleChecked = document.getElementById('fetchTitle').checked;

        var pages = allPages();

        if (fetchTitleChecked || (originalUrl != changedUrl && changedUrl !== '') ) {
            var img = box.getElementsByTagName('img')[0];
            img.setAttribute('src', scriptData.loadingImage);

            $('#fadeOverlay').fadeOut("slow", function() {
                $("#fadeOverlay").remove();
            });
            external.speedDial.loadThumbnail(a.getAttribute('href'), fetchTitleChecked);
        } else {
            hideEditBox();
        }
        emitChanged(pages);
    });
}

function allPages() {
    var urls = $('a[class="boxUrl"]');
    var titles = $('span[class="boxTitle"]');
    var value = "";
    $('div.entry').each(function(i) {
        var url = $(this).children('a').first().attr('href');
        var title = $(this).children('span[class="boxTitle"]').first().text();

        value += 'url:"' + escapeUrl(url) + '"|title:"' + escapeTitle(title) + '";';
    });

    return value;
}

function reloadAll() {
    if (confirm(scriptData.titleWarnRel))
        $('div.entry').each(function(i) {
            onReloadClick($(this));
        });
}

function addBox(url, title, img_source) {
    var div = document.createElement('div');
    div.setAttribute('class', 'entry');
    var img = document.createElement('img');
    img.setAttribute('src', img_source);
    var a = document.createElement('a');
    a.setAttribute('href', url);
    a.setAttribute('class', 'boxUrl');
    var span1 = document.createElement('span');
    span1.setAttribute('class', 'boxTitle');
    span1.setAttribute('title', unescapeTitle(title));
    span1.innerText = unescapeTitle(title);
    var span2 = document.createElement('span');
    span2.setAttribute('class', 'edit');
    span2.setAttribute('title', scriptData.titleEdit);
    span2.onclick = function() {
        onEditClick(div);
    };
    var span3 = document.createElement('span');
    span3.setAttribute('class', 'close');
    span3.setAttribute('title', scriptData.titleRemove);
    span3.onclick = function() {
        onRemoveClick(div);
    };
    var span4 = document.createElement('span');
    span4.setAttribute('class', 'reload');
    span4.setAttribute('title', scriptData.tileReload);
    span4.onclick = function() {
        onReloadClick(div);
    };
    div.appendChild(img);
    div.appendChild(img);
    div.appendChild(a);
    div.appendChild(span1);
    div.appendChild(span2);
    div.appendChild(span3);
    div.appendChild(span4);
    document.getElementById("quickdial").appendChild(div);
    if (img_source == scriptData.loadingImage) {
        external.speedDial.loadThumbnail(url, false);
    }
    return div;
}

function setBoxImage(id, img_source) {
    var box = document.getElementById('quickdial').getElementsByTagName('div')[id];
    if (box === undefined)
        return;
    var img = box.getElementsByTagName('img')[0];
    img.setAttribute('src', img_source + '?' + new Date());
}

function setTitleToUrl(url, title) {
    var changed = false;
    var boxes = document.getElementById('quickdial').getElementsByTagName('div');
    for (i = 0; i < boxes.length; ++i) {
        var box = boxes[i];

        if (box === undefined)
            continue;

        var boxUrl = box.getElementsByTagName('a')[0].getAttribute('href');
        if (url != boxUrl)
            continue;

        var span = box.getElementsByTagName('span')[0];
        if (span.innerText != title) {
            changed = true;
            span.innerText = title;
        }
    }
    if (changed)
        emitChanged(allPages());
}

function setImageToUrl(url, img_source) {
    var aElement = $('a[href="' + url + '"]');
    $(aElement).each(function() {
        var box = $(this).parent();
        var imgElement = $(box).children("img").first();
        if ($(imgElement).size() == 0)
            return;
        $(imgElement).attr('src', img_source/* + '?' + new Date()*/);
    });
}

function setBoxUrl(id, url) {
    var box = document.getElementById('quickdial').getElementsByTagName('div')[id];
    if (box === undefined)
        return;
    var a = box.getElementsByTagName('a')[0];
    a.setAttribute('href', url);
}

function setBoxTitle(id, title) {
    var box = document.getElementById('quickdial').getElementsByTagName('div')[id];
    if (box === undefined)
        return;
    var span = box.getElementsByTagName('span')[0];
    span.innerText = title;
}

function removeBox(id) {
    if (confirm(scriptData.titleWarn))
        var box = document.getElementById('quickdial').getElementsByTagName('div')[id];
    if (box === undefined)
        return;
    var url = box.getElementsByTagName('a')[0].getAttribute('href');
    document.getElementById("quickdial").removeChild(box);
    alignPage();
    external.speedDial.removeImageForUrl(url);
    emitChanged(allPages());
}

function alignPage() {
    var dialWidth = parseInt(scriptData.dialWidth);
    var dialHeight = Math.floor(Math.round(dialWidth / 1.54));
    $('head').append('<style>#quickdial img{height:auto;width:'+dialWidth+'px}</style>');
    $('#quickdial div.entry').css({'width' : dialWidth + 'px',
        'height' : dialHeight  + 'px'});
    var fullwidth = $(window).width();
    var width = Math.floor(fullwidth - 76);
    var height = $(window).height();
    var boxWidth = Math.floor(dialWidth + 12);
    var boxHeight = dialHeight + 22;
    var maxBoxes = Math.floor(width / boxWidth);
    if (maxBoxes > scriptData.maxPagesRow) maxBoxes = scriptData.maxPagesRow;
    if (maxBoxes < 1) maxBoxes = 1;
    var maxwidth = maxBoxes * boxWidth;
    $("#quickdial").css('width', maxwidth + 'px');
    var boxesCount = $("#quickdial").children("div").size();
    var rows = Math.ceil(boxesCount / maxBoxes);
    var margintop = (height - rows * boxHeight) / 2;
    if (margintop < 0) margintop = 0;
    $("#quickdial").css('margin-top', margintop + 'px');
    $("span.boxTitle").css('font-size', ((dialHeight - dialWidth / 1.77) / 1.5) + 'px');
    if (scriptData.sdCenter == "true") {
        enableCentering();
    } else {
        disableCentering();
    }
}

function bgImageSel() {
    external.speedDial.getOpenFileName(function(arr) {
        if (arr.length) {
            document.getElementById('BgImgHold').value = arr[0];
            document.getElementById('BgImgHoldUrl').value = arr[1];
            bgImgUpdate();
        }
    });
}

function saveSettings() {
    scriptData.maxPagesRow = $('#PgInRow').val();
    scriptData.dialWidth = $('#SdSize').val();
    scriptData.sdCenter = $('#SdCntrToggle').prop('checked');
    external.speedDial.setBackgroundImage($('#BgImgHoldUrl').val());
    external.speedDial.setBackgroundImageSize($('#BgImgSelSiz').val());
    external.speedDial.setPagesInRow(scriptData.maxPagesRow);
    external.speedDial.setSdSize(scriptData.dialWidth);
    external.speedDial.setSdCentered(scriptData.sdCenter == "true");
    alignPage();
}

function bgImgToggle() {
    var check = document.getElementById('BgImgToggle');
    var BgImgSel = document.getElementById('BgImgSel');
    var BgImgHoldUrl = document.getElementById('BgImgHoldUrl');
    var BgImgSz = document.getElementById('BgImgSelSiz');
    BgImgSel.disabled = (check.checked ? false : true);
    BgImgHoldUrl.disabled = (check.checked ? false : true);
    BgImgSz.disabled = (check.checked ? false : true);
    BgImgHoldUrl.value = (check.checked ? scriptData.urlBackground : '');
    if ($('#BgImgToggle').prop('checked') != true) {
        $('#ImgSelectorMenu').css({'color' : 'rgba(0,0,0, 0.0)', 'text-shadow' : 'none'});
        $('#BgImgSel').css({'color' : 'rgba(0,0,0, 0.0)', 'text-shadow' : 'none'});
        $('#BgImgSelSiz').css('visibility', 'hidden');
    } else {
        $('#ImgSelectorMenu').css({'color' : '#eaeaea', 'text-shadow' : '1px 1px 2px #000000, 0 0 1em #000000'});
        $('#BgImgSel').css({'color' : '#eaeaea', 'text-shadow' : '1px 1px 2px #000000, 0 0 1em #000000'});
        $('#BgImgSelSiz').css('visibility', 'visible');
    };
}

function sdSizeToggle() {
    var check = document.getElementById('SdSizeToggle');
    var SdSize = document.getElementById('SdSize');
    var SdSizeSl = document.getElementById('sliderValueSd');
    SdSize.disabled = (check.checked ? false : true);
    SdSize.value = (check.checked ? SdSize.value : 240);
    SdSizeSl.innerHTML = (check.checked ? scriptData.dialWidth : 240);
    if ($('#SdSizeToggle').prop('checked') != true) {
        $('#SdSizeStateColor').css('color', 'rgba(0,0,0, 0.0)');
    } else {
    $('#SdSizeStateColor').css('color', '#eaeaea')
    }
}

function bgImgUpdate() {
    var imgUrl = document.getElementById('BgImgHold').value;
    var imgSize = document.getElementById('BgImgSelSiz').value;
    var imgThumb = document.getElementById('thumb');
    imgThumb.style.backgroundImage = 'url("' + imgUrl + '")';
    imgThumb.title = imgUrl.substring(imgUrl.lastIndexOf('/')+1);
    imgThumb.style.backgroundSize = imgSize;
    document.documentElement.style.backgroundImage = 'url("' + imgUrl + '")';
    document.documentElement.style.backgroundSize = imgSize;
}

function enableCentering() {
    $('#quickdial div.entry').css({
        float: 'none',
        display: 'inline-block'
    });
}

function disableCentering() {
    $('#quickdial div.entry').css({
        float: scriptData.leftStr,
        display: 'block'
    });
}

function init() {
    scriptData = document.getElementById("script-data").dataset;

    document.getElementById("button-configure-speed-dial").onclick = configureSpeedDial;
    document.getElementById("button-add-speed-dial").onclick = addSpeedDial;
    document.getElementById("PgInRow").oninput = function() {
        $('#sliderValuePg').html(this.value);
    };
    document.getElementById("SdSizeToggle").onchange = sdSizeToggle;
    document.getElementById("SdSize").oninput = function() {
        $('#sliderValueSd').html(this.value);
    };
    document.getElementById("BgImgSel").onclick = function() {
        if ($('#BgImgSelSiz').attr('disabled') != 'disabled') {
            bgImageSel();
        }
    };
    document.getElementById("BgImgToggle").onchange = function() {
        bgImgToggle();
        bgImgUpdate();
    };
    document.getElementById("BgImgSelSiz").onchange = bgImgUpdate;
    document.getElementById("button-cancel").onclick = function() {
        $('#fadeOverlay2').fadeOut('slow');
        location.reload();
    };
    document.getElementById("button-apply").onclick = function() {
        saveSettings();
        $('#fadeOverlay2').fadeOut('slow');
        location.reload();
    };

    if (scriptData.imgBackground == '') {
        document.getElementById("html").style.backgroundSize = "cover";
    }

    $(document).keyup(function(e) {
        if (editingId == -1)
            return;
        if (e.keyCode == 13)
            boxEdited();
        else if (e.keyCode == 27)
            $('#fadeOverlay').click();
    });

    var pages = JSON.parse(b64DecodeUnicode(scriptData.initialScript));
    for (var i = 0; i < pages.length; ++i) {
        var page = pages[i];
        addBox(page.url, page.title, page.img);
    }

    external.speedDial.pagesChanged.connect(function() {
        if (ignoreNextChanged) {
            ignoreNextChanged = false;
            return;
        }
        window.location.reload();
    });

    external.speedDial.thumbnailLoaded.connect(setImageToUrl);
    external.speedDial.pageTitleLoaded.connect(setTitleToUrl);

    $(window).resize(function() { alignPage(); });
    $("#quickdial").sortable({
        revert: true,
        cursor: 'move',
        containment: 'document',
        opacity: 0.8,
        distance: 40,
        start: function(event, ui) {
            disableCentering();
        },
        stop: function(event, ui) {
            if (scriptData.sdCenter == "true")
                enableCentering();
        },
        update: function(event, ui) {
            emitChanged(allPages());
        }
    });

    alignPage();
}

%JQUERY%
%JQUERY-UI%

// Initialize
if (window._falkon_external) {
    init();
} else {
    document.addEventListener("_falkon_external_created", init);
}
