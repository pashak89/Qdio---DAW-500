import QtQuick 2.15
import QtGraphicalEffects 1.15

Rectangle {
    id: root
    anchors.fill: parent

    property string name: 'info'
    property color colorTxt: theme.tColor

    property int fontSize: info.fontInfo.pixelSize

    property real topPaddingTxt: 0
    property real bottomPaddingTxt: 0
    property real leftPaddingTxt: 0
    property real rightPaddingTxt: 0
    property alias contentW: info.contentWidth
    property alias contentH: info.contentHeight

    property real thisPadding: 0

    color: 'transparent'

    //    GaussianBlur {
    //        anchors.fill: info
    //        source: info
    //        radius: 0.5
    //        samples: Math.round(radius * 2 + 1)
    //        cached: true
    //    }
    Text {
        id: info
        renderType: Text.QtRendering
        text: root.name
        anchors.fill: parent

        color: colorTxt

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        leftPadding: leftPaddingTxt

        fontSizeMode: Text.VerticalFit //Text.Fit

        font.kerning: false

        elide: Text.ElideRight

        minimumPixelSize: 5

        font {
            pixelSize: topfontPixelSize
            family: topFontFamily
            letterSpacing: 0.5
        }
    }
}
