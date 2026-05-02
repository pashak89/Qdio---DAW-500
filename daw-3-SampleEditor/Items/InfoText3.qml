import QtQuick 2.12

TextInput {
    id: rootTI

    renderType: Text.QtRendering
    cursorVisible: false
    activeFocusOnPress: !fixText
    maximumLength: max.toString().length + decimal + 3
    validator: DoubleValidator {
        bottom: min
        top: max
        decimals: decimal
    }
    color: "transparent"

    property double step: 0.01
    property double range: Math.abs(maxValue - minValue) / step

    function percentPerStep(step) {
        var range = maxValue - minValue
        return (step / range) * 100
    }

    property bool fixText: false

    property bool pressed: false
    property int index: 1

    property double y_pos: scaleY * defaultValue
    property double diff: 0

    property real scaleY: range / 10000.0
    property real noramlScaleCoeff: 1
    property real shiftScaleCoeff: 10

    property real minValue: 0
    property real maxValue: 100

    property real min: 0
    property real max: 100

    property real value: 50

    property real defaultValue: 0.00
    property int decimal: 2

    property real fontSize: topfontPixelSize
    property string fontFamily: topFontFamily
    property color fontColor: "#15D0E9"

    property color brColor: "transparent"
    property color flColor: "transparent"

    property bool holdShift: false
    property bool externalValue: false

    signal sigValueChanged(real value, real diff)
    function valueChange(diff) {

        var v = applyTransform(
                    ((rootTI.value - min) / (max - min) * (maxValue - minValue) + minValue))

        text = (v).toFixed(decimal)
        sigValueChanged(value, diff)
    }

    // Map from [minValue, maxValue] → [min, max]
    function mapValue(value) {

        return min + ((value - minValue) / (maxValue - minValue)) * (max - min)
    }

    // Reverse map from [min, max] → [minValue, maxValue]
    function mapReverse(value) {
        return (value - min) * (maxValue - minValue) / (max - min) + minValue
    }

    FontMetrics {
        id: fm
        font.family: rootTI.fontFamily
        font.pixelSize: rootTI.fontSize
        font.bold: true
    }

    Text {
        id: myTxtValue

        text: rootTI.text
        color: fontColor

        renderType: Text.NativeRendering

        font.pixelSize: fontSize
        font.family: fontFamily
        font.bold: true

        // Center text inside the Rectangle, but don't force its size
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        anchors.leftMargin: 1

        // Align according to direction
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        Rectangle {
            id: borderRec
            z: -1
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            height: fm.height
            width: myTxtValue.width - 0.2 * myTxtValue.width
            color: ma.pressed ? "#4D4D4D" : (ma.containsMouse ? "#333333" : "transparent")
        }
    }

    property var valueTransform: function (v) {
        return v
    }

    function applyTransform(vRaw) {
        var vFinal = valueTransform ? valueTransform(vRaw) : vRaw
        return vFinal
    }

    function bound(minValue, myValue, maxValue) {
        if (myValue > maxValue)
            return maxValue
        else if (myValue < minValue)
            return minValue
        else
            return myValue
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onEntered: _clipArea.setOverrideCursor(Qt.ArrowCursor)
    }
    Keys.onPressed: {
        if (event.key === Qt.Key_Shift) {
            holdShift = true
        }
    }

    Keys.onReleased: {
        holdShift = false
    }

    MouseArea {
        id: ma
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        hoverEnabled: true
        enabled: !fixText

        onDoubleClicked: {
            value = defaultValue
            y_pos = value * scaleY
            valueChange(0)
        }

        onClicked: {

            rootTI.focus = true
            ma.focus = true
        }

        onPressed: {
            rootTI.pressed = true

            var globalPosition = mapToGlobal(mouse.x, mouse.y)
            var startX = globalPosition.x
            var startY = globalPosition.y

            y_pos = value * scaleY

            rootTI.focus = true
            ma.focus = true

            _clipArea.setOverrideCursor(Qt.BlankCursor)

            // store for later use via dynamic properties on ma
            ma.__startX = startX
            ma.__startY = startY
        }

        onReleased: {
            rootTI.pressed = false
            ma.focus = true

            _clipArea.setOverrideCursor(Qt.ArrowCursor)
            _clipArea.setCursorPosition(ma.__startX, ma.__startY)
        }

        onPositionChanged: {

            if (!rootTI.pressed)
                return

            var currentPos = mapToGlobal(mouse.x, mouse.y)

            diff = index * (ma.__startY - currentPos.y)

            y_pos += diff / (holdShift ? shiftScaleCoeff : (noramlScaleCoeff))
            y_pos = bound(min * scaleY, y_pos, max * scaleY)

            if (externalValue) {

                sigValueChanged(0, diff)
            } else {

                value = y_pos / (scaleY)
                valueChange(diff)
            }

            _clipArea.setCursorPosition(ma.__startX, ma.__startY,
                                        percentPerStep(10))
        }

        // internal storage (so we don't need root-level props for these)
        property int __startX: 0
        property int __startY: 0
    }
}
