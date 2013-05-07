import QtQuick 1.1
import org.kde.plasma.core 0.1
import org.kde.plasma.components 0.1

Row {
    property bool editable: false
    property int max: 10
    property real rating: 2
    visible: rating>=0
    
    id: view
    height: 25
    
    Component {
        id: del
        IconItem {
            height: view.height; width: view.height
            source: "rating"
            opacity: (max/5*index)>rating ? 0.2 : 1

            MouseArea {
                enabled: editable
                
                anchors.fill: parent
                onClicked: rating = (max/5*index)
            }
        }
    }
    
    spacing: 2
    Repeater {
        model: 5
        delegate: del
    }
}
