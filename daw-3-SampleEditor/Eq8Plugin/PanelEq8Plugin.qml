import QtQuick 2.15
import QtQuick.Controls 2.15
import Tools 1.0
import "Item/ButtonCombobox"
import "../"
import "qrc:/TrackPanels/Audio"
import "qrc:/TrackPanels"
import "qrc:/Items"

Item {

    id: root

    Theme1 {
        id: theme1
    }
    Theme2 {
        id: theme2
    }
    property alias tabIndex: settingsTabEq8.tabIndex
    property var cursorPosition: cursorPositionClass
    property var resolution: cursorPosition.getResolution()
    property point positionInWindow
    property string topFontFamily: myFont2.name
    property int topfontPixelSize: scaleSize(28)
    property real topRadius: radiusCal(3)
    property int topWidth: resolution[0] >= 3840 & resolution[1]
                           >= 2170 ? ((refSize < .83) ? 1 : (refSize < 1.12) ? 2 : 2) : ((refSize < .83) ? 1 : (refSize < 1.12) ? 1 : 2)

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
    function scaleSize(value) {
        value = Math.round(value * _scale * refSize / 2)
        return value
    }
    property Item theme: _areaInfo.themeType === 0 ? theme1 : theme2

    property bool trackEnable: wavePanel.afilterVisualizer.enabled
    FontLoader {
        id: myFont2
        source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf'
    }
    property var filterVisualizer: wavePanel.afilterVisualizer

    Button_Combobox {
        id: dropMenuBtn
        width: parent.width
        height: scaleSize(54)

        checked: wavePanel.afilterVisualizer.enabled == true
        onClicked: {

            _clipArea.tracksModel.saveUndoNoramized(trackIndex, "EQ",
                                                   "DeviceOn")

            filterVisualizer.deviceOnEnable(!filterVisualizer.enabled)

            if (wavePanel.afilterVisualizer.enabled === false)
                wavePanel.afilterVisualizer.enabled = true
            else
                wavePanel.afilterVisualizer.enabled = false
        }
    }

    Rectangle {
        id: background

        anchors {
            top: dropMenuBtn.bottom
            topMargin: scaleSize(13)
            bottom: parent.bottom
        }
        width: parent.width

        color: theme.bg1
        radius: scaleSize(10)

        WavePanel {
            id: wavePanel
        }

        SettingsTabEq8 {
            id: settingsTabEq8
            anchors {
                left: parent.left
                leftMargin: scaleSize(8)
                right: parent.right
                rightMargin: scaleSize(8)
                bottom: parent.bottom
                bottomMargin: scaleSize(12)
                top: wavePanel.bottom
                topMargin: scaleSize(12)
            }
        }
    }

    Popup {
        id: rightPanel
        visible: false
        width: rPanel.parentWidth
        height: rPanel.parentHeight + rPanel.margin * 2

        //color: 'transparent'

        //signal action(int index,string text)
        //flags: Qt.Popup | Qt.Dialog | Qt.FramelessWindowHint
        x: positionInWindow.x
        y: positionInWindow.y

        property var rightModules: ['', '']
        property var leftModules: ['Show Automation', 'Show Automation in New Lane']
        property alias panelWidth: rPanel.parentWidth
        property alias isTwoGroups: rPanel.twoGroups
        property alias firstStr: rPanel.firstGroup
        property int index: 0
        property var callback

        property alias cc: rPanel.panel

        onVisibleChanged: {

            rPanel.resize()
        }

        margins: 0
        padding: 0
        contentItem: RightPanel {
            id: rPanel
            anchors.fill: parent

            onAction: {
                rightPanel.callback(rightPanel.index, text)
            }
        }
    }
}
