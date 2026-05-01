import QtQuick 2.12

Rectangle {
    id: seperator
    y: (rowsExpanded + 1) * (scaleSize2(37) + scaleSize2(15)) + scaleSize2(10) - height
    // width:  automationChildren > 0 & showAutomation ? rootMainTrack.width + scaleSize2(10) : parent.width
    height: scaleSize2(5)
    color:  '#2f3032'

    // onYChanged: console.log('y: ',y, (y-scaleSize2(10)+height)/(scaleSize2(37) + scaleSize2(15)))

    MouseArea {
        anchors.fill: parent
        drag.axis: Drag.YAxis
        drag.minimumY: scaleSize2(37) + scaleSize2(10) + scaleSize2(15)
        drag.target: parent

        hoverEnabled: true

        onEntered: cursorPositionClass.vCursorShape()
        onExited: if(!pressed) cursorPositionClass.onlyShowCursor()
        onPressed: cursorPositionClass.vCursorShape()
        onReleased: cursorPositionClass.onlyShowCursor()

        onMouseYChanged: {
            if(pressed) {
                parent.y = Math.round((parent.y - scaleSize2(10)) / (scaleSize2(37) + scaleSize2(15))) * (scaleSize2(37) + scaleSize2(15)) +
                          scaleSize2(10) - height

                var _i = Math.round((seperator.y - scaleSize2(10)) / (scaleSize2(37) + scaleSize2(15))) - 1
                _rowsExpanded = _i
                if(_i !== rowsExpanded)
                    rowFunc(_i)
            }
        }
    }


    Rectangle {
        id: draggingRec
        visible:        inDraggingCounter > -1 & automationChildren < 1
        anchors.fill:   parent
        color:          inDraggingCounter > -1 ? theme.betweenRec_dragging_o20 : 'transparent'
    }


    states: [State {
            name: "Right"
            when: !globalValues.tracksOnLeft

            PropertyChanges {
                target: draggingRec
                anchors.rightMargin: type === "Group" ? (in_set_parent.count - inDraggingCounter+1) * scaleSize2(50) :
                                                        (in_set_parent.count - inDraggingCounter) * scaleSize2(50)
                anchors.leftMargin: 0
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            PropertyChanges {
                target: draggingRec
                anchors.rightMargin: 0
                anchors.leftMargin: type === "Group" ? (in_set_parent.count - inDraggingCounter+1) * scaleSize2(50) :
                                                       (in_set_parent.count - inDraggingCounter) * scaleSize2(50)
            }
        }]
}
