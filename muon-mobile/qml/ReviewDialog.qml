import QtQuick 1.1
import QtDesktop 0.1

Dialog {
    property QtObject application
    property alias rating: ratingInput.rating
    property alias summary: summaryInput.text
    property alias review: reviewInput.text
    id: reviewDialog
    height: 400
    width: 400
    modal: true
    
    title: i18n("Reviewing %1", application.name)
    
    Item {
        anchors.fill: parent
        Column {
            id: info
            spacing: 5
            anchors.left: parent.left
            anchors.right: parent.right
            Label { text: i18n("Rating:") }
            Rating {
                id: ratingInput
                editable: true
            }
            
            Label { text: i18n("Summary:") }
            TextField {
                id: summaryInput
                anchors.left: parent.left
                anchors.right: parent.right
            }
            Label { text: i18n("Description:") }
        }
        
        TextArea {
            id: reviewInput
            width: parent.width
            anchors.top: info.bottom
            anchors.bottom: parent.bottom
            anchors.margins: 5
        }
    }
}