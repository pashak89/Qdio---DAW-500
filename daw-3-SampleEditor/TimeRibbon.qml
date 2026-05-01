import QtQuick 2.15
import QtQuick.Shapes 1.15

Canvas {
    width: 10
    height: 10

    property real hFactor: 0.6
    property real angle: 0 // degrees
    property color triColor: "#15D0E9"

    onPaint: {
        var ctx = getContext("2d")
        ctx.reset()

        ctx.save()

        // move origin to center
        ctx.translate(width / 2, height / 2)
        // rotate
        ctx.rotate(angle * Math.PI / 180)
        // move origin back
        ctx.translate(-width / 2, -height / 2)

        ctx.fillStyle = triColor

        var topY = height * (1 - hFactor)

        ctx.beginPath()
        ctx.moveTo(width / 2, topY)
        ctx.lineTo(width, height)
        ctx.lineTo(0, height)
        ctx.closePath()
        ctx.fill()

        ctx.restore()
    }

    onHFactorChanged: requestPaint()
    onAngleChanged: requestPaint()
}
