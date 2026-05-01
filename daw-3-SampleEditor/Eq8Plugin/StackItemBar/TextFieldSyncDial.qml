import QtQuick 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.2
import "../Item"

// import 'qrc:/Items/'
TextInput {
    id: root

    // width: parent.width
    signal moved
    signal pressed

    // height: scaleSize(25)
    property bool disable: false

    // semi slider
    property real y_in_s: 0
    property bool isSlider: false

    property bool maFocus: ma.focus

    FontLoader {
        id: thisFont
        source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf'
    }

    readonly property int textsize: topfontPixelSize

    property real xPosRightClick: 0
    property real yPosRightClick: 0

    property double y_pos: 0
    property double diff: 0

    property double step: 0.01
    property double sense: 20

    // property double range:          (max-min) / step
    property double scaleY: 1

    property int gain: 1
    property double lastPos
    property double pressPos
    property var itemPos
    property var glb_pos

    property int outputRightClick

    property real enterValue: 0

    property real min: -10
    property real max: 200

    property real value: 0
    property real _x: 1 //variance ? (value - min) / (max - min) : 1
    property real _value: value //variance ? min * Math.pow(10, _x * Math.log(max/min) / Math.log(10) ) : value
    property bool variance: false
    property real defaultValue: 0.00
    property bool isDecimal: true
    property int decimal: Math.abs(value) < 10 | value >= 990 & value
                          < 9994 ? 2 : value >= 100 & value < 980 ? 0 : 1

    property real infValue: -7000.00
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
        anchors {
            fill: parent
            margins: -topWidth
            leftMargin: topWidth
            rightMargin: topWidth
        }

        color: parent.focus | parent.activeFocus
               | ma.focus ? trackEnable ? disable ? 'transparent' : flColor : 'transparent' : 'transparent'
        border.color: parent.focus | parent.activeFocus | ma.focus ? 'black' /*brColor*/
                                                                   : 'transparent'
        border.width: topWidth
    }

    text: _value.toFixed(
              decimal) //value >= 1000 ? (value/1000).toFixed(decimal) : value.toFixed(decimal)

    font.pixelSize: 15
    font.weight: Font.DemiBold
    font.family: thisFont.name
    verticalAlignment: Text.AlignVCenter
    horizontalAlignment: Text.AlignHCenter

    rightPadding: (unit === '' || unit === ' '
                   || isInf === false) ? 0 : unitText.contentW
    leftPadding: value <= infValue ? 500 : 0
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

    function varianceInvFunc(value) {

        if (variance) {
            var v = Math.log(value / min) / Math.log(
                        max / min) * (max - min) + min
        } else
            v = value
        return v
    }

    InfoText {
        id: myTxtValue
        text: root.text
        rightPadding: didMarker | unit === '' ? 0 : unitText.contentW //Math.round(parent.width - contentW) //- unitText.contentW/4
        color: fontColor
        fontSize: textsize
    }

    Component.onCompleted: {
        scaleY = 200 / (max - min)
        y_pos = Math.round(scaleY * defaultValue)

        if (variance) {
            value = varianceInvFunc(defaultValue)
        } else
            value = defaultValue
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
        visible: !didMarker
        text: _value >= 1000 ? 'k' + unit : ' '
                               + unit //value <= infValue && isGain ? '-inf  dB' : unit

        anchors.fill: parent
        leftPadding: value <= infValue ? Math.round(parent.width / 2 - contentW
                                                    / 2) : testTextSize.contentW
                                         * 1.1 //Math.round(parent.width/2 - contentW/2) + root.contentWidth/2
        color: fontColor
        fontSize: textsize
    }

    InfoText {
        id: testTextSize
        text: '8.88'
        anchors.fill: parent
        color: 'transparent'
        fontSize: textsize
    }

    // onActiveFocusChanged: { if(rootTI.focus) didShadow = true }
    Keys.onReleased: {
        if (event.key === Qt.Key_Shift) {

            var maxLastPos = root.height * scaleY
            scaleY /= Math.round(sense)
            var maxPos = root.height * scaleY
            y_pos = y_pos * maxPos / maxLastPos
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Shift) {

            var maxLastPos = root.height * scaleY
            scaleY *= Math.round(sense)
            var maxPos = root.height * scaleY
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


            /*if(isGain) {
                if(text <= infValue) {
                    value = infValue
                    isInf = true
                    root.leftPadding = 500
                }//-----infValue

                else if(text >= min && text <= max/6) {
                    value = (text*1)>0 ? text*6 : text
                    text =  value>0 ? (value/6).toFixed(decimal) :value.toFixed(decimal)
                }
                else if(text > max/6) {
                    value = (text*1)>0 ? (max).toFixed(decimal) :max.toFixed(decimal)
                    text =  value>0 ? (max/6).toFixed(decimal) :max.toFixed(decimal)
                    isInf = true ///////////////////////////////////////////////////
                }
                else if(text < min) {
                    root.leftPadding = 0
                    value = min.toFixed(decimal)
                    text =  min.toFixed(decimal)
                }
            }
            else if(isTimeSignature) {
                value = timeSignature(text*1)
                text =  value >= 1000 ? (value/1000).toFixed(decimal) : value.toFixed(decimal)
            }

            // if it is not gain =>
           //
            else*/ {
                if (text >= min && text <= max) {
                    value = varianceInvFunc(text * 1) //text*1
                    // text =  value >= 1000 ? (value/1000).toFixed(decimal) : value.toFixed(decimal)
                } else if (text > max) {
                    value = varianceInvFunc(max) //max.toFixed(decimal)
                    // text =  value >= 1000 ? (value/1000).toFixed(decimal) : value.toFixed(decimal)
                    isInf = true ///////////////////////////////////////////////////
                } else if (text < min) {
                    root.leftPadding = 0
                    value = varianceInvFunc(min) //min.toFixed(decimal)
                    // text =  value >= 1000 ? (value/1000).toFixed(decimal) : value.toFixed(decimal)
                }
            }
        } // press Enter key.
    }

    onValueChanged: {
        if (_value > infValue) {
            isInf = true
            root.leftPadding = 0
        } else if (_value <= infValue)
            root.leftPadding = 500
    }

    //---------- Below Slider:
    Rectangle {
        id: rect
        enabled: root.hasSlider
        visible: root.hasSlider

        anchors.fill: parent
        // anchors.leftMargin:     1//mainRoot_id.refSize
        anchors.rightMargin: 1 //mainRoot_id.refSize
        anchors.bottomMargin: topWidth - 1
        radius: topWidth
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

            height: topWidth - 1
            color: //control.checked ? theme.s21_on :
                   theme.s20_off
            anchors {
                left: parent.left
                bottom: parent.bottom
            }
        } // sliderLine_a
    } // rect of slider.

    //---------- Below Slider.
    MouseArea {
        id: ma

        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        onDoubleClicked: {

            if (variance) {
                value = varianceInvFunc(defaultValue)
            } else
                value = defaultValue
            // root.text =       defaultValue.toFixed(decimal)
            y_pos = value * scaleY

            moved()
        }

        onPressed: {

            var globalPosition = mapToGlobal(mouse.x, mouse.y)
            firstY = globalPosition.y

            // itemPos =               mapToItem(root, mouse.x, mouse.y)
            itemPos = mapToGlobal(mouse.x, mouse.y)
            lastPos = itemPos.y

            // value =            value >= 1000 ? root.text.split(' ')[0]*1000 : root.text.split(' ')[0]*1
            y_pos = value * scaleY

            root.focus = true
            ma.focus = true

            cursorPositionClass.saveCursorPos(globalPosition.x,
                                              globalPosition.y)
            cursorPositionClass.hideCursor()

            // cursorShape =           Qt.BlankCursor
            didStart = true

            //slider:
            // y_in_s = mouse.y
            root.pressed()
        }

        onReleased: {
            didStart = false

            // ma.focus =              true
            isClear = true
            // cursorShape =           Qt.ArrowCursor
            cursorPositionClass.showCursor()
        }

        onFocusChanged: {
            if (!root.focus || !ma.focus | !focus) {

                if (didMarker) {
                    value = varianceInvFunc(bound(min, text, max))
                    moved()
                }

                didMarker = false
            }
        }

        onMouseYChanged: {
            if (didStart) {
                // itemPos =       mapToItem(root.parent, mouse.x, mouse.y)
                itemPos = mapToGlobal(mouse.x, mouse.y)

                // var pos =       mapToItem(root.parent, mouse.x, 60)
                //slider:
                //            if(y_in_s > 0)
                //                y_in_s =         mouse.y >= 0 ? mouse.y : 0+


                /*            if(isSlider) {
                y_in_s = (mouse.y >= 0 && mouse.y <= 60) ? mouse.y :
                                                           mouse.y < 0 ? 0 :
                                                                         mouse.y > 60 ? 60 :
                                                                                        mouse.y
                if(mouse.y > 60)
                    cursorPosition.moveCursor(60, 0)
            } else*/ //{

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

                //                rootTI.text =   isGain && value>0 ? (value/6).toFixed(decimal) :
                //                                                    value.toFixed(decimal)

                // root.text =   (isGain && value>0) ? (value/6).toFixed(decimal) :
                //                                     isTimeSignature ?
                //                                     timeSignature(value) :
                //                                     value.toFixed(decimal)

                // text =  value >= 1000 ? (value/1000 ).toFixed(decimal) : value.toFixed(decimal)
                lastPos = itemPos.y

                glb_pos = mapToGlobal(mouse.x, mouse.y)
                var vv = 100
                if ((glb_pos.y <= vv) && ((value) < max)
                        || (glb_pos.y >= (Screen.height - vv))
                        && ((value) > min)) {
                    cursorPositionClass.moveCursor((Screen.width - vv) / 2,
                                                   ((Screen.height - vv) / 2))
                    isEdge = true
                    // }
                } // else.

                moved()
            }
        } //-- mouse y.
    }
}
