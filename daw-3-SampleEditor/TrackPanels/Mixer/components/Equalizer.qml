import QtQuick 2.15
import FilterVisualizer 1.0

import Global 1.0
import "../delegates"
import "../../../Items"

Rectangle {
    id: root
    property alias afilterVisualizer: filterVisualizer
    property int trackIndex: _areaInfo.selectedTrackItem

    property var effectChain: _clipArea.tracksModel.effectChain(trackIndex)

    width: parent.width
    height: Global.scaleSize2(300)

    color: 'black'

    radius: Global.scaleSize2(10)
    FilterVisualizer {
        id: filterVisualizer
        anchors.fill: parent

        areaInfo: _areaInfo
        multieq: _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                     "EQ").plugin
    }

    Component.onCompleted: {
        filterVisualizer.size = Qt.size(root.width, root.height)
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
}
