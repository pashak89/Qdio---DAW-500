import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Controls 2.15
import "Items"

TreeView {
    id: delegateTreeView
    width: listView.width
    selectionEnabled: true
    hoverEnabled: true
    highlight: Rectangle {
        color: "transparent"
    }

    hoverColor: theme.bg6

    selectedItemColor: theme.s7
    handleColor: theme.s7

    onChildSelectedIndexChanged: {
        rowDoubleClicked(childSelectedIndex)
    }

    property var cursorPosition: cursorPositionClass

    property string topFontFamily: myFont2.name
    property int topfontPixelSize: scaleSize2(28)
    property real topRadius: radiusCal(3)
    property int topWidth: resolution[0] >= 3840 & resolution[1]
                           >= 2170 ? ((refSize < .83) ? 1 : (refSize < 1.12) ? 2 : 2) : ((refSize < .83) ? 1 : (refSize < 1.12) ? 1 : 2)

    property var resolution: cursorPosition.getResolution()

    property real refSize: _areaInfo.zoomFactor
    property real _scale: cursorPosition.getScale()

    // === math: invert your DB(Y) curve ===
    // DB = -42.3529 * Y^2 - 33.6471 * Y + 6
    // given real dB (negative) -> Y in [0..1]
    function decibel2yNorm(db) {
        var a = -42.3529
        var b = -33.6471
        var c = 6 - db

        var discriminant = b * b - 4 * a * c
        if (discriminant < 0)
            return NaN

        var sqrtD = Math.sqrt(discriminant)

        // Y >= 0 root
        return (-b - sqrtD) / (2 * a)
    }

    // Convert *display* dB (0,3,6,10,…) to pixel Y
    // top = 0, bottom = height
    function dbDisplayToYPixel(dbDisplay, h) {
        var realDb = -dbDisplay
        // 0 -> 0 dB, 60 -> -60 dB
        var yNorm = decibel2yNorm(realDb)
        return yNorm * h
    }

    Theme1 {
        id: theme1
    }
    Theme2 {
        id: theme2
    }
    property Item theme: _areaInfo.themeType === 0 ? theme1 : theme2

    FontLoader {
        id: myFont
        source: "qrc:/Resource/font/Open_Sans/OpenSans-VariableFont.ttf"
    }
    FontLoader {
        id: myFont2
        source: "qrc:/Resource/font/SourceSansPro-Regular.ttf"
    }

    function radiusCal(value) {
        value = value * _scale * refSize
        return Math.round(value)
    }
    function scaleSize2(value) {
        value = Math.round(value * _scale * refSize / 2)
        return value
    }
    function oddSize(value) {
        value = Math.round(value)
        if (value % 2 !== 0)
            return value
        else
            return value - 1
    }
    function evenSize(value) {
        value = Math.round(value)
        if (value % 2 === 0)
            return value
        else
            return value - 1
    }

    contentItem: Row {
        spacing: 10

        Text {
            verticalAlignment: Text.AlignVCenter

            color: currentRow.isHoveredIndex ? theme.f10_ : theme.f1_
            text: currentRow.currentData

            font {
                pixelSize: topfontPixelSize
                family: topFontFamily
                letterSpacing: 0.5
                weight: Font.DemiBold
            }
        }
    }

    scrollItem: ScrollBar {
        id: control
        orientation: Qt.Vertical
        active: true

        // normalized values (0..1)
        size: 0.3
        position: 0.2

        contentItem: Rectangle {
            implicitWidth: scaleSize2(15)

            implicitHeight: Math.max(scaleSize2(20),
                                     control.size * control.availableHeight)

            radius: scaleSize2(5)
            color: theme.bg7

            // correct vertical position
            y: control.position * (control.availableHeight - height)
        }
    }
}
