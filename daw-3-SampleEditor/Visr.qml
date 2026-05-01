import QtQuick 2.3
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import "Items"

ColumnLayout {
    property int trackIndex: -1

    // property int cHeight: contentItem.height
    Layout.fillHeight: true
    Layout.fillWidth: true
    enabled: false

    //    onTrackIndexChanged: {
    //        if (trackIndex >= 0) {
    //            // button.enabled = clipArea.tracksModel.visrEffect(trackIndex).enabled
    //            connection1.target = clipArea.tracksModel.visrEffect(trackIndex)
    //        }
    //    }

    //    Connections {
    //        id: connection1
    //        target: clipArea.tracksModel.visrEffect(trackIndex)
    //        onEnabledChanged: {
    //            console.log(effectEnabled,
    //                        clipArea.tracksModel.visrEffect(trackIndex).enabled)
    //        }
    //    }
    spacing: 5
    RowLayout {
        Layout.fillWidth: true
        Button {
            id: button
            text: "Enable"
            checkable: true
            highlighted: checked
            Layout.fillWidth: true
            checked: false
            onClicked: {

                _clipArea.tracksModel.visrEffect(
                            trackIndex).enabled = !_clipArea.tracksModel.visrEffect(
                            trackIndex).enabled
            }
        }
        Button {
            text: "Reload"
            Layout.fillWidth: true
            onClicked: {
                _clipArea.tracksModel.visrEffect(trackIndex).reload()

                if (_clipArea.tracksModel.visrEffect(trackIndex).reverbEnabled) {

                    coeffRepater.model = _clipArea.tracksModel.visrEffect(
                                trackIndex).reverbFilters

                    lateReverbLevels.model = _clipArea.tracksModel.visrEffect(
                                trackIndex).lateReverbLevels()

                    lateReverbAttacktime.model = _clipArea.tracksModel.visrEffect(
                                trackIndex).lateReverbAttackTime()

                    lateReverbDecayconst.model = _clipArea.tracksModel.visrEffect(
                                trackIndex).lateReverbDecayconst()

                    lateReverbDelay.text = _clipArea.tracksModel.visrEffect(
                                trackIndex).lateReverbDelay()

                    discreteReflectionPositionX.text = _clipArea.tracksModel.visrEffect(
                                trackIndex).getDiscreteReflectionPositionX(0)

                    discreteReflectionPositionY.text = _clipArea.tracksModel.visrEffect(
                                trackIndex).getDiscreteReflectionPositionY(0)

                    discreteReflectionPositionZ.text = _clipArea.tracksModel.visrEffect(
                                trackIndex).getDiscreteReflectionPositionZ(0)

                    discreteReflectionLevel.text = _clipArea.tracksModel.visrEffect(
                                trackIndex).getDiscreteReflectionLevel(0)

                    discreteReflectionDelay.text = _clipArea.tracksModel.visrEffect(
                                trackIndex).getDiscreteReflectionDelay(0)
                }
            }
        }
        Button {
            id: loadButton
            text: "Load Speaker Config File"
            Layout.fillWidth: true
            onClicked: {
                fileDialog_xml.open()
            }
        }
    }
    RowLayout {
        Layout.fillWidth: true
        Label {
            id: loadSpeakerFile
            text: "File: "
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
        }
        Label {
            id: speakerCount
            Layout.fillWidth: true
        }
    }

    onEnabledChanged: {
        connection.target = _clipArea.tracksModel.visrEffect(trackIndex)
        loadSpeakerFile.text = ""
        listView.model = null

        //listViewReverb.model = null
        if (enabled) {

            button.checked = _clipArea.tracksModel.visrEffect(trackIndex).enabled

            reverb.checked = _clipArea.tracksModel.visrEffect(
                        trackIndex).reverbEnabled

            sliderX.value = parseFloat(_clipArea.tracksModel.visrEffect(
                                           trackIndex).getValue("PosX")) * 100
            sliderY.value = parseFloat(_clipArea.tracksModel.visrEffect(
                                           trackIndex).getValue("PosY")) * 100
            sliderZ.value = parseFloat(clipArea.tracksModel.visrEffect(
                                           trackIndex).getValue("PosZ")) * 100

            speakerCount.text = "Speaker: " + _clipArea.tracksModel.visrEffect(
                        trackIndex).speakerCount
            loadSpeakerFile.text = "File: " + _clipArea.tracksModel.visrEffect(
                        trackIndex).loadSpeakerConfig

            listView.model = _clipArea.tracksModel.visrEffect(
                        trackIndex).speakerModel

            objectCreator.setObjectLocation(trackIndex, sliderX.value / 100.0,
                                            sliderY.value / 100.0,
                                            sliderZ.value / 100.0)
        }
    }

    Connections {
        id: connection
        target: _clipArea.tracksModel.visrEffect(trackIndex)

        onPositionChanged: {

            sliderX.value = parseFloat(_clipArea.tracksModel.visrEffect(
                                           trackIndex).getValue("PosX")) * 100
            sliderY.value = parseFloat(_clipArea.tracksModel.visrEffect(
                                           trackIndex).getValue("PosY")) * 100
            sliderZ.value = parseFloat(_clipArea.tracksModel.visrEffect(
                                           trackIndex).getValue("PosZ")) * 100

            objectCreator.setObjectLocation(trackIndex, sliderX.value / 100.0,
                                            sliderY.value / 100.0,
                                            sliderZ.value / 100.0)
            //console.log(sliderX.value,sliderY.value,sliderZ.value)
        }
    }

    RowLayout {
        id: posX
        Layout.fillWidth: true

        Label {
            text: "PosX: " + (sliderX.value / 100).toFixed(2)
        }

        Slider {
            id: sliderX
            Layout.fillWidth: true
            stepSize: 1
            enabled: true
            from: -10000
            to: 10000
            value: 0
            onMoved: {
                _clipArea.tracksModel.visrEffect(trackIndex).setValue(
                            "PosX", value / 100.0)

                objectCreator.setObjectLocation(trackIndex, sliderX.value / 100.0,
                                                sliderY.value / 100.0,
                                                sliderZ.value / 100.0)
            }
        }
    }
    RowLayout {
        id: posY
        Layout.fillWidth: true
        Label {
            text: "PosY: " + (sliderY.value / 100).toFixed(2)
        }

        Slider {
            Layout.fillWidth: true
            id: sliderY
            stepSize: 1
            enabled: true
            from: -10000
            to: 10000
            value: 0
            onMoved: {
                _clipArea.tracksModel.visrEffect(trackIndex).setValue(
                            "PosY", value / 100.0)

                objectCreator.setObjectLocation(trackIndex, sliderX.value / 100.0,
                                                sliderY.value / 100.0,
                                                sliderZ.value / 100.0)
            }
        }
    }
    RowLayout {
        id: posZ
        Layout.fillWidth: true
        Label {
            text: "PosZ: " + (sliderZ.value / 100.0).toFixed(2)
        }

        Slider {
            Layout.fillWidth: true
            id: sliderZ
            stepSize: 1
            enabled: true
            from: -10000
            to: 10000
            value: 0
            onMoved: {
                _clipArea.tracksModel.visrEffect(trackIndex).setValue(
                            "PosZ", value / 100.0)

                objectCreator.setObjectLocation(trackIndex, sliderX.value / 100.0,
                                                sliderY.value / 100.0,
                                                sliderZ.value / 100.0)
            }
        }
    }

    GridView {
        id: listView
        cellWidth: 250
        cellHeight: cHeight
        Layout.minimumHeight: count * cHeight / 2
        Layout.maximumHeight: count * cHeight / 2
        Layout.fillWidth: true
        property int cHeight: 40

        delegate: RowLayout {
            id: rect
            height: listView.cHeight
            spacing: 0
            CheckBox {
                id: checkBox
                checked: model.SpeakerChecked
                font.pixelSize: 15
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: "Speaker " + model.SpeakerName
                onClicked: {
                    model.SpeakerChecked = checked
                }
            }
            SpinBox {
                from: 0
                editable: true
                Layout.fillHeight: true
                to: _engine.audioEngine.channels - 1
                value: model.SpeakerIndex
                Layout.maximumWidth: listView.cHeight
                up.indicator: null
                down.indicator: null
            }
        }
    }

    RowLayout {
        Layout.fillHeight: true
        CheckBox {
            text: "Frequency Dependent Panning"
            Layout.fillWidth: true
        }
        CheckBox {
            id: reverb
            text: "Reverb"
            Layout.fillWidth: true
            enabled: _clipArea.paused
            checked: false
            onClicked: {
                _clipArea.tracksModel.visrEffect(
                            trackIndex).reverbEnabled = reverb.checked
            }
        }
        Button {
            text: "ReverbConfig"
            enabled: _clipArea.paused
            Layout.fillWidth: true
            onClicked: {
                reverbConfig.open()
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        id: reverbFrame
        enabled: reverb.checked

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 30
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Label {
                    text: "ReverbObjects:"
                }
                SpinBox {
                    from: 0
                    editable: true
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    to: 10
                    value: 1
                    onValueChanged: {
                        _clipArea.tracksModel.visrEffect(
                                    trackIndex).numReverbObjects = value
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Label {
                    text: "discreteReflectionsPerObject:"
                }
                SpinBox {
                    from: 0
                    editable: true
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    to: 10
                    value: 1

                    onValueChanged: {
                        _clipArea.tracksModel.visrEffect(
                                    trackIndex).discreteReflectionsPerObject = value
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 30
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Label {
                    text: "lateFilterLength:"
                }
                SpinBox {
                    from: 0
                    editable: true
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    to: 10000
                    value: 50
                    onValueChanged: {
                        _clipArea.tracksModel.visrEffect(
                                    trackIndex).lateFilterLengthSeconds = value / 1000.0
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Label {
                    text: "maxDiscreteReflectionDelay:"
                }
                SpinBox {
                    from: 0
                    editable: true
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    to: 10000
                    value: 20
                    onValueChanged: {
                        _clipArea.tracksModel.visrEffect(
                                    trackIndex).maximumDiscreteReflectionDelay = value / 1000.0
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Button {
                text: "select"
                onClicked: {
                    fileDialog_lateDiffusionFilters.open()
                }
            }
            Label {
                Layout.fillWidth: true
                id: lateDiffusionFilters
                text: "lateDiffusionFilters: "
            }
        }
    }

    Popup {
        id: reverbConfig
        width: 1000
        height: 600
        parent: Overlay.overlay
        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)
        contentItem: ColumnLayout {
            Layout.maximumWidth: 1000
            Layout.maximumHeight: 500
            GroupBox {
                title: qsTr("DiscreteReflection")
                Layout.maximumWidth: 1000
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout {
                    anchors.fill: parent
                    RowLayout {
                        Label {
                            text: "PosX"
                        }
                        TextField {
                            Layout.fillWidth: true
                            id: discreteReflectionPositionX

                            validator: DoubleValidator {
                                top: 1000
                                bottom: 0
                                decimals: 100000000
                                notation: DoubleValidator.StandardNotation
                            }
                            onTextEdited: {
                                _clipArea.tracksModel.visrEffect(
                                            trackIndex).setDiscreteReflectionPositionX(
                                            0, text)
                            }
                        }
                    }
                    RowLayout {
                        Label {
                            text: "PosY"
                        }
                        TextField {
                            Layout.fillWidth: true
                            id: discreteReflectionPositionY

                            validator: DoubleValidator {
                                top: 1000
                                bottom: 0
                                decimals: 100000000
                                notation: DoubleValidator.StandardNotation
                            }
                            onTextEdited: {
                                _clipArea.tracksModel.visrEffect(
                                            trackIndex).setDiscreteReflectionPositionY(
                                            0, text)
                            }
                        }
                    }
                    RowLayout {
                        Label {
                            text: "PosZ"
                        }
                        TextField {
                            Layout.fillWidth: true
                            id: discreteReflectionPositionZ

                            validator: DoubleValidator {
                                top: 1000
                                bottom: 0
                                decimals: 100000000
                                notation: DoubleValidator.StandardNotation
                            }
                            onTextEdited: {
                                _clipArea.tracksModel.visrEffect(
                                            trackIndex).setDiscreteReflectionPositionZ(
                                            0, text)
                            }
                        }
                    }
                    RowLayout {
                        Label {
                            text: "Level"
                        }
                        TextField {
                            Layout.fillWidth: true
                            id: discreteReflectionLevel

                            validator: DoubleValidator {
                                top: 1000
                                bottom: 0
                                decimals: 100000000
                                notation: DoubleValidator.StandardNotation
                            }
                            onTextEdited: {
                                _clipArea.tracksModel.visrEffect(
                                            trackIndex).setDiscreteReflectionLevel(
                                            0, text)
                            }
                        }
                    }
                    RowLayout {
                        Label {
                            text: "Delay"
                        }
                        TextField {
                            Layout.fillWidth: true
                            id: discreteReflectionDelay
                            validator: DoubleValidator {
                                top: 1000
                                bottom: 0
                                decimals: 100000000
                                notation: DoubleValidator.StandardNotation
                            }
                            onTextEdited: {
                                _clipArea.tracksModel.visrEffect(
                                            trackIndex).setDiscreteReflectionDelay(
                                            0, text)
                            }
                        }
                    }
                    Repeater {
                        id: coeffRepater
                        RowLayout {

                            spacing: 0
                            Label {
                                text: "B0"
                            }
                            TextField {
                                Layout.fillWidth: true
                                text: model.B0
                                validator: DoubleValidator {
                                    top: 1000
                                    bottom: -1000
                                    decimals: 100000000
                                    notation: DoubleValidator.StandardNotation
                                }
                                onTextEdited: {
                                    _clipArea.tracksModel.visrEffect(
                                                trackIndex).setReverbFilterB0(
                                                0, text)
                                }
                            }
                            Label {
                                text: "B1"
                            }
                            TextField {
                                Layout.fillWidth: true

                                text: model.B1
                                validator: DoubleValidator {
                                    top: 1
                                    bottom: 0
                                    decimals: 10
                                    notation: DoubleValidator.StandardNotation
                                }
                                onTextEdited: {
                                    _clipArea.tracksModel.visrEffect(
                                                trackIndex).setReverbFilterB1(
                                                0, text)
                                }
                            }
                            Label {
                                text: "B2"
                            }
                            TextField {
                                Layout.fillWidth: true

                                text: model.B2
                                validator: DoubleValidator {
                                    top: 1000
                                    bottom: -1000
                                    decimals: 100000000
                                    notation: DoubleValidator.StandardNotation
                                }
                                onTextEdited: {
                                    _clipArea.tracksModel.visrEffect(
                                                trackIndex).setReverbFilterB2(
                                                0, text)
                                }
                            }
                            Label {
                                text: "A1"
                            }
                            TextField {
                                Layout.fillWidth: true

                                text: model.A2
                                validator: DoubleValidator {
                                    top: 1000
                                    bottom: -1000
                                    decimals: 100000000
                                    notation: DoubleValidator.StandardNotation
                                }

                                onTextEdited: {
                                    _clipArea.tracksModel.visrEffect(
                                                trackIndex).setReverbFilterA1(
                                                0, text)
                                }
                            }
                            Label {
                                text: "A2"
                            }
                            TextField {
                                Layout.fillWidth: true

                                text: model.A2
                                validator: DoubleValidator {
                                    top: 1000
                                    bottom: -1000
                                    decimals: 100000000
                                    notation: DoubleValidator.StandardNotation
                                }

                                onTextEdited: {
                                    _clipArea.tracksModel.visrEffect(
                                                trackIndex).setReverbFilterA2(
                                                0, text)
                                }
                            }
                        }
                    }
                }
            }
            GroupBox {
                Layout.maximumWidth: 1000
                title: qsTr("LateReverb")
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout {
                    anchors.fill: parent
                    RowLayout {
                        Label {
                            text: "delay"
                        }
                        TextField {
                            Layout.fillWidth: true
                            id: lateReverbDelay

                            validator: DoubleValidator {
                                top: 1000
                                bottom: -1000
                                decimals: 100000000
                                notation: DoubleValidator.StandardNotation
                            }
                            onTextEdited: {
                                _clipArea.tracksModel.visrEffect(
                                            trackIndex).setLateReverbDelay(
                                            parseFloat(lateReverbDelay.text))
                            }
                        }
                    }
                    RowLayout {
                        Label {
                            text: "decayconst"
                        }
                        Repeater {
                            id: lateReverbDecayconst
                            TextField {
                                Layout.fillWidth: true
                                text: modelData
                                validator: DoubleValidator {
                                    top: 1000
                                    bottom: -1000
                                    decimals: 100000000
                                    notation: DoubleValidator.StandardNotation
                                }
                                onTextEdited: {
                                    _clipArea.tracksModel.visrEffect(
                                                trackIndex).setLateReverbDecayconst(
                                                index, text)
                                }
                            }
                        }
                    }
                    RowLayout {
                        Label {
                            text: "attacktime"
                        }
                        Repeater {
                            id: lateReverbAttacktime
                            TextField {
                                Layout.fillWidth: true
                                text: modelData
                                validator: DoubleValidator {
                                    top: 1000
                                    bottom: -1000
                                    decimals: 100000000
                                    notation: DoubleValidator.StandardNotation
                                }
                                onTextEdited: {
                                    _clipArea.tracksModel.visrEffect(
                                                trackIndex).setLateReverbAttackTime(
                                                index, text)
                                }
                            }
                        }
                    }
                    RowLayout {
                        Label {
                            text: "levels"
                        }
                        Repeater {
                            id: lateReverbLevels
                            TextField {
                                Layout.fillWidth: true
                                text: modelData
                                validator: DoubleValidator {
                                    top: 1000
                                    bottom: -1000
                                    decimals: 100000000
                                    notation: DoubleValidator.StandardNotation
                                }
                                onTextEdited: {
                                    _clipArea.tracksModel.visrEffect(
                                                trackIndex).setLateReverbLevels(
                                                index, text)
                                }
                            }
                        }
                    }
                }
            }

            RowLayout {

                Layout.fillWidth: true
                Item {
                    Layout.fillWidth: true
                }
                Button {
                    text: "Apply"
                    onClicked: {
                        _clipArea.tracksModel.visrEffect(
                                    trackIndex).applyReverb(index)
                    }
                }
                Button {
                    text: "Close"
                    onClicked: {
                        reverbConfig.close()
                    }
                }
            }
        }
    }

    property url lastdir: fileDialog_xml.shortcuts.home
    FileDialog {
        id: fileDialog_xml

        title: "Please choose a file"

        nameFilters: ["Xml file (*.xml)"]
        onAccepted: {
            var path = fileDialog_xml.fileUrl.toString()

            console.log("trackIndex", trackIndex,
                        _clipArea.tracksModel.visrEffect(trackIndex))
            _clipArea.tracksModel.visrEffect(trackIndex).loadSpeakerConfig = path

            speakerCount.text = "Speaker: " + _clipArea.tracksModel.visrEffect(
                        trackIndex).speakerCount
            loadSpeakerFile.text = "File: " + _clipArea.tracksModel.visrEffect(
                        trackIndex).loadSpeakerConfig

            listView.model = _clipArea.tracksModel.visrEffect(
                        trackIndex).speakerModel
        }
    }

    FileDialog {
        id: fileDialog_lateDiffusionFilters

        title: "Please choose a file"

        nameFilters: ["Xml file (*.xml)"]
        onAccepted: {
            var path = fileDialog_lateDiffusionFilters.fileUrl.toString()
            _clipArea.tracksModel.visrEffect(
                        trackIndex).lateDiffusionFilters = path
            lateDiffusionFilters.text = "lateDiffusionFilters: " + path
        }
    }
}
