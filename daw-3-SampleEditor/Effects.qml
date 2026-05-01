import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQml 2.2
import QtQuick.Dialogs 1.2
import ScreenInterface 1.0
import SampleEditor 1.0
import AudioManager 1.0
import AudioEngine 1.0
import AudioDevice 1.0

import Tools 1.0
import BottomArea 1.0
import TopArea 1.0

import FilterVisualizer 1.0
//import MultiEQ 1.0
import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import QtQml 2.12
import Vst3PluginManager 1.0

ColumnLayout {
    RowLayout {
        ComboBox {
            id: comboBox
            model: ListModel {
                id: model
                ListElement {
                    text: "EQ"
                }
                ListElement {
                    text: "VISR"
                }
            }
        }
        Button {
            text: "ADD"
            onClicked: {
                //                if (comboBox.currentIndex == 0) {
                //                    if (_areaInfo.selectedTrackItem >= 0) {
                //                        _clipArea.addMultiEqEffect(_areaInfo.selectedTrackItem)

                //                        _clipArea.saveAddEffectInUndo(
                //                                    _areaInfo.selectedTrackItem, "eq")
                //                    }
                //                }
                if (comboBox.currentIndex == 1) {
                    if (_areaInfo.selectedTrackItem >= 0) {

                        _clipArea.saveAddEffectInUndo(
                                    _areaInfo.selectedTrackItem, "visr")

                        _clipArea.addVisr(_areaInfo.selectedTrackItem)
                    }
                }
            }
        }
        Button {
            text: "Remove"
            onClicked: {
                if (comboBox.currentIndex == 0) {
                    if (_areaInfo.selectedTrackItem >= 0) {
                        _clipArea.removeMultiEqEffect(
                                    _areaInfo.selectedTrackItem)

                        _clipArea.saveRemoveEffectInUndo(
                                    _areaInfo.selectedTrackItem, "eq")
                    }
                }
                if (comboBox.currentIndex == 1) {
                    if (_areaInfo.selectedTrackItem >= 0) {
                        _clipArea.removeVisr(_areaInfo.selectedTrackItem)

                        _clipArea.saveRemoveEffectInUndo(
                                    _areaInfo.selectedTrackItem, "visr")
                    }
                }
            }
        }
        Button {
            text: "Rescan"
            onClicked: {
                vst3.rescanEffects()
            }
        }
    }
    ColumnLayout {

        ProgressBar {
            indeterminate: true
            visible: vst3.effectIsLoading
            Layout.fillWidth: true
        }
        EffectView {
            model: vst3.vst3modules
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            onRowDoubleClicked: {

                var i = vst3.vst3modules.getTreeItemIndex(index)
                var path = vst3.vst3modules.getTreeItemPath(index)
                var name = vst3.vst3modules.getTreeItemName(index)

                _clipArea.addVst3Effect(_areaInfo.selectedTrackItem,
                                        path, name, i)
            }
        }
        EffectView {
            model: vst3.vst2modules
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            onRowDoubleClicked: {

                var i = vst3.vst2modules.getTreeItemIndex(index)
                var path = vst3.vst2modules.getTreeItemPath(index)
                var name = vst3.vst3modules.getTreeItemName(index)

                _clipArea.addVst3Effect(_areaInfo.selectedTrackItem,
                                        path, name, i)
            }
        }
    }

    Connections {
        target: _clipArea
        onSigEffectLoading: {
            progressBar.visible = progress
        }
    }

    ProgressBar {
        id: progressBar
        indeterminate: true
        visible: false
        Layout.fillWidth: true
    }
    ListView {
        id: listView
        Layout.fillHeight: true
        Layout.fillWidth: true
        interactive: false
        model: _clipArea.tracksModel
        delegate: ColumnLayout {

            width: listView.width
            height: _areaInfo.selectedTrackItem === index ? listView1.contentHeight : 0
            visible: _areaInfo.selectedTrackItem === index

            ListView {
                id: listView1
                interactive: false
                Layout.fillWidth: true
                Layout.preferredHeight: listView1.contentHeight
                model: _clipArea.tracksModel.effectChain(index)
                delegate: Column {
                    EQ {
                        name: model.name
                        enabled: model.type === "EQ"
                        visible: model.type === "EQ"
                        trackIndex: model.TrackIndex
                    }
                    Vst3Effect {
                        width: listView.width
                        name: model.name
                        controlVisible: model.controlVisible
                        enabled: model.type === "Vst3"
                        visible: model.type === "Vst3"
                        trackIndex: model.TrackIndex
                    }
                }
            }
            Visr {
                id: visrItem
                trackIndex: index
                enabled: visr
                visible: visr
            }
        }
    }
}
