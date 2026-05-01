import QtQuick 2.12
import QtQuick.Window 2.12


Rectangle {
    id: scene

    width:      20
    height:     156
    color:      'transparent'
    radius:     5

    //------ save:
    property var obj:               null
    property bool is_ValueChanged:  false
    property real previousValue:    0
    property bool firstValueClick:  true
    property bool afterLoaded:      false

    property double y_pos:          0
    property double diff:           0

    property double max:            24*6
    property double min:            -70
    property double value:          defaultValue
    property double step:           0.01
    property double sense:          20

    property real defaultValue:     0


    property real saveXPos:         0
    property real saveYPos:         0

    property double range:          (max-min) / step
    property int scaleY:            1

    property double lastPos
    property double pressPos
    property var itemPos
    property var glb_pos

    property bool isEdge:           false
    property bool isIgnore:         false

    property color valueColor:      'black'
    property color emptyColor:      'black'
    property color brKnobeColor:    'black'
    property color flKnobeColor:    'black'

    property real wHand:            1
    property real wLine:            1

    property bool isDouble:         false


    function undoCtrl_slider() {
        //undo:
        if(is_ValueChanged) {

            if(firstValueClick) {
                if(undoManager.saveIndex === undoManager.actionsSave.length-1) {
                    undoManager.actionsSave.push({
                                                     object: obj,
                                                     name: obj.objectName ,
                                                     value: obj.previousValue,
                                                     type: 'slider'
                                                 })
                    undoManager.saveIndex++
                } else {
                    undoManager.saveIndex++
                    undoManager.actionsSave[undoManager.saveIndex] =
                            ({
                                 object: obj,
                                 name: obj.objectName ,
                                 value: obj.previousValue,
                                 type: 'slider'
                             })
                }
                firstValueClick = false
            }

            if(undoManager.saveIndex === undoManager.actionsSave.length-1) {
                undoManager.actionsSave.push({
                                                 object: obj,
                                                 name: obj.objectName,
                                                 value: obj.value,
                                                 type: 'slider'
                                             })
                undoManager.saveIndex++
            } else {
                undoManager.saveIndex++
                undoManager.actionsSave[undoManager.saveIndex] =
                        ({
                             object: obj,
                             name: obj.objectName ,
                             value: obj.value,
                             type: 'slider'
                         })
            }
            is_ValueChanged = false
        }// undo.
    }

    function pixToValue() {
        var result =    (-max + min) * (y_pos/scaleY) / scene.height  +max
        value =         result
    }

    function bound(minValue, myValue, maxValue)
    {
        if(myValue > maxValue)
            return maxValue
        else if(myValue < minValue)
            return minValue
        else
            return myValue
    }

    onValueChanged: {
        y_pos =     scaleY* scene.height * (max-value)/(max-min)

        knobe.y =    y_pos/scaleY - knobe.height/2

        // undo:
        if(afterLoaded)
            is_ValueChanged = true
        if(value === defaultValue)
            firstValueClick = true
        // undo.
    }

    onHeightChanged: {
        scaleY =    (range / scene.height) / 25
        y_pos =     scaleY* scene.height * (max-value)/(max-min)

        knobe.y =   y_pos/scaleY - knobe.height/2
    }

    Component.onCompleted: {
        scaleY =        (range / scene.height) / 25
        value =         defaultValue
        y_pos =         Math.round(max/(max-min) * scene.height  * scaleY)

        // undo:
        afterLoaded =   true
    }

    Rectangle {
        id:                         empty
        anchors.horizontalCenter:   parent.horizontalCenter
        anchors.top:                parent.top
        anchors.bottom:             parent.bottom

        width:      wLine
        radius:     scene.radius
        color:      emptyColor
    }
    Rectangle {
        id:                         filler
        anchors.horizontalCenter:   parent.horizontalCenter

        // the number '3' for remove a little top of knobe
        y:          knobe.y +3
        height:     empty.height - knobe.y -3
        width:      empty.width
        radius:     empty.radius
        color:      valueColor
    }

    Rectangle {
        id:                         knobe
        anchors.horizontalCenter:   empty.horizontalCenter
        width:      wHand
        height:     width
        radius:     width/2
        color:      flKnobeColor
        border {
            color:  brKnobeColor
            width:  mainRoot_id.topWidth
        }
    }



    Keys.onPressed: {
        if (event.key === Qt.Key_Shift) {
            var maxLastPos =    scene.height * scaleY
            scaleY *=           sense
            var maxPos =        scene.height * scaleY
            y_pos =             y_pos * maxPos / maxLastPos
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Shift) {
            var maxLastPos =    scene.height * scaleY
            scaleY /=           sense
            var maxPos =        scene.height * scaleY
            y_pos =             y_pos * maxPos / maxLastPos
        }
    }




    TrackTextField {
        anchors.fill: parent
        obj: undefined
        flColor: 'transparent'
        fontColor: 'transparent'
        brColor: 'transparent'
//                                            fakeSliderValue: true
//                                            isGain: true
        isSlider: true
        max: 24*6
        min: -70
        forSpeed: 160

        hSlider: scene.height
        onValueChanged: {
//            knobe.y = ySlider - knobe.height/2
            knobe.y = (ySlider-max)/(-max + min)*(scene.height) - knobe.height/2
            scene.value = value
//            knobe.y = ySlider - knobe.height/2
        }

    }
/*
    MouseArea {
        id:                     ma
        anchors.fill:           parent
        anchors.topMargin:      -knobe.height/2
        anchors.bottomMargin:   -knobe.height/2

        onDoubleClicked: {
            value =             defaultValue
            isDouble =          true
        }

        onPressed: {
            itemPos =           mapToItem(scene, mouse.x, mouse.y)
            pressPos = lastPos = itemPos.y
            y_pos =             itemPos.y *scaleY
            scene.focus =       true
            ma.focus =          true
            mainRoot_id.cursorPosition.hideCursor()
        }

        onReleased: {
            if(isDouble) {
                mainRoot_id.cursorPosition.onlyShowCursor()
                isDouble =      false
            } else {
                glb_pos =       mapToGlobal(knobe.x + knobe.width/2, knobe.y + knobe.height)
                saveXPos =      glb_pos.x
                saveYPos =      glb_pos.y
                mainRoot_id.cursorPosition.saveCursorPos(saveXPos, saveYPos)
                mainRoot_id.cursorPosition.showCursor()
            }

            undoCtrl_slider()
        }

        onMouseYChanged: {
            itemPos =           mapToItem(scene, mouse.x, mouse.y)

            if(isEdge) {
                isEdge =    false
                lastPos =   itemPos.y
            }

            diff =          itemPos.y - lastPos
            y_pos +=        diff

            y_pos =         bound(0, y_pos , scene.height * scaleY);
            knobe.y =       y_pos/scaleY - knobe.height/2
            pixToValue()

            lastPos =       itemPos.y

            glb_pos =       mapToGlobal(mouse.x, mouse.y)
            var vv =        100
            if((glb_pos.y <= vv ) && ((scene.value) < scene.max) ||
                    (glb_pos.y >= (Screen.height-vv)) && ((scene.value) > scene.min)) {
                mainRoot_id.cursorPosition.moveCursor((Screen.width-vv)/2 ,((Screen.height-vv)/2))
                isEdge = true
            }
        }// mouse in Y changed.
    }// mouse area.
    */
}





