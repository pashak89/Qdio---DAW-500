pragma Singleton

import QtQuick 2.0
import Tools 1.0

QtObject {

    property var areaInfo: _areaInfo
    property int _width: scaleSize2(600)
    property int minWidth: scaleSize2(600)
    property string topFontFamily: myFont2.name
    property int topfontPixelSize: scaleSize2(32)
    property real topRadius: radiusCal(3)
    property int topWidth: resolution[0] >= 3840 & resolution[1]
    >= 2170 ? ((refSize < .83) ? 1 : (refSize < 1.12) ? 2 : 2) : ((refSize < .83) ? 1 : (refSize < 1.12) ? 1 : 2)

    function decibel2yNorm(db) {
        var a = -42.3529
        var b = -33.6471
        var c = 6 - db
        var d = b * b - 4 * a * c
        if (d < 0)
            return NaN
        return (-b - Math.sqrt(d)) / (2 * a)
    }

    function dbDisplayToYPixel(dbDisplay, h) {
        return decibel2yNorm(-dbDisplay) * h
    }

    property var cursorPosition
    property var resolution: cursorPosition.getResolution()

    property real refSize: _areaInfo.zoomFactor
    property real scale: cursorPosition.scale
    property Theme1 theme1: Theme1 {}

    property Theme2 theme2: Theme2 {}
    property Item theme: theme1

    property FontLoader myFont: FontLoader {
        source: 'qrc:/Resource/font/Open_Sans/OpenSans-VariableFont.ttf'
    }

    property FontLoader myFont2: FontLoader {
        source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf'
    }

    function radiusCal(value) {
        value = value * scale * refSize
        return Math.round(value)
    }
    function scaleSize2(value) {
        value = Math.round(value * scale * refSize / 2)
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
}
