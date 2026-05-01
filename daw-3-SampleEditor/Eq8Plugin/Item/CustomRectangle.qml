import QtQuick 2.12

Rectangle {
    id: root
    implicitWidth: 50
    implicitHeight: 50

    radius: 2

    property int borderWidth: 0
    property color borderColor: 'brown'

    border.width: borderWidth
    border.color: borderColor

    //property int corners: Qt.AlignLeft | Qt.AlignRight | Qt.AlignTop | Qt.AlignBottom /* Default: */
    property variant corners: [Qt.AlignLeft | Qt.AlignTop, Qt.AlignLeft
        | Qt.AlignBottom, Qt.AlignRight | Qt.AlignTop, Qt.AlignRight | Qt.AlignBottom]

    Repeater {
        model: [{
                "x": 0,
                "y": 0,
                "visible": internal.aligns(Qt.AlignLeft | Qt.AlignTop),
                "radius": root.radius
            }, {
                "x": root.width - root.radius,
                "y": 0,
                "visible": internal.aligns(Qt.AlignRight | Qt.AlignTop),
                "radius": root.radius
            }, {
                "x": 0,
                "y": root.height - root.radius,
                "visible": internal.aligns(Qt.AlignLeft | Qt.AlignBottom),
                "radius": root.radius
            }, {
                "x": root.width - root.radius,
                "y": root.height - root.radius,
                "visible": internal.aligns(Qt.AlignRight | Qt.AlignBottom),
                "radius": root.radius
            }]

        Rectangle {
            x: modelData.x
            y: modelData.y
            width: modelData.radius
            height: width
            visible: !modelData.visible
            color: parent.color
            border.width: borderWidth
            border.color: borderColor

            Rectangle {
                x: index % 2 === 0 ? borderWidth : -borderWidth
                y: index > 1 ? -borderWidth : borderWidth
                width: modelData.radius
                height: width
                visible: borderWidth > 0
                color: parent.color
            }
        }
    }

    QtObject {
        id: internal

        function aligns(direction) {
            if (Array.isArray(root.corners)) {
                for (var i = 0; i < root.corners.length; i++) {
                    if ((root.corners[i] & direction) === direction)
                        return true
                }

                return false
            } else {
                return (root.corners & direction) === direction
            }
        }
    }
}
