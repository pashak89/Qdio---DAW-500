import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.2
import "qrc:/Items"

Item {
    id: root

    signal trackSelectPressed

    property int channels: 8

    property int numberAudio: 1

    anchors.left: parent.left
    anchors.right: parent.right
    height: parent.height //scaleSize2(140)*0 + spanVert.value//+ spanVert.stepSize

    //    onHeightChanged: console.log('height: ' + height)
    property var alphabet: ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"]

    property Item nameItem: textLabel

    property bool splitStereo: false

    property real delta0: 0
    property real newH: 0

    property real lastPos: 0
    property var itemPos

    property real lastPosX: 0
    property var itemPosX
    property real newW: 0

    property string name: alphabet[numberAudio] + ' Return'

    property alias draggableSize: collapsPanel.width
    property alias volumSize: volumeMeter.width

    property bool didFold: spanVert.is_main > 0 ? true : false
    property bool didLight: false
    property bool secondLevelLight: select_btn.didShadow | solo_btn.didShadow
                                    | tf2.didShadow | ttf.didShadow
    property bool firstLevelLight: didLight | automationBtn.didShadow | viewBtn.didShadow
                                   | recordBtn.didShadow | sendBtn.didShadow
    property bool parentFocus: on

    // property bool onLeftSide:   true
    property alias colorPanel: collapsPanel.color

    property alias didSelectTrack: select_btn.isClicked
    property alias didSelectSolo: solo_btn.isClicked

    states: [
        State {
            name: "Right"
            when: !globalValues.tracksOnLeft

            AnchorChanges {
                target: collapsPanel
                anchors.left: undefined
                anchors.right: parent.right
            }

            AnchorChanges {
                target: solo_btn
                anchors.left: undefined
                anchors.right: collapsPanel.left
            }
            PropertyChanges {
                target: solo_btn
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(10)
            }

            AnchorChanges {
                target: select_btn
                anchors.left: undefined
                anchors.right: solo_btn.left
            }
            PropertyChanges {
                target: select_btn
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: name
                anchors.left: volumeMeter.right
                anchors.right: solo_btn.left
            }
            PropertyChanges {
                target: name
                anchors.leftMargin: scaleSize2(10)
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: tf_10
                anchors.left: undefined
                anchors.right: collapsPanel.left
            }
            PropertyChanges {
                target: tf_10
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(10)
            }

            AnchorChanges {
                target: tf_11
                anchors.left: undefined
                anchors.right: tf_10.left
            }
            PropertyChanges {
                target: tf_11
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: record_send_btn
                anchors.left: undefined
                anchors.right: tf_11.left
            }
            PropertyChanges {
                target: record_send_btn
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: auto_view_btn
                anchors.left: undefined
                anchors.right: record_send_btn.left
            }
            PropertyChanges {
                target: auto_view_btn
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: volumeMeter
                anchors.right: undefined
                anchors.left: parent.left
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: collapsPanel
                anchors.right: undefined
                anchors.left: parent.left
            }

            AnchorChanges {
                target: select_btn
                anchors.right: undefined
                anchors.left: collapsPanel.right
            }
            PropertyChanges {
                target: select_btn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(10)
            }

            AnchorChanges {
                target: solo_btn
                anchors.right: undefined
                anchors.left: select_btn.right
            }
            PropertyChanges {
                target: solo_btn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: name
                anchors.right: volumeMeter.left
                anchors.left: solo_btn.right
            }
            PropertyChanges {
                target: name
                anchors.rightMargin: scaleSize2(10)
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: tf_10
                anchors.right: undefined
                anchors.left: collapsPanel.right
            }
            PropertyChanges {
                target: tf_10
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(10)
            }

            AnchorChanges {
                target: tf_11
                anchors.right: undefined
                anchors.left: tf_10.right
            }
            PropertyChanges {
                target: tf_11
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: record_send_btn
                anchors.right: undefined
                anchors.left: tf_11.right
            }
            PropertyChanges {
                target: record_send_btn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: auto_view_btn
                anchors.right: undefined
                anchors.left: record_send_btn.right
            }
            PropertyChanges {
                target: auto_view_btn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: volumeMeter
                anchors.left: undefined
                anchors.right: parent.right
            }
        }
    ]

    property bool didGradient: parentFocus //| automationBtn.didShadow | viewBtn.didShadow | recordBtn.didShadow | sendBtn.didShadow
    property Gradient _3DGradient: Gradient {
        GradientStop {
            position: 0.0
            color: didGradient ? "#C5C7C9" : theme.s11_Enable_FIL
        }
        GradientStop {
            position: 0.17
            color: didGradient ? "#BDBEBF" : theme.s11_Enable_FIL
        }
        GradientStop {
            position: 0.5
            color: didGradient ? "#AAABAD" : theme.s11_Enable_FIL
        }
        GradientStop {
            position: 0.82
            color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL
        }
        GradientStop {
            position: 1.0
            color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL
        }
    }

    // ----------------------------------------------------------------------------------------------------- span snap:
    SpanVertical {
        id: spanVert
        isReverb: true
        //        withColor: true
        btn: select_btn
        anchors.right: parent.right
        anchors.rightMargin: globalValues.tracksOnLeft ? volumeMeter.width : collapsPanel.width
        anchors.left: parent.left
        anchors.leftMargin: globalValues.tracksOnLeft ? collapsPanel.width : volumeMeter.width
    }

    // ----------------------------------------------------------------------------------------------------- span snap vertival
    Rectangle {
        id: collapsPanel
        width: scaleSize2(50)
        height: parent.height
        color: '#1791E9'

        Image {
            id: fold
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: scaleSize2(15)

            source: 'qrc:/Resource/Triangle.svg'
            sourceSize: Qt.size(oddSize(parent.width / 2),
                                oddSize(parent.width / 2))
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    //                    didFold = !didFold
                    //                    if(didFold)
                    //                    { rowsExpanded = 0 ; spanVert.yIndicator = 0 }
                    //                    else
                    //                    { rowsExpanded = 1 ; spanVert.yIndicator = 1 }
                    if (rowsExpanded > 0) {
                        rowsExpanded = 0
                        spanVert.yIndicator = 0
                    } else {
                        rowsExpanded = 1
                        spanVert.yIndicator = spanVert.stepSize
                    }
                }
            }

            rotation: didFold ? -90 : 0
        }

        Slider2 {
            id: sliderWidth
            enabled: false
            visible: false
            width: parent.width
            height: parent.height
        }
    }

    // ------------------------------------------------------------------------------------------ item 1
    TrackButton {
        id: select_btn
        anchors {
            top: parent.top
            topMargin: scaleSize2(10)
        }

        height: scaleSize2(37)
        width: scaleSize2(106)

        name: alphabet[numberAudio]
        hasRightPanel: true

        isClicked: true

        fontColor: {
            if (!solo_on_return && isClicked)
                theme.f4_
            else if (solo_on_return && !isClicked)
                theme.f2_
            else if (solo_on_return && isClicked)
                theme.f7_
            else if (!solo_on_return && !isClicked)
                theme.f1_
        }

        fillStyle: {
            if (!solo_on_return && isClicked)
                theme.b11_Enable1_FIL
            else if (solo_on_return && !isClicked)
                theme.b10_Disable_FIL
            else if (solo_on_return && isClicked)
                theme.b20_Disable_FIL
            else if (!solo_on_return && !isClicked)
                theme.b10_Disable_FIL
        }
        brColor: isClicked ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD

        onSendMessage: if (msg === 'released')
                           trackSelectPressed()
    }

    // ------------------------------------------------------------------------------------------ item 2
    TrackButton {
        id: solo_btn
        anchors {
            top: select_btn.top
        }

        height: scaleSize2(37)
        width: scaleSize2(106)
        name: 'S'
        isSoloBtn: true
        fontColor: isClicked ? theme.f4_ : theme.f2_

        fillStyle: isClicked ? theme.b12_Enable2_FIL : theme.b20_Disable_FIL
        brColor: isClicked ? theme.b12_Enable2_BRD : theme.b20_Disable_BRD

        onSendMessage: if (msg === "released")
                           trackReturnSelection(ind_return, solo_btn)
    }

    Rectangle {
        id: name //-----------------------------------------------------------------------
        anchors {
            top: solo_btn.top
        }
        z: spanVert.z - 1

        height: scaleSize2(37)
        color: 'transparent'

        clip: true
        TextInputElide {
            id: textLabel
            fullText: root.name
            anchors.fill: parent
            //            onDidMarkerChanged: name.z = didMarker ? spanVert.z +1 : spanVert.z -1
        }
    }

    // ------------------------------------------------------------------------------------------ item 3
    Rectangle {
        id: tf_10
        anchors {
            top: select_btn.bottom
            topMargin: scaleSize2(15)
        }

        height: scaleSize2(37)
        width: scaleSize2(106)

        color: 'transparent'

        TrackTextField2 {
            id: tf2
            anchors.fill: parent
            bigHeight: true

            enableRightClick: true
            shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
            moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

            //            shortcutKeyTxt:     ['', '', '']
            //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
            //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
            //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
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
        id: tf_11
        anchors {
            top: tf_10.top
        }

        height: scaleSize2(37)
        width: scaleSize2(106)

        color: 'transparent'
        radius: topRadius

        RowLayout {
            id: rowFirst
            visible: splitStereo
            anchors.fill: parent

            TrackTextField {
                Layout.fillHeight: true
                Layout.preferredWidth: Math.round(parent.width * 0.45)
                Layout.alignment: Qt.AlignLeft
                bigHeight: true

                enableRightClick: true
                shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
                moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                //            shortcutKeyTxt:     ['', '', '']
                //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
                //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
                //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
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
                Layout.fillHeight: true
                Layout.preferredWidth: Math.round(parent.width * 0.45)
                Layout.alignment: Qt.AlignLeft
                bigHeight: true

                enableRightClick: true
                shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
                moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                //            shortcutKeyTxt:     ['', '', '']
                //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
                //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
                //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
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
            id: ttf
            visible: !splitStereo
            anchors.fill: parent
            bigHeight: true

            enableRightClick: true
            shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
            moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
            //            shortcutKeyTxt:     ['', '', '']
            //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
            //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
            //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
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
        }
    }

    Rectangle {
        id: record_send_btn // ------------------------------------------------------------------- 1
        anchors {
            top: tf_11.top
            bottom: tf_11.bottom
        }
        width: scaleSize2(106)
        gradient: _3DGradient
        radius: topRadius

        Rectangle {
            id: betweenLine_send_btn
            visible: recordBtn.isClicked || sendBtn.isClicked
            enabled: recordBtn.isClicked || sendBtn.isClicked
            x: recordBtn.x + recordBtn.width
            height: parent.height
            width: topWidth
            z: 1
            color: theme.s11_Enable_FIL
        }

        RowLayout {
            id: rr
            anchors.fill: parent
            spacing: topWidth

            TrackSubButton {
                id: recordBtn
                Layout.fillHeight: true
                Layout.fillWidth: true

                onLeft: true

                flColor: parentFocus ? (isClicked ? '#DE3C47' : 'transparent') : isClicked ? '#DE3C47' : theme.s11_Enable_FIL

                onIsClickedChanged: focus = true

                Image {
                    id: recordImg
                    anchors.centerIn: parent
                    sourceSize: {
                        if (recordBtn.height % 2 !== 0)
                            Qt.size(oddSize(parent.width * .3),
                                    oddSize(parent.width * .3))
                        else
                            Qt.size(evenSize(parent.width * .3),
                                    evenSize(parent.width * .3))
                    }
                    source: 'qrc:/Resource/rec_icon.svg'
                    visible: false
                    mipmap: true
                }

                ColorOverlay {
                    id: colorImageRecord
                    cached: true
                    anchors.fill: recordImg

                    source: recordImg
                    //                    clip:                   true
                    //                    antialiasing:           true
                    color: recordBtn.isClicked | parentFocus ? theme.a1 : theme.a2
                }
                //                MouseArea {
                //                    anchors.fill: parent
                //                    onClicked: console.log('image: ' + recordImg.height + ', btn: '+ recordBtn.height + ', img w: ' +
                //                                           recordImg.width + ', btn w: '+ recordBtn.width)
                //                }
                onSendMessage: trackReturnLighting(ind_return, select_btn)
            } // recordBtn.

            TrackSubButton {
                id: sendBtn
                Layout.fillHeight: true
                Layout.fillWidth: true
                flColor: parentFocus ? (isClicked ? '#13C44B' : 'transparent') : isClicked ? '#13C44B' : theme.s11_Enable_FIL

                Image {
                    id: sendImg
                    anchors.centerIn: parent
                    sourceSize: {
                        if (sendBtn.height % 2 !== 0)
                            Qt.size(oddSize(parent.width * .6),
                                    oddSize(parent.width * .6))
                        else
                            Qt.size(evenSize(parent.width * .6),
                                    evenSize(parent.width * .6))
                    }
                    source: 'qrc:/Resource/send_icon.svg'
                    visible: false
                }
                ColorOverlay {
                    id: colorImageSend
                    cached: true
                    anchors.fill: sendImg

                    source: sendImg
                    clip: true
                    antialiasing: true
                    color: sendBtn.isClicked | parentFocus ? theme.a1 : theme.a2
                }
                //                MouseArea {
                //                    anchors.fill: parent
                //                    onClicked: console.log('image: ' + sendImg.height + ', btn: '+ sendBtn.height)
                //                }
                onSendMessage: trackReturnLighting(ind_return, select_btn)
            } // sendBtn.
        }

        Rectangle {
            anchors {
                fill: parent
                topMargin: -1
                bottomMargin: -1
            }
            color: 'transparent'
            border.color: theme.b10_Disable_BRD
            border.width: topWidth
            radius: topRadius
        }
    }

    Rectangle {
        id: auto_view_btn //---------------------------------------------------------------------- 2
        anchors {
            top: record_send_btn.top
            bottom: record_send_btn.bottom
        }

        width: scaleSize2(106)

        gradient: _3DGradient
        radius: topRadius

        Rectangle {
            id: betweenLine_auto_view
            visible: automationBtn.isClicked || viewBtn.isClicked
            enabled: automationBtn.isClicked || viewBtn.isClicked
            x: automationBtn.x + automationBtn.width
            height: parent.height
            width: topWidth
            z: 1
            color: theme.s11_Enable_FIL
        }

        RowLayout {
            anchors.fill: parent
            spacing: topWidth

            TrackSubButton {
                id: automationBtn
                Layout.fillHeight: true
                Layout.fillWidth: true

                flColor: parentFocus ? (isClicked ? '#EFA441' : 'transparent') : isClicked ? '#EFA441' : theme.s11_Enable_FIL

                onLeft: true

                Image {
                    id: automationImg
                    anchors.centerIn: parent
                    //                    anchors.verticalCenterOffset: parent.height % 2 === 0 ? parent.height/2 > height ? 1 : -1 : 0
                    sourceSize: {
                        if (automationBtn.height % 2 !== 0)
                            Qt.size(oddSize(parent.width * .6),
                                    oddSize(parent.width * .6))
                        else
                            Qt.size(evenSize(parent.width * .6),
                                    evenSize(parent.width * .6))
                    }
                    source: 'qrc:/Resource/automation_icon.svg'
                    visible: false
                }
                ColorOverlay {
                    id: colorImageAutomation
                    cached: true
                    anchors.fill: automationImg
                    source: automationImg
                    //                    clip:                   true
                    //                    antialiasing:           true
                    color: automationBtn.isClicked | parentFocus ? theme.a1 : theme.a2
                }
                onSendMessage: trackReturnLighting(ind_return, select_btn)
            } // automationBtn.

            TrackSubButton {
                id: viewBtn
                Layout.fillHeight: true
                Layout.fillWidth: true

                flColor: {
                    if (isClicked)
                        '#8E7EE0'
                    else if (parentFocus)
                        'transparent'
                    else
                        theme.s11_Enable_FIL
                }

                Image {
                    id: viewImg
                    anchors.centerIn: parent
                    //                    anchors.verticalCenterOffset: parent.height % 2 === 0 ? parent.height/2 > height ? 1 : -1 : 0
                    sourceSize: {
                        if (viewBtn.height % 2 !== 0)
                            Qt.size(oddSize(parent.width * .8),
                                    oddSize(parent.width * .8))
                        else
                            Qt.size(evenSize(parent.width * .8),
                                    evenSize(parent.width * .8))
                    }

                    source: 'qrc:/Resource/object_icon.svg'
                    visible: false
                }
                ColorOverlay {
                    id: colorImageView
                    cached: true
                    anchors.fill: viewImg

                    source: viewImg
                    color: viewBtn.isClicked | parentFocus ? theme.a1 : theme.a2
                }
                onSendMessage: trackReturnLighting(ind_return, select_btn)
            } // viewBtn.
        }

        Rectangle {
            anchors {
                fill: parent
                topMargin: -1
                bottomMargin: -1
            }
            color: 'transparent'
            border.color: theme.b10_Disable_BRD
            border.width: topWidth //Math.min(topWidth *2, 2)
            radius: topRadius
        }
    }

    Rectangle {
        id: volumeMeter //----------------------------------------------------------------------------------------
        width: channels * scaleSize2(
                   8) + (topWidth * 2) * 2 + (channels - 1) * (topWidth)
        height: parent.height
        color: 'black'

        RowLayout {
            id: row
            anchors.fill: parent
            anchors.margins: topWidth * 2 //+1
            spacing: topWidth

            Repeater {
                model: channels
                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: '#042327'

                    Rectangle {
                        id: channel
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: Math.round(parent.height * .75)
                        color: '#15D0E9'
                    } // rec1.
                } // rec2.
            } // repeater.
        } // row.
    } // rightVolume.
}
