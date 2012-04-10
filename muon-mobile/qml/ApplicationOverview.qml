import QtQuick 1.1
import QtDesktop 0.1
import org.kde.qtextracomponents 0.1
import org.kde.muon 1.0

Item {
    id: appInfo
    property QtObject application
    
    ScrollBar {
        id: scroll
        orientation: Qt.Vertical
//         flickableItem: overviewContentsFlickable
        anchors {
            top: overviewContentsFlickable.top
            bottom: overviewContentsFlickable.bottom
            right: parent.right
        }
    }
    Flickable {
        clip: true
        id: overviewContentsFlickable
        width: 2*parent.width/3
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
            margins: 10
        }
        contentHeight: overviewContents.childrenRect.height
        Column {
            id: overviewContents
            width: parent.width
            spacing: 10
            
            property QtObject ratingInstance: appInfo.application ? app.appBackend.reviewsBackend().ratingForApplication(appInfo.application) : null
            Rating {
                anchors.horizontalCenter: parent.horizontalCenter
                rating: overviewContents.ratingInstance.rating()
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n("%1 reviews", overviewContents.ratingInstance.ratingCount())
            }
            
            
            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n("Homepage")
                iconSource: "image://desktoptheme/go-home"
                enabled: application.homepage
                onClicked: app.openUrl(application.homepage)
            }
            
            Button {
                visible: application.isInstalled
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n("Launch")
                enabled: application.canExecute
                onClicked: application.invokeApplication()
            }
            
            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: application.isInstalled && app.appBackend.reviewsBackend().hasCredentials
                text: i18n("Review")
                onClicked: reviewDialog.visible=true
            }
            
            Label {
                text: i18n(  "<b>Total Size:</b> %1<br/>"
                            +"<b>Version:</b> %2 %3<br/>"
                            +"<b>License:</b> %4<br/>",
                            application.sizeDescription,
                            application.name, (application.isInstalled ?
                                                        application.installedVersion : application.availableVersion),
                            application.license
                )
            }
            
            Label {
                id: info
                anchors {
                    left: parent.left
                    right: parent.right
                    margins: 5
                }
                horizontalAlignment: Text.AlignJustify
                wrapMode: Text.WordWrap
                text: application.longDescription
            }
        }
    }
    ReviewDialog {
        id: reviewDialog
        application: appInfo.application
        onAccepted: app.appBackend.reviewsBackend().submitReview(appInfo.application, summary, review, rating)
    }
    
    Item {
        id: shadow
        state: "thumbnail"
        
        Rectangle {
            id: shadowItem
            anchors.fill: parent
            color: "black"
            Behavior on opacity { NumberAnimation { duration: 1000 } }
        }
        
        Image {
            id: screenshot
            anchors.centerIn: parent
            height: sourceSize ? Math.min(parent.height-5, sourceSize.height) : parent.height
            width: sourceSize ? Math.min(parent.width-5, sourceSize.width) : parent.width
            
            asynchronous: true
            fillMode: Image.PreserveAspectFit
            
            onStatusChanged: if(status==Image.Error) {
                sourceSize.width = sourceSize.height = 200
                source="image://icon/image-missing"
            }
        }
        
        states: [
            State { name: "thumbnail"
                PropertyChanges { target: shadowItem; opacity: 0.1 }
                PropertyChanges { target: shadow; width: overviewContentsFlickable.x-x-5 }
                PropertyChanges { target: shadow; height: parent.height }
                PropertyChanges { target: shadow; x: 0 }
                PropertyChanges { target: shadow; y: 5 }
                PropertyChanges { target: screenshot; source: application.screenshotUrl(0) }
            },
            State { name: "full"
                PropertyChanges { target: shadowItem; opacity: 0.7 }
                PropertyChanges { target: shadow; x: 0 }
                PropertyChanges { target: shadow; y: 0 }
                PropertyChanges { target: shadow; height: appInfo.height }
                PropertyChanges { target: shadow; width: appInfo.width }
                PropertyChanges { target: shadow; z: 0 }
                PropertyChanges { target: screenshot; source: application.screenshotUrl(1) }
            }
        ]
        Behavior on y { NumberAnimation { easing.type: Easing.OutQuad; duration: 500 } }
        Behavior on width { NumberAnimation { easing.type: Easing.OutQuad; duration: 500 } }
        Behavior on height { NumberAnimation { easing.type: Easing.OutQuad; duration: 500 } }
        
        MouseArea {
            anchors.fill: parent
            onClicked: { shadow.state = shadow.state == "thumbnail" ? "full" : "thumbnail" }
        }
    }
}