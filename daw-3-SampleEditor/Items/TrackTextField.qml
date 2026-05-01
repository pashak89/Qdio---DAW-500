import QtQuick 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.2

TextInput {
    id: rootTI

    property string name: "test1"
    property bool pressed: false
    property real ySlider: 0
    property real hSlider: 1
    property bool isSlider: false

    property bool isMaster: false

    property bool bigHeight: false

    property bool enableFocus: true

    //------ save:
    property var obj: null
    property real previousValue: 0
    property bool canUndo: true

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
    property real lastValue: 0
    property real defaultValue: 0.00
    property int decimal: 2
    property real infValue: -70.00
    property string unit: ''

    property real fontSize: topfontPixelSize
    property string fontFamilty: topFontFamily
    property color fontColor: theme.tColor

    property real firstY: 0

    property bool isClear: true
    property bool isInf: true
    property bool didShadow: false
    property bool fakeSliderValue: false
    property bool isEdge: false
    property bool isTempo: false
    property bool innerTempo: false
    property bool hasSlider: false
    property bool has2Slider: false
    property bool didMarker: false
    property bool isTimeSignature: false
    property bool isGain: false

    property real space2slider: -1 //height / 50

    property color brColor: theme.s11_Enable_BRD
    property color flColor: theme.s10_Disable_FIL

    property real lineW: track_list.refSize

    property int forSpeed: 0

    property var shortcutKeyTxt: ['Ctrl+M']
    property var moduls: ['EDIT MIDI Map']
    property bool enableRightClick: true
    property var rightComponent
    property var object_rightPanel
    property bool is2Groups: false
    property string splitStr: ''

    signal sigValueChanged(real value)
    function valueChange() {

        sigValueChanged(parseFloat(rootTI.text))
    }
    signal action(int index, string text)

    function callback(index, text) {
        action(index, text)
    }

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

    text: rootTI.has2Slider ? Math.abs(value.toFixed(
                                           0)) : value.toFixed(decimal)
    font.pixelSize: rootTI.fontSize
    font.weight: Font.DemiBold
    font.family: fontFamilty
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    rightPadding: (unit === '' || unit === ' '
                   || isInf === false) ? 0 : unitText.contentW
    leftPadding: value <= infValue && isGain ? 500 : 0
    color: 'transparent'

    onLeftPaddingChanged: {
        if (leftPadding > 200) {
            myTxtValue.visible = false
            myTxtValue.enabled = false
        } else {
            myTxtValue.visible = true
            myTxtValue.enabled = true
        }
    }

    InfoText2 {
        id: myTxtValue
        name: {
            if (has2Slider) {
                if (value > -1 && value < 1)
                    'C'
                else if (value <= -1)
                    rootTI.text * -1 + "L"
                else if (value >= 1)
                    rootTI.text + "R"
            } else
                rootTI.text
        }

        anchors.fill: parent
        bottomPaddingTxt: hasSlider ? sliderLine_a.height
                                      + 1 : has2Slider ? sliderLine_b.height + 1 : 0

        fontSize: rootTI.fontSize
        colorTxt: fontColor
    }

    Component.onCompleted: {
        scaleY = Math.round(range / rootTI.height / (70 + forSpeed))
        y_pos = Math.round(scaleY * defaultValue)
        value = y_pos / scaleY

        innerTempo = isTempo
    }

    onHeightChanged: {
        scaleY = Math.round(range / rootTI.height / (70 + forSpeed))
        y_pos = Math.round(scaleY * defaultValue)
        value = y_pos / scaleY

        innerTempo = isTempo
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
        name: value <= infValue && isGain ? '-inf  dB' : unit

        anchors.leftMargin: value <= infValue
                            && isGain ? Math.round(
                                            parent.width / 2 - contentW / 2) : Math.round(
                                            parent.width / 2 - contentW / 2)
                                        + rootTI.contentWidth / 2
        anchors.topMargin: 1
        fontSize: rootTI.fontSize
        colorTxt: fontColor
    }

    onActiveFocusChanged: {
        if (rootTI.focus & enableFocus)
            didShadow = true
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Shift) {
            innerTempo = isTempo

            var maxLastPos = rootTI.height * scaleY
            scaleY /= Math.round(sense)
            var maxPos = rootTI.height * scaleY
            y_pos = y_pos * maxPos / maxLastPos
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Shift) {
            innerTempo = false

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
                didShadow = true
            }
        } // type numbers

        if ((event.key === 16777220) || (event.key === Qt.Key_Enter)) {
            didMarker = false
            if (isGain) {
                if (text <= infValue) {
                    value = infValue
                    isInf = true
                    rootTI.leftPadding = 500
                    valueChange()
                } //-----infValue
                else if (text >= min && text <= max / 6) {
                    value = isGain && (text * 1) > 0 ? text * 6 : text
                    text = isGain
                            && value > 0 ? (value / 6).toFixed(
                                               decimal) : value.toFixed(decimal)

                    valueChange()
                } else if (text > max / 6) {
                    value = isGain
                            && (text * 1) > 0 ? (max).toFixed(
                                                    decimal) : max.toFixed(
                                                    decimal)
                    text = isGain
                            && value > 0 ? (max / 6).toFixed(
                                               decimal) : max.toFixed(decimal)
                    isInf = true ///////////////////////////////////////////////////
                    valueChange()
                } else if (text < min) {
                    rootTI.leftPadding = 0
                    text = min.toFixed(decimal)
                    value = min
                    valueChange()
                }
            } else if (isTimeSignature) {
                value = timeSignature(text * 1)
                text = value
                valueChange()
            } // if it is not gain =>
            else {
                if (text >= min && text <= max) {
                    value = text
                    text = value.toFixed(decimal)
                    valueChange()
                } else if (text > max) {
                    value = max.toFixed(decimal)
                    text = max.toFixed(decimal)
                    isInf = true ///////////////////////////////////////////////////
                    valueChange()
                } else if (text < min) {
                    rootTI.leftPadding = 0
                    text = min.toFixed(decimal)
                    value = min.toFixed(decimal)
                    valueChange()
                }
            }

            //            undoCtrl_textField()
        } // press Enter key.
    }

    //---------- Below Slider:
    Rectangle {
        id: rectSlider
        enabled: rootTI.hasSlider
        visible: rootTI.hasSlider

        anchors.fill: parent
        anchors.leftMargin: track_list.refSize
        anchors.rightMargin: track_list.refSize
        anchors.bottomMargin: track_list.topWidth + (bigHeight ? -1 : 0)
        radius: topRadius
        color: 'transparent'

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: rectSlider.width
                height: rectSlider.height
                radius: rectSlider.radius
            }
        }
        Rectangle {
            id: sliderLine_a
            width: (value - min) / (max - min) * (parent.width)

            height: Math.round(
                        rectSlider.height
                        / 10) //track_list.topWidth === 2 ? 3 : track_list.topWidth
            color: isMaster ? '#9C36FB' : /*envelopeBtn.isClicked ?*/ theme.s21_on //:
            //                                            theme.s20_off
            anchors {
                left: parent.left
                bottom: parent.bottom
            }
        } // sliderLine_a
    } // rect of slider.
    //---------- Below Slider.

    //---------- Below Between Slider:
    Rectangle {
        id: rect2Slider
        enabled: rootTI.has2Slider
        visible: rootTI.has2Slider

        anchors.fill: parent
        anchors.leftMargin: track_list.refSize
        anchors.rightMargin: track_list.refSize
        anchors.bottomMargin: track_list.topWidth + (bigHeight ? -1 : 0)
        radius: topRadius
        color: 'transparent'

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Rectangle {
                width: rect2Slider.width
                height: rect2Slider.height
                radius: rect2Slider.radius
            }
        }
        Rectangle {
            id: sliderLine_b
            x: value > 0 ? parent.width / 2 : parent.width / 2 - width
            width: Math.abs(value / (max - min) * parent.width)

            height: Math.round(
                        rect2Slider.height
                        / 10) //track_list.topWidth === 2 ? 3 : track_list.topWidth
            color: theme.s22_on

            anchors {
                //                left:   parent.left
                bottom: parent.bottom
            }
        } // sliderLine_b
    } // rect of slider.

    //---------- Below Slider.
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onEntered: {

            _clipArea.setOverrideCursor(Qt.ArrowCursor)
        }
    }

    // ---------------- Right click setting:
    MouseArea {
        id: ma0
        visible: enableRightClick
        enabled: enableRightClick
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        onClicked: {
            if (enableFocus) {
                rootTI.focus = true
                ma0.focus = true
            }
            //positionInWindow = mapToGlobal(mouse.x, mouse.y)
            positionInWindow = Qt.point(mouse.x, mouse.y)

            rightPanel.leftModules = moduls
            rightPanel.rightModules = shortcutKeyTxt
            rightPanel.isTwoGroups = is2Groups
            rightPanel.firstStr = splitStr
            rightPanel.cc = parent
            rightPanel.visible = true
            rightPanel.index = index
            rightPanel.callback = rootTI.callback
            rightPanel.parent = rootTI

            if (trackRightClick.visible)
                trackRightClick.visible = false
        }

        onFocusChanged: {
            if (rootTI.focus || ma0.focus & enableFocus)
                didShadow = true
            else {
                rootTI.pressed = false
                if (trackRightClick.visible)
                    trackRightClick.visible = false
                rightPanel.visible = false
                didShadow = false
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
            value = defaultValue
            rootTI.text = defaultValue.toFixed(decimal)
            y_pos = value * scaleY
            valueChange()
        }

        onPressed: {
            /// undo
            rootTI.pressed = true
            if (trackRightClick.visible)
                trackRightClick.visible = false
            if (rightPanel.visible)
                rightPanel.visible = false

            if (isGain)
                previousValue = value
            else
                previousValue = text * 1

            var globalPosition = mapToGlobal(mouse.x, mouse.y)
            firstY = globalPosition.y

            itemPos = mapToItem(rootTI, mouse.x, mouse.y)
            lastPos = itemPos.y

            //            if (!isGain) {
            //                value = rootTI.text.split(' ')[0] * 1
            //            }
            y_pos = value * scaleY

            if (enableFocus) {
                rootTI.focus = true
                ma.focus = true
            }

            _clipArea.setOverrideCursor(Qt.BlankCursor)
            _clipArea.setCursorPosition(globalPosition.x, globalPosition.y)

            ySlider = mouse.y
        }

        onReleased: {

            rootTI.pressed = false
            if (enableFocus)
                ma.focus = true
            isClear = true

            _clipArea.setOverrideCursor(Qt.ArrowCursor)
        }

        onFocusChanged: {

            if (rootTI.focus || ma.focus) {
                didShadow = true
            } else {

                rootTI.pressed = false
                if (trackRightClick.visible)
                    trackRightClick.visible = false
                rightPanel.visible = false

                didShadow = false
                didMarker = false

                if (isGain) {
                    text = bound(min, text * 1, max / 6).toFixed(decimal)
                    value = bound(min, text * 1, max / 6)
                } else if (isTimeSignature) {
                    var num = bound(min, text * 1, max).toFixed(decimal)
                    text = timeSignature(num)
                    value = bound(min, text * 1, max)
                } else {
                    text = bound(min, text * 1, max).toFixed(decimal)
                    value = bound(min, text * 1, max)
                }
            }
        }

        onPositionChanged: {
            itemPos = mapToItem(rootTI, mouse.x, mouse.y)

            ySlider = value

            if (isEdge) {
                isEdge = false
                lastPos = itemPos.y
            }

            diff = lastPos - itemPos.y

            y_pos += diff
            y_pos = bound(min * scaleY, y_pos, max * scaleY)

            value = innerTempo ? Math.round(
                                     y_pos / scaleY) : isTempo ? roundToTenth(
                                                                     y_pos / scaleY) : y_pos
                                                                 / scaleY

            if (isSlider === false)
                rootTI.text = (isGain && value
                               > 0) ? (value / 6).toFixed(
                                          decimal) : isTimeSignature ? timeSignature(
                                                                           value) : value.toFixed(
                                                                           decimal)

            value = parseInt(rootTI.text)
            if (lastValue.toFixed(decimal) != value.toFixed(decimal)) {
                value = value.toFixed(decimal)
                valueChange()
                lastValue = value.toFixed(decimal)
            }
            lastPos = itemPos.y

            glb_pos = mapToGlobal(mouse.x, mouse.y)
            var vv = 100
            if ((glb_pos.y <= vv) && ((value) < max)
                    || (glb_pos.y >= (Screen.height - vv)) && ((value) > min)) {
                _clipArea.setCursorPosition((Screen.width - vv) / 2,
                                            ((Screen.height - vv) / 2))
                isEdge = true
            }
        } //-- mouse y.
    }

    Rectangle {
        visible: didShadow
        anchors.fill: borderRec
        color: theme.b10_Disable_BRD //'transparent'

        anchors.margins: -refSize
        radius: borderRec.radius
        z: -1
    }
}
