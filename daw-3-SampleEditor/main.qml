import QtQuick 2.12
import QtQuick 2.15
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls 2.15
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
import TrackArea 1.0
import BackgroundArea 1.0

import QtGraphicalEffects 1.0
import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import ObjectCreator 1.0
import QDGE 1.0
import QuickFlux 1.1
import QtQml 2.12
import RubberBandItem 1.0
import PlotShapeItem 1.0
import MarkerShapeItem 1.0
import QtWebEngine 1.10
import QtWebChannel 1.0
import "qrc:/Common/Forms/views/CommonItems"
import "qrc:/Common/Forms/views/CustomItems"
import "qrc:/Common/Forms/views/Settings"
import "qrc:/Common/Forms/views"
import "qrc:/Common/Forms/middlewares"
import "qrc:/Common/Forms/actions"
import "qrc:/Common/Forms/stores"
import "qrc:/CommonEngine/Forms/"
import "qrc:/Items"
import "qrc:/TrackPanels/Mixer"

Item {
    property QEngine qEngine: QEngine

    MiddlewareList {
        applyTarget: AppActions

        EngineMiddleware {
            window: mainWindow
        }
    }

    Connections {
        target: qEngine
        function onStartedEngine() {
            console.log("Engine Started.")
        }
    }
    ApplicationWindow {
        id: mainWindow
        visible: true

        GlobalInstance {
            id: globalInstance
        }

        property ThemeManager themeObject: globalInstance.themeObject
        property Definitions definitionsObject: globalInstance.definitionsObject

        Material.theme: themeObject.getCurrentQMLTheme()
        Material.accent: themeObject.getCurrentColor(
                             ThemeManager.ColorType.AccentColor)
        Material.primary: themeObject.getCurrentColor(
                              ThemeManager.ColorType.PrimaryColor)
        Material.background: themeObject.getCurrentColor(
                                 ThemeManager.ColorType.WindowBG)
        Material.foreground: themeObject.getCurrentColor(
                                 ThemeManager.ColorType.TextsColor)

        color: trackList.theme.bg2
        width: 1900
        height: 1000
        property int leftPanelWidth: trackList.scaleSize2(1500)

        Component.onCompleted: {
            _clipArea.setWindowRootItem(mainWindow.contentItem)
        }

        title: qsTr("Wave Editor")

        FileDialog {
            id: saveAsDialog
            title: "Please choose a folder"
            folder: _clipArea.defaultSavePath
            selectMultiple: false
            selectFolder: true
            selectExisting: true
            onAccepted: {
                _clipArea.defaultSavePath = saveAsDialog.folder
            }
            onRejected: {
                saveAsDialog.close()
            }
        }

        SaveAsDialog {
            id: selectExportFileDialog
            title: "Please choose a file"
            onAccepted: {
                exportProgress.open()
                var path = selectExportFileDialog.file.toString()
                path = path.replace(/^(file:\/{3})/, "")
                var cleanPath = decodeURIComponent(path)

                _clipArea.exportAudio(cleanPath, exportSettings.samplerate,
                                      exportSettings.bitDepth,
                                      exportSettings.mono)
            }
            onRejected: {
                selectExportFileDialog.close()
            }
        }

        Popup {
            id: exportProgress
            width: 500
            height: 100
            modal: true
            closePolicy: Popup.CloseOnEscape
            parent: Overlay.overlay
            x: Math.round((mainWindow.width - width) / 2)
            y: Math.round((mainWindow.height - height) / 2)

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Text {
                    text: "Exporting..."
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                ProgressBar {
                    value: _clipArea.exportProgress / 100.0
                    width: parent.width
                }

                Text {
                    text: Math.round(_clipArea.exportProgress) + "%"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        FileDialog {
            id: loadDialog
            title: "Please choose a file"
            folder: _clipArea.defaultSavePath
            nameFilters: ["QDIO project files (*.json)"]
            selectMultiple: false
            selectFolder: false
            selectExisting: true
            onAccepted: {
                projectProgress.open()
                _clipArea.loadAsyncProject(loadDialog.fileUrl)
            }
            onRejected: {
                loadDialog.close()
            }
        }

        Popup {
            id: saveAsPopup
            width: 500
            height: 100
            modal: true
            closePolicy: Popup.CloseOnEscape
            parent: Overlay.overlay
            x: Math.round((mainWindow.width - width) / 2)
            y: Math.round((mainWindow.height - height) / 2)
            contentItem: ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5
                RowLayout {
                    Rectangle {
                        id: borderRectangle

                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        border.color: "transparent"
                        border.width: 1

                        TextField {
                            id: nameField
                            anchors.fill: parent
                            anchors.margins: 2
                            placeholderText: qsTr("Enter project's name")
                            onTextChanged: {
                                if (nameField.text.length > 0) {
                                    borderRectangle.border.color = "transparent"
                                }
                            }
                        }
                    }
                    Button {
                        text: "..."
                        onClicked: saveAsDialog.open()
                    }
                }

                RowLayout {
                    Label {
                        text: _clipArea.defaultSavePath
                    }
                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        id: submitButton
                        text: "Apply"
                        onClicked: {

                            if (nameField.text.length === 0) {
                                borderRectangle.border.color = "red"
                            } else {
                                projectProgress.open()
                                // first we send a signal to objectcreator to save all object then it emit a signal named  sigSaveObjectsReady, then we saveAsyncProject inside that
                                objectCreator.save()
                            }

                            saveAsPopup.close()
                        }
                    }
                    Button {
                        text: "Close"
                        onClicked: saveAsPopup.close()
                    }
                }
            }
        }

        Popup {
            id: projectProgress
            parent: Overlay.overlay
            x: Math.round((mainWindow.width - width) / 2)
            y: Math.round((mainWindow.height - height) / 2)
            closePolicy: Popup.NoAutoClose
            modal: true
            width: 200
            height: 50
            contentItem: ProgressBar {
                indeterminate: true
            }
        }
        Connections {
            target: _clipArea
            onSigLoad3dView: {
                // 3D scene window is opened from C++ at startup; nothing to do.
            }

            onSigExportFinished: {
                exportProgress.close()
            }
            onSigSaveAsProject: {
                saveAsPopup.open()
            }
            onSigLoadProject: {
                loadDialog.open()
            }
            onSigProjectSaved: {
                projectProgress.close()
            }
            onSigProjectSaving: {
                projectProgress.open()
            }

            onSigProjectLoaded: {
                // 3D scene window is opened from C++ at startup; nothing to do.
                objectCreator.load(objects)
                projectProgress.close()
            }
            onSigProjectLoading: {
                projectProgress.open()
            }
        }

        Popup {
            id: exportDialog
            parent: Overlay.overlay
            x: Math.round((mainWindow.width - width) / 2)
            y: Math.round((mainWindow.height - height) / 2)
            closePolicy: Popup.CloseOnEscape
            modal: true
            width: 350
            height: 850
            contentItem: ExportSettings {
                id: exportSettings
                width: 350
                height: 850
                onCancel: {
                    exportDialog.close()
                }
                onExports: {
                    selectExportFileDialog.open()
                    exportDialog.close()
                }
            }
        }

        Popup {
            id: stackPopup
            parent: Overlay.overlay
            x: Math.round((mainWindow.width - width) / 2)
            y: Math.round((mainWindow.height - height) / 2)
            width: mainWindow.leftPanelWidth + 40
            height: mainWindow.height * 0.8
            modal: true
            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
            padding: 10

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: "PortAudio"
                        onClicked: {
                            stack.currentIndex = 0
                        }
                    }

                    //                    Button {
                    //                        text: "Effects"
                    //                        onClicked: {
                    //                            stack.currentIndex = 1
                    //                        }
                    //                    }
                    Button {
                        text: "Settings"
                        onClicked: {
                            stack.currentIndex = 1
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Close"
                        onClicked: stackPopup.close()
                    }
                }

                StackLayout {
                    id: stack
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    ColumnLayout {
                        Layout.fillHeight: true

                        Layout.maximumWidth: mainWindow.leftPanelWidth

                        Frame {
                            Layout.fillWidth: true
                            ColumnLayout {
                                Layout.fillWidth: true
                                RowLayout {
                                    Layout.fillWidth: true
                                    ComboBox {
                                        id: driverComboBox
                                        focusPolicy: Qt.NoFocus
                                        model: _engine.audioEngine.aduioDevice.setupDevice.backend
                                        textRole: "Name"
                                        valueRole: "Index"
                                        currentIndex: _engine.audioEngine.aduioDevice.setupDevice.findCurrentBackendIndex(_engine.audioEngine.aduioDevice.hostApiName)
                                        onActivated: {
                                            if (driverComboBox.currentValue === -1) {
                                                _mixer.outputDeviceModel.hostApi = -1
                                            } else {
                                                _engine.audioEngine.aduioDevice.setupDevice.setBackend(
                                                            driverComboBox.currentValue)
                                            }
                                        }
                                    }
                                    ComboBox {

                                        focusPolicy: Qt.NoFocus
                                        id: outputDeviceComboBox
                                        model: _engine.audioEngine.aduioDevice.setupDevice.device
                                        currentIndex: _engine.audioEngine.aduioDevice.setupDevice.findCurrentDeviceIndex(_engine.audioEngine.aduioDevice.deviceName)
                                        textRole: "Name"
                                        valueRole: "Index"
                                        Layout.fillWidth: true

                                        onActivated: {

                                        }
                                    }
                                    ColumnLayout {
                                        Label {
                                            text: "Max Channel:  "
                                                  + _engine.audioEngine.aduioDevice.maxOutChannels
                                            font.pixelSize: 14
                                            color: "white"
                                        }
                                        Label {
                                            id: outputLatency
                                            text: "OutLatency: " + _engine.audioEngine.outputLatency
                                            font.pixelSize: 14
                                            color: "white"
                                        }
                                    }
                                }
                                RowLayout {
                                    Label {
                                        text: "Output Channels: "
                                        font.pixelSize: 14
                                        color: "white"
                                    }
                                    SpinBox {
                                        id: channelNumber
                                        from: 0
                                        value: 2
                                        to: _engine.audioEngine.aduioDevice.maxOutChannels
                                    }

                                    Label {
                                        text: "SampleRate:"
                                        font.pixelSize: 14
                                        color: "white"
                                    }
                                    ComboBox {
                                        id: sampleRatesComboBox
                                        model: _engine.audioEngine.aduioDevice.setupDevice.deviceSampleRates(
                                                   outputDeviceComboBox.currentValue)
                                        currentIndex: _engine.audioEngine.aduioDevice.setupDevice.findSampleRates(
                                                          _engine.audioEngine.aduioDevice.setupDevice.hostIndexToDeviceIndex(_engine.audioEngine.aduioDevice.outDevIdx),
                                                          _engine.audioEngine.outputSampleRate)
                                    }
                                }

                                RowLayout {

                                    Label {
                                        text: "OutBufferSize:"
                                        font.pixelSize: 15
                                        color: "white"
                                    }
                                    SpinBox {
                                        id: outputBufferSizeSpinBox
                                        from: 256
                                        to: 11152
                                        editable: true
                                        value: _engine.audioEngine.outputBufferSize
                                    }

                                    Button {
                                        text: "SetToDefault"
                                        onClicked: {
                                            _engine.audioEngine.outputLatency = _engine.audioEngine.aduioDevice.setupDevice.defaultOutoutLowLatency(
                                                        outputDeviceComboBox.currentValue)
                                        }
                                    }
                                    Button {
                                        Layout.fillWidth: true
                                        text: "Apply Latency"

                                        onClicked: {
                                            _engine.audioEngine.outputBufferSize
                                                    = outputBufferSizeSpinBox.value
                                        }
                                    }
                                }
                                RowLayout {
                                    Layout.fillWidth: true
                                    Button {
                                        text: "Audio Engine On"
                                        checkable: true
                                        checked: true
                                        highlighted: checked

                                        onClicked: {
                                            if (checked == false) {
                                                _engine.audioEngineOff()
                                            } else {
                                                _engine.audioEngineOn()
                                            }
                                        }
                                    }
                                    Button {

                                        text: "Apply Device"
                                        focus: false
                                        focusPolicy: Qt.NoFocus
                                        onClicked: {

                                            _engine.audioEngineOn(
                                                        outputDeviceComboBox.currentValue,
                                                        channelNumber.value)
                                        }
                                    }
                                    Button {
                                        id: asioSetting
                                        text: "Asio Pannel"
                                        enabled: _engine.audioEngine.aduioDevice.setupDevice.isAsioDevice(
                                                     outputDeviceComboBox.currentValue)
                                        onClicked: {
                                            _engine.audioEngine.aduioDevice.setupDevice.openAsioControlPanel()
                                        }
                                    }
                                }
                            }
                        }

                        Frame {
                            Layout.fillWidth: true
                            RowLayout {
                                ToolButton {
                                    text: "Play"
                                    focusPolicy: Qt.NoFocus
                                    onClicked: {
                                        _clipArea.play()
                                    }
                                }
                                ToolButton {
                                    text: "Stop"
                                    focusPolicy: Qt.NoFocus
                                    onClicked: {
                                        _clipArea.pause()
                                    }
                                }
                                Label {
                                    visible: !_areaInfo.selectedAreaActive
                                    text: "Insert Marker: " + _areaInfo.timeSelectionStart.join(
                                              ".")
                                    color: "white"
                                    font.pixelSize: 14
                                }
                                Label {
                                    visible: _areaInfo.selectedAreaActive
                                    text: "Time Selection :"
                                    color: "white"
                                    font.pixelSize: 14
                                }
                                Label {
                                    visible: _areaInfo.selectedAreaActive
                                    text: "Start :" + _areaInfo.timeSelectionStart.join(
                                              ".")
                                    color: "white"
                                    font.pixelSize: 14
                                }
                                Label {
                                    visible: _areaInfo.selectedAreaActive
                                    text: "End:" + _areaInfo.timeSelectionEnd.join(
                                              ".")
                                    color: "white"
                                    font.pixelSize: 14
                                }
                                Label {
                                    visible: _areaInfo.selectedAreaActive
                                    text: "Length :" + _areaInfo.timeSelectionLength.join(
                                              ".")
                                    color: "white"
                                    font.pixelSize: 14
                                }
                            }
                        }

                        Frame {
                            Layout.fillWidth: true
                            ColumnLayout {
                                anchors.fill: parent
                                RowLayout {
                                    Label {
                                        text: "Tempo: " + _clipArea.tempo
                                        Layout.fillWidth: true
                                    }
                                    Slider {
                                        id: tempo
                                        from: 40
                                        stepSize: 1
                                        value: 120
                                        to: 400
                                        focusPolicy: Qt.NoFocus
                                        Layout.fillWidth: true
                                        onValueChanged: {
                                            _clipArea.tempo = value
                                        }
                                    }
                                }

                                RowLayout {
                                    Label {
                                        text: "Beat Per Bar: " + _clipArea.beatPerBar
                                        Layout.fillWidth: true
                                    }

                                    Slider {
                                        id: beatPerBarSlider
                                        from: 1
                                        stepSize: 1
                                        value: 4
                                        to: 30
                                        Layout.fillWidth: true
                                        focusPolicy: Qt.NoFocus
                                        onValueChanged: {
                                            _clipArea.beatPerBar = value
                                        }
                                    }
                                }

                                RowLayout {

                                    Label {
                                        text: "Lenght of Beat: " + _clipArea.lenghtOfBeat
                                        Layout.fillWidth: true
                                    }

                                    Slider {
                                        id: slider
                                        from: 0
                                        stepSize: 1
                                        value: 2
                                        to: 4
                                        focusPolicy: Qt.NoFocus
                                        Layout.fillWidth: true
                                        onValueChanged: {

                                            _clipArea.lenghtOfBeat = Math.pow(
                                                        2, value)
                                        }
                                    }
                                }

                                RowLayout {
                                    Label {
                                        text: "Fixed Grid"
                                    }
                                    ComboBox {
                                        id: fixedGrid

                                        model: ["Off", "8 Bar", "4 Bar", "2 Bar", "1 Bar", "1/2", "1/4", "1/8", "1/16", "1/32"]
                                        currentIndex: _clipArea.fixedGrid
                                        onCurrentIndexChanged: {
                                            _clipArea.fixedGrid = fixedGrid.currentIndex
                                        }
                                    }
                                    Label {
                                        text: "Adaptive Grid"
                                    }
                                    ComboBox {
                                        id: addaptive
                                        currentIndex: _clipArea.adaptiveGrid
                                        model: ["Off", "Narrow", "Narrowest", "Wide", "Widest", "Noraml"]
                                        onCurrentIndexChanged: {
                                            _clipArea.adaptiveGrid = addaptive.currentIndex
                                        }
                                    }
                                }
                            }
                        }

                        Frame {
                            Layout.fillWidth: true
                            ColumnLayout {
                                anchors.fill: parent
                                Theme1 {
                                    id: theme1
                                }
                                Theme2 {
                                    id: theme2
                                }
                                RowLayout {
                                    ToolButton {
                                        id: them1
                                        text: "Theme 1"
                                        onClicked: {
                                            _areaInfo.themeType = 0

                                            trackList.theme = theme1
                                        }
                                    }
                                    ToolButton {
                                        id: them2
                                        text: "Theme 2"
                                        onClicked: {
                                            _areaInfo.themeType = 1

                                            trackList.theme = theme2
                                        }
                                    }
                                }

                                Slider {
                                    id: slider1
                                    from: 50
                                    value: _areaInfo.zoomFactor * 100
                                    to: 200
                                    stepSize: 1
                                    live: true

                                    onMoved: {
                                        _areaInfo.zoomFactor = value / 100.0
                                    }

                                    ToolTip {
                                        parent: slider1.handle
                                        visible: slider1.pressed
                                        text: slider1.value.toFixed(1)
                                    }
                                }
                            }
                        }
                    }

                    //                    Effects {
                    //                        Layout.fillHeight: true

                    //                        Layout.maximumWidth: mainWindow.leftPanelWidth
                    //                    }
                    Settings {}
                }
            }
        }

        menuBar: MenuBar {
            focusPolicy: Qt.NoFocus
            Menu {
                title: qsTr("&File")

                Action {
                    text: qsTr("&Add...")
                    onTriggered: {
                        _clipArea.addSampleClip()
                    }
                }
                Action {
                    text: qsTr("&Export...")
                    onTriggered: {
                        exportDialog.open()
                    }
                }
            }
            Menu {
                title: qsTr("&View")

                Action {
                    text: qsTr("&Settings Panel...")
                    onTriggered: {
                        stackPopup.open()
                    }
                }
            }
        }
        Keys.onPressed: {
            console.log("asdads")
        }
        FileDialog {
            id: fileDialog
            title: "Please choose a file"
            folder: shortcuts.home
            onAccepted: {
                var path = fileDialog.fileUrl.toString()
                samplseEditor.setSampleFile(path)
            }
        }
        Connections {
            target: objectCreator
            onSigCurrentStatus: {
                _clipArea.tracksModel.setCurrentObjectStatus(trackIndex,
                                                             currentChanges)
            }

            onSigKeyFrameEdited: {
                _clipArea.tracksModel.saveKeyFrameEdited(trackIndex, status)
            }

            onSigObjectMoved: {

                _clipArea.tracksModel.setObjectPosition(trackIndex, x, y, z)
                if (_clipArea.tracksModel.isVisrLoaded(trackIndex)) {
                    _clipArea.tracksModel.visrEffect(trackIndex).setValue(
                                "PosX", x)
                    _clipArea.tracksModel.visrEffect(trackIndex).setValue(
                                "PosY", y)
                    _clipArea.tracksModel.visrEffect(trackIndex).setValue(
                                "PosZ", z)
                }
            }
            onSigSaveObjectMove: {

                _clipArea.tracksModel.saveObjectPosition(trackIndex, x, y, z)
            }

            onSigCurrentTimeChanged: {
                _clipArea.setCurrentIndicator(time)
            }
            onSigSaveObjectsReady: {
                _clipArea.saveAsyncProject(
                            result,
                            _clipArea.defaultSavePath + "/" + nameField.text + ".json")
            }
            onSigEngineStarted: {

                for (var i = 0; i < _clipArea.tracksModel.size(); i++) {

                    objectCreator.createObject(
                                i, _clipArea.tracksModel.objectModelEnabled(i))

                    objectCreator.setKeyFrames(
                                i, _clipArea.tracksModel.keyFrames(i),
                                _clipArea.tracksModel.objectPositionX(i),
                                _clipArea.tracksModel.objectPositionY(i),
                                _clipArea.tracksModel.objectPositionZ(i))
                }
            }

            //void sigKeyFrameAdded(int trackIndex, qint64 time);
            //void sigKeyFrameRemoved(int trackIndex, qint64 time);
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: trackList.scaleSize2(12)
            spacing: 0

            TopBar {
                Layout.fillWidth: true
                Layout.minimumHeight: scaleSize2(75)
                Layout.preferredHeight: scaleSize2(75)
                Layout.topMargin: scaleSize2(30)
                Layout.bottomMargin: scaleSize2(30)
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: trackList.scaleSize2(8)

            Rectangle {
                Layout.fillHeight: true

                Layout.minimumWidth: eQComponent.width + trackList.scaleSize2(
                                         20)
                Layout.preferredWidth: eQComponent.width + trackList.scaleSize2(
                                           20)
                color: "transparent"
                border.color: "#2f3032"
                border.width: trackList.scaleSize2(10)
                radius: trackList.scaleSize2(20)
                clip: true

                Flickable {
                    id: flickable
                    anchors.fill: parent

                    clip: true
                    interactive: false
                    contentWidth: eQComponent.width + trackList.scaleSize2(20)
                    contentHeight: eQComponent.height + mixer.height
                    anchors.verticalCenter: parent.verticalCenter

                    property bool isScrolling: false
                    Timer {
                        id: scrollTimer
                        interval: 1000
                        onTriggered: flickable.isScrolling = false
                    }

                    ScrollBar.vertical: ScrollBar {
                        id: scrollBar
                        policy: ScrollBar.AsNeeded
                        anchors.right: parent.right
                        width: 8
                        contentItem: Rectangle {
                            implicitWidth: 8
                            radius: width / 2
                            color: trackList.theme.bg7
                            opacity: flickable.isScrolling ? 1.0 : 0.0
                            Behavior on opacity {
                                NumberAnimation {
                                    duration: 200
                                }
                            }
                        }
                    }

                    Column {
                        id: leftColumn
                        width: eQComponent.width
                        spacing: 0
                        anchors.horizontalCenter: parent.horizontalCenter

                        Item {
                            width: parent.width
                            height: eQComponent.height
                            EQ {
                                id: eQComponent
                                name: "EQ"
                                enabled: true
                                visible: true
                                trackIndex: _areaInfo.selectedTrackItem
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }

                        Item {
                            width: parent.width
                            height: mixer.height

                            TrackMixerPanel {
                                id: mixer
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton
                    propagateComposedEvents: true
                    onWheel: {
                        var delta = wheel.angleDelta.y / 120
                        flickable.contentY = Math.max(
                                    0, Math.min(
                                        flickable.contentHeight - flickable.height,
                                        flickable.contentY - delta * 20))
                        flickable.isScrolling = true
                        scrollTimer.restart()
                        wheel.accepted = true
                    }
                }
            }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: trackList.scaleSize2(8)

                    SplitView {
                        id: mainSplitView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        orientation: Qt.Horizontal
                        handle: Rectangle {
                            implicitWidth: trackList.scaleSize2(8)
                            color: "#4C4C4C"
                            HoverHandler {
                                cursorShape: Qt.SplitHCursor
                            }
                            Item {
                                anchors.fill: parent
                                anchors.leftMargin: -trackList.scaleSize2(6)
                                anchors.rightMargin: -trackList.scaleSize2(6)
                                HoverHandler {
                                    cursorShape: Qt.SplitHCursor
                                }
                            }
                        }

                Frame {
                    SplitView.fillWidth: true
                    SplitView.minimumWidth: trackList._width + trackList.scaleSize2(80)

                    spacing: 0
                    padding: trackList.scaleSize2(10)
                    background: Rectangle {
                        color: "transparent"
                        border.color: "#2f3032"
                        border.width: trackList.scaleSize2(10)
                        radius: trackList.scaleSize2(20)
                    }

                    PlotShapeItem {
                        id: plotItem
                        areaInfo: _areaInfo
                        clipArea: _clipArea
                        anchors.top: parent.top
                        height: parent.height - bottomArea.height - trackList.scaleSize2(
                                    150)
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: trackList._width + trackList.scaleSize2(
                                                10)
                        z: trackList.z - 1
                    }

                    Rectangle {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        height: trackList.scaleSize2(150)
                        width: parent.width
                        color: "#2f3032"
                    }

                    Rectangle {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.leftMargin: trackList._width
                        width: trackList.scaleSize2(8)
                        color: "#2f3032"
                        z: trackList.z + 2
                    }

                    TrackList {
                        id: trackList
                        anchors.top: parent.top
                        anchors.topMargin: trackList.scaleSize2(150)
                        anchors.left: parent.left
                        height: parent.height - bottomArea.height - trackList.scaleSize2(
                                    150)
                        width: parent.width
                    }

                    MarkerShapeItem {

                        clipArea: _clipArea
                        areaInfo: _areaInfo
                        anchors.top: trackList.top
                        anchors.bottom: trackList.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: trackList._width + trackList.scaleSize2(
                                                10)
                        z: trackList.z + 1
                    }

                    Rectangle {
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        height: trackList.scaleSize2(40)
                        width: trackList._width + trackList.scaleSize2(10)
                        color: "#2f3032"
                    }

                    TopArea {
                        id: topArea
                        clipArea: _clipArea
                        areaInfo: _areaInfo
                        width: parent.width - trackList._width - trackList.scaleSize2(
                                   10)
                        height: trackList.scaleSize2(224)
                        anchors.left: parent.left
                        anchors.leftMargin: trackList._width + trackList.scaleSize2(
                                                10)
                        anchors.top: parent.top
                        z: trackList.z + 1

                        TimeRibbon {
                            width: trackList.scaleSize2(23)
                            height: trackList.scaleSize2(23)
                            angle: 180
                            anchors.bottom: parent.bottom
                            x: _areaInfo.arrangementStartPixel - width / 2
                            visible: x > trackList.x
                                     && x < trackList.x + trackList.width
                        }
                    }

                    BottomArea {
                        id: bottomArea
                        areaInfo: _areaInfo
                        width: parent.width - trackList._width
                        height: trackList.scaleSize2(40)
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.leftMargin: trackList._width + trackList.scaleSize2(
                                                10)

                        TimeRibbon {
                            width: trackList.scaleSize2(23)
                            height: trackList.scaleSize2(23)
                            angle: 0
                            anchors.top: parent.top
                            x: _areaInfo.arrangementStartPixel - width / 2
                            visible: x > trackList.x
                                     && x < trackList.x + trackList.width
                        }
                    }

                    Rectangle {
                        z: 9999
                        anchors.fill: parent
                        anchors.margins: -trackList.scaleSize2(10)
                        color: "transparent"
                        border.color: "#2f3032"
                        border.width: trackList.scaleSize2(10)
                        radius: trackList.scaleSize2(20)
                    }
                }

                        // Right panel — 3D (top) + 2D (bottom), equal heights
                        ColumnLayout {
                            SplitView.preferredWidth: mainSplitView.width > 0
                                ? Math.round(mainSplitView.width * 0.25)
                                : trackList.scaleSize2(350)
                            SplitView.minimumWidth: trackList.scaleSize2(250)
                            spacing: trackList.scaleSize2(8)

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                color: "transparent"
                                border.color: "#2f3032"
                                border.width: trackList.scaleSize2(10)
                                radius: trackList.scaleSize2(20)
                                clip: true

                                WebEngineView {
                                    anchors.fill: parent
                                    anchors.margins: trackList.scaleSize2(10)
                                    url: "qrc:/web/scene3d.html"
                                    webChannel: webChannelObj
                                    onJavaScriptConsoleMessage: scene3D.log("3D-JS L" + lineNumber + ": " + message)
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                color: "transparent"
                                border.color: "#2f3032"
                                border.width: trackList.scaleSize2(10)
                                radius: trackList.scaleSize2(20)
                                clip: true

                                WebEngineView {
                                    anchors.fill: parent
                                    anchors.margins: trackList.scaleSize2(10)
                                    url: "qrc:/web/scene2d.html"
                                    webChannel: webChannelObj
                                    onJavaScriptConsoleMessage: scene3D.log("2D-JS L" + lineNumber + ": " + message)
                                }
                            }
                        }
                    }   // close SplitView (clip area + right panel)

                    SampleEditor {
                        id: samplseEditor
                        visible: true
                        Layout.fillWidth: true
                        Layout.preferredHeight: trackList.scaleSize2(280)
                    }
                }   // close right-content ColumnLayout
            }   // close outer horizontal RowLayout (sidebar + right-content)
        }
    }

}
