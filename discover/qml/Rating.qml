/*
 *   Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 1.1
import org.kde.plasma.core 0.1
import org.kde.plasma.components 0.1

Item {
    id: view
    property bool editable: false
    property int max: 10
    property real rating: 2
    visible: rating>=0
    clip: true
    height: (width/5)-theRow.spacing
    width: 20*5
    Row {
        id: theRow
        spacing: 1
        height: parent.height
        anchors.right: parent.right
        
        Component {
            id: del
            IconItem {
                height: view.height; width: view.height
                source: "rating"
                opacity: (view.max/theRepeater.count*index)>view.rating ? 0.2 : 1

                MouseArea {
                    enabled: editable
                    
                    anchors.fill: parent
                    onClicked: rating = (max/5*index)
                }
            }
        }
        
        Repeater {
            id: theRepeater
            model: 5
            delegate: del
        }
    }
}