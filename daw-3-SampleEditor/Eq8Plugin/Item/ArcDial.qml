import QtQuick 2.15
import QtQuick.Window 2.15

Item {
    id: root

    // Rectangle {anchors.fill: parent;color: 'green';opacity: 0.3;z: 3}
    implicitWidth: 55
    implicitHeight: 55

    signal moved
    signal pressed

    property bool isActive: false
    property bool isAutomated: false
    property real value: 0
    property real angleValue: 280 / (max - min) * (value - min) - 230
    property bool variance: false

    property real defaultValue: 0

    property bool isZero: defaultValue === 0
    property bool fromZero: angleValue < -90
    property bool form: isZero ? fromZero : false

    property int lineWidth: scaleSize(6)

    property real min: -1
    property real max: 1

    property bool disable: false
    property bool checked: true

    property double y_pos: 0
    property int diff: 0

    property double sense: 20

    property real saveXPos: 0
    property real saveYPos: 0

    property double scaleY: 1

    property double lastPos
    property double pressPos
    property var itemPos
    property var glb_pos
    property bool didStart: false

    property color imgColor: disable ? theme.d10_DeactiveDial_FIL : theme.d11_ActiveDial_BRD
    property color valDialColor: disable ? theme.d10_DeactiveDial_FIL : checked
                                           & trackEnable ? theme.d11_ActiveDial_FIL : theme.d10_DeactiveDial_FIL
    property color rootDialColor: disable ? theme.d10_DeactiveDial_BRD : checked ? theme.d11_ActiveDial_BRD : theme.d12_ActiveDial_BRD

    property real firstY: 0
    property bool isEdge: false

    function bound(minValue, myValue, maxValue) {
        if (myValue > maxValue)
            return maxValue
        else if (myValue < minValue)
            return minValue
        else
            return myValue
    }

    Component.onCompleted: {
        scaleY = 200 / (max - min)
        y_pos = Math.round(scaleY * defaultValue)
        //        if (variance) {
        //            var v = Math.log(defaultValue / min) / Math.log(
        //                        max / min) * (max - min) + min
        //            value = v
        //        } else
        //            value = defaultValue
    }

    onWidthChanged: {
        indicator.requestPaint()
        canvas2.requestPaint()
        canvasValue.requestPaint()
    }
    onHeightChanged: {
        indicator.requestPaint()
        canvas2.requestPaint()
        canvasValue.requestPaint()
    }

    Canvas {
        id: indicator
        visible: isZero
        anchors.horizontalCenter: parent.horizontalCenter

        width: scaleSize(22)
        height: scaleSize(14)
        contextType: "2d"

        property color lineColor: imgColor
        onLineColorChanged: requestPaint()

        onPaint: {
            context.reset()
            context.moveTo(0, 0)
            context.lineTo(width, 0)
            context.lineTo(width, height / 4)
            context.lineTo(width / 2, height)
            context.lineTo(0, height / 4)
            context.closePath()
            context.fillStyle = lineColor
            context.fill()
        }
    }

    Item {
        id: dial

        Rectangle {
            id: automatedLight
            z: 1000
            visible: isActive
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 1
            anchors.topMargin: 0
            color: isAutomated ? "#EA2027" : "#808080"
            width: scaleSize2(11)
            height: scaleSize2(11)
            radius: scaleSize2(11)
        }
        anchors {
            top: indicator.bottom
            topMargin: 0
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: scaleSize(17)
        }

        Canvas {
            id: canvas2
            anchors.fill: parent

            property color lineColor: rootDialColor

            onLineColorChanged: requestPaint()

            onPaint: {

                var ctx = getContext("2d")
                ctx.reset()

                var centreX = width / 2
                var centreY = width / 2

                ctx.strokeStyle = lineColor
                ctx.lineWidth = lineWidth
                ctx.beginPath()
                ctx.arc(centreX, centreY, width / 2 - lineWidth,
                        -Math.PI / 180 * 230, Math.PI / 180 * 50, false)
                ctx.lineCap = "round"
                ctx.stroke()
            }
        }

        Canvas {
            id: canvasValue

            anchors.fill: parent

            property real endAngle: Math.PI / 180 * angleValue
            antialiasing: true

            onEndAngleChanged: {
                requestPaint()
            }

            property color lineColor: valDialColor
            onLineColorChanged: requestPaint()

            onPaint: {
                var ctx = getContext("2d")
                ctx.reset()

                var centreX = width / 2
                var centreY = width / 2

                ctx.strokeStyle = lineColor
                ctx.lineWidth = lineWidth //+ 0.5
                ctx.beginPath()
                ctx.arc(centreX, centreY, width / 2 - lineWidth,
                        -Math.PI / 180 * (isZero ? 90 : 230), endAngle, form)
                ctx.lineCap = isZero ? "butt" : "round"
                ctx.stroke()
            }
        }

        Rectangle {
            id: handleItem
            x: parent.width / 2 - width / 2
            y: parent.width / 2 - height / 2
            width: lineWidth
            height: parent.width / 2 - refSize
            color: rootDialColor
            radius: lineWidth / 2
            smooth: true

            Rectangle {
                id: leftRec
                visible: angleValue === -90 ? isZero : form
                anchors.right: parent.left
                anchors.top: parent.top
                anchors.topMargin: -1
                height: lineWidth * 3
                width: lineWidth
                color: theme.bg2
            }
            Rectangle {
                id: rightRec
                visible: angleValue === -90 ? true : !form
                anchors.left: parent.right
                anchors.top: parent.top
                height: lineWidth * 3
                width: lineWidth
                color: theme.bg2
            }

            transform: [
                Translate {
                    y: -handleItem.height / 2
                },
                Rotation {
                    angle: angleValue + 90
                    origin.x: handleItem.width / 2
                    origin.y: handleItem.height / 2
                }
            ]
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Shift) {

            var maxLastPos = root.height * scaleY
            scaleY *= Math.round(sense)
            var maxPos = root.height * scaleY
            y_pos = y_pos * maxPos / maxLastPos
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Shift) {

            var maxLastPos = root.height * scaleY
            scaleY /= Math.round(sense)
            var maxPos = root.height * scaleY
            y_pos = y_pos * maxPos / maxLastPos
        }
    }

    MouseArea {
        id: ma
        anchors.fill: parent

        onDoubleClicked: {
            if (variance) {
                var v = Math.log(defaultValue / min) / Math.log(
                            max / min) * (max - min) + min
                value = v
            } else
                value = defaultValue
            y_pos = value * scaleY

            moved()
        }
        onPressed: {
            var globalPos = mapToGlobal(mouse.x, mouse.y)
            firstY = globalPos.y

            itemPos = mapToGlobal(mouse.x, mouse.y)
            lastPos = itemPos.y

            y_pos = value * scaleY

            ma.focus = true
            root.focus = true

            cursorPositionClass.hideCursor()
            cursorPositionClass.saveCursorPos(globalPos.x, globalPos.y)

            didStart = true

            root.pressed()
        }
        onReleased: {
            didStart = false

            ma.focus = true
            cursorPositionClass.showCursor()
        }

        onMouseYChanged: {
            if (didStart) {
                itemPos = mapToGlobal(mouse.x, mouse.y)

                if (isEdge) {
                    isEdge = false
                    lastPos = itemPos.y
                }

                diff = lastPos - itemPos.y

                y_pos += diff
                y_pos = bound(min * scaleY, y_pos, max * scaleY)

                value = y_pos / scaleY

                moved()

                lastPos = itemPos.y

                glb_pos = mapToGlobal(mouse.x, mouse.y)
                var vv = 50
                if ((glb_pos.y <= vv) && ((value) < max)
                        || (glb_pos.y >= (Screen.height - vv))
                        && ((value) > min)) {
                    cursorPositionClass.moveCursor((Screen.width - vv) / 2,
                                                   ((Screen.height - vv) / 2))
                    isEdge = true
                } // reset mouse position while drag.
            } // if did start.
        } // mouse in Y changed.
    } // mouse area
}
