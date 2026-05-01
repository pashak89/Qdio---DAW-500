import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.2
import "qrc:/Items"
import "qrc:/"
import "."

Rectangle {
    id: root
    color: "green"

    //property var effectList
    property int trackIndex
    property real heightReference: scaleSize2(37)
    width: trackListView.width //(parent.width ? parent.width : 0) + scaleSize2(10)
    height: AutomationLaneHeight

    FontLoader {
        id: myFont2
        source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf'
    }

    property int rowCount: AutomationLaneRowCount

    property real _refSize: refSize
    on_RefSizeChanged: seperator.y = AutomationLaneRowCount * (heightReference + scaleSize2(
                                                                   15))

    // -------------------------------------------------------------------------------    Row 1
    Rectangle {
        id: recMinus
        anchors.top: root.top
        anchors.topMargin: (_clipArea.minStepSize / 2) - heightReference / 2

        width: heightReference
        height: heightReference
        radius: Math.round(heightReference / 2)

        border.color: theme.s11_Enable_BRD
        border.width: topWidth
        color: theme.s11_Enable_FIL

        Image {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill: parent
            anchors.margins: Math.round(parent.height / 5)
            sourceSize: Qt.size(height, height)
            source: 'qrc:/Resource/minus.svg'
            fillMode: Image.PreserveAspectFit
            clip: true
            antialiasing: false
            ColorOverlay {
                anchors.fill: parent
                source: parent
                color: theme.tColor
                antialiasing: false
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {

                console.log(trackIndex, index)

                _clipArea.tracksModel.automationLaneModel(
                            trackIndex).remove(index)
                //                trackAudioModel.get(ind).automationList.remove(index, 1)
                //                trackAudioModel.setProperty(ind, 'automationChildren',
                //                                            automationList.count)
            }
        }
    }

    AutomationDropMenu {
        id: menu
        anchors {
            top: AutomationLaneRowCount < 2 ? recMinus.top : recMinus.bottom
            topMargin: AutomationLaneRowCount < 2 ? 0 : scaleSize2(15)
        }

        height: heightReference
        width: scaleSize2(106) * 2 + 2 * scaleSize2(15) + scaleSize2(53)
        displayMenu: model.AutomationLaneSubMenuTitle
        //effectList: root.effectList
    }

    RowLayout {
        id: rowsTextFields
        anchors.top: menu.top

        height: heightReference
        width: scaleSize2(106)
        Rectangle {
            visible: model.AutomationLaneMenuTitle !== 'None'
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: 'transparent'
            radius: topRadius

            TrackTextField2 {
                id: tfVolume
                name: model.AutomationLaneSubMenuTitle

                anchors.fill: parent

                isPan: model.AutomationLaneSubMenuTitle === 'Pan'
                isVolume: model.AutomationLaneSubMenuTitle === 'Volume'
                isOnOff: model.AutomationLaneSubMenuTitle === 'DeviceOn'
                         | model.AutomationLaneSubMenuTitle === 'SpeakerOn'

                max: 100
                min: 0
                decimal: model.AutomationLaneSubMenuTitle === 'Pan' ? 0 : 2
                has2Slider: model.AutomationLaneSubMenuTitle === 'Pan'
                hasSlider: model.AutomationLaneSubMenuTitle !== 'Pan'

                // bigHeight: true
                enableRightClick: true
                shortcutKeyTxt: model.automatedEnabled ? ['', '', '', 'Del', 'Ctrl + Del'] : ['', '', '', '', 'Del', 'Ctrl + Del']
                moduls: model.automatedEnabled ? ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation'] : ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                flColor: theme.s11_Enable_FIL
                brColor: theme.s11_Enable_BRD

                property real automatedValue: model.automatedValue
                property real automatedNormalized: model.automatedNormalized
                property real automatedActive: model.automatedActive
                property real automatedEnabled: model.automatedEnabled
                property real normalized: model.normalized

                text: model.value.toFixed(decimal)
                defaultValue: model.defaultNormalized * 100
                value: model.normalized * 100

                onSigPressed: {
                    _clipArea.tracksModel.saveUndoNoramized(
                                trackIndex, model.AutomationLaneMenuTitle,
                                model.AutomationLaneSubMenuTitle)
                }

                onAutomatedEnabledChanged: {

                    if (!model.automatedEnabled) {
                        shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                        moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                    } else {
                        shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                        moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                    }
                }
                onAutomatedNormalizedChanged: {
                    if (model.automatedEnabled) {

                        //console.log(model.automatedNormalized,model.automatedValue)
                        value = model.automatedNormalized * 100
                        text = model.automatedValue.toFixed(decimal)
                    } else {

                    }
                }

                onNormalizedChanged: {

                    //console.log("onNormalizedChanged",)
                    value = model.normalized * 100
                    text = model.value.toFixed(decimal)
                }
                onSigValueChanged: {

                    if (model.automatedActive) {
                        if (model.automatedEnabled) {
                            model.automatedEnabled = false
                            shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                            moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                        }
                    }
                    model.normalized = value / 100.0

                    tfVolume.value = model.normalized * 100
                    tfVolume.text = model.value.toFixed(decimal)
                }

                onAction: {

                    if (text == "Re-Enable Automation") {
                        model.automatedEnabled = true
                        tfVolume.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                        tfVolume.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                    }

                    if (text == "Delete Automation") {

                        _clipArea.tracksModel.automationClear(
                                    trackIndex, model.AutomationLaneMenuTitle,
                                    model.AutomationLaneSubMenuTitle)
                    }
                }

                Rectangle {
                    z: 1000
                    visible: model.automatedActive
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 1
                    anchors.topMargin: 0
                    color: model.automatedEnabled ? "#EA2027" : "#808080"
                    width: scaleSize2(11)
                    height: scaleSize2(11)
                    radius: scaleSize2(11)
                }
            }
        }
    }

    Rectangle {
        id: seperator
        anchors.bottom: root.bottom
        width: parent.width

        height: _clipArea.seperatorHeight
        color: '#2f3032'

        Rectangle {
            id: seperatorHighlight
            visible: automationListViewPanel.automationIndexDrop === index
            anchors.fill: parent
            color: 'lightgrey'
        }

        MouseArea {
            anchors.left: parent.left
            anchors.top: parent.top
            width: _width
            drag.axis: Drag.YAxis
            drag.minimumY: heightReference + scaleSize2(15) //- height
            drag.target: parent

            hoverEnabled: true

            onEntered: {
                _clipArea.setOverrideCursor(Qt.SizeVerCursor)
            }
            onExited: {
                _clipArea.setOverrideCursor(Qt.ArrowCursor)
            }
            onMouseYChanged: {
                _clipArea.setOverrideCursor(Qt.SizeVerCursor)
                if (pressed) {

                    const item = trackListView.itemAtIndex(ind)

                    const localPoint = mapToItem(trackListView, mouse.x, mouse.y)

                    _clipArea.tracksModel.automationLaneModel(
                                trackIndex).setLaneHeight(index,localPoint.y)
                }
            }
        }

        Rectangle {
            id: seperatorHighlight2
            visible: inDraggingCounter > -1 & automationLaneCount === index + 1
            anchors.fill: parent
            color: inDraggingCounter > -1 ? theme.betweenRec_dragging_o20 : 'transparent'
        }
    }

    states: [
        State {
            name: "Right"
            when: !globalValues.tracksOnLeft

            AnchorChanges {
                target: recMinus
                anchors.right: parent.right
                anchors.left: undefined
            }

            AnchorChanges {
                target: menu
                anchors.right: recMinus.left
                anchors.left: undefined
            }
            PropertyChanges {
                target: menu
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: undefined
            }

            AnchorChanges {
                target: rowsTextFields
                anchors.right: menu.left
                anchors.left: undefined
            }
            PropertyChanges {
                target: rowsTextFields
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: undefined
            }

            AnchorChanges {
                target: seperator
                anchors.right: parent.right
                anchors.left: undefined
            }
            PropertyChanges {
                target: seperator
                anchors.rightMargin: automationList.count - 1
                                     === index ? -draggableSize - scaleSize2(
                                                     10) : -scaleSize2(10)
                anchors.leftMargin: undefined
            }
            PropertyChanges {
                target: seperatorHighlight
                anchors.rightMargin: trackAudioModel.get(
                                         ind).automationList.count - 1 === index ? draggableSize : 0
                anchors.leftMargin: 0
            }

            PropertyChanges {
                target: seperatorHighlight2
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

            AnchorChanges {
                target: recMinus
                anchors.right: undefined
                anchors.left: parent.left
            }

            AnchorChanges {
                target: menu
                anchors.right: undefined
                anchors.left: recMinus.right
            }
            PropertyChanges {
                target: menu
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: rowsTextFields
                anchors.right: undefined
                anchors.left: menu.right
            }
            PropertyChanges {
                target: rowsTextFields
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: seperator
                anchors.right: undefined
                anchors.left: parent.left
            }
            PropertyChanges {
                target: seperator
                anchors.rightMargin: undefined
                anchors.leftMargin: _clipArea.tracksModel.automationLaneModel(
                                        trackIndex).count - 1
                                    === index ? -draggableSize - scaleSize2(
                                                    10) : -scaleSize2(10)
            }
            PropertyChanges {
                target: seperatorHighlight
                anchors.rightMargin: 0
                anchors.leftMargin: _clipArea.tracksModel.automationLaneModel(
                                        trackIndex).count - 1 === index ? draggableSize : 0
            }

            PropertyChanges {
                target: seperatorHighlight2
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
