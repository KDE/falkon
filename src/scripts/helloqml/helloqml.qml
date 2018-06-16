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
    
    browserAction: Falkon.BrowserAction {
        name: 'helloqml-button'
        identity: 'helloqml-id'
        title: 'Testing QML Title'
        toolTip: 'Testing QML Tooltip'
        icon: Qt.resolvedUrl('qrc:/icons/preferences/extensions.svg')
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
                    source: Qt.resolvedUrl('qrc:/icons/other/startpage.svg')
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
    
    sideBar: Falkon.SideBar {
        name: 'helloqml-sidebar'
        title: 'Testing QML SideBar'
        icon: Qt.resolvedUrl('qrc:/icons/preferences/extensions.svg')
        checkable: true
        Window {
            Image {
                source: Qt.resolvedUrl('qrc:/icons/other/startpage.svg')
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
}