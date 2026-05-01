import QtQuick 2.12
import QtGraphicalEffects 1.2

Rectangle {
    id: root
    anchors.fill: parent

    //        anchors.topMargin:              Math.round(contentH/8)
    width: 73
    height: 23

    property string name: 'info'
    property color colorTxt: theme.tColor

    property real fontSize: topfontPixelSize
    property string fontFamily: topFontFamily

    property real topPaddingTxt: 0
    property real bottomPaddingTxt: 0
    property real leftPaddingTxt: 0
    property real rightPaddingTxt: 0
    property alias contentW: info.contentWidth
    property alias contentH: info.contentHeight

    property real thisPadding: 0

    property bool right2Left: false

    color: 'transparent'

    //    GaussianBlur {
    //        anchors.fill:   info
    //        source:         info
    //        radius:         0.5
    //        samples:        Math.round(radius * 2+1)
    //        cached:         true
    //    }
    Text {
        id: info
        renderType: Text.QtRendering
        text: root.name
        anchors.fill: parent

        color: colorTxt

        //        width: parent.width
        //        height: parent.height

        //        onHeightChanged: console.log(contentHeight)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: right2Left ? Text.AlignRight : Text.AlignHCenter

        //fontSizeMode:       Text.VerticalFit
        font.kerning: false

        //        padding:            thisPadding

        //        bottomPadding:      bottomPaddingTxt
        //        topPadding: contentHeight % 2 === 0 ? -1 : 0
        minimumPixelSize: 5

        font {
            pixelSize: fontSize
            family: fontFamily
            //            weight:         refSize > 0.74 ? Font.DemiBold : Font.Normal
            letterSpacing: 0.5
        }
    }
}
