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

    color: theme.bg2
    Theme1 {
        id: theme1
    }
    Theme2 {
        id: theme2
    }
    property Item theme: _areaInfo.themeType === 0 ? theme1 : theme2

    FontLoader {
        id: myFont
        source: 'qrc:/Resource/font/Open_Sans/OpenSans-VariableFont.ttf'
    }
    FontLoader {
        id: myFont2
        source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf'
    }

    FontLoader {
        id: swisFont
        source: 'qrc:/Resource/font/Swis721CnBTBold.ttf'
    }

    property var cursorPosition: cursorPositionClass

    property int _width: scaleSize2(600)
    property int minWidth: scaleSize2(600)
    property string topFontFamily: myFont2.name
    property int topfontPixelSize: scaleSize2(28)
    property real topRadius: radiusCal(3)
    property int topWidth: resolution[0] >= 3840 & resolution[1]
                           >= 2170 ? ((refSize < .83) ? 1 : (refSize < 1.12) ? 2 : 2) : ((refSize < .83) ? 1 : (refSize < 1.12) ? 1 : 2)

    property var resolution: cursorPosition.getResolution()

    property real refSize: _areaInfo.zoomFactor
    property real _scale: cursorPosition.getScale()

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
    RowLayout {
        height: scaleSize2(200)
        anchors.centerIn: parent
        spacing: scaleSize2(50)
        width: scaleSize2(900)

        RowLayout {
            id: leftLayout
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: scaleSize2(10)

            TrackButton {
                id: playBtn

                Layout.maximumHeight: scaleSize2(58)
                Layout.minimumHeight: scaleSize2(58)

                Layout.minimumWidth: scaleSize2(66)
                Layout.maximumWidth: scaleSize2(66)
                Layout.alignment: Qt.AlignHCenter

                //isClicked: !_clipArea.paused
                fillStyle: !_clipArea.paused ? theme.b11_Enable1_FIL : theme.b10_Disable_FIL
                brColor: !_clipArea.paused ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD

                Image {
                    id: playImg
                    anchors.centerIn: parent

                    sourceSize: {
                        if (playBtn.height % 2 !== 0)
                            Qt.size(oddSize(parent.width * .65),
                                    oddSize(parent.width * .65))
                        else
                            Qt.size(evenSize(parent.width * .65),
                                    evenSize(parent.width * .65))
                    }
                    source: 'qrc:/Resource/play_icon.svg'
                    visible: false
                }
                ColorOverlay {
                    cached: true
                    anchors.fill: playImg

                    source: playImg
                    clip: true
                    antialiasing: true
                    color: !_clipArea.paused ? theme.button_clicked_color : theme.button_unclicked_color
                }
                onSendMessage: {
                    if (msg === "pressed") {
                        _clipArea.play()
                    }
                }
            }

            TrackButton {
                id: stopBtn

                Layout.maximumHeight: scaleSize2(58)
                Layout.minimumHeight: scaleSize2(58)

                Layout.minimumWidth: scaleSize2(66)
                Layout.maximumWidth: scaleSize2(66)
                Layout.alignment: Qt.AlignHCenter

                fillStyle: isPressed ? theme.b41_Enable_FIL : theme.b10_Disable_FIL
                brColor: isPressed ? theme.b41_Enable_BRD : theme.b10_Disable_BRD

                Image {
                    id: stopImg
                    anchors.centerIn: parent
                    sourceSize: {
                        if (stopBtn.height % 2 !== 0)
                            Qt.size(oddSize(parent.width * .60),
                                    oddSize(parent.width * .60))
                        else
                            Qt.size(evenSize(parent.width * .60),
                                    evenSize(parent.width * .60))
                    }
                    source: 'qrc:/Resource/stop_icon.svg'
                    visible: false
                }
                ColorOverlay {
                    cached: true
                    anchors.fill: stopImg

                    source: stopImg
                    clip: true
                    antialiasing: true
                    color: stopBtn.isPressed ? theme.button_clicked_color : theme.button_unclicked_color
                }
                onSendMessage: {
                    if (msg === "pressed") {
                        _clipArea.stop()
                    }
                }
            }

            TrackButton {
                id: recBtn

                Layout.maximumHeight: scaleSize2(58)
                Layout.minimumHeight: scaleSize2(58)

                Layout.minimumWidth: scaleSize2(66)
                Layout.maximumWidth: scaleSize2(66)
                Layout.alignment: Qt.AlignHCenter

                fillStyle: isClicked ? theme.b80_enable_FIL : theme.b10_Disable_FIL
                brColor: isClicked ? theme.b80_enable_BRD : theme.b10_Disable_BRD

                Image {
                    id: recImg
                    anchors.centerIn: parent
                    sourceSize: {
                        if (recBtn.height % 2 !== 0)
                            Qt.size(oddSize(parent.width * .35),
                                    oddSize(parent.width * .35))
                        else
                            Qt.size(evenSize(parent.width * .35),
                                    evenSize(parent.width * .35))
                    }
                    source: 'qrc:/Resource/rec_icon.svg'
                    visible: false
                }
                ColorOverlay {
                    cached: true
                    anchors.fill: recImg

                    source: recImg
                    clip: true
                    antialiasing: true
                    color: recBtn.isClicked ? theme.button_clicked_color : theme.button_unclicked_color
                }
            }
            TrackButton {
                id: loopBtn

                Layout.maximumHeight: scaleSize2(58)
                Layout.minimumHeight: scaleSize2(58)

                Layout.minimumWidth: scaleSize2(66)
                Layout.maximumWidth: scaleSize2(66)
                Layout.alignment: Qt.AlignHCenter

                fillStyle: _areaInfo.loopActive ? theme.b12_Enable2_FIL : theme.b10_Disable_FIL
                brColor: _areaInfo.loopActive ? theme.b12_Enable2_BRD : theme.b10_Disable_BRD

                Image {
                    id: loopImg
                    anchors.centerIn: parent
                    sourceSize: {
                        if (loopBtn.height % 2 !== 0)
                            Qt.size(oddSize(parent.width * .65),
                                    oddSize(parent.width * .65))
                        else
                            Qt.size(evenSize(parent.width * .65),
                                    evenSize(parent.width * .65))
                    }
                    source: 'qrc:/Resource/loop_icon.svg'
                    visible: false
                }
                ColorOverlay {
                    cached: true
                    anchors.fill: loopImg

                    source: loopImg
                    clip: true
                    antialiasing: true
                    color: _areaInfo.loopActive ? theme.button_clicked_color : theme.button_unclicked_color
                }

                onSendMessage: {
                    if (msg === "pressed") {
                        _areaInfo.loopActive = !_areaInfo.loopActive
                    }
                }
            }
        }
        Rectangle {
            id: blueRect

            property real s: 1.6

            radius: radiusCal(3) * s
            color: "#1E1E1E"

            Layout.preferredHeight: scaleSize2(52) * s

            Layout.alignment: Qt.AlignVCenter

            // padding widened
            width: childrenRect.width
            implicitWidth: childrenRect.width

            border.width: topWidth * 0
            border.color: "#1E1E1E"

            Connections {
                target: _areaInfo
                onTimeArrangementStartChanged: {

                    info1.value = info1.mapValue(
                                _areaInfo.timeArrangementStart[0])
                    info1.text = _areaInfo.timeArrangementStart[0]

                    info2.value = info2.mapValue(
                                _areaInfo.timeArrangementStart[1])
                    info2.text = _areaInfo.timeArrangementStart[1]

                    info3.value = info3.mapValue(
                                _areaInfo.timeArrangementStart[2])
                    info3.text = _areaInfo.timeArrangementStart[2]
                }
            }

            InfoText3 {
                id: info1

                minValue: 1
                maxValue: 9999
                decimal: 0
                value: mapValue(_areaInfo.timeArrangementStart[0])
                text: _areaInfo.timeArrangementStart[0]
                defaultValue: mapValue(_areaInfo.timeArrangementStart[0])
                externalValue: true

                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(40 * 2) * blueRect.s
                height: parent.height

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name

                onSigValueChanged: {

                    if (diff != 0) {
                        _areaInfo.changeArrangementStartTimeBar(diff > 0)
                    }
                }
            }

            // Second text
            InfoText3 {
                id: info2

                minValue: 1
                maxValue: 4
                decimal: 0
                value: mapValue(_areaInfo.timeArrangementStart[1])
                text: _areaInfo.timeArrangementStart[1]
                defaultValue: mapValue(_areaInfo.timeArrangementStart[1])
                externalValue: true
                noramlScaleCoeff: 10
                shiftScaleCoeff: 20

                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(38) * blueRect.s
                height: parent.height

                anchors.left: info1.right
                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name

                onSigValueChanged: {

                    if (diff != 0)
                        _areaInfo.changeArrangementStartTimeBeat(diff > 0)
                }
            }

            // Third text
            InfoText3 {
                id: info3

                minValue: 1
                maxValue: 4
                decimal: 0
                value: mapValue(_areaInfo.timeArrangementStart[2])
                text: _areaInfo.timeArrangementStart[2]
                defaultValue: mapValue(_areaInfo.timeArrangementStart[2])
                externalValue: true
                noramlScaleCoeff: 10
                shiftScaleCoeff: 20

                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(38) * blueRect.s
                height: parent.height

                anchors.left: info2.right

                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name

                onSigValueChanged: {

                    if (diff != 0)
                        _areaInfo.changeArrangementStartTimeSixteenth(diff > 0)
                }
            }

            // Separator after numbers
            Rectangle {
                id: sep1
                width: Math.max(1, Math.round(1 * blueRect.s))
                color: "#333333"

                anchors.left: info3.right

                anchors.verticalCenter: parent.verticalCenter
                height: parent.height - 2 * scaleSize2(5) * blueRect.s
            }

            // D#
            InfoText3 {
                id: infoKey

                text: "D"
                fixText: true
                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(28) * blueRect.s
                height: parent.height

                anchors.left: sep1.right

                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name
            }

            InfoText3 {
                id: infoKey2

                text: "#"
                fixText: true
                fontSize: scaleSize2(20) * blueRect.s
                width: scaleSize2(20) * blueRect.s
                height: parent.height

                anchors.left: infoKey.right
                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name
            }

            // min
            InfoText3 {
                id: infoScale

                text: "min"
                fixText: true
                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(56) * blueRect.s
                height: parent.height

                anchors.left: infoKey2.right

                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name
            }

            // Separator after scale
            Rectangle {
                id: sep2
                width: Math.max(1, Math.round(1 * blueRect.s))
                color: "#333333"

                anchors.left: infoScale.right

                anchors.verticalCenter: parent.verticalCenter
                height: parent.height - 2 * scaleSize2(5) * blueRect.s
            }

            // 4/4
            InfoText3 {
                id: infoTimeSigNum1

                minValue: 1
                maxValue: 99
                decimal: 0
                value: mapValue(4)
                defaultValue: mapValue(4)
                text: "4"

                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(38) * blueRect.s
                height: parent.height

                anchors.left: sep2.right

                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name

                onValueChanged: {

                    _clipArea.beatPerBar = Math.round(mapReverse(value))
                }
            }

            InfoText3 {
                id: infoTimeSigSlash

                text: "/"
                fixText: true
                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(20) * blueRect.s
                height: parent.height

                anchors.left: infoTimeSigNum1.right

                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name
            }

            InfoText3 {
                id: infoTimeSigNum2

                minValue: 0
                maxValue: 4
                decimal: 0
                value: 25
                text: "2"
                defaultValue: 25
                noramlScaleCoeff: 10
                shiftScaleCoeff: 20

                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(38) * blueRect.s
                height: parent.height

                valueTransform: function (v) {
                    return Math.pow(2, Math.floor(v)) // using parseInt
                }
                anchors.left: infoTimeSigSlash.right

                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name

                onValueChanged: {
                    _clipArea.lenghtOfBeat = Math.pow(2, Math.round(
                                                          mapReverse(value)))
                }
            }

            // Separator after time signature
            Rectangle {
                id: sep3
                width: Math.max(1, Math.round(1 * blueRect.s))
                color: "#333333"

                anchors.left: infoTimeSigNum2.right
                anchors.verticalCenter: parent.verticalCenter
                height: parent.height - 2 * scaleSize2(5) * blueRect.s
            }

            // ♩=180.00
            InfoText3 {
                id: infoTempoNote
                text: "♩"
                fixText: true
                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(28) * blueRect.s
                height: parent.height

                anchors.left: sep3.right

                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name
            }

            InfoText3 {
                id: infoTempoEq

                text: "="
                fixText: true
                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(28) * blueRect.s
                height: parent.height

                anchors.left: infoTempoNote.right
                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name
            }

            InfoText3 {
                id: infoTempoValue
                minValue: 20
                maxValue: 999
                decimal: 2
                value: mapValue(_clipArea.tempo)
                defaultValue: mapValue(_clipArea.tempo)
                text: _clipArea.tempo.toFixed(decimal)

                fontSize: scaleSize2(28) * blueRect.s
                width: scaleSize2(100) * blueRect.s
                height: parent.height

                anchors.left: infoTempoEq.right
                anchors.verticalCenter: parent.verticalCenter
                fontFamily: swisFont.name
                onValueChanged: {
                    _clipArea.tempo = mapReverse(value)
                }
            }
        }
    }
}
