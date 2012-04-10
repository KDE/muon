import QtQuick 1.1
import QtDesktop 0.1
import org.kde.muon 1.0

ListView
{
    id: addonsView
    property alias application: addonsModel.application
    property alias addonsHaveChanged: addonsModel.hasChanges
    property bool isInstalling: false
    property alias isEmpty: addonsModel.isEmpty
    
    ApplicationAddonsModel { id: addonsModel }
    
    model: addonsModel
    
    clip: true
    delegate: Row {
        height: 50
        width: 50
        spacing: 10
        CheckBox {
            enabled: !isInstalling
            anchors.verticalCenter: parent.verticalCenter
            checked: model.checked
            onClicked: addonsModel.changeState(display, checked)
        }
        Image {
            source: "image://icon/applications-other"
            height: parent.height; width: height
            smooth: true
            opacity: isInstalling ? 0.3 : 1
        }
        Label {
            enabled: !isInstalling
            anchors.verticalCenter: parent.verticalCenter
            text: i18n("<qt>%1<br/><em>%2</em></qt>", display, toolTip)
        }
    }
    
    ScrollBar {
        id: scroll
        orientation: Qt.Vertical
//         flickableItem: parent
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
    }
    
    header: addonsModel.hasChanges && !isInstalling ? buttonsRow : null
    
    Component {
        id: buttonsRow
        Row {
            layoutDirection: Qt.RightToLeft
            anchors {
                top: parent.top
                right: parent.right
            }
            spacing: 5
            
            Button {
                iconSource: "image://desktoptheme/dialog-ok"
                text: i18n("Apply")
                onClicked: addonsModel.applyChanges()
            }
            Button {
                iconSource: "image://desktoptheme/document-revert"
                text: i18n("Discard")
                onClicked: addonsModel.discardChanges()
            }
        }
    }
}