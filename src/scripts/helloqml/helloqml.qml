import org.kde.falkon 1.0 as Falkon
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick 2.3

Falkon.PluginInterface {
    init: function(state, settingsPath){
        console.log('"Hello QML" plugin loaded')
    }
    
    testPlugin: function() {
        return true
    }
    
    unload: function() {
        console.log('Bye!')
    }
    
    Falkon.BrowserAction {
        name: 'helloqml-button'
        identity: 'helloqml-id'
        title: 'Testing QML Title'
        toolTip: 'Testing QML Tooltip'
        icon: ':/icons/preferences/extensions.svg'
        location: Falkon.BrowserAction.NavigationToolBar | Falkon.BrowserAction.StatusBar
        popup: Window {
            property var borderMargin: 1
            property var imageWidth: 256
            property var imageHeight: 200
            property var buttonHeight: 40
            
            width: imageWidth + 2 * borderMargin
            height: imageHeight + buttonHeight + 2 * borderMargin
            color: 'black'
            Rectangle {
                anchors.fill: parent
                anchors.leftMargin: borderMargin
                anchors.rightMargin: borderMargin
                anchors.topMargin: borderMargin
                anchors.bottomMargin: borderMargin
                color: 'white'
                Image {
                    id: image
                    source: 'qrc:/icons/other/startpage.svg'
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                }
                
                Button {
                    text: 'Click Me!'
                    height: buttonHeight
                    anchors.top: image.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                }
            }
        }
    }
    
    Falkon.SideBar {
        name: 'helloqml-sidebar'
        title: 'Testing QML SideBar'
        icon: ':/icons/preferences/extensions.svg'
        checkable: true
        Window {
            Image {
                source: 'qrc:/icons/other/startpage.svg'
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
            }
            
            Button {
                text: 'Hello Qml Plugin'
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }
    }
    
    populateWebViewMenu: function(menu, webHitTestResult) {
        var text = 'My first qml plugin action'
        var action = menu.addAction({
            text: text,
            icon: ':/icons/preferences/extensions.svg'
        })
        
        if (webHitTestResult.isImage()) {
            action.update({
                text: text + " on image"
            })
        } else if (webHitTestResult.isLink()) {
            action.update({
                text: text + " on link"
            })
        } else if (webHitTestResult.isContentEditable()) {
            action.update({
                text: text + " on input"
            })
        }
        
        action.triggered.connect(function() {
            Falkon.Notifications.create({
                heading: 'Hello QML',
                message: 'First qml plugin action works :-)',
                icon: ':/icons/preferences/extensions.svg'
            })
        })
    }
    
    Falkon.Settings {
        id: settings
        name: 'HelloQML'
    }
    
    settingsWindow: Window {
        id: window
        width: 256
        height: 200
        Image {
            id: image
            source: 'qrc:/icons/other/about.svg'
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
        }
        TextField {
            id: textField
            text: settings.value({key: 'text'})
            placeholderText: 'Enter text to save'
            width: 256
            height: 50
            anchors.top: image.bottom
            onTextChanged: function() {
                button.text = 'Save'
            }
        }
        Button {
            id: button
            text: 'Save'
            width: 256
            height: 50
            anchors.top: textField.bottom
            onClicked: function() {
                var res = settings.setValue({
                    key: 'text',
                    value: textField.text
                })
                if (res) {
                    button.text = 'Saved!'
                } else {
                    button.text = 'Error occurred, try again!'
                }
            }
        }
    }
}
