import QtQuick 1.1
import org.kde.plasma.components 0.1
import org.kde.qtextracomponents 0.1
import org.kde.muon 1.0

Item {
    property alias application: transactions.application
    width: 100
    height: 30
    
    TransactionListener {
        id: transactions
        backend: app.appBackend
    }
    
    state: transactions.isRunning ? "working" : "idle"
    
    Button {
        id: button
        state: application.isInstalled ? "willremove" : "willinstall"
        visible: parent.state=="idle"
        anchors.fill: parent
        
        onClicked: {
            if(state=="willinstall") app.appBackend.installApplication(application)
            else if(state=="willremove") app.appBackend.removeApplication(application)
        }
        
        states: [
            State {
                name: "willinstall"
                PropertyChanges { target: button;  text: i18n("Install") }
            },
            State {
                name: "willremove"
                PropertyChanges { target: button;  text: i18n("Remove") }
            }
        ]
    }
    
    Row {
        visible: parent.state=="working"
        spacing: 10
        
        Label {
            anchors.verticalCenter: parent.verticalCenter
            text: transactions.comment
        }
        ProgressBar {
            id: progress
            anchors.verticalCenter: parent.verticalCenter
            minimumValue: 0
            maximumValue: 100
            value: transactions.progress
        }
        
        Button {
            anchors.verticalCenter: parent.verticalCenter
            iconSource: "dialog-cancel"
        }
    }
    
    states: [
        State { name: "idle" },
        State { name: "working" }
    ]
}