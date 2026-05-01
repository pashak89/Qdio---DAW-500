import QtQuick 2.12
import FilterVisualizer 1.0
import QtQuick 2.15
import "./Item"

Rectangle {
    id: root
    width: parent.width
    height: scaleSize(300)

    color: 'black'

    radius: scaleSize(10)

    property alias afilterVisualizer: filterVisualizer

    FilterVisualizer {
        id: filterVisualizer
        anchors.fill: parent

        areaInfo: _areaInfo
        multieq: _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                     "EQ").plugin
    }

    Component.onCompleted: {
        filterVisualizer.size = Qt.size(root.width, root.height)
        repater.model = filterVisualizer.bandModel
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.radius
        color: parent.color
    }

    //    Rectangle {
    //        y: 55
    //        height: topWidth * 2
    //        width: parent.width
    //        color: trackEnable ? theme.filterLine : theme.d10_DeactiveDial_BRD
    //    }
    Repeater {
        id: repater
        anchors.fill: parent
        z: myCanvas.z + 1

        delegate: dragComponent
    }
    onWidthChanged: {
        myCanvas.requestPaint()
        filterVisualizer.size = Qt.size(root.width, root.height)
    }
    onHeightChanged: {
        myCanvas.requestPaint()
        filterVisualizer.size = Qt.size(root.width, root.height)
    }

    Canvas {
        id: myCanvas
        z: 0
        antialiasing: true
        anchors.fill: parent
        renderTarget: Canvas.FramebufferObject

        onPaint: {

            var ctx = getContext("2d")

            ctx.clearRect(0, 0, width, height)

            var ratio = myCanvas.devicePixelRatio
            ctx.scale(ratio, ratio) // Retrieve the points from the backend
            var points = filterVisualizer.magnitude

            if (points && points.length > 0) {
                ctx.beginPath()

                // Start at the first point
                ctx.moveTo(points[0].x, points[0].y)

                // Draw lines to the rest of the points
                for (var i = 1; i < points.length; i++) {
                    ctx.lineTo(points[i].x, points[i].y)
                }

                // Apply stroke style and draw
                ctx.strokeStyle
                        = filterVisualizer.enabled ? theme.filterLine : theme.d10_DeactiveDial_BRD
                ctx.lineWidth = topWidth * 2
                ctx.stroke()
            }
        }

        // Redraw when the points change
        Component.onCompleted: {
            filterVisualizer.sigCircleMoved.connect(requestPaint)
            filterVisualizer.enabledChanged.connect(requestPaint)
        }
    }

    Component {
        id: dragComponent
        Rectangle {
            id: control
            width: text.contentH
            height: width
            z: 2
            onWidthChanged: {
                filterVisualizer.circleSize = text.contentH
            }
            onHeightChanged: {
                filterVisualizer.circleSize = text.contentH
            }

            visible: model.Enabled
            enabled: model.Enabled

            property bool isChecked: filterVisualizer.isBandEnabled(index)

            x: model.PosX - control.width / 2
            y: model.PosY - control.height / 2
            radius: width / 2
            Drag.active: mouseArea.drag.active

            border {
                width: topWidth * 2
                color: trackEnable ? theme.b11_Enable1_FIL : theme.b20_Disable_FIL
            }
            color: tabIndex === index & trackEnable ? theme.b11_Enable1_FIL : tabIndex === index ? theme.b20_Disable_FIL : 'transparent' //theme.b10_Disable_FIL

            InfoText {
                id: text
                text: index + 1
                color: {
                    if (tabIndex === index & trackEnable)
                        theme.f4_
                    else if (tabIndex !== index & trackEnable)
                        theme.b11_Enable1_FIL
                    else if (tabIndex === index & !trackEnable)
                        theme.f4_
                    else
                        theme.b20_Disable_FIL //theme.f1_
                }
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                drag.target: parent
                drag.minimumX: 0

                onPressed: {
                    control.isChecked = !control.isChecked
                    tabIndex = index
                    filterVisualizer.EQ4_activeBand = index
                    cursorPositionClass.hideCursor()

                    filterVisualizer.saveUndoOperation()
                }
                onReleased: {
                    var globalPosition = repater.mapToGlobal(
                                model.PosX + control.width / 2,
                                model.PosY + control.height / 2)
                    cursorPositionClass.saveCursorPos(globalPosition.x,
                                                      globalPosition.y)

                    cursorPositionClass.showCursor()
                }

                onPositionChanged: {

                    filterVisualizer.mouseMoveEvent(
                                control.x + control.width / 2,
                                control.y + control.height / 2)
                }
            }
        }
    }
}
