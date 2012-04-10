import QtQuick 1.1
import org.kde.plasma.components 0.1
import org.kde.qtextracomponents 0.1
import org.kde.muon 1.0
import QtDesktop 0.1

Page
{
    id: page
    property QtObject application
    
    Item {
        id: intro
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }
        height: icon.height
        QIconItem {
            id: icon
            anchors.top: parent.top
            anchors.left: parent.left
            width: 40
            height: 40
            
            icon: application.icon
        }
        
        Column {
            id: header
            anchors.top: parent.top
            anchors.left: icon.right
            anchors.right: parent.right
            anchors.leftMargin: 5
            spacing: 5
            
            Text {
                text: application.name
                width: parent.width
                font.bold: true
            }
            Label {
                text: application.comment
                wrapMode: Text.WordWrap
                width: parent.width
            }
        }
        
        InstallApplicationButton {
            id: installButton
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            application: page.application
        }
    }
    
    TabFrame {
        id: tabs
        anchors {
            top: intro.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 10
        }
        Tab {
            title: i18n("Overview")
            ApplicationOverview {
                id: applicationOverview
                application: page.application
                anchors.fill: parent
            }
        }
        Tab {
            title: i18n("Add-ons")
            visible: !addonsView.isEmpty //TODO: probably should be enabled: instead
            AddonsView {
                id:addonsView
                application: page.application
                isInstalling: installButton.isInstalling
                anchors.fill: parent
            }
        }
        Tab {
            title: i18n("Reviews")
            ReviewsView {
                id: reviewsView
                application: page.application
                anchors.fill: parent
            }
        }
    }
}