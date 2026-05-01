import QtQuick 2.15
import QtGraphicalEffects 1.2

Item {
    id: root
    anchors.fill: parent

    width: 73
    height: 23

    property string text: 'info'
    property color color: theme.tColor

    property real fontSize: topfontPixelSize

    property real topPadding: 0
    property real bottomPadding: 0
    property real leftPadding: 0
    property real rightPadding: 0
    property alias contentW: info.contentWidth
    property alias contentH: info.contentHeight

    property bool right2Left: false
    property bool onTheLeft: false

    GaussianBlur {
        anchors.fill: info
        source: info
        radius: 0.5
        samples: Math.round(radius * 2 + 1)
        cached: true
    }
    Text {
        id: info
        renderType: Text.QtRendering
        text: root.text
        anchors.fill: parent

        color: root.color

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: right2Left ? Text.AlignRight : onTheLeft ? Text.AlignRleft : Text.AlignHCenter

        // fontSizeMode:       Text.Fit
        font.kerning: false

        bottomPadding: root.bottomPadding
        topPadding: root.topPadding
        leftPadding: root.leftPadding
        rightPadding: root.rightPadding

        minimumPixelSize: 5

        font {
            pixelSize: fontSize
            family: topFontFamily
            // weight:         Font.DemiBold//refSize > 0.74 ? Font.DemiBold : Font.Normal
            letterSpacing: 0.5
        }
    }
}
