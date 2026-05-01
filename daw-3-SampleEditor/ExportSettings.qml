import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    spacing: 2
    signal cancel
    signal exports
    property bool mono: monoCheckBox.checked
    property int bitDepth: bitDepthComboBox.currentValue
    property int samplerate: sampleRateComboBox.currentValue
    GroupBox {
        title: "Selection"
        Layout.fillHeight: true
        Layout.fillWidth: true
        ColumnLayout {
            spacing: 2
            anchors.fill: parent

            ComboBox {
                id: renderedTrack
                model: ["Main"]
                currentIndex: 0
                editable: false
                Layout.fillWidth: true
            }

            RowLayout {
                spacing: 2
                Layout.fillHeight: true
                Layout.fillWidth: true

                Label {
                    text: "Render Start"
                    Layout.fillWidth: true
                }
                TextField {
                    text: _areaInfo.timeSelectionStart[0]
                    Layout.maximumWidth: 40
                }
                TextField {
                    text: _areaInfo.timeSelectionStart[1]
                    Layout.maximumWidth: 40
                }
                TextField {
                    text: _areaInfo.timeSelectionStart[2]
                    Layout.maximumWidth: 40
                }
            }

            RowLayout {
                spacing: 2
                Layout.fillHeight: true
                Layout.fillWidth: true

                Label {
                    text: "Render Length"
                    Layout.fillWidth: true
                }
                TextField {
                    text: _areaInfo.timeSelectionLength[0]
                    Layout.maximumWidth: 40
                }
                TextField {
                    text: _areaInfo.timeSelectionLength[1]
                    Layout.maximumWidth: 40
                }
                TextField {
                    text: _areaInfo.timeSelectionLength[2]
                    Layout.maximumWidth: 40
                }
            }
        }
    }

    GroupBox {
        title: "Rendering Options"
        Layout.fillHeight: true
        Layout.fillWidth: true
        ColumnLayout {
            spacing: 2
            anchors.fill: parent

            //            CheckBox {
            //                text: "Include Return and Main Effects"
            //            }
            //            CheckBox {
            //                text: "Render as Loop"
            //            }
            CheckBox {
                id: monoCheckBox
                text: "Convert to Mono"
            }
            //            CheckBox {
            //                text: "Normalize"
            //            }
            //            CheckBox {
            //                text: "Create Analysis File"
            //                checked: true
            //            }
            RowLayout {
                spacing: 2
                Layout.fillHeight: true
                Layout.fillWidth: true
                Label {
                    text: "Sample Rate"
                }
                ComboBox {
                    id: sampleRateComboBox
                    model: ["22050", "32000", "44100", "48000", "88200", "96000", "176400", "192000"]
                    currentIndex: 2
                }
            }
        }
    }

    GroupBox {
        title: "PCM"
        Layout.fillHeight: true
        Layout.fillWidth: true
        ColumnLayout {
            spacing: 2
            anchors.fill: parent

            CheckBox {
                text: "Encode PCM"
                checked: true
            }

            RowLayout {
                spacing: 2
                Layout.fillHeight: true
                Layout.fillWidth: true
                Label {
                    text: "File Type"
                }
                ComboBox {
                    model: ["WAV" /*, "AIFF", "FLAC"*/
                    ]
                    currentIndex: 0
                }
            }

            RowLayout {
                spacing: 2
                Layout.fillHeight: true
                Layout.fillWidth: true
                Label {
                    text: "Bit Depth"
                }
                ComboBox {
                    id: bitDepthComboBox
                    model: ["16", "24", "32"]
                    currentIndex: 0
                }
            }

            RowLayout {
                spacing: 2
                Layout.fillHeight: true
                Layout.fillWidth: true
                Label {
                    text: "Dither Options"
                }
                ComboBox {
                    model: ["No Dither", "Rectangler", "Triangular"]
                    currentIndex: 2
                }
            }
        }
    }

    //    GroupBox {
    //        title: "MP3"
    //        Layout.fillHeight: true
    //        Layout.fillWidth: true
    //        ColumnLayout {
    //            spacing: 2
    //            anchors.fill: parent
    //            CheckBox {
    //                text: "Encode MP3 (CBR 320)"
    //            }
    //        }
    //    }
    RowLayout {
        spacing: 2
        Layout.fillHeight: true
        Layout.fillWidth: true
        Button {
            text: "Export"
            onClicked: {
                exports()
            }
        }
        Button {
            text: "Cancel"
            onClicked: {
                cancel()
            }
        }
    }
}
