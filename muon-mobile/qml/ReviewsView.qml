import QtQuick 1.1
import QtDesktop 0.1
import org.kde.muon 1.0

Item
{
    property alias application: reviewsModel.application
    ListView {
        id: reviewsView
        clip: true
        anchors {
            fill: parent
            rightMargin: scroll.width+anchors.margins
            margins: 5
        }
        visible: reviewsView.count>0
        spacing: 5
        
        header: Label { text: i18n("<b>Reviews:</b>") }
        
        delegate: ListItem {
            anchors {
                left: parent.left
                right: parent.right
            }
            visible: model["shouldShow"]
            height: content.height+10
            
            function usefulnessToString(favorable, total)
            {
                return total==0
                        ? i18n("<em>Tell us about this review!</em>")
                        : i18n("<em>%1 out of %2 people found this review useful</em>", favorable, total)
            }
            
            Label {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                
                id: content
                text: i18n("<b>%1</b> by %2<p/>%3<br/>%4", summary, reviewer,
                           display, usefulnessToString(usefulnessFavorable, usefulnessTotal))
                wrapMode: Text.WordWrap
            }
            
            Label {
                visible: app.appBackend.reviewsBackend().hasCredentials
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    bottomMargin: -5
                }
                opacity: delegateArea.containsMouse ? 1 : 0.2
                
                text: i18n("<em>Useful? <a href='true'>Yes</a>/<a href='false'>No</a></em>")
                onLinkActivated: reviewsModel.markUseful(index, link)
            }
            
            Rating {
                anchors.top: parent.top
                anchors.right: parent.right
                rating: model["rating"]
                height: content.font.pixelSize
            }
            MouseArea {
                id: delegateArea
                anchors.fill: parent
                hoverEnabled: true
            }
        }
        
        model: ReviewsModel {
            id: reviewsModel
            backend: app.appBackend.reviewsBackend()
        }
    }
    
    ScrollBar {
        id: scroll
        orientation: Qt.Vertical
//         flickableItem: reviewsView
        anchors {
            top: reviewsView.top
            right: parent.right
            bottom: reviewsView.bottom
        }
    }
}