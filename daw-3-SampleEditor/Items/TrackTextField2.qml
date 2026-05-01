import QtQuick 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.2

TextInput {
    id: rootTI

    signal sigPressed
    property string name: ""
    property bool pressed: false
    property int index: -1
    property real ySlider: 0
    property real hSlider: 1
    property bool isSlider: false

    property bool isMaster: false

    property bool bigHeight: false

    //------ save:
    property var obj: null
    property bool canUndo: true

    property real xPosRightClick: 0
    property real yPosRightClick: 0

    property double y_pos: 0
    property double diff: 0

    property double step: 0.01

    property double sense: 20

    property double range: Math.abs(max - min) / step
    property int scaleY: 1

    property double lastPos
    property double pressPos
    property var itemPos
    property var glb_pos

    property int outputRightClick

    property real enterValue: 0

    property real min: 0
    property real max: 100

    property real svalue: 50
    property real value: 50

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
    property bool isOnOff: false
    property bool isVolume: false
    property bool isPan: false
    property bool didShadow: false
    property bool fakeSliderValue: false
    property bool isEdge: false
    property bool hasSlider: false
    property bool has2Slider: false
    property bool didMarker: false

    property real space2slider: -1 //height / 50

    property color brColor: theme.s11_Enable_BRD
    property color flColor: theme.s10_Disable_FIL

    property real lineW: refSize

    property int forSpeed: 0

    property var shortcutKeyTxt: ['Ctrl+M']
    property var moduls: ['EDIT MIDI Map']
    property bool enableRightClick: true
    property bool is2Groups: false
    property var rightComponent
    property var object_rightPanel

    property string splitStr: ''
    property color sliderLineColor: theme.s21_on

    signal action(int index, string text)

    signal sigValueChanged(real value)
    function valueChange() {
        sigValueChanged(rootTI.value)
    }

    function callback(index, text) {
        action(index, text)
    }

    onHeightChanged: {
        scaleY = Math.round(range / rootTI.height / (70 + forSpeed))
        y_pos = Math.round(scaleY * defaultValue)
        // value = y_pos / scaleY
    }

    maximumLength: max.toString().length + decimal + 3
    validator: DoubleValidator {
        bottom: min
        top: max
        decimals: decimal
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

    font.pixelSize: rootTI.fontSize
    font.weight: Font.DemiBold
    font.family: fontFamilty
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    color: 'transparent'

    InfoText2 {
        id: myTxtValue
        visible: !infText.visible
        name: {

            if (isPan) {
                if (Math.abs(value - 50) <= 1)
                    "C"
                else if (value > 50)
                    Math.abs(rootTI.text) + "L"
                else if (value < 50)
                    Math.abs(rootTI.text) + "R"
            } else if (isOnOff) {
                if (value == 25)
                    "On"
                else
                    "Off"
            } else
                rootTI.text
        }

        anchors.fill: parent
        bottomPaddingTxt: hasSlider ? sliderLine_a.height
                                      + 1 : has2Slider ? sliderLine_b.height + 1 : 0

        fontSize: rootTI.fontSize
        colorTxt: fontColor
    }

    function bound(minValue, myValue, maxValue) {
        if (myValue > maxValue)
            return maxValue
        else if (myValue < minValue)
            return minValue
        else
            return myValue
    }

    InfoText {
        id: infText
        visible: parseFloat(text) <= infValue && isVolume
        name: '-inf'
        anchors.leftMargin: Math.round(parent.width / 2 - contentW / 2)
        fontSize: rootTI.fontSize
        colorTxt: fontColor
    }

    onActiveFocusChanged: {
        if (rootTI.focus)
            didShadow = true
    }

    Rectangle {
        id: rectSlider
        enabled: rootTI.hasSlider
        visible: rootTI.hasSlider

        anchors.fill: parent
        anchors.leftMargin: refSize
        anchors.rightMargin: refSize
        anchors.bottomMargin: topWidth + (bigHeight ? -1 : 0)
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
            width: (max - value) / Math.abs(max - min) * (parent.width)

            height: Math.round(
                        rectSlider.height
                        / 10) //track_list.topWidth === 2 ? 3 : track_list.topWidth
            color: isMaster ? '#9C36FB' : /*envelopeBtn.isClicked ?*/ sliderLineColor //:
            //                                            theme.s20_off
            anchors {
                left: parent.left
                bottom: parent.bottom
            }
        } // sliderLine_a
    } // rect of slider.

    Rectangle {
        id: rect2Slider
        enabled: rootTI.has2Slider
        visible: rootTI.has2Slider

        anchors.fill: parent
        anchors.leftMargin: refSize
        anchors.rightMargin: refSize
        anchors.bottomMargin: topWidth + (bigHeight ? -1 : 0)
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
            x: value < 50 ? parent.width / 2 : parent.width / 2 - width
            width: Math.abs((50 - value) / Math.abs(max - min) * parent.width)

            height: Math.round(
                        rect2Slider.height
                        / 10) //track_list.topWidth === 2 ? 3 : track_list.topWidth
            color: theme.s22_on

            anchors {
                //                left:   parent.left
                bottom: parent.bottom
            }
        }
    }

    MouseArea {
        id: ma0
        visible: enableRightClick
        enabled: enableRightClick
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        onClicked: {

            rootTI.focus = true
            ma0.focus = true
            if (enableRightClick) {
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

                if (enableRightClick && trackRightClick.visible)
                    trackRightClick.visible = false
            }
        }

        onFocusChanged: {

            if (rootTI.focus || ma0.focus)
                didShadow = true
            else {
                didShadow = false
                didMarker = false
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onEntered: {

            _clipArea.setOverrideCursor(Qt.ArrowCursor)
        }
    }

    property int startX: 0
    property int startY: 0
    property int lastX: 0
    property int lastY: 0
    MouseArea {
        id: ma

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        onDoubleClicked: {

            value = defaultValue
            y_pos = value * scaleY
            valueChange()
        }

        onPressed: {

            /// undo
            rootTI.pressed = true
            if (enableRightClick && trackRightClick.visible)
                trackRightClick.visible = false
            if (enableRightClick && rightPanel.visible)
                rightPanel.visible = false

            var globalPosition = mapToGlobal(mouse.x, mouse.y)
            startX = globalPosition.x
            startY = globalPosition.y
            svalue = value

            y_pos = value * scaleY

            rootTI.focus = true
            ma.focus = true

            _clipArea.setOverrideCursor(Qt.BlankCursor)

            if (mouse.button === Qt.LeftButton) {
                sigPressed()
            }
        }

        onReleased: {

            rootTI.pressed = false
            ma.focus = true
            isClear = true

            _clipArea.setOverrideCursor(Qt.ArrowCursor)

            _clipArea.setCursorPosition(startX, startY)
        }

        onFocusChanged: {

            if (rootTI.focus || ma.focus)
                didShadow = true
            else {

                if (enableRightClick) {
                    if (trackRightClick.visible)
                        trackRightClick.visible = false
                    rightPanel.visible = false
                }

                didShadow = false
                didMarker = false

                //value = bound(min, text * 1, max)
            }
        }

        onPositionChanged: {

            var currentPos = mapToGlobal(mouse.x, mouse.y)

            diff = startY - currentPos.y

            y_pos -= diff / (holdShift ? 10 : 1)
            y_pos = bound(min * scaleY, y_pos, max * scaleY)

            if (isOnOff) {
                if (value == 75) {
                    if (diff > 0) {
                        value = 25
                    } else {
                        value = 75
                    }
                } else {
                    if (diff < 0) {
                        value = 75
                    } else {
                        value = 25
                    }
                }
            } else {
                value = y_pos / scaleY
            }

            valueChange()

            _clipArea.setCursorPosition(startX, startY)
        }
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
