import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.2

import TrackShapeItem 1.0
import "qrc:/Items"

Rectangle {
    id: root

    property int channels: trackShapeItem.masterTrack.channelCount
    height: trackShapeItem.masterTrack.totalRect.height - _clipArea.seperatorHeight
    width: parent.width

    property int numberAudio: 1

    property Item nameItem: textLabel
    property TrackShapeItem trackArea: trackShapeItem

    property bool splitStereo: false

    property real delta0: 0
    property real newH: 0

    property real lastPos: 0
    property var itemPos

    property real lastPosX: 0
    property var itemPosX
    property real newW: 0

    property string name: 'Master'

    property alias draggableSize: collapsPanel.width
    property alias volumSize: volumeMeter.width

    property bool didFold: trackShapeItem.masterTrack.rowsExpanded ? true : false
    property bool didLight: false
    property bool secondLevelLight: select_btn.didShadow | tf_01.didShadow
                                    | tf2.didShadow | ttf.didShadow
    property bool firstLevelLight: didLight //| automationBtn.didShadow /*| viewBtn.didShadow | recordBtn.didShadow*/ | sendBtn.didShadow
    property bool parentFocus: on

    property alias colorPanel: collapsPanel.color

    property alias didSelectTrack: select_btn.isClicked

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

        btn: select_btn
        isMaster: true
        visible: false

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
        anchors.left: parent.left
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

                    console.log(trackShapeItem.masterTrack.rowsExpanded,
                                trackShapeItem.masterTrack.rect.height)
                    if (trackShapeItem.masterTrack.rowsExpanded === true) {
                        trackShapeItem.masterTrack.rowsExpanded = false
                        //spanVert.yIndicator = 0
                    } else {
                        trackShapeItem.masterTrack.rowsExpanded = true
                        // spanVert.yIndicator = spanVert.stepSize
                    }
                    console.log(trackShapeItem.masterTrack.rowsExpanded,
                                trackShapeItem.masterTrack.rect.height)
                    trackShapeItem.masterTrack.updateRect()
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

        anchors.leftMargin: scaleSize2(10)
        anchors.left: collapsPanel.right
        anchors.top: parent.top
        anchors.topMargin: scaleSize2(10)

        height: scaleSize2(37)
        width: scaleSize2(106)

        name: 'M' //numberAudio
        hasRightPanel: true

        isClicked: true

        brColor: isClicked ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD
    }

    // ------------------------------------------------------------------------------------------ item 2
    Rectangle {
        id: tf_01
        anchors.left: select_btn.right

        anchors.leftMargin: scaleSize2(15)
        anchors.top: select_btn.top

        height: scaleSize2(37)
        width: scaleSize2(106)

        color: 'transparent'

        TrackTextField2 {
            id: tf1
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
            isMaster: true

            max: 6
            min: -70

            decimal: 1

            hasSlider: true
        }
    }

    Rectangle {
        id: name //-----------------------------------------------------------------------
        anchors.left: tf_01.right
        anchors.right: volumeMeter.left
        anchors.rightMargin: scaleSize2(10)
        anchors.leftMargin: scaleSize2(15)

        anchors.top: tf_01.top
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

        anchors.left: collapsPanel.right
        anchors.leftMargin: scaleSize2(10)
        anchors.top: select_btn.bottom
        anchors.topMargin: scaleSize2(15)
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

        anchors.top: tf_10.top
        anchors.left: tf_10.right
        anchors.leftMargin: scaleSize2(15)
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
        id: volumeMeter //----------------------------------------------------------------------------------------
        width: channels * scaleSize2(
                   8) + (topWidth * 2) * 2 + (channels - 1) * (topWidth)
        height: parent.height
        color: 'black'

        anchors.left: parent.left
        anchors.leftMargin: scaleSize2(10) + _width - volumeMeter.width
        RowLayout {
            id: row
            anchors.fill: parent
            anchors.margins: topWidth * 2 //+1
            spacing: topWidth

            Repeater {

                model: trackShapeItem.masterTrack.truePeakModel

                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: '#042327'

                    Rectangle {
                        width: parent.width
                        height: 2
                        y: 0.15 * parent.height
                        color: 'black'
                    }

                    Rectangle {

                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: Math.floor(
                                    parent.height * (1 - model.AverageMeterLevel))
                        color: '#15D0E9'
                    }

                    Rectangle {
                        id: rect1
                        width: parent.width
                        height: model.PeakMeterPeakLevel >= 1 ? 0 : 1
                        y: Math.floor(
                               parent.height * (model.PeakMeterPeakLevel))

                        color: '#15D0E9'
                    }
                    Rectangle {

                        width: parent.width

                        height: Math.floor(
                                    parent.height * ((1 - model.PeakMeterLevel)
                                                     - (1 - model.AverageMeterLevel)))
                        y: Math.floor(parent.height * (model.PeakMeterLevel))
                        color: '#0F707D'
                    }
                }
            }
        }
    }

    TrackShapeItem {
        id: trackShapeItem

        trackIndex: -1
        areaInfo: _areaInfo

        clipArea: _clipArea

        anchors.left: parent.left
        anchors.leftMargin: scaleSize2(10) + _width

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        height: trackShapeItem.masterTrack.totalRect.height - _clipArea.seperatorHeight
        width: root.width - _width
    }
}
