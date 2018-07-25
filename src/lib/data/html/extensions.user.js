// ==UserScript==
// @name     _falkon_extensions
// @run-at   document-end
// @include  falkon:extensions
// ==/UserScript==

(function() {

    function createNavigation() {

        $("#extensions-button").click(function() {
            display("extensions", this);
        });

        $("#themes-button").click(function() {
            display("themes", this);
        });

        function display(id, button) {
            var content = $("#" + id).html();
            $("#content").html(content);
            $(".sidebar-menu").removeClass("active");
            $("#" + button.id).addClass("active");
        }
    }

    function initToggleDescription() {
        $(document).on("click", ".extension-description-state-button", function() {
            $(".extension-description-short").addClass("visible");
            $(".extension-description").removeClass("visible");
            var extensionId = this.id.substring(0, this.id.length - "-description-state-button-id".length);
            document.getElementById(extensionId + "-description-short-id").className = document.getElementById(extensionId + "-description-short-id").className.replace("visible", "");
            document.getElementById(extensionId + "-description-id").className += " visible";
        });

        $(document).on("click", ".theme-description-state-button", function() {
            $(".theme-description-short").addClass("visible");
            $(".theme-description").removeClass("visible");
            var themeId = this.id.substring(0, this.id.length - "-description-state-button-id".length);
            document.getElementById(themeId + "-description-short-id").className = document.getElementById(themeId + "-description-short-id").className.replace("visible", "");
            document.getElementById(themeId + "-description-id").className += " visible";
        });
    }

    function initControlButtonsAndCheckbox() {
        $(document).on("click", ".extension-control-button.enable", function() {
            var extensionId = this.id.substring(0, this.id.length - "-enable-button-id".length);
            external.extensions.pluginStateChanged(extensionId);
            if (this.html() == "Enable") {
                document.getElementById(extensionId + "-settings-button-id").disabled = true;
                this.html("Disable");
            } else {
                document.getElementById(extensionId + "-settings-button-id").disabled = false;
                this.html("Enable");
            }
        });

        $(document).on("click", ".extension-control-button.settings", function() {
            var extensionId = this.id.substring(0, this.id.length - "-settings-button-id".length);
            external.extensions.showSettings(extensionId);
        });

        $(document).on("click", ".extension-control-button.remove", function() {
            var extensionId = this.id.substring(0, this.id.length - "-remove-button-id".length);
            external.extensions.removeExtension(extensionId);
        });

        $(document).on("click", ".allow-in-incognito-button-checkbox", function() {
            var extensionId = this.id.substring(0, this.id.length - "-allow-in-incognito-id".length);
            external.extensions.allowInIncognito(extensionId, this.checked);
        });

        $(document).on("click", ".theme-control-button.enable", function() {
            $(".theme-control-button.enable").prop('disabled', false);
            this.disabled = true;
            var themeId = this.id.substring(0, this.id.length - "-enable-button-id".length);
            external.themes.makeCurrent(themeId);
        });

        $(document).on("click", ".theme-control-button.license", function() {
            var themeId = this.id.substring(0, this.id.length - "-license-button-id".length);
            external.themes.showLicense(themeId);
        });
    }

    function init() {
        createNavigation();
        initToggleDescription();
        initControlButtonsAndCheckbox();

        external.extensions.requestSync();
        external.extensions.reload.connect(function() { location.reload() });
    }

    %JQUERY%
    %JQUERY-UI%

    if (window._falkon_external) {
        init();
    } else {
        document.addEventListener("_falkon_external_created", init);
    }
})();
