import QtQuick 2.12
import QtGraphicalEffects 1.2

Rectangle {
    id: root
    anchors.fill: parent

    //    anchors.topMargin:              Math.round(contentH/8)
    property string name: ''
    property string text: ''
    property color colorTxt: theme.tColor

    property real fontSize: topfontPixelSize

    property real topPaddingTxt: 0
    property real bottomPaddingTxt: 0
    property real leftPaddingTxt: 0
    property real rightPaddingTxt: 0
    property alias contentW: info.contentWidth
    property alias contentH: info.contentHeight

    property bool right2Left: false

    color: 'transparent'
    antialiasing: true

    Rectangle {
        anchors.fill: parent
        color: 'transparent'
        antialiasing: true

        //        GaussianBlur {
        //            anchors.fill:   info
        //            source:         info
        //            radius:         0.5
        //            samples:        Math.round(radius * 2+1)
        //            cached:         true
        //        }
        Text {
            id: info
            renderType: Text.QtRendering
            text: root.name == "" ? root.text : root.name
            color: colorTxt
            height: parent.height

            antialiasing: true

            anchors.left: right2Left ? undefined : parent.left
            anchors.leftMargin: leftPaddingTxt

            anchors.right: right2Left ? parent.right : undefined
            anchors.rightMargin: rightPaddingTxt

            font {
                pixelSize: fontSize
                family: topFontFamily
                //                weight:         refSize > 0.74 ? Font.DemiBold : Font.Normal
                letterSpacing: 0.5
            }
        }
    }
}
