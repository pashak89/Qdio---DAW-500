import QtQuick 2.12

Rectangle {
    id: slider

    property bool withColor: false

    property int minimumValue: 0

    property int stepSize: scaleSize2(65)

    property int main: 1

    property var btn

    property int minimumSize: 0

    property bool isMaster: false
    property bool isReverb: false

    property bool allowDragging: false

    signal sigSelected
    width: 30
    height: 500

    property alias yIndicator: indicator.y

    color: withColor ? 'orange' : 'transparent'

    onFocusChanged: didLight = focus

    Rectangle {
        id: snapper
        width: 50
        height: minimumSize /*+ value*/
        color: withColor ? 'teal' : 'transparent'
    }

    Rectangle {
        id: indicator
        y: stepSize //+ snapper.height
        height: 10
        width: parent.width - 2
        color: withColor ? 'red' : 'transparent'
    }

    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property bool allow2clear: false

        property int _index: 0

        hoverEnabled: true
        onEntered: {
            _clipArea.setOverrideCursor(Qt.SizeVerCursor)
        }
        onExited: {

            _clipArea.setOverrideCursor(Qt.ArrowCursor)
        }

        onClicked: {
            if (groupTrackLight.length === 1 & !holdCtrl & !holdShift & allow2clear) {
                groupTrackLight.length = 0
                offLightAllTracks()
            }
        }

        onPressed: {

            _index = _clipArea.findTrackResizing(mapToItem(clipArea, mouseY,
                                                          mouseY).y)

            slider.focus = true
            if (!isMaster && !isReverb) {

                if (mouse.button === Qt.RightButton)
                    trackLighting_rightclick(index, btn)
                else
                    sigSelected()
            } else if (isReverb)
                trackReturnLighting(ind_return, btn)
            else if (isMaster)
                trackMasterLighting(0, btn)

            if (mouse.button === Qt.RightButton) {
                positionInWindow = mapToItem(track_list.contentItem,
                                             mouse.x, mouse.y)
                trackRightClick.numberItem = index
                trackRightClick.visble_grouper = allow2MakeGroup()
                trackRightClick.visble_ungrouper = groupTrackLight.length === 1
                        & trackAudioModel.get(
                            groupTrackLight[0]).type === 'Group'
                trackRightClick.visible = true

                if (rightPanel.visible)
                    rightPanel.visible = false
            } else
                trackRightClick.visible = false
        }

        drag {
            target: indicator
            axis: Drag.YAxis
            minimumY: 0
            maximumY: slider.height - indicator.height
        }

        onPositionChanged: {
            if (pressed) {
                _clipArea.setOverrideCursor(Qt.SizeVerCursor)
                _clipArea.setTrackHeight(_index, mapToItem(clipArea, mouseY,
                                                          mouseY).y)
            }
        }

        onReleased: allowDragging = false
    }
}
