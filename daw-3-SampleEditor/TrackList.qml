import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import "qrc:/TrackPanels/Audio"
import "qrc:/TrackPanels"
import "qrc:/Items"
import Tools 1.0
import TrackItem 1.0

Rectangle {
    id: track_list
    visible: true
    color: "transparent"
    clip: true
    property var cursorPosition: cursorPositionClass

    property int _width: scaleSize2(600)
    property int minWidth: scaleSize2(600)
    property string topFontFamily: myFont2.name
    property int topfontPixelSize: scaleSize2(28)
    property real topRadius: radiusCal(3)
    property int topWidth: resolution[0] >= 3840 & resolution[1]
                           >= 2170 ? ((refSize < .83) ? 1 : (refSize < 1.12) ? 2 : 2) : ((refSize < .83) ? 1 : (refSize < 1.12) ? 1 : 2)

    property var resolution: cursorPosition.getResolution()

    property alias trackListView: trackListView

    property real refSize: _areaInfo.zoomFactor
    property real _scale: cursorPosition.getScale()

    Theme1 {
        id: theme1
    }
    Theme2 {
        id: theme2
    }
    property Item theme: theme1

    FontLoader {
        id: myFont
        source: 'qrc:/Resource/font/Open_Sans/OpenSans-VariableFont.ttf'
    }
    FontLoader {
        id: myFont2
        source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf'
    }

    function radiusCal(value) {
        value = value * _scale * refSize
        return Math.round(value)
    }
    function scaleSize2(value) {
        value = Math.round(value * _scale * refSize / 2)
        return value
    }
    function oddSize(value) {
        value = Math.round(value)
        if (value % 2 !== 0)
            return value
        else
            return value - 1
    }
    function evenSize(value) {
        value = Math.round(value)
        if (value % 2 === 0)
            return value
        else
            return value - 1
    }

    property variant groupTrackArray: []
    property variant groupTrackLight: []
    property int group_width: 0

    FunctionTrackAudio {
        id: funcs
    }

    signal globalPositionTrackChanged(var item, var position)

    property var globalPosition
    property var positionInWindow

    /* track UI: */
    property int firstNum: -1
    property int secondNum: -1
    property int firstLightTrackAudioOnShift: -1
    property int secondLightTrackAudioOnShift: -1

    property int firstLightTrackReturnOnShift: -1
    property int secondLightTrackReturnOnShift: -1

    property bool holdShift: false
    property bool holdCtrl: false
    property bool solo_on_return: false

    HeaderTracks {
        id: header
        width: _width
        anchors.top: parent.top
        anchors.left: parent.left
    }

    Rectangle {
        id: seperator_first
        width: parent.width
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: _clipArea.seperatorHeight
        color: '#2f3032'
        z: 1003
    }

    property int delay2UpAutoScroll: 1
    property int delay2DownAutoScroll: 1
    property int requireSizeForAutoScroll: 35

    Component {
        id: returnTrackComponent
        Rectangle {
            id: returnTrack
            // visible: trackListButton.returnIsOn
            // enabled: trackListButton.returnIsOn
            Layout.fillWidth: true
            Layout.preferredHeight: scaleSize2(18) + scaleSize2(
                                        37) + scaleSize2(56) * rowsExpanded
            color: /*on*/ trackReturn.parentFocus ? theme.bg1 : theme.bg2

            clip: true

            property int ind_return: index
            property bool on: trackReturnModel.get(ind_return).light

            TrackReturn {
                id: trackReturn
                //                onLeftSide: _2Left
                numberAudio: returnTrack.ind_return
                didSelectSolo: trackReturnModel.get(ind_return).selectSolo
                didSelectTrack: trackReturnModel.get(ind_return).selectTrack
                onTrackSelectPressed: {
                    if (trackReturnModel.get(ind_return).selectTrackDependSolo)
                        trackReturnModel.setProperty(ind_return,
                                                     "selectTrackDependSolo",
                                                     false)
                    else
                        trackReturnModel.setProperty(ind_return,
                                                     "selectTrackDependSolo",
                                                     true)
                }
            }
        }
    }

    Component {
        id: masterTrackComponent
        TrackMaster {
            id: trackMaster

            color: /*on*/ !trackMaster.parentFocus ? theme.bg1 : theme.bg2

            clip: true

            property bool on: false
        }
    }

    MouseArea {
        id: mouseArea2
        anchors.top: trackListView.top
        anchors.left: trackListView.left
        anchors.bottom: parent.bottom
        anchors.right: trackListView.right
        anchors.leftMargin: leftMargin
        hoverEnabled: false
        propagateComposedEvents: true
        property bool startByMasterTrack: false
        property int leftMargin: _width + scaleSize2(10)
        property real logicalX: 0
        property real logicalY: 0

        // Where the press happened in *global* coords
        property point pressGlobalPos: Qt.point(0, 0)

        property bool ignoreNextMove: false

        property bool automationPressed: false

        property Item pressedItem

        z: recCols.z + 1
        onWheel: {
            if (wheel.angleDelta.y < 0) {
                trackListView.contentY += _areaInfo.minStepSize
            } else {
                trackListView.contentY -= _areaInfo.minStepSize
            }
        }
        onReleased: {
            mouse.accepted = false

            let localAreaPoint = trackListView.mapFromGlobal(logicalX, logicalY)

            let index = trackListView.indexAt(localAreaPoint.x,
                                              localAreaPoint.y)
            var item = trackListView.itemAtIndex(index)

            if (automationPressed) {
                item = pressedItem
            }
            automationPressed = false

            if (item) {
                let localPoint = Qt.point(
                        localAreaPoint.x - item.x - leftMargin,
                        localAreaPoint.y - item.y)
                _clipArea.handleMouseReleased(
                            localPoint, Qt.point(localAreaPoint.x - leftMargin,
                                                 localAreaPoint.y),
                            mouse.button)

                _areaInfo.updateUI()
            } else {
                const item = trackListView.itemAtIndex(trackListView.count - 1)
                if (item) {
                    let localPoint = Qt.point(
                            localAreaPoint.x - item.x - leftMargin,
                            localAreaPoint.y - item.y)
                    _clipArea.handleMouseReleased(
                                localPoint,
                                Qt.point(localAreaPoint.x - leftMargin,
                                         localAreaPoint.y), mouse.button)
                    _areaInfo.updateUI()
                }
            }

            //            if (automationPressed) {
            //                _clipArea.setCursorPosition(logicalX, logicalY)
            //            }
        }

        onPositionChanged: {

            mouse.accepted = false

            if (automationPressed) {
                if (ignoreNextMove) {
                    ignoreNextMove = false
                    return
                }

                let g = _clipArea.globalPos()
                let dx = g.x - pressGlobalPos.x
                let dy = g.y - pressGlobalPos.y

                logicalX += dx
                logicalY += dy
            } else {
                let g = _clipArea.globalPos()
                logicalX = g.x
                logicalY = g.y
            }

            let masterTrack = false

            if (automationPressed == false) {

                let localAreaPoint = track_list.mapFromGlobal(logicalX,
                                                              logicalY)
                const hoveredItem = track_list.childAt(localAreaPoint.x,
                                                       localAreaPoint.y)

                // Mouse in recCols coords
                const posInRec = recCols.mapFromGlobal(logicalX, logicalY)

                // Strictly over recCols
                const overRec = hoveredItem === recCols

                // Below recCols (same X band, Y beyond its bottom)
                const withinXBand = posInRec.x >= 0
                                  && posInRec.x <= recCols.width
                const underRec = withinXBand && posInRec.y > recCols.height

                // If you prefer "over OR below" to be considered master track:
                if (overRec || underRec) {
                    _clipArea.handleMasterTrackMouseMoved(
                                Qt.point(posInRec.x - scaleSize2(10) - _width,
                                         posInRec.y), mouse.button)
                    _areaInfo.updateUI()
                    masterTrack = true
                } else {
                    if (!startByMasterTrack) {
                        _clipArea.clearMasterTrackSelection()
                        _areaInfo.updateUI()
                    } else {
                        _clipArea.handleMasterTrackMouseMoved(
                                    Qt.point(posInRec.x - scaleSize2(
                                                 10) - _width, posInRec.y),
                                    mouse.button)
                    }
                }
            }

            let localAreaPoint = trackListView.mapFromGlobal(logicalX, logicalY)

            let index = trackListView.indexAt(localAreaPoint.x,
                                              localAreaPoint.y)
            var item = trackListView.itemAtIndex(index)

            if (automationPressed) {
                item = pressedItem
            }

            if (item) {

                if (pressed) {
                    let localPoint = Qt.point(
                            localAreaPoint.x - item.x - leftMargin,
                            localAreaPoint.y - item.y)
                    _clipArea.handleMouseMoved(
                                localPoint,
                                Qt.point(localAreaPoint.x - leftMargin,
                                         localAreaPoint.y), mouse.button)
                } else {
                    let localPoint = Qt.point(
                            localAreaPoint.x - item.x - leftMargin,
                            localAreaPoint.y - item.y)
                    _clipArea.handleHoverMoved(
                                localPoint,
                                Qt.point(localAreaPoint.x - leftMargin,
                                         localAreaPoint.y), mouse.button)
                }
            } else {

                item = trackListView.itemAtIndex(trackListView.count - 1)
                if (startByMasterTrack && masterTrack) {
                    _clipArea.clearSelection()
                    return
                }

                if (item) {
                    let localPoint = Qt.point(
                            localAreaPoint.x - item.x - leftMargin,
                            localAreaPoint.y - item.y)

                    if (pressed) {
                        _clipArea.handleMouseMoved(
                                    localPoint,
                                    Qt.point(localAreaPoint.x - leftMargin,
                                             localAreaPoint.y), mouse.button)
                    } else {
                        _clipArea.handleHoverMoved(
                                    localPoint,
                                    Qt.point(localAreaPoint.x - leftMargin,
                                             localAreaPoint.y), mouse.button)
                    }
                    _areaInfo.updateUI()
                }
            }
            if (automationPressed) {
                ignoreNextMove = true
                _clipArea.setCursorPosition(pressGlobalPos.x, pressGlobalPos.y)
            }
        }

        onPressed: {

            mouse.accepted = true
            automationPressed = false
            var g = _clipArea.globalPos()
            pressGlobalPos = Qt.point(g.x, g.y)

            logicalX = g.x
            logicalY = g.y

            let localAreaPoint = trackListView.mapFromGlobal(logicalX, logicalY)

            const index = trackListView.indexAt(localAreaPoint.x,
                                                localAreaPoint.y)
            var item = trackListView.itemAtIndex(index)

            startByMasterTrack = false

            {

                let localAreaPoint = track_list.mapFromGlobal(logicalX,
                                                              logicalY)
                const item1 = track_list.childAt(localAreaPoint.x,
                                                 localAreaPoint.y)

                if (item1 === recCols) {
                    _clipArea.clearMasterTrackSelection()
                    startByMasterTrack = true
                }
            }

            if (item) {
                let localPoint = Qt.point(
                        localAreaPoint.x - item.x - leftMargin,
                        localAreaPoint.y - item.y)
                automationPressed = _clipArea.handleMousePressed(
                            localPoint, Qt.point(localAreaPoint.x - leftMargin,
                                                 localAreaPoint.y),
                            mouse.button)
                if (automationPressed) {
                    pressedItem = item
                }

                _areaInfo.updateUI()
            } else {
                const item = trackListView.itemAtIndex(trackListView.count - 1)
                if (item) {
                    let localPoint = Qt.point(
                            localAreaPoint.x - item.x - leftMargin,
                            localAreaPoint.y - item.y)
                    _clipArea.handleMousePressed(
                                localPoint,
                                Qt.point(localAreaPoint.x - leftMargin,
                                         localAreaPoint.y), mouse.button)

                    _areaInfo.updateUI()
                }
            }
        }
    }

    MouseArea {

        anchors.top: trackListView.top
        anchors.left: trackListView.left
        anchors.bottom: trackListView.bottom
        width: _width

        hoverEnabled: false
        propagateComposedEvents: true
        z: trackListView.z + 1
        acceptedButtons: Qt.NoButton
        onWheel: {
            if (wheel.angleDelta.y < 0) {
                trackListView.contentY += _areaInfo.minStepSize
            } else {
                trackListView.contentY -= _areaInfo.minStepSize
            }
        }
    }

    ListView {
        id: trackListView

        anchors.top: seperator_first.bottom
        height: _clipArea.trackListStartHeight
        width: parent.width
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: recCols.top

        clip: true

        spacing: 0
        cacheBuffer: 1000

        flickableDirection: Flickable.VerticalFlick
        interactive: false
        boundsMovement: Flickable.StopAtBounds
        boundsBehavior: Flickable.StopAtBounds
        maximumFlickVelocity: 500
        property int upIndexItem: indexAt(2, contentY)
        property int downIndexItem: indexAt(2, contentY + height)
        onContentYChanged: {
            delay2UpAutoScroll = 1
            delay2DownAutoScroll = 1
            _areaInfo.startContentY = trackListView.contentY
        }

        onUpIndexItemChanged: delay2UpAutoScroll = 35
        onDownIndexItemChanged: delay2DownAutoScroll = 35

        model: _clipArea.tracksModel
        delegate: TrackAudio {
            clipArea: _clipArea
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            anchors.topMargin: 3
            anchors.bottomMargin: 3
            anchors.rightMargin: 3
            width: 6
            position: trackListView.contentHeight > 0
                      ? trackListView.contentY / trackListView.contentHeight : 0
            size: trackListView.contentHeight > 0
                  ? trackListView.height / trackListView.contentHeight : 1
            contentItem: Rectangle {
                implicitWidth: 6
                radius: width / 2
                color: track_list.theme.bg7
                opacity: trackListView.contentHeight > trackListView.height ? 0.75 : 0.0
            }
            background: Rectangle { color: "transparent" }
        }
    }

    /*****/
    Rectangle {
        id: recCols
        z: trackListView.z + 1
        width: parent.width
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: trackMaster.height + trackMasterSeperator.height

        Rectangle {
            id: trackMasterSeperator
            width: parent.width
            anchors.top: parent.top
            height: _clipArea.seperatorHeight
            color: '#2f3032'

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                property real startGlobalY: 0
                property real accumulatedDelta: 0
                property real startHeight: 0

                acceptedButtons: Qt.LeftButton | Qt.RightButton
                hoverEnabled: true

                onEntered: {
                    _clipArea.setOverrideCursor(Qt.SizeVerCursor)
                }
                onExited: {
                    _clipArea.setOverrideCursor(Qt.ArrowCursor)
                }

                onReleased: {
                    _clipArea.setOverrideCursor(Qt.ArrowCursor)
                }
                onPressed: {
                    startGlobalY = mapToGlobal(0, mouseY).y
                }

                onPositionChanged: {

                    if (pressed) {
                        _clipArea.setOverrideCursor(Qt.SizeVerCursor)

                        var currentY = mapToGlobal(0, mouseY).y
                        var delta = currentY - startGlobalY
                        if (Math.abs(delta) >= _areaInfo.minStepSize) {
                            startGlobalY = mapToGlobal(0, mouseY).y
                            trackMaster.trackArea.setMasterTrackHeight(-delta)
                        }
                    }
                }
            }
        }
        TrackMaster {
            id: trackMaster

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            color: /*on*/ !trackMaster.parentFocus ? theme.bg1 : theme.bg2
            clip: true

            property bool on: false
        }
    }

    Popup {
        id: trackRightClick
        width: panel.width //Math.round(350 *0.75 * refSize)
        height: panel.height //Math.round(500 *0.75 * refSize)
        visible: false

        x: positionInWindow !== undefined ? positionInWindow.x + track_list.x : 0
        y: positionInWindow !== undefined ? positionInWindow.y + track_list.y : 0
        property var callback
        property int index: 0

        property alias numberItem: panel.numberItem
        property bool visble_grouper: funcs.allow2MakeGroup()
        property bool visble_ungrouper: false
        margins: 0
        padding: 0
        contentItem: TrackRightClick {
            id: panel
            anchors.fill: parent
            isVisible_groupMaker: trackRightClick.visble_grouper
            isVisible_ungrouper: trackRightClick.visble_ungrouper
            onAction: {
                trackRightClick.close()
                if (trackRightClick.callback)
                    trackRightClick.callback(trackRightClick.index, text)
            }
        }
    }

    Popup {
        id: rightPanel
        visible: false
        width: rPanel.parentWidth
        height: rPanel.parentHeight + rPanel.margin * 2

        x: positionInWindow !== undefined ? positionInWindow.x + track_list.x : 0
        y: positionInWindow !== undefined ? positionInWindow.y + track_list.y : 0

        property var rightModules: ['', '']
        property var leftModules: ['Show Automation', 'Show Automation in New Lane']
        property alias panelWidth: rPanel.parentWidth
        property alias isTwoGroups: rPanel.twoGroups
        property alias firstStr: rPanel.firstGroup
        property int index: 0
        property var callback

        onVisibleChanged: {

            rPanel.resize()
        }

        property alias cc: rPanel.panel
        margins: 0
        padding: 0
        contentItem: RightPanel {
            id: rPanel
            anchors.fill: parent
            property var rightModules: rightPanel.rightModules
            property var leftModules: rightPanel.leftModules
            onAction: {
                rightPanel.callback(rightPanel.index, text)
            }
        }
    }

    //    //--------------------------------------------------------------------------------- Brightness:
    //    Rectangle {
    //        z: 1
    //        anchors.fill: header
    //        color: globalValues.brightness < 0 ? 'white' : 'black'
    //        opacity: Math.abs(globalValues.brightness) / 200
    //    }
    //    Rectangle {
    //        z: 1
    //        anchors.fill: trackListView
    //        color: globalValues.brightness < 0 ? 'white' : 'black'
    //        opacity: Math.abs(globalValues.brightness) / 200
    //    }
    //    //--------------------------------------------------------------------------------- Brightness.
}
