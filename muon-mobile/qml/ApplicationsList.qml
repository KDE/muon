import QtQuick 1.1
import QtDesktop 0.1
import org.kde.qtextracomponents 0.1
import "navigation.js" as Navigation

Item {
    id: parentItem
    property Item stack
    property int elemHeight: 65
    property alias count: view.count
    property alias header: view.header
    property alias section: view.section
    property bool preferUpgrade: false
    property alias model: view.model
    
    ListView
    {
        id: view
        anchors {
            fill: parent
            rightMargin: 10+scroll.width
            leftMargin: 10+scroll.width
        }
        spacing: 3
        
        delegate: ListItem {
                width: view.width
                property real contHeight: elemHeight*0.7
                height: elemHeight
                QIconItem {
                    id: icon
                    icon: model["icon"]; width: contHeight; height: contHeight
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                }
                
                QIconItem {
                    anchors.right: icon.right
                    anchors.bottom: icon.bottom
                    visible: installed
                    icon: "dialog-ok"
                    height: 16
                    width: 16
                }
                Label {
                    anchors.top: icon.top
                    anchors.left: icon.right
                    anchors.right: ratingsItem.left
                    anchors.leftMargin: 5
                    font.pointSize: commentLabel.font.pointSize*1.7
                    elide: Text.ElideRight
                    text: name
                }
                Label {
                    id: commentLabel
                    anchors.bottom: icon.bottom
                    anchors.left: icon.right
                    anchors.leftMargin: 5
                    text: "<em>"+comment+"</em>"
                    opacity: delegateArea.containsMouse ? 1 : 0.2
                }
                Rating {
                    id: ratingsItem
                    anchors {
                        right: parent.right
                        top: parent.top
                    }
                    height: contHeight*.5
                    rating: model.rating
                }
                
                MouseArea {
                    id: delegateArea
                    anchors.fill: parent
                    onClicked: Navigation.openApplication(application)
                    hoverEnabled: true
                    
                    InstallApplicationButton {
                        id: installButton
                        width: ratingsItem.width
                        height: contHeight*0.5
                        anchors {
                            bottom: parent.bottom
                            margins: 5
                        }
                        
                        property bool isVisible: delegateArea.containsMouse && !installButton.canHide
                        x: ratingsItem.x
                        opacity: isVisible ? 1 : 0
                        application: model.application
                        preferUpgrade: parentItem.preferUpgrade
                        
                        Behavior on opacity {
                            NumberAnimation {
                                duration: 100
                                easing.type: Easing.InQuad
                            }
                        }
                    }
                }
            }
    }
    
    ScrollBar {
        id: scroll
        orientation: Qt.Vertical
//         flickableItem: view
        anchors {
                top: parent.top
                right: parent.right
                bottom: parent.bottom
        }
    }
}