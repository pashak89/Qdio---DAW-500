import QtQuick 2.12

import ClipArea 1.0
import ClipShapeItem 1.0
import TrackShapeItem 1.0
import RubberBandItem 1.0
import IndicatorShapeItem 1.0
import AutomationLaneItem 1.0
import AutomationShapeItem 1.0
import "."
import "qrc:/"
import "qrc:/Items"

Rectangle {
    id: root

    color: "transparent"

    property ClipArea clipArea
    property int ind: index

    height: totalTrackRect.height
    width: parent.width

    clip: true
    Rectangle {
        color: selectedTrack ? theme.bg1 : theme.bg2
        anchors.left: parent.left
        anchors.top: parent.top
        height: parent.height
        width: _width
    }

    MouseArea {
        id: ma0
        propagateComposedEvents: true
        enabled: false
        anchors.bottom: parent.bottom
        anchors.top: parent.top
    }

    signal automationText(string automateText)
    onAutomationText: automateDisplay = automateText
    property string automateDisplay: ''

    // ----------------------------------------------------------------------------------------------------------------------------------  properties:
    property bool splitStereo: false

    property bool didGradient: selectedTrack //| automationBtn.didShadow | viewBtn.didShadow | recordBtn.didShadow | sendBtn.didShadow

    property bool didFold: !rowsExpanded

    property alias draggableSize: collapsPanel.width

    property bool allow2clear: false

    // Automation
    // property alias automationRun: automationBtn.isClicked
    property bool inAutomationMenu: false
    property bool inAutomationChild: false

    property int _rowsExpanded: -1

    property bool inTrack: false

    property int channels: model.channelCount
    property var truePeakModel: model.truePeakLevel
    // ---------------------------------------------------------------------------------------------------------------------  properties.
    Collapse {
        id: collapsPanel
        anchors.bottom: parent.bottom
        height: totalTrackRect.height
        anchors.top: parent.top
    }
    function callback(index, text) {

        if (text === "Delete" && index >= 0) {
            clipArea.removeSampleClip(index)
        }
    }

    Rectangle {
        id: rootMainTrack

        color: "transparent"
        height: trackRect.height
        width: parent.width
        anchors.left: parent.left

        anchors.leftMargin: collapsPanel.width + scaleSize2(10)
        MouseArea {
            id: maRoot
            anchors.fill: parent
            anchors.topMargin: row1.height
            anchors.bottomMargin: scaleSize2(5) + 1
            propagateComposedEvents: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            // property bool allow2clear: false
            onClicked: {
                if (groupTrackLight.length === 1 & !holdCtrl & !holdShift & allow2clear) {
                    groupTrackLight.length = 0
                    funcs.offLightAllTracks()
                }
            }

            onPressed: {

                parent.focus = true
                root.focus = true

                if (mouse.button === Qt.RightButton) {

                    //funcs.trackLighting_rightclick(ind, 'btn')
                } else {

                    model.selectedTrack = true
                }

                if (mouse.button === Qt.RightButton) {
                    positionInWindow = Qt.point(mouse.x, mouse.y)

                    trackRightClick.numberItem = ind
                    trackRightClick.visble_grouper = funcs.allow2MakeGroup()
                    trackRightClick.visible = true
                    trackRightClick.parent = rootMainTrack
                    trackRightClick.callback = root.callback
                    trackRightClick.index = ind

                    if (rightPanel.visible)
                        rightPanel.visible = false
                } else {
                    trackRightClick.visible = false
                }
            }
        }

        Row1 {
            id: row1
            anchors.top: parent.top
            anchors.topMargin: scaleSize2(10)
            width: parent.width
        }

        Row2 {
            id: row2

            width: parent.width
            anchors.top: row1.bottom
            anchors.topMargin: scaleSize2(15)
        }

        AutomationRow {
            id: automationRow

            height: clipArea.minStepSize
            anchors.top: row2.bottom
            anchors.topMargin: scaleSize2(15)
        }
    }

    ClipShapeItem {
        id: clipItem
        trackIndex: ind
        clipArea: _clipArea
        areaInfo: _areaInfo
        anchors.left: parent.left
        anchors.leftMargin: _width + scaleSize2(10)

        anchors.top: root.top
        anchors.right: root.right
        anchors.bottom: root.bottom
        height: totalTrackRect.height
        width: root.width - _width - scaleSize2(10)

        layer.enabled: true
        layer.samples: 8
    }

    IndicatorShapeItem {
        id: indicator
        trackIndex: ind
        clipArea: _clipArea
        areaInfo: _areaInfo
        anchors.left: parent.left
        anchors.leftMargin: _width + scaleSize2(10)

        anchors.top: root.top
        anchors.right: root.right
        anchors.bottom: root.bottom
        height: totalTrackRect.height
        width: root.width - _width - scaleSize2(10)
    }

    AutomationLaneItem {
        trackIndex: ind
        clipArea: _clipArea
        areaInfo: _areaInfo
        anchors.left: parent.left
        anchors.leftMargin: _width + scaleSize2(10)

        anchors.top: root.top
        anchors.right: root.right
        anchors.bottom: root.bottom
        height: totalTrackRect.height
        width: root.width - _width - scaleSize2(10)
    }

    AutomationShapeItem {

        trackIndex: ind
        clipArea: _clipArea
        areaInfo: _areaInfo
        anchors.left: parent.left
        anchors.leftMargin: _width + scaleSize2(10)

        anchors.top: root.top
        anchors.right: root.right
        anchors.bottom: root.bottom
        height: totalTrackRect.height
        width: root.width - _width - scaleSize2(10)
        z: clipItem.z + 1

        layer.enabled: true
        layer.samples: 8
    }

    TrackShapeItem {
        id: trackShapeItem

        trackIndex: ind
        clipArea: _clipArea
        areaInfo: _areaInfo

        anchors.left: root.left
        anchors.leftMargin: _width + scaleSize2(10)

        anchors.top: root.top
        anchors.right: root.right
        anchors.bottom: root.bottom
        height: totalTrackRect.height
        width: root.width - _width - scaleSize2(10)
    }

    Seperator {
        id: seperator

        y: rootMainTrack.height + rootMainTrack.y - clipArea.seperatorHeight

        anchors.leftMargin: model.automationLaneCount > 0 & model.automationLaneEnabled
                            & rowsExpanded ? -scaleSize2(10) : 0
        anchors.right: parent.right
        anchors.left: model.automationLaneCount > 0 & model.automationLaneEnabled
                      & rowsExpanded ? rootMainTrack.left : parent.left

        MouseArea {
            id: mouseArea
            anchors.left: parent.left
            anchors.top: parent.top
            width: _width

            property real startGlobalY: 0
            property real accumulatedDelta: 0
            property real startHeight: 0

            acceptedButtons: Qt.LeftButton | Qt.RightButton
            hoverEnabled: true

            onEntered: {
                clipArea.setOverrideCursor(Qt.SizeVerCursor)
            }
            onExited: {
                clipArea.setOverrideCursor(Qt.ArrowCursor)
            }

            onReleased: {
                clipArea.setOverrideCursor(Qt.ArrowCursor)
            }
            onPressed: {
                startGlobalY = mapToGlobal(0, mouseY).y
            }

            onPositionChanged: {

                if (pressed) {
                    clipArea.setOverrideCursor(Qt.SizeVerCursor)
                    var currentY = mapToGlobal(0, mouseY).y
                    var delta = currentY - startGlobalY
                    if (Math.abs(delta) >= _areaInfo.minStepSize) {
                        startGlobalY = mapToGlobal(0, mouseY).y
                        clipArea.setTrackHeight(index, delta)
                    }
                }
            }
        }
    }
    AutomationListView {
        id: automationListView
        anchors.top: seperator.bottom
        anchors.bottom: parent.bottom
        anchors.left: rootMainTrack.left
        width: parent.width
        visible: rowsExpanded
    }
    Channels {
        id: channelRows
        height: parent.height
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: scaleSize2(5)
        anchors.left: parent.left
        anchors.leftMargin: _width - channelRows.width
        z: seperator.z + 1
    }

    Rectangle {
        visible: coverBound
        anchors.fill: parent

        color: 'transparent'
        border.color: theme.betweenRec_dragging_o20
        border.width: scaleSize2(5)
    }

    // -------------------------------------------------------------------------------------------------------------------  Function:
    function changeRowCounter(num) {
        seperator.y = scaleSize2(37) * num + scaleSize2(15) * num + scaleSize2(
                    10) - seperator.height

        var _i = Math.round((seperator.y - scaleSize2(
                                 10)) / (scaleSize2(37) + scaleSize2(15))) - 1
        if (_i !== rowsExpanded)
            rowFunc(_i)
    }

    // onRowsExpandedChanged: {//console.log('----------------0-----------------------> ', rowsExpanded, spanVert.main, trackAudioModel.get(ind).rowsExpanded)
    //    onRowCounterChanged: {//console.log('\u001b[1;32m row counter  track ', ind, 'is changed to ' , rowCounter ,'\u001b[1;35m')
    function rowFunc(rowCounter) {

        //trackAudioModel.setProperty(index, 'rowsExpanded', rowCounter)
        var arr = []
        if (type === 'Group' & trackAudioModel.get(ind).rowsExpanded === 1) {
            trackAudioModel.setProperty(ind, 'imgURL',
                                        'qrc:/Resource/folder_off_icon.svg')

            for (var i = 1; i <= trackAudioModel.get(
                     ind).sub_set_children.count; i++) {
                if (trackAudioModel.get(ind + i).type === 'Group') {
                    trackAudioModel.setProperty(ind + i, 'isCollapsed', true)
                }

                arr.length = 0
                if (trackAudioModel.get(ind + i).type === 'audio') {
                    for (var j = 0; j < trackAudioModel.get(
                             ind + i).in_set_parent.count; j++) {
                        for (var k = 0; k < trackAudioModel.count; k++) {
                            if (trackAudioModel.get(ind + i).in_set_parent.get(
                                        j).member === trackAudioModel.get(
                                        k).pretext) {
                                if (trackAudioModel.get(k).rowsExpanded > 0)
                                    arr.push(true)
                                else
                                    arr.push(false)
                            }
                        }
                    }
                }

                if (arr.indexOf(false) === -1)
                    trackAudioModel.setProperty(ind + i, 'isCollapsed', true)
            } // main for.
            trackListView.positionViewAtIndex(ind, ListView.Visible)

            funcs.offLightAllTracks()
        }

        if (rowCounter === 0)
            automationLaneEnabled = false
        else
            automationLaneEnabled = true
        // console.log('----------------1-----------------------> ', rowsExpanded, spanVert.main, trackAudioModel.get(ind).rowsExpanded)
    }

    function getRandomInt(max) {
        return Math.floor(Math.random() * max)
    }

    function change_rowExpanding() {
        model.rowsExpanded = !model.rowsExpanded
    }
}
