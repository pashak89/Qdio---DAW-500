import QtQuick 2.12
import ClipArea 1.0
Rectangle {
    id: seperator
    z: 1002
    width: automationLaneCount > 0 & automationLaneEnabled ? rootMainTrack.width + scaleSize2(
                                                                 10) : parent.width
    height: clipArea.seperatorHeight
    color: '#2f3032'



    Rectangle {
        id: draggingRec
        visible: inDraggingCounter > -1 & automationLaneCount < 1
        anchors.fill: parent
        color: inDraggingCounter > -1 ? theme.betweenRec_dragging_o20 : 'transparent'
    }

    states: [
        State {
            name: "Right"
            when: !globalValues.tracksOnLeft

            PropertyChanges {
                target: draggingRec
                anchors.rightMargin: type === "Group" ? (in_set_parent.count
                                                         - inDraggingCounter + 1) * scaleSize2(
                                                            50) : (in_set_parent.count - inDraggingCounter) * scaleSize2(
                                                            50)
                anchors.leftMargin: 0
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            PropertyChanges {
                target: draggingRec
                anchors.rightMargin: 0
                anchors.leftMargin: type === "Group" ? (in_set_parent.count
                                                        - inDraggingCounter + 1) * scaleSize2(
                                                           50) : (in_set_parent.count
                                                                  - inDraggingCounter) * scaleSize2(
                                                           50)
            }
        }
    ]
}
