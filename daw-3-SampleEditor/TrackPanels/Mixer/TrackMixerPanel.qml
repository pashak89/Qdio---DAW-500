// TrackPanel/TrackMixerPanel.qml
import QtQuick.Controls 2.15
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

import "../../Items"

import "../Audio"
import Global 1.0
import "components"
import "../.." as App

Item {
    id: root
    property int _width: Global.scaleSize2(300)
    property int minWidth: Global.scaleSize2(300)

    implicitWidth: _width + 2 * Global.scaleSize2(20)
    property real currentItemHeight: 0
    implicitHeight: root.currentItemHeight + 2 * Global.scaleSize2(20)

    width: implicitWidth
    height: implicitHeight

    // --- Inputs ---
    property string title: "27L"
    property real meterL: 0.88
    property real meterR: 0.95
    property bool holdShift: false

    property bool holdCtrl: _areaInfo.ctrlkeyPressed

    property var cursorPosition: cursorPositionClass
    property var resolution: cursorPosition.getResolution()

    property real refSize: _areaInfo.zoomFactor
    property real scale: cursorPosition.getScale()
    property int currentIndex: _areaInfo.selectedTrackItem

    // theme
    App.Theme1 {
        id: theme1
    }
    App.Theme2 {
        id: theme2
    }
    property Item theme: _areaInfo.themeType === 0 ? theme1 : theme2

    FontLoader {
        id: myFont2
        source: "qrc:/Resource/font/SourceSansPro-Regular.ttf"
    }
    property string topFontFamily: myFont2.name
    property int topfontPixelSize: Global.scaleSize2(28)
    property real topRadius: Global.radiusCal(3)
    property int topWidth: resolution[0] >= 3840
                           && resolution[1] >= 2170 ? ((refSize < .83) ? 1 : (refSize < 1.12) ? 2 : 2) : ((refSize < .83) ? 1 : (refSize < 1.12) ? 1 : 2)

    // keep ScaleUtils in sync
    Component.onCompleted: {
        Global.scale = scale
        Global.refSize = refSize
    }
    onScaleChanged: Global.scale = scale
    onRefSizeChanged: Global.refSize = refSize

    PluginPopup {
        id: pluginPopup_
        theme: root.theme
        topfontPixelSize: root.topfontPixelSize
        topFontFamily: root.topFontFamily

        clipArea: _clipArea
        areaInfo: _areaInfo
    }

    Repeater {
        id: repeaterMainColumn
        model: _clipArea.tracksModel

        delegate: Column {

            IOPopup {
                id: ioPopup_

                clipArea: _clipArea
                areaInfo: _areaInfo

                onChannelCountChanged: {
                    model.outputChannelsCount = value
                    model.inputChannelsCount = value
                }
            }

            id: mainColumn
            visible: index === root.currentIndex

            onHeightChanged: if (visible)
                                 root.currentItemHeight = mainColumn.implicitHeight
            Component.onCompleted: if (visible)
                                       root.currentItemHeight = mainColumn.implicitHeight
            onVisibleChanged: if (visible)
                                  root.currentItemHeight = mainColumn.implicitHeight

            x: Global.scaleSize2(20)
            y: Global.scaleSize2(20)
            width: root._width
            spacing: Global.scaleSize2(10)

            IOSection {

                effectModel: _clipArea.tracksModel.effectChain(
                                 _areaInfo.selectedTrackItem)
                draggingEnabled: root.holdCtrl
                ioPopup: ioPopup_
                clipArea: _clipArea
                areaInfo: _areaInfo
            }

            Equalizer {
                id: wavePanel
                width: parent.width
                height: Global.scaleSize2(100)
            }

            FxSection {

                effectModel: _clipArea.tracksModel.effectChain(
                                 _areaInfo.selectedTrackItem)
                draggingEnabled: root.holdCtrl
                pluginPopup: pluginPopup_
                clipArea: _clipArea
                areaInfo: _areaInfo
            }

            SendsSection {
                effectModel: _clipArea.tracksModel.effectChain(
                                 _areaInfo.selectedTrackItem)

                title: "L Sends"
                pluginPopup: pluginPopup_
                clipArea: _clipArea
                areaInfo: _areaInfo
                draggingEnabled: root.holdCtrl
            }

            SendsSection {
                title: "G Sends"

                draggingEnabled: root.holdCtrl
            }
            PanField {

                clipArea: _clipArea
            }

            VolumeFields {

                clipArea: _clipArea
                truePeak: mainStrip.maxTruePeak
            }

            MainStrip {
                id: mainStrip
                clipArea: _clipArea
                meterL: root.meterL
                meterR: root.meterR
            }

            TrackButtonsRow {
                clipArea: _clipArea
            }

            DisabledPanField {}
        }
    }
}
