import QtQuick 1.0
import org.kde.plasma.components 0.1 as Plasma
import org.kde.muon 1.0
import QtDesktop 0.1

Plasma.Page {
    id: page
    
    tools: Row {
        anchors.fill: parent
        visible: page.visible
        ToolButton {
            iconSource: "image://desktoptheme/list-add"
            text: i18n("Add Source")
            onClicked: newSourceDialog.visible=true
            anchors.verticalCenter: parent.verticalCenter
        }
        
        Repeater {
            model: ["software_properties"]
            
            delegate: MuonToolButton {
                property QtObject action: app.getAction(modelData)
                height: parent.height
                text: action.text
                onClicked: action.trigger()
                enabled: action.enabled
                icon: action.icon
            }
        }
    }
    
    Dialog {
        id: newSourceDialog
        title: i18n("Specify the new source")
        height: 300
        width: 500
        
        Item {
            anchors.fill: parent
            Column {
                id: info
                spacing: 5
                anchors.fill: parent
                
                Label {
                    anchors {
                        left: parent.left
                        right: parent.right
                        margins: 10
                    }
                    wrapMode: Text.WordWrap
                    text: i18n("<sourceline> - The apt repository source line to add. This is one of:\n"
                                +"  a complete apt line, \n"
                                +"  a repo url and areas (areas defaults to 'main')\n"
                                +"  a PPA shortcut.\n\n"

                                +"  Examples:\n"
                                +"    deb http://myserver/path/to/repo stable myrepo\n"
                                +"    http://myserver/path/to/repo myrepo\n"
                                +"    https://packages.medibuntu.org free non-free\n"
                                +"    http://extras.ubuntu.com/ubuntu\n"
                                +"    ppa:user/repository") }
                TextField {
                    id: repository
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Keys.onEnterPressed: newSourceDialog.accept()
                }
            }
        }
        
        onAccepted: origins.addRepository(repository.text)
    }
    OriginsBackend { id: origins }
    
    ScrollBar {
        id: scroll
        orientation: Qt.Vertical
//         flickableItem: view
        anchors {
                top: view.top
                right: parent.right
                bottom: view.bottom
        }
    }
    ListView {
        id: view
        anchors {
            fill: parent
            rightMargin: scroll.width+3
            leftMargin: 3
        }
        clip: true
        section.property: "uri"
        section.delegate: Label {
            text: section
            horizontalAlignment: Text.AlignRight
            width: parent.width
            font.bold: true
        }
        
        model: origins.sources
        
        delegate: ListItem {
            height: removeButton.height
            Label {
                anchors {
                    left: removeButton.right
                    verticalCenter: removeButton.verticalCenter
                    
                    leftMargin: removeButton.width+5
                }
                text: modelData.isSource ? i18n("%1 (Source)", modelData.suite) : modelData.suite
            }
            ToolButton {
                id: removeButton
                anchors.left: parent.left
                iconSource: "image://desktoptheme/list-remove"
                onClicked: origins.removeRepository(modelData.uri)
            }
        }
    }
}
