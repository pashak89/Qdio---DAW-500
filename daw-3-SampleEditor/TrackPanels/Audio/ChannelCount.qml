import QtQuick 2.12
import QtQuick.Controls 2.15
import "."
import "qrc:/"
import "qrc:/Items"

Rectangle {
    id: rootBtn

    property double step: 0.01
    property string text: "2"
    property real fontSize: topfontPixelSize
    property string fontFamilty: topFontFamily
    property bool pressed: false
    property real min: 1
    property real max: 64
    property color brColor: theme.s11_Enable_BRD
    property color flColor: theme.s10_Disable_FIL
    property bool bigHeight: false
    property bool didMarker: false
    property bool didShadow: false
    property bool isSlider: false
    property double y_pos: 0
    property color fontColor: theme.tColor
    property int scaleY: 1
    property real value: 2
    property real svalue: 2
    property double range: Math.abs(max - min) / step
    property int forSpeed: 0
    property real defaultValue: 2
    property int startX: 0
    property int startY: 0
    property int lastX: 0
    property int lastY: 0
    property double diff: 0

    function bound(minValue, myValue, maxValue) {
        if (myValue > maxValue)
            return maxValue
        else if (myValue < minValue)
            return minValue
        else
            return myValue
    }

    signal sigValueChanged(real value)
    function valueChange() {

        sigValueChanged(value)
        rootTI.text = Math.floor(value).toString()
    }

    height: scaleSize2(37)
    width: scaleSize2(70)

    color: 'transparent'

    TextInput {
        id: rootTI

        anchors.fill: parent

        color: 'transparent'

        onHeightChanged: {
            scaleY = Math.round(range / rootTI.height / (70 + forSpeed))
            y_pos = Math.round(scaleY * defaultValue)
            rootTI.text = Math.floor(value).toString()
        }

        readOnly: true
        validator: IntValidator {
            bottom: min
            top: max
        }

        renderType: Text.QtRendering
        cursorVisible: false

        cursorDelegate: Rectangle {
            id: marker
            visible: didMarker && didShadow && isSlider === false
            anchors.verticalCenter: parent.verticalCenter
            width: 2.4
            height: borderRec.height
            radius: 1
            color: 'salmon'

            SequentialAnimation on color {
                loops: Animation.Infinite
                ColorAnimation {
                    from: '#EA2027'
                    to: "#ecf0f1"
                    duration: 350
                }
                ColorAnimation {
                    from: "#ecf0f1"
                    to: '#EA2027'
                    duration: 350
                }
            }
        }

        Rectangle {
            id: borderRec
            z: -1
            anchors {
                fill: parent
                topMargin: bigHeight ? -1 : 0
                bottomMargin: bigHeight ? -1 : 0
            }

            color: flColor
            border.color: brColor
            border.width: topWidth
            radius: topRadius
        }

        InfoText2 {
            id: myTxtValue

            name: rootTI.text

            anchors.fill: parent
            bottomPaddingTxt: 0

            fontSize: rootBtn.fontSize
            colorTxt: fontColor
        }

        MouseArea {
            id: ma

            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            onPressed: {

                /// undo
                rootBtn.pressed = true
                if (trackRightClick.visible)
                    trackRightClick.visible = false
                if (rightPanel.visible)
                    rightPanel.visible = false

                var globalPosition = mapToGlobal(mouse.x, mouse.y)
                startX = globalPosition.x
                startY = globalPosition.y
                svalue = value

                y_pos = value * scaleY

                rootTI.focus = true
                ma.focus = true

                _clipArea.setOverrideCursor(Qt.BlankCursor)
            }

            onReleased: {

                rootBtn.pressed = false
                ma.focus = true

                _clipArea.setOverrideCursor(Qt.ArrowCursor)
                _clipArea.setCursorPosition(startX, startY)
            }

            onFocusChanged: {

                if (rootTI.focus || ma.focus)
                    didShadow = true
                else {

                    if (trackRightClick.visible)
                        trackRightClick.visible = false
                    rightPanel.visible = false

                    didShadow = false
                    didMarker = false
                }
            }

            onPositionChanged: {

                var currentPos = mapToGlobal(mouse.x, mouse.y)

                diff = startY - currentPos.y

                y_pos -= diff / (holdShift ? 10 : 1)
                y_pos = bound(min * scaleY, y_pos, max * scaleY)

                value = y_pos / scaleY

                valueChange()

                _clipArea.setCursorPosition(startX, startY)
            }
        }
    }
}
