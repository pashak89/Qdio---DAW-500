import QtQuick 2.12
import QtGraphicalEffects 1.2

Rectangle {
    id: root

    property string text: ""

    property real fontSize: topfontPixelSize

    property real topPaddingTxt: 0
    property real bottomPaddingTxt: 0
    property real leftPaddingTxt: 0
    property real rightPaddingTxt: 0
    property alias contentW: info.contentWidth
    property alias contentH: info.contentHeight

    property bool right2Left: false

    // NEW: alignment parameters
    property int horizontalTextAlignment: Text.AlignHCenter // AlignLeft, AlignHCenter, AlignRight, AlignJustify
    property int verticalTextAlignment: Text.AlignVCenter // AlignTop, AlignVCenter, AlignBottom

    property string fontFamilty: topFontFamily
    property color fontColor: theme.f1_

    // (Optional but useful for Columns / Layouts)
    implicitWidth: info.contentWidth + leftPaddingTxt + rightPaddingTxt
    implicitHeight: info.contentHeight + topPaddingTxt + bottomPaddingTxt

    color: "transparent"
    antialiasing: true

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        antialiasing: true
        radius: root.radius
        Text {
            id: info
            renderType: Text.QtRendering
            text: root.text
            color: fontColor

            antialiasing: true

            // Fill parent and use margins/paddings
            anchors.fill: parent
            anchors.leftMargin: leftPaddingTxt
            anchors.rightMargin: rightPaddingTxt
            anchors.topMargin: topPaddingTxt
            anchors.bottomMargin: bottomPaddingTxt

            // Use alignment properties
            horizontalAlignment: root.horizontalTextAlignment
            verticalAlignment: root.verticalTextAlignment

            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            font {
                pixelSize: fontSize
                family: fontFamilty
                letterSpacing: 0.5
                weight: Font.DemiBold
            }
        }
    }
}
