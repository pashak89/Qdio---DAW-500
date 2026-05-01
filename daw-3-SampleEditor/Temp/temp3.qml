import QtQuick 2.12
import QtQuick.Window 2.14
import QtQuick.Layouts 1.12
//import QtQuick.Extras 1.2
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.2
import Qt.labs.animation 1.0
import 'qrc:/Automation/'
import Tools 1.0


Window {
    id: win
    width: 250
    height: 600
    visible: true
    color: 'grey'

    Text {
        anchors.right: parent.right
        // text: control.value
    }

    CursorPosition { id: cursor }

    ListModel {
        id: myModel
        ListElement {
            title: 'n1'
            value: 1
            _color: 'teal'
        }
        ListElement {
            title: 'n2'
            value: 2
            _color: 'brown'
        }
        ListElement {
            title: 'n3'
            value: 3
            _color: 'purple'
        }

        ListElement {
            title: 'n3'
            value: 3
            _color: 'gold'
        }
    }



    ColumnLayout {
        id: col
        anchors.right: parent.right; anchors.rightMargin: 50
        width: 100
        height: children.height
        y: 50

        spacing: 0

        Repeater {
            model: myModel
            delegate: comp
        }
    }


    Component {
        id: comp
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: rec.y + rec.height
        color: _color

        Label { anchors.centerIn: parent; text: parent.height }

        Rectangle {
            id: rec
            y: 35 - height
            width: parent.width
            height: 5
            color: 'darkgrey'

            MouseArea {
                id: dragArea
                anchors.fill: parent
                drag.axis: Drag.YAxis
                drag.minimumY: 30// - height
                drag.target: parent

                hoverEnabled: true

                onEntered: cursor.vCursorShape()
                onExited: if(!pressed) cursor.onlyShowCursor()
                onPressed: cursor.vCursorShape()
                onReleased: cursor.onlyShowCursor()

                onMouseYChanged: if(pressed) parent.y = Math.round(parent.y / 15) * 15
            }}}
    }



    Button {
        anchors {
            bottom: parent.bottom
        }

        text: 'btn 1'

        onClicked: myModel.move(0, 1, 1)
    }

    Button {
        anchors {
            bottom: parent.bottom
            // horizontalCenter: parent.horizontalCenter
            right: parent.right
        }

        text: 'btn 2'

        onClicked: myModel.move(1, 2, 1)
    }

}




