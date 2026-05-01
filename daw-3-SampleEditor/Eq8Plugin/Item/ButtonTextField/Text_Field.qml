import QtQuick 2.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15
import ".."

TextInput {
    id: rootTI

    width: 120
    height: 55

    signal moved(real value)
    signal pressed();

    property bool is_ValueChanged: false
    property real previousValue: 0
    property bool firstValueClick: true

    // semi slider
    property real y_in_s: 0
    property bool isSlider: false

    property bool maFocus: ma.focus

    FontLoader {
        id: thisFont
        source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf'
    }

    property real xPosRightClick: 0
    property real yPosRightClick: 0

    property double y_pos: 0
    property double diff: 0

    property double step: 0.01
    property double sense: 20

    property double range: (max - min) / step
    property int scaleY: 1

    property double lastPos
    property double pressPos
    property var itemPos
    property var glb_pos

    property int outputRightClick

    property real enterValue: 0

    property real min: -10
    property real max: 200

    property real value: 0
    property real defaultValue: 0.00
    property int decimal: 2
    property real infValue: -70.00
    property string unit: ''

    property color fontColor: theme.tColor

    property real firstY: 0

    property bool isClear: true
    property bool isInf: true
    property bool fakeSliderValue: false
    property bool isEdge: false
    property bool hasSlider: false
    property bool didMarker: false
    property bool isTimeSignature: false
    property bool didStart: false

    property int forSpeed: 0

    property var shortcutKeyTxt: ['Ctrl+M']
    property var moduls: ['EDIT MIDI Map']
    property bool enableRightClick: true
    property var rightComponent
    property var object_rightPanel

    maximumLength: max.toString().length + decimal + 2
    validator: DoubleValidator {
        bottom: min
        top: max
        decimals: decimal
    }

    renderType: Text.QtRendering
    cursorVisible: false

    function timeSignature(number) {
        var result

        if (number <= 1) {
            result = 1
        } else if (number > 1 && number <= 2) {
            result = 2
        } else if ((number > 2) && (number <= 4)) {
            result = 4
        } else if ((number > 4) && (number <= 8)) {
            result = 8
        } else if ((number > 8) && (number <= 16)) {
            result = 16
        } else if (number > 16) {
            result = 16
        }
        return result
    } // timeSignature func.

    cursorDelegate: Rectangle {
        id: marker
        visible: didMarker && isSlider === false
        anchors.verticalCenter: parent.verticalCenter
        width: 2.4
        height: myTxtValue.contentH
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
        anchors.fill: parent
        // anchors.margins:    -1
        color: flColor
        // border.color:       isSlider ? 'transparent' : brColor
        // border.width:       topWidth
        radius: topRadius

        Rectangle {
            width: topWidth
            height: parent.height
            color: flColor
        }
    }

    text: value.toFixed(decimal)
    font.pixelSize: 15
    font.weight: Font.DemiBold
    font.family: thisFont.name
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    rightPadding: (unit === '' || unit === ' '
                   || isInf === false) ? 0 : unitText.contentW
    leftPadding: value <= infValue ? 500 : 0
    color: 'transparent'

    // onLeftPaddingChanged: {
    //     if(leftPadding > 200) {
    //         myTxtValue.visible = false
    //         myTxtValue.enabled = false
    //     }
    //     else {
    //         myTxtValue.visible = true
    //         myTxtValue.enabled = true
    //     }
    // }
    InfoText {
        id: myTxtValue
        text: rootTI.text
        // anchors.topMargin:  1
        anchors.fill: parent
        // alignV: Text.AlignTop
        // leftPaddingTxt: Math.round(parent.width/2 - contentW/2) -
        // (didMarker ? 0 : unitText.contentW/2)
        // fontSize: 15//rootTI.fontSize
        color: fontColor
    }

    Component.onCompleted: {
        scaleY = Math.round(range / rootTI.height / (70 + forSpeed))
        y_pos = Math.round(scaleY * defaultValue)
        value = y_pos / scaleY
    }

    function bound(minValue, myValue, maxValue) {
        if (myValue > maxValue)
            return maxValue
        else if (myValue < minValue)
            return minValue
        else
            return myValue
    }

    function roundToTenth(num) {
        return Number(num.toFixed(1))
    }

    InfoText {
        id: unitText
        visible: isInf && unit !== ''
        text: value <= infValue ? '-inf  dB' : unit

        anchors.fill: parent
        // leftPaddingTxt: value <= infValue && isGain ? Math.round(parent.width/2 - contentW/2) :
        // Math.round(parent.width/2 - contentW/2) + rootTI.contentWidth/2
        anchors.topMargin: 1
        // fontSize: rootTI.fontSize
        color: fontColor
    }

    // onActiveFocusChanged: { if(rootTI.focus) didShadow = true }
    Keys.onReleased: {
        if (event.key === Qt.Key_Shift) {

            var maxLastPos = rootTI.height * scaleY
            scaleY /= Math.round(sense)
            var maxPos = rootTI.height * scaleY
            y_pos = y_pos * maxPos / maxLastPos
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Shift) {

            var maxLastPos = rootTI.height * scaleY
            scaleY *= Math.round(sense)
            var maxPos = rootTI.height * scaleY
            y_pos = y_pos * maxPos / maxLastPos
        }

        //--- when typing :
        //-----------------
        if ((min < 0 && event.key === 45) || (event.key === Qt.Key_0) || (event.key === Qt.Key_1) || (event.key === Qt.Key_2) || (event.key === Qt.Key_3) || (event.key === Qt.Key_4) || (event.key === Qt.Key_5) || (event.key === Qt.Key_6) || (event.key === Qt.Key_7) || (event.key === Qt.Key_8) || (event.key === Qt.Key_9)) {

            if (isClear) {
                enterValue = value

                isInf = false
                clear()
                isClear = false
                didMarker = true
                // didShadow = true
            }
        } // type numbers

        if ((event.key === 16777220) || (event.key === Qt.Key_Enter)) {
            didMarker = false

            if (isTimeSignature) {
                value = timeSignature(text * 1)
                text = value
            } // if it is not gain =>
            else {
                if (text >= min && text <= max) {
                    value = text
                    text = value.toFixed(decimal)
                } else if (text > max) {
                    value = max.toFixed(decimal)
                    text = max.toFixed(decimal)
                    isInf = true ///////////////////////////////////////////////////
                } else if (text < min) {
                    rootTI.leftPadding = 0
                    text = min.toFixed(decimal)
                    value = min.toFixed(decimal)
                }
            }
        } // press Enter key.
    }

    onValueChanged: {
        if (value > infValue) {
            isInf = true
            rootTI.leftPadding = 0
        } else if (value <= infValue)
            rootTI.leftPadding = 500
        text = value.toFixed(decimal)
    }

    //---------- Below Slider:
    Rectangle {
        id: rect
        enabled: rootTI.hasSlider
        visible: rootTI.hasSlider

        anchors.fill: parent
        // anchors.leftMargin:     1//mainRoot_id.refSize
        anchors.rightMargin: 1 //mainRoot_id.refSize
        anchors.bottomMargin: topWidth //-1
        radius: topRadius
        color: 'transparent'

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: rect.width
                height: rect.height
                radius: rect.radius
                Rectangle {
                    width: parent.radius
                    height: parent.height
                }
            }
        }
        Rectangle {
            id: sliderLine_a
            width: (value - min) / (max - min) * parent.width

            height: Math.round(rect.height / 10)
            color: control.checked & trackEnable ? theme.s21_on : theme.s20_off
            anchors {
                left: parent.left
                bottom: parent.bottom
            }
        } // sliderLine_a
    } // rect of slider.
    //---------- Below Slider.

    // ---------------- Right click setting:
    MouseArea {
        id: ma0
        visible: enableRightClick
        enabled: enableRightClick
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        onClicked: {

            rootTI.focus = true
            ma0.focus = true
            rightComponent = Qt.createComponent("RightPanel.qml")
            object_rightPanel = rightComponent.createObject(rootTI)

            var posRightClick = mapToItem(rootTI, mouse.x, mouse.y)
            xPosRightClick = posRightClick.x
            yPosRightClick = posRightClick.y
            object_rightPanel.x = xPosRightClick
            object_rightPanel.y = yPosRightClick
            object_rightPanel.outputRightClick = rootTI.outputRightClick
            object_rightPanel.shortcutKeyTxt = rootTI.shortcutKeyTxt
            object_rightPanel.moduls = rootTI.moduls
            enableRightClick = !enableRightClick
        }

        onFocusChanged: {
            if (!rootTI.focus || !ma0.focus) //didShadow = true
            {
                if (object_rightPanel !== undefined) {
                    object_rightPanel.destroy()
                    object_rightPanel = undefined
                    enableRightClick = true
                }
                // didShadow =     false
                didMarker = false
            }
        }
    }

    // ---------------- Right click setting.
    MouseArea {
        id: ma

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        onDoubleClicked: {
            value = defaultValue.toFixed(decimal)
            rootTI.text = defaultValue.toFixed(decimal)
            y_pos = value * scaleY

            is_ValueChanged = true
        }

        onPressed: {
            /// undo

            rootTI.pressed()

            previousValue = text * 1

            if (object_rightPanel !== undefined) {
                object_rightPanel.destroy()
                object_rightPanel = undefined
                enableRightClick = true
            }

            var globalPosition = mapToGlobal(mouse.x, mouse.y)
            firstY = globalPosition.y

            itemPos = mapToItem(rootTI, mouse.x, mouse.y)
            lastPos = itemPos.y

            value = rootTI.text.split(' ')[0] * 1
            y_pos = value * scaleY

            rootTI.focus = true
            ma.focus = true

            cursorPositionClass.saveCursorPos(globalPosition.x,
                                              globalPosition.y)
            cursorPositionClass.hideCursor()

            // cursorShape =           Qt.BlankCursor
            didStart = true

            //slider:
            y_in_s = mouse.y
        }

        onReleased: {
            didStart = false

            ma.focus = true
            isClear = true
            // cursorShape =           Qt.ArrowCursor
            cursorPositionClass.showCursor()


        }

        onFocusChanged: {
            if (!rootTI.focus || !ma.focus) {

                if (object_rightPanel !== undefined) {
                    object_rightPanel.destroy()
                    object_rightPanel = undefined
                    enableRightClick = true
                }

                didMarker = false
                if (isTimeSignature) {
                    var num = bound(min, text * 1, max).toFixed(decimal)
                    text = timeSignature(num)
                    value = text
                } else {
                    text = bound(min, text * 1, max).toFixed(decimal)
                    value = text
                }
            }
        }

        onMouseYChanged: {
            // if(didStart) {
            itemPos = mapToItem(rootTI, mouse.x, mouse.y)

            var pos = mapToItem(rootTI, mouse.x, 60)
            //slider:
            //            if(y_in_s > 0)
            //                y_in_s =         mouse.y >= 0 ? mouse.y : 0+
            if (isSlider) {
                y_in_s = (mouse.y >= 0
                          && mouse.y <= 60) ? mouse.y : mouse.y
                                              < 0 ? 0 : mouse.y > 60 ? 60 : mouse.y
                if (mouse.y > 60)
                    cursorPositionClass.moveCursor(60, 0)
            } else {

                // console.log('mouse y: '+mouse.y+", hh: "+itemPos.y)
                if (isEdge) {
                    isEdge = false
                    lastPos = itemPos.y
                    return
                }

                diff = lastPos - itemPos.y

                y_pos += diff
                y_pos = bound(min * scaleY, y_pos, max * scaleY)

                value = y_pos / scaleY

                text = value.toFixed(decimal)


                /*rootTI.text =   (isGain && value>0) ? (value/6).toFixed(decimal) :
                                                    isTimeSignature ?
                                                    timeSignature(value) :
                                                    value.toFixed(decimal)*/
                lastPos = itemPos.y

                glb_pos = mapToGlobal(mouse.x, mouse.y)
                var vv = 100
                if ((glb_pos.y <= vv) && ((value) < max)
                        || (glb_pos.y >= (Screen.height - vv))
                        && ((value) > min)) {
                    cursorPositionClass.moveCursor((Screen.width - vv) / 2,
                                                   ((Screen.height - vv) / 2))
                    isEdge = true
                }

                moved(value)
            } // else.
        } //-- mouse y.
    }
}
