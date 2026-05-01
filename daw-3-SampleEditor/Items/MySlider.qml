import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Shapes 1.12
import QtGraphicalEffects 1.2
import QtQuick.Layouts 1.12


Rectangle {
    id: scene

    width: 20
    height: 156
    color: 'transparent'
    radius: 5

    //------ save:
    property var obj:               null
    property int i: 0
    property var values: []
    property bool is_ValueChanged: false
    property real previousValue: 0
    property bool firstValueClick: true
    property bool afterLoaded: false
    property bool isSubtitute: false

    property int y_pos:     0
    property double diff:      0

    property double max:    24*6
    property double min:    -70
    property double value: 0
    property double step:   0.01
    property double sense:  20

    property real defaultValue: 0


    property real saveXPos: 0
    property real saveYPos: 0

    property double range:  (max-min) / step
    property int scaleY: 1

    property double lastPos
    property double pressPos
    property var itemPos
    property var glb_pos

    property bool _isEdge: false
    property bool isIgnore: false

    property color valueColor:      'black'
    property color emptyColor:      'black'
    property color brKnobeColor:    'black'
    property color flKnobeColor:    'black'

    property real wHand:            1

    property real wLine:            1


    function pixToValue()
    {
        var result = (-max + min) * (y_pos/scaleY) / scene.height  +max
        value = result
        //        }
        //                console.log('value: '+value.toFixed(2))
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
        y_pos = scaleY* scene.height * (max-value)/(max-min)
        //        console.log('in Slider (scaleY) :-=-=-=->  '+ scaleY)

        knobe.y = y_pos/scaleY - knobe.height/2

        if(afterLoaded)
            is_ValueChanged = true
    }

    onHeightChanged: {
        scaleY = Math.round((range / scene.height) / 25)
        // console.log('on scaleY in height change: ' + scaleY)
        y_pos = scaleY* scene.height * (max-value)/(max-min)

        knobe.y = y_pos/scaleY - knobe.height/2
    }

    Component.onCompleted: {
        scaleY = Math.round((range / scene.height) / 25)
        //        console.log('scaleY in slider: ' + scaleY)
        value = 0
        y_pos = Math.round(max/(max-min) * scene.height  * scaleY)
        // pixToValue()
        afterLoaded = true
    }

    Rectangle{
        id: empty
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: wLine
        radius: scene.radius
        color: emptyColor
    }
    Rectangle{
        id: filler
        anchors.horizontalCenter: parent.horizontalCenter
        // the number '3' for remove a little top of knobe
        y:      knobe.y //+3
        height: empty.height - knobe.y //-3
        width: empty.width
        radius: empty.radius
        color: valueColor
    }

    Rectangle{
        id: knobe
        z: 1
        anchors.horizontalCenter: empty.horizontalCenter
        width: wHand
        height: width
        radius: width/2
        color: flKnobeColor
        border{
            color: brKnobeColor
            width: mainRoot_id.topWidth
        }
        //        y: y_pos/scaleY
        transformOrigin: Item.Center

        // Rectangle{
        //     id:er
        //     anchors.fill: parent
        //     anchors.margins: Math.round(parent.width/2.3)
        //     radius: width/2
        //     color: 'red'
        // }
    }


    property bool is2: false



    Keys.onPressed: {
        if (event.key === Qt.Key_Shift){
            var maxLastPos = scene.height * scaleY;
            scaleY *=sense;
            var maxPos = scene.height * scaleY;
            y_pos = y_pos * maxPos / maxLastPos;
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Shift){
            var maxLastPos = scene.height * scaleY;
            scaleY /=sense;
            var maxPos = scene.height * scaleY;
            y_pos = y_pos * maxPos / maxLastPos;
        }
    }

    property bool isHold: false
    MouseArea
    {
        id: ma
        anchors.fill:           parent
        anchors.rightMargin:    knobe.width/2
        anchors.leftMargin:     knobe.width/3
        anchors.topMargin:      -knobe.height/2
        anchors.bottomMargin:   -knobe.height/2

        onDoubleClicked: {
            itemPos = mapToItem(scene, mouse.x, mouse.y)
            pressPos = lastPos = itemPos.y;

            scene.focus = true
            ma.focus = true
            value =                defaultValue
            is2 = true
            y_pos = scaleY* scene.height * (max-value)/(max-min)
            knobe.y = y_pos/scaleY - knobe.height/2
            //tt.restart()
        }
        onPressed: {

            //            if(!is2){
            //            console.log('ih')
            itemPos = mapToItem(scene, mouse.x, mouse.y)
            pressPos = lastPos = itemPos.y;

            y_pos = itemPos.y *scaleY
            scene.focus = true
            ma.focus = true

            y_pos = bound( 0, y_pos , scene.height * scaleY);

            pixToValue()
//            console.log('y pos: '+y_pos)
            is2 = true
            mainRoot_id.cursorPosition.hideCursor()
        }

        onPressAndHold: isHold = true

        onReleased: {

            if(!is2){
//                console.log('jijij')
                //            {
                // -- set position of cursor:
                var qglb_pos =    mapToGlobal(knobe.x, knobe.y + knobe.width)

                //            var qglb_pos =    mapFromItem(knobe, knobe.x + knobe.width/2,
                //                                            knobe.y - knobe.width*.5)
                //            qglb_pos = mapToGlobal(qglb_pos.x, qglb_pos.y)
                saveXPos =   qglb_pos.x
                saveYPos =   qglb_pos.y
                mainRoot_id.cursorPosition.saveCursorPos(saveXPos, saveYPos)
                mainRoot_id.cursorPosition.showCursor()

            }
            mainRoot_id.cursorPosition.onlyShowCursor()
            is2 = false
            //undo:
            if(obj.is_ValueChanged){

                if(obj.firstValueClick){
                    undoManager.actionsSave.push({object: obj, name: obj.objectName ,
                                                     it: obj.i, value: obj.values.push(obj.previousValue), type: 'slider'})
                    undoManager.saveIndex++
                    obj.firstValueClick = false
                }

                if(obj.isSubtitute){
                    i++
                    obj.values[i] = obj.value
                    undoManager.actionsSave[undoManager.saveIndex] =
                            ({object: obj, name: obj.objectName, it: obj.i, value: obj.value, type: 'slider'})
                    obj.isSubtitute = false
                }else{
                    obj.i++
                    undoManager.actionsSave.push({object: obj, name: obj.objectName ,
                                                     it: obj.i, value: obj.values.push(obj.value), type: 'slider'})
                }
                undoManager.saveIndex++
                obj.is_ValueChanged = false
            }// undo.

        }

        onMouseYChanged: {
//console.log('-----------------------------------')
            itemPos = mapToItem(scene, mouse.x, mouse.y)

            if(_isEdge){
                _isEdge =           false
                lastPos = itemPos.y
            }

            var dd = itemPos.y - lastPos
            y_pos += dd

            y_pos = bound( 0, y_pos , scene.height * scaleY);

            pixToValue()
            lastPos = itemPos.y
            is2 = false
            knobe.y = y_pos/scaleY - knobe.height/2
            glb_pos =   mapToGlobal(mouse.x, mouse.y)
            if( glb_pos.y > (mainRoot_id.y + mainRoot_id.height) || glb_pos.y < mainRoot_id.y){
                mainRoot_id.cursorPosition.moveCursor(mainRoot_id.x + mainRoot_id.width/2,
                                                      mainRoot_id.y + mainRoot_id.height/2)
                _isEdge = true
            }// reset pos cursor in Y.
        }//         mouse y changed.
        onMouseXChanged: {
            glb_pos =   mapToGlobal(mouse.x, mouse.y)
            if( glb_pos.x > (mainRoot_id.x + mainRoot_id.width) || glb_pos.x < mainRoot_id.x){
                mainRoot_id.cursorPosition.moveCursor(mainRoot_id.x + mainRoot_id.width/2,
                                                          mainRoot_id.y + mainRoot_id.height/2)
            }// reset pos cursor in X.
        }//   mouse x changed.

    }



}





