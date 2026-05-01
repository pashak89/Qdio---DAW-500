import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Shapes 1.12
import QtGraphicalEffects 1.2
import QtQuick.Layouts 1.12

Rectangle{
    id:             rootTrackDial
    anchors.fill:   parent
    color:         'transparent'


    //------ save:
    property var obj:               null
    property bool is_ValueChanged:  false
    property real previousValue:    0
    property bool firstValueClick:  true
    property bool canUndo:          true
    property bool afterLoaded:      false


    property int y_pos:             0
    property int diff:              0

    property double step:           0.01
    property double sense:          20

    property real saveXPos:         0
    property real saveYPos:         0

    property double range:          (maxVal-minVal) / step
    property double scaleY:         1

    property double lastPos
    property double pressPos
    property var itemPos
    property var glb_pos
    property bool didStart:         false

    property color imgColor:        '#1E1E1E'
    property color valDialColor:    '#1DB4C8'
    property color rootDialColor:   "#1E1E1E"
    property color triColor:        rootDialColor

    property alias value:           dial.value

    property int decimal:           2

    property real speedNormal:      2.5*5
    property real speedShift:       decimal>1 ? 0.0001 : 0.1
    property real speed:            speedNormal

    property real firstY:           0

    property real minVal:           -10
    property real maxVal:           10

    property real lineWidth:        3
    property real dialWidth:        4

    property real defaultValue:     0.00

    property bool isInitZero:       false
    property bool isEdge:           false

    function undoCtrl_dial() {
        if(is_ValueChanged && canUndo) {
            // undo:
            if(firstValueClick) {
                if(undoManager.saveIndex === undoManager.actionsSave.length - 1) {
                    undoManager.actionsSave.push({
                                                     object: obj,
                                                     name: obj.objectName,
                                                     value: obj.previousValue,
                                                     type: 'dial'
                                                 })
                    undoManager.saveIndex++
                } else {
                    undoManager.saveIndex++
                    undoManager.actionsSave[undoManager.saveIndex] =
                            ({
                                 object: obj,
                                 name: obj.objectName,
                                 value: obj.previousValue,
                                 type: 'dial'
                             })
                }
                firstValueClick = false
            }

            if(undoManager.saveIndex === undoManager.actionsSave.length - 1) {
                undoManager.actionsSave.push({
                                                 object: obj,
                                                 name: obj.value,
                                                 value: obj.previousValue,
                                                 type: 'dial'
                                             })
                undoManager.saveIndex++
            } else {
                undoManager.saveIndex++
                undoManager.actionsSave[undoManager.saveIndex] =
                        ({
                             object: obj,
                             name: obj.objectName,
                             value: obj.value,
                             type: 'dial'
                         })
            }

            is_ValueChanged = false
        }// undo.
    }

    Image {
        id:                         topTriangle
        anchors.horizontalCenter:   parent.horizontalCenter

        width:              resoless(9)
        height:             resoless(7)
        source:             "qrc:/tri-rec.svg"
        sourceSize.width:   resoless(13)
        sourceSize.height:  resoless(10)

        smooth:             true
        layer {
            enabled:        true
            effect: ColorOverlay {
                color:      triColor
                cached:     true
            }
        }
        MouseArea {
            anchors.fill:   parent
            onClicked: {
                dial.value =defaultValue.toFixed(decimal)
                y_pos =     dial.value * scaleY
            }
        }
    }

    function bound(minValue, myValue, maxValue) {
        if(myValue > maxValue)
            return maxValue
        else if(myValue < minValue)
            return minValue
        else
            return myValue
    }

    Component.onCompleted: {
        scaleY =    Math.round(range / rootTrackDial.height / 80)
        y_pos =     Math.round(scaleY * defaultValue)
        value =     y_pos /scaleY

        // undo:
        afterLoaded = true
    }

    onValueChanged: {
        triColor =  (value === defaultValue) ? rootDialColor : valDialColor
        // undo:
        if(afterLoaded)
            is_ValueChanged = true
        if(value === defaultValue)
            firstValueClick = true
        // undo.
    }

    Dial {
        id:         dial

        inputMode:  Dial.Vertical

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:              parent.top
        anchors.topMargin:        resoless(10)

        width:                    dialWidth
        height:                   width

        background: Shape {
            x:             dial.width / 2 - width / 2
            y:             dial.height / 2 - height / 2
            width:         Math.max(dial.width, Math.min(dial.width, dial.height))
            height:        width

            smooth:        true
            layer.enabled: true
            layer.samples: 24
            layer.smooth:  true

            ShapePath {
                fillColor:      "transparent"
                strokeColor:    rootDialColor
                strokeWidth:    lineWidth
                capStyle:       ShapePath.RoundCap

                PathAngleArc {
                    centerX:    dial.width/2; centerY: dial.width/2
                    radiusX:    dial.width/2.25; radiusY: dial.width/2.25
                    startAngle: -230
                    sweepAngle: 280
                }
            }
        }


        Shape {
            id:             valueDial
            x:              dial.width / 2 - width / 2
            y:              dial.height / 2 - height / 2
            width:          Math.max(dial.width, Math.min(dial.width, dial.height))
            height:         width

            smooth:         true
            layer.enabled:  true
            layer.samples:  24
            layer.smooth:   true

            ShapePath {
                fillColor:   "transparent"
                strokeColor: valDialColor
                strokeWidth: lineWidth
                capStyle:    ShapePath.FlatCap

                PathAngleArc {
                    centerX:    dial.width/2; centerY: dial.width/2
                    radiusX:    dial.width/2.25; radiusY: dial.width/2.25
                    startAngle: isInitZero ? -230 : -90
                    sweepAngle: dial.angle + (isInitZero ? 140 : 0)
                }
            }
        }

        from:               minVal.toFixed(decimal)
        value:              0
        to:                 maxVal.toFixed(decimal)

        onValueChanged:     (value != (minVal+maxVal)/2 && parent.enabled) ?
                             imgColor = valDialColor :
                             imgColor = rootDialColor

        handle: Rectangle {
            id:             handleItem
            x:              dial.background.x + dial.background.width / 2 - width / 2
            y:              dial.background.y + dial.background.height / 2 - height/2
            z:              5
            width:          dial.width /13
            height:         parent.height/2.0
            color:          rootDialColor
            radius:         4
            antialiasing:   true
            smooth:         true


            Rectangle{
                id:                 leftRec
                visible:            dial.value<=0 && dial.value > dial.from
                anchors.right:      parent.left
                anchors.top:        parent.top
                anchors.topMargin:  -1
                height:             lineWidth*2
                width:              2
                color:              theme.w1_fill_Color
                smooth:             true

                layer.enabled:      true
                layer.samples:      24
                layer.smooth:       true
            }
            Rectangle{
                id:             rightRec
                visible:        dial.value>=0 && dial.value < dial.to
                anchors.left:   parent.right
                anchors.top:    parent.top
                height:         lineWidth*2
                width:          2
                color:          theme.w1_fill_Color

                smooth:         true
            }

            transform: [
                Translate {
                    y:          -handleItem.height / 2
                },
                Rotation {
                    angle:      dial.angle
                    origin.x:   handleItem.width / 2
                    origin.y:   handleItem.height / 2
                }
            ]
        }///------ handle
    }//----> dial

    Keys.onPressed: {
        if(event.key === Qt.Key_Shift){

            var maxLastPos =    rootTrackDial.height * scaleY;
            scaleY *=           Math.round(sense)
            var maxPos =        rootTrackDial.height * scaleY;
            y_pos =             y_pos * maxPos / maxLastPos;
        }
    }

    Keys.onReleased: {
        if(event.key === Qt.Key_Shift){

            var maxLastPos =    rootTrackDial.height * scaleY
            scaleY /=           Math.round(sense)
            var maxPos =        rootTrackDial.height * scaleY
            y_pos =             y_pos * maxPos / maxLastPos
        }
    }

    MouseArea {
        id:                 ma
        anchors.bottom:     parent.bottom
        anchors.left:       parent.left
        anchors.right:      parent.right
        height:             dial.height - topTriangle.height - resoless(8-3)

        onDoubleClicked: {
            dial.value =    defaultValue.toFixed(decimal)
            y_pos =         dial.value * scaleY
        }
        onPressed: {
            var globalPos = mapToGlobal(mouse.x, mouse.y)
            firstY =        globalPos.y

            itemPos =       mapToGlobal(mouse.x, mouse.y)
            lastPos =       itemPos.y

            y_pos =         value * scaleY

            previousValue = value

            ma.focus =      true
            rootTrackDial.focus =   true

            mainRoot_id.cursorPosition.hideCursor()
            mainRoot_id.cursorPosition.saveCursorPos(globalPos.x, globalPos.y)

            didStart =      true
        }
        onReleased: {
            didStart =      false

            ma.focus =      true
            mainRoot_id.cursorPosition.showCursor()

            undoCtrl_dial()
        }

        onMouseYChanged: {
            if(didStart) {
                itemPos =       mapToGlobal(mouse.x, mouse.y)

                if(isEdge){
                    isEdge =    false
                    lastPos =   itemPos.y
                }

                diff =          lastPos - itemPos.y

                y_pos +=        diff
                y_pos =         bound(minVal*scaleY, y_pos , maxVal*scaleY)


                value =         y_pos/scaleY

                lastPos =       itemPos.y

                glb_pos =       mapToGlobal(mouse.x, mouse.y)
                var vv =        50
                if((glb_pos.y <= vv ) && ((value) < maxVal) ||
                        (glb_pos.y >= (Screen.height-vv)) && ((value) > minVal)){
                    mainRoot_id.cursorPosition.moveCursor((Screen.width-vv)/2 ,
                                                          ((Screen.height-vv)/2))
                    isEdge =    true
                }// reset mouse position while drag.
            }// if did start.
        }// mouse in Y changed.
    }// mouse area
}
