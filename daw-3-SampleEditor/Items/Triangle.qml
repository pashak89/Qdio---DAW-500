import QtQuick 2.12

Canvas {
    id: canvas

    width: 12
    height: 12
    contextType: "2d"

//    antialiasing: true

    property color fillStyle: 'red'

    onFillStyleChanged: requestPaint()

    onPaint: {
        context.save();
        context.reset();
        context.moveTo(0, 0);
        context.lineTo(width, 0);
        context.lineTo(width / 2, height);
        context.closePath();
        context.fillStyle = fillStyle
        context.fill();
    }
}

