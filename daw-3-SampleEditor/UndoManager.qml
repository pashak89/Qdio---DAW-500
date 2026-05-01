import QtQuick 2.12

Item {
    id: root
    property var actionsSave:   []
    property int saveIndex:     -1
    property int currentIndex:  -1

    property var allSaves:      []

    /*------------------ ports
     * - object:
     *
     * - name:
     *
     * - value:
     *
     * - type:
    */

    function setValueDropMenu(obj) {
        var objValue = actionsSave[saveIndex]
        obj.choose.name = obj.items[objValue.value]
    }
    function setValueBtn(obj) {
        var objValue = actionsSave[saveIndex]
        obj.isClicked = objValue.value
    }
    function setValue(obj) {
        var objValue = actionsSave[saveIndex]
        obj.value = objValue.value
    }

    function checkType(type) {
        var obj = actionsSave[saveIndex].object

//        for(var i = 0; i<actionsSave.length; i++) {
//            actionsSave[i].object.focus = false
//        }
//        obj.focus = true
        // focus.

        //--- BUTTON:
        if(type === "button")
            setValueBtn(obj)

        //--- TEXT FIELD:
        else if(type === "text field")
            setValue(obj)

        //--- SLIDER:
        else if(type === "slider")
            setValue(obj)

        //--- DIAL:
        else if(type === "dial")
            setValue(obj)

        //--- COMBO BOX:
        else if(type === "combo box")
           setValueDropMenu(obj)

        //--- RIGHT CLICK:
        else if(type === "right click")
            setValue(obj)

        else
            console.log('NOT in types!!')
    }//--- end function

    Item {
        id: undoManager

        // top:
        Shortcut {
            sequence: StandardKey.Redo

            onActivated:
            {
                if(saveIndex < actionsSave.length-1)
                {
                    saveIndex++; checkType(actionsSave[saveIndex].type)
                }
            }
        }// up.

        // down:
        Shortcut {
            sequence: StandardKey.Undo

            onActivated:
            {
                if(saveIndex > 0)
                {
                    saveIndex--; checkType(actionsSave[saveIndex].type)
                }
            }
        }// down.
    }//-- undoManager


    onSaveIndexChanged: {
        console.log('undo => saveIndex: ' + saveIndex + ', name: ' + actionsSave[saveIndex].name +
                    ', value: '+ actionsSave[saveIndex].value +  ', type: ' + actionsSave[saveIndex].type)
    }
}
