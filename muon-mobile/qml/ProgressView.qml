import QtQuick 1.1
import QtDesktop 0.1
import org.kde.qtextracomponents 0.1
import org.kde.muon 1.0

Item {
    visible: height>0
    id: page
    property QtObject backend: app.appBackend
    property bool active: transactionsModel.count>0
    height: active ? contents.height+2*contents.anchors.margins : 0
    
    Behavior on height {
        NumberAnimation { duration: 250; easing.type: Easing.InOutQuad }
    }
    
    Connections {
        id: backendConnections
        target: backend
        onApplicationTransactionAdded: transactionsModel.append({'app': app})
    }
    
    ListModel {
        id: transactionsModel
    }
    
    ListView {
        id: contents
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 3
        }
        
        spacing: 3
        height: 30
        orientation: ListView.Horizontal
        
        model: transactionsModel
        
        delegate: ListItem {
            width: launcherRow.childrenRect.width+5
            height: contents.height
            TransactionListener {
                id: listener
                application: model.app
                backend: page.backend
                onCancelled: model.remove(index)
            }
            
            Row {
                id: launcherRow
                spacing: 2
                QIconItem { icon: model.app.icon; height: parent.height; width: height }
                Label { text: model.app.name }
                Label { text: listener.comment; visible: listener.isActive }
                ToolButton {
                    iconSource: "dialog-cancel"
                    visible: listener.isDownloading
                    onClicked: app.appBackend.cancelTransaction(application)
                }
                ToolButton {
                    iconSource: "system-run"
                    visible: model.app.isInstalled && !listener.isActive && model.app.canExecute
                    onClicked: {
                        model.app.invokeApplication()
                        model.remove(index)
                    }
                }
            }
        }
    }
    
    ToolButton {
        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 5
        }
        iconSource: "image://desktoptheme/dialog-close"
        onClicked: transactionsModel.clear()
    }
}
