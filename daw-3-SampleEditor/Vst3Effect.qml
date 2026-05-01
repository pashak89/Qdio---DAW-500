import QtQuick 2.3
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Vst3Effect 1.0
import EffectChain 1.0

Frame {
    id: frame

    height: scrollView.visible ? scrollView.contentHeight + 2
                                 * parameters.height : 2 * parameters.height

    property bool controlVisible: false
    property int trackIndex: -1

    property string name
    property string type

    onTrackIndexChanged: {

        if (trackIndex >= 0) {

            repeater.model = _clipArea.tracksModel.effectChain(
                        trackIndex).getEffect(name).controls


        }
    }

    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            id: header
            Layout.fillWidth: true
            Layout.preferredHeight: parameters.height
            RoundButton {
                text: "Enable"
                checkable: true
                highlighted: checked
                checked: model.enabled
                onClicked: {
                    model.enabled = !model.enabled
                }
            }
            RoundButton {
                id: configure
                text: "Configure"
                checkable: true
                highlighted: checked

                checked: false
                onClicked: {
                    _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                                name).setConfigure(checked)
                }
            }
            RoundButton {
                id: parameters
                text: "Parameters"
                checkable: true
                highlighted: checked
                checked: false
            }
            RoundButton {
                text: "Control"
                id: controlButton

                checkable: true
                highlighted: checked
                checked: controlVisible
                onClicked: {
                    if (controlVisible === false) {
                        _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                                    name).showControl()
                    } else {

                        _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                                    name).hideControl()
                    }
                }
            }
            Label {

                Layout.fillWidth: true
                text: frame.name
                font.pixelSize: 10
            }
            RoundButton {
                text: "Remove"
                onClicked: {
                    _clipArea.removeVst3Effect(trackIndex, model.index)
                }
            }
            Item {
                Layout.fillWidth: true
            }

            RoundButton {
                text: "UP"
                onClicked: {

                    _clipArea.tracksModel.effectChain(trackIndex).up(model.index)
                }
            }
            RoundButton {
                text: "Down"
                onClicked: {

                    _clipArea.tracksModel.effectChain(trackIndex).down(
                                model.index)
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "white"
        }

        ScrollView {
            id: scrollView
            clip: true
            visible: parameters.checked
            Layout.maximumWidth: frame.width
            Layout.maximumHeight: frame.height

            Layout.fillHeight: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            GridLayout {
                id: gridmain
                anchors.fill: parent
                columns: 3
                rowSpacing: 5
                columnSpacing: 5
                Repeater {
                    id: repeater
                    delegate: ColumnLayout {
                        spacing: 0
                        visible: model.name !== "DeviceOn"
                        RowLayout {
                            Label {
                                text: name
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.alignment: Qt.AlignLeft
                                horizontalAlignment: Text.AlignHCenter
                            }
                            Label {
                                text: value
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.alignment: Qt.AlignHCenter
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        Slider {
                            Layout.preferredWidth: 120
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            stepSize: 1
                            from: 0
                            to: 100
                            value: model.normalized * 100
                            onMoved: {
                                model.normalized = value / 100.0
                            }
                        }
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
