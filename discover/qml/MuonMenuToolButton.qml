import QtQuick 1.1
import org.kde.plasma.components 0.1

MuonToolButton {
    id: button
    property alias delegate: menuRepeater.delegate
    property alias model: menuRepeater.model
    
    checkable: true
    checked: false
    
    Item {
        id: menuItem
        width: 100
        height: buttons.height
        anchors.right: parent.right
        anchors.top: parent.bottom
        visible: button.checked
        Rectangle {
            anchors.fill: parent
            radius: 10
            opacity: 0.4
        }
        
        Column {
            id: buttons
            width: parent.width
            
            Repeater {
                id: menuRepeater
            }
        }
    }
}
