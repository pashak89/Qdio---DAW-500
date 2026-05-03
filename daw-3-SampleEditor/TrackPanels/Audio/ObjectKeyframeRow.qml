import QtQuick 2.12
import QtQuick.Layouts 1.12
import "."
import "qrc:/"
import "qrc:/Items"

Item {
    id: rowRoot

    property int ind: 0

    property real posX: 0
    property real posY: 0
    property real posZ: 0

    property var keyframeMap: ({})
    property var keyframeTimes: []

    function refreshKeyframes() {
        keyframeMap = _clipArea.tracksModel.keyFrames(ind)
        keyframeTimes = Object.keys(keyframeMap).map(function (k) { return Number(k) })
    }

    Component.onCompleted: {
        if (typeof scene3D !== 'undefined' && scene3D) {
            var p = scene3D.entityPositionMap(ind)
            if (p) { posX = p.x; posY = p.y; posZ = p.z }
        }
        refreshKeyframes()
    }

    Connections {
        target: scene3D
        ignoreUnknownSignals: true
        function onEntityPositionChanged(idx, x, y, z) {
            if (idx === rowRoot.ind) { rowRoot.posX = x; rowRoot.posY = y; rowRoot.posZ = z }
        }
    }

    Connections {
        target: _clipArea.tracksModel
        ignoreUnknownSignals: true
        function onKeyFramesChanged(idx) {
            if (idx === rowRoot.ind) rowRoot.refreshKeyframes()
        }
    }

    // Strong separator line at the BOTTOM — sits at the intersection between
    // this track and the next, matching the existing inter-track Seperator.
    Rectangle {
        id: capLine
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: clipArea.seperatorHeight
        color: '#2f3032'
        z: 5
    }

    // Left segment: "Position  X.XX, Y.YY, Z.ZZ" + add button
    Item {
        id: leftSegment
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: capLine.top
        width: _width

        Text {
            id: lblName
            anchors.left: parent.left
            anchors.leftMargin: scaleSize2(20)
            anchors.verticalCenter: parent.verticalCenter
            text: "Position"
            color: theme.a2
            renderType: Text.QtRendering
            font.kerning: false
            font {
                pixelSize: topfontPixelSize
                family: topFontFamily
                letterSpacing: 0.5
            }
        }

        Text {
            id: lblValue
            anchors.left: lblName.right
            anchors.leftMargin: scaleSize2(15)
            anchors.right: addBtn.left
            anchors.rightMargin: scaleSize2(10)
            anchors.verticalCenter: parent.verticalCenter
            text: rowRoot.posX.toFixed(2) + ", " + rowRoot.posY.toFixed(2) + ", " + rowRoot.posZ.toFixed(2)
            color: theme.a1
            elide: Text.ElideRight
            renderType: Text.QtRendering
            font.kerning: false
            font {
                pixelSize: topfontPixelSize
                family: topFontFamily
                letterSpacing: 0.5
            }
        }

        Rectangle {
            id: addBtn
            width: scaleSize2(28)
            height: scaleSize2(28)
            radius: scaleSize2(6)
            color: addMa.containsMouse ? theme.b11_Enable1_BRD : "transparent"
            border.color: theme.b10_Disable_BRD
            border.width: topWidth
            anchors.right: parent.right
            anchors.rightMargin: scaleSize2(10)
            anchors.verticalCenter: parent.verticalCenter

            Text {
                anchors.centerIn: parent
                text: "+"
                color: theme.a1
                renderType: Text.QtRendering
                font.kerning: false
                font {
                    pixelSize: topfontPixelSize
                    family: topFontFamily
                    letterSpacing: 0.5
                }
            }

            MouseArea {
                id: addMa
                anchors.fill: parent
                hoverEnabled: true
                onClicked: _clipArea.tracksModel.addKeyFrame(rowRoot.ind, _areaInfo.playheadMarker, 0)
            }
        }
    }

    // Right segment: timeline strip with diamond keyframe markers
    Item {
        id: stripSegment
        anchors.left: leftSegment.right
        anchors.leftMargin: scaleSize2(10)
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: capLine.top
        clip: true

        Repeater {
            model: rowRoot.keyframeTimes

            Rectangle {
                property real kfTime: modelData
                width: scaleSize2(14)
                height: scaleSize2(14)
                radius: scaleSize2(2)
                rotation: 45
                color: theme.a1
                border.color: theme.a2
                border.width: topWidth
                x: _areaInfo.time2Pixel(kfTime) - width / 2
                anchors.verticalCenter: parent.verticalCenter

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        if (mouse.button === Qt.RightButton) {
                            _clipArea.tracksModel.removeKeyFrame(rowRoot.ind, kfTime)
                        } else {
                            _areaInfo.sigCurrentSelectedTime(kfTime)
                        }
                    }
                }
            }
        }
    }
}
