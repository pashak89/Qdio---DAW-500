import QtQuick 2.12
import EventItem 1.0

EventItem {
    id:         eventItem
    width:      knobeDiameter + knobeBorder
    height:     200

    //------ save:
    property var obj:               null
    property bool is_ValueChanged:  false
    property real previousValue:    0
    property bool firstValueClick:  true
    property bool afterLoaded:      false

    mouseEventsEnabled:             true
    mouseHoverEventsEnabled:        false

    knobeDiameter:  20
    sliderWidth:    10

    valueColor:     'teal'
    emptyColor:     'white'
    brKnobeColor:   'black'
    flKnobeColor:   'yellow'

    property real defaultValue:    0


    Component.onCompleted: {
        var globalCoordinate =  gainSlider.mapToItem(waveEditor_root, 0, 0)
        eventItem.itemY =       mainRoot_id.y + globalCoordinate.y + knobeDiameter
        afterLoaded =           true
    }

    knobeBorder:    mainRoot_id.topWidth + 1
    radius:         2

    onIsReleaseChanged: if(isRelease) { mainRoot_id.cursorPosition.onlyShowCursor(); undoCtrl_slider() }
    onIsPressChanged:   if(isPress) mainRoot_id.cursorPosition.hideCursor()

    onValueChanged: {
        // undo:
        if(afterLoaded)
            is_ValueChanged = true
        if(value === defaultValue)
            firstValueClick = true
        // undo.
    }

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
}
