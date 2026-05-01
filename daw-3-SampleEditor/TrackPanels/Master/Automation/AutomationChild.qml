import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.2
import 'qrc:/Items'
import 'qrc:/'


Item {
    id: root

    property real heightReference: scaleSize2(37)
    width: trackListView.width//(parent.width ? parent.width : 0) + scaleSize2(10)
    height: rowCounter * (heightReference + scaleSize2(15)) + seperator.height//seperator.y + seperator.height

    FontLoader{ id: myFont2; source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf' }

    property int rowCounter: 3

    property real _refSize: refSize
    on_RefSizeChanged: seperator.y = rowCounter * (heightReference + scaleSize2(15))


//    property real _tf3: tf3
//    on_Tf3Changed: if(inTrack | inAutomationMenu) {tfPan3.value = tf3; tfPan3.text = tfPan3.value}




    // MouseArea {
    //     id: maRoot
    //     anchors.fill: parent; anchors.leftMargin: -scaleSize2(10)
    //     acceptedButtons: Qt.LeftButton | Qt.RightButton

    //     property bool allow2clear: false

    //     onClicked: {
    //         if(groupTrackLight.length === 1 & !holdCtrl & !holdShift & allow2clear)
    //         {
    //             groupTrackLight.length = 0
    //             offLightAllTracks()
    //         }
    //     }

    //     onPressed: {
    //         parent.focus = true
    //         root.focus = true
    //         automationListView.focus = true

    //         if(mouse.button === Qt.RightButton)
    //             trackLighting_rightclick(ind, 'btn')
    //         else
    //             trackLighting(ind, 'btn')

    //         if (mouse.button === Qt.RightButton) {
    //             positionInWindow = mapToItem(track_list.contentItem, mouse.x, mouse.y)
    //             trackRightClick.numberItem = ind
    //             trackRightClick.visble_grouper = allow2MakeGroup()
    //             trackRightClick.visble_ungrouper = groupTrackLight.length === 1 & trackMasterModel.get(groupTrackLight[0]).type === 'Group'
    //             trackRightClick.visible = true

    //             if(rightPanel.visible)
    //                 rightPanel.visible = false
    //         }
    //         else
    //             trackRightClick.visible = false
    //     }
    // }

    // -------------------------------------------------------------------------------    Row 1
    Rectangle {
        id: recMinus
        anchors.top: root.top

        width: heightReference; height: heightReference
        radius: Math.round(heightReference/2)

        border.color: theme.s11_Enable_BRD
        border.width: topWidth
        color: theme.s11_Enable_FIL

        Image {
            anchors.centerIn: parent
            anchors.fill: parent
            anchors.margins: Math.round(parent.height/5)
            sourceSize:     Qt.size(height, height)
            source:         'qrc:/Resource/minus.svg'
            fillMode:       Image.PreserveAspectFit
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
                trackMasterModel.get(ind).automationList.remove(index, 1)
                trackMasterModel.setProperty(ind, 'automationChildren', automationList.count)
            }
        }
    }


    AutomationDropMenu {
        id: menu
        anchors {
            top: rowCounter < 2 ? recMinus.top : recMinus.bottom; topMargin: rowCounter < 2 ? 0 : scaleSize2(15)
        }

        height: heightReference
        width: scaleSize2(106) * 2 + 2 * scaleSize2(15) + scaleSize2(53)
        displayMenu: displayText
    }

    RowLayout {
        id: rowsTextFields
        anchors.top: menu.top

        height: heightReference
        width: scaleSize2(106)

        Rectangle {
            visible: displayText === 'Volume'
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: 'transparent'

            TrackTextField2 {
                id: tfVolume
                anchors.fill: parent
                // bigHeight: true

                enableRightClick: true
                shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

                flColor: theme.s11_Enable_FIL
                brColor: theme.s11_Enable_BRD

                max: 6
                min: -70

                decimal: 1

                hasSlider: true
            }
        }

        // ------------------------------------------------------------------------------------------ item 4
        Rectangle {
            visible: displayText === 'Pan'
            Layout.fillHeight: true
            Layout.fillWidth: true

            color:                  'transparent'
            radius:                 topRadius

            RowLayout {
                id: rowFirst
                visible: splitStereo
                anchors.fill: parent

                TrackTextField {
                    id: tfPan1
                    Layout.fillHeight: true
                    Layout.preferredWidth: Math.round(parent.width *0.45)
                    Layout.alignment: Qt.AlignLeft
                    // bigHeight: true

                    enableRightClick: true
                    shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                    moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

                    splitStr: 'Stereo Pan Mode'
                    is2Groups: true

                    flColor: theme.s11_Enable_FIL
                    brColor: theme.s11_Enable_BRD
                    min: -50
                    max: 50
                    defaultValue: 0
                    has2Slider: true
                    decimal: 0
                    text: value
                }

                TrackTextField {
                    id: tfPan2
                    Layout.fillHeight: true
                    Layout.preferredWidth: Math.round(parent.width *0.45)
                    Layout.alignment: Qt.AlignLeft
                    // bigHeight: true

                    enableRightClick: true
                    shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                    moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

                    splitStr: 'Stereo Pan Mode'
                    is2Groups: true

                    flColor: theme.s11_Enable_FIL
                    brColor: theme.s11_Enable_BRD
                    min: -50
                    max: 50
                    defaultValue: 0
                    has2Slider: true
                    decimal: 0
                    text: value
                }
            }

            TrackTextField {
                id: tfPan3
                visible: !splitStereo
                anchors.fill: parent
                // bigHeight: true

                enableRightClick: true
                shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

                splitStr: 'Split Stereo Pan'
                is2Groups: true

                flColor: theme.s11_Enable_FIL
                brColor: theme.s11_Enable_BRD
                min: -50
                max: 50
                defaultValue: 0
                has2Slider: true
                decimal: 0
                text: value
                onValueChanged: {inTrack = false; inAutomationMenu = false; inAutomationChild = true ;tf3 = value.toFixed(0)}

                onFocusChanged: console.log('fos: ', focus)
            }
        }
    }


    Rectangle {
        id: seperator
        y: heightReference*3 + scaleSize2(15)*3
        width: parent.width
        height: scaleSize2(5)
        color: '#2f3032'

        Rectangle {
            id: seperatorHighlight
            visible: automationListViewPanel.automationIndexDrop === index
            anchors.fill: parent
            color: 'lightgrey'
        }

        MouseArea {
            anchors.fill: parent
            drag.axis: Drag.YAxis
            drag.minimumY: heightReference + scaleSize2(15) //- height
            drag.target: parent

            hoverEnabled: true

            onEntered: cursorPositionClass.vCursorShape()
            onExited: if(!pressed) cursorPositionClass.onlyShowCursor()
            onPressed: cursorPositionClass.vCursorShape()
            onReleased: {
                cursorPositionClass.onlyShowCursor()

                if(rowCounter !== size)
                    automationList.setProperty(index, 'size', rowCounter)
            }

            onMouseYChanged: {
                if(pressed) parent.y = Math.round(parent.y / (heightReference + scaleSize2(15))) * (heightReference + scaleSize2(15)) //- height
                rowCounter = seperator.y / (heightReference + scaleSize2(15))
            }
        }



        Rectangle {
            id: seperatorHighlight2
            visible:        inDraggingCounter > -1 & automationChildren === index + 1
            anchors.fill:   parent
            color:          inDraggingCounter > -1 ? theme.betweenRec_dragging_o20 : 'transparent'
        }

    }



    states: [State {
            name: "Right"
            when:!globalValues.tracksOnLeft

            AnchorChanges {
                target: recMinus
                anchors.right: parent.right
            }

            AnchorChanges {
                target: menu
                anchors.right: recMinus.left
            }
            PropertyChanges {
                target: menu
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: rowsTextFields
                anchors.right: menu.left
            }
            PropertyChanges {
                target: rowsTextFields
                anchors.rightMargin: scaleSize2(15)
            }




            AnchorChanges {
                target: seperator
                anchors.right: parent.right
            }
            PropertyChanges {
                target: seperator
                anchors.rightMargin: automationList.count -1 === index ?
                                         - draggableSize - scaleSize2(10) : - scaleSize2(10)
            }

            PropertyChanges {
                target: seperatorHighlight
                anchors.rightMargin: trackMasterModel.get(ind).automationList.count -1 === index ? draggableSize : 0
                anchors.leftMargin: 0
            }

            PropertyChanges {
                target: seperatorHighlight2
                anchors.rightMargin: type === "Group" ?
                                         (in_set_parent.count - inDraggingCounter+1) * scaleSize2(50) :
                                         (in_set_parent.count - inDraggingCounter) * scaleSize2(50)
                anchors.leftMargin: 0
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: recMinus
                anchors.left: parent.left
            }

            AnchorChanges {
                target: menu
                anchors.left: recMinus.right
            }
            PropertyChanges {
                target: menu
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: rowsTextFields
                anchors.left: menu.right
            }
            PropertyChanges {
                target: rowsTextFields
                anchors.leftMargin: scaleSize2(15)
            }





            AnchorChanges {
                target: seperator
                anchors.left: parent.left
            }
            PropertyChanges {
                target: seperator
                anchors.leftMargin: automationList.count -1 === index ?
                                        - draggableSize - scaleSize2(10) : - scaleSize2(10)
            }

            PropertyChanges {
                target: seperatorHighlight
                anchors.rightMargin: 0
                anchors.leftMargin: trackMasterModel.get(ind).automationList.count -1 === index ? draggableSize : 0
            }

            PropertyChanges {
                target: seperatorHighlight2
                anchors.rightMargin: 0
                anchors.leftMargin: type === "Group" ?
                                        (in_set_parent.count - inDraggingCounter+1) * scaleSize2(50) :
                                        (in_set_parent.count - inDraggingCounter) * scaleSize2(50)
            }

        }]

}
