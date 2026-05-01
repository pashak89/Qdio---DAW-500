// TrackPanel/components/MainStrip.qml
import QtQuick 2.15
import QtGraphicalEffects 1.15
import Global 1.0

Item {
    id: root

    property var clipArea

    // optionally feed from outside if you want
    property real meterL: 0.88
    property real meterR: 0.95
    property real maxTruePeak: 0

    width: parent ? parent.width : Global.scaleSize2(300)
    height: Global.scaleSize2(1200)

    Row {
        id: mainStripRow
        anchors.fill: parent
        anchors.topMargin: Global.scaleSize2(65)
        anchors.bottomMargin: Global.scaleSize2(65)
        spacing: Global.scaleSize2(18)
        anchors.horizontalCenter: parent.horizontalCenter

        // ---- Fader ----
        Item {
            id: faderArea
            width: root.width / 2
            height: parent.height

            Rectangle {
                id: faderTrack
                width: Global.scaleSize2(12)
                radius: Global.scaleSize2(3)
                color: theme.bg5
                anchors.centerIn: parent
                height: parent.height
            }

            Rectangle {
                id: knob
                width: Global.scaleSize2(76)
                height: Global.scaleSize2(137)
                radius: Global.scaleSize2(8)
                color: "#bdbdbd"
                border.color: "#0f1011"
                border.width: 1
                x: (parent.width - width) / 2

                property real automatedValue: model.automatedVolumeValue
                property real automatedNormalized: model.automatedVolumeNormalized
                property real automatedActive: model.automatedVolumeActive
                property real automatedEnabled: model.automatedVolumeEnabled
                property real normalized: model.volumeNormalized

                onAutomatedValueChanged: {

                    if (model.automatedVolumeEnabled) {
                        knob.value = model.automatedVolumeNormalized * 100
                        knob.y = (model.automatedVolumeNormalized * faderArea.height) - height / 2
                    } else {
                        knob.value = model.volumeNormalized * 100
                        knob.y = (model.volumeNormalized * faderArea.height) - height / 2
                    }
                }
                onNormalizedChanged: {

                    knob.value = model.volumeNormalized * 100
                    knob.y = (model.volumeNormalized * faderArea.height) - height / 2
                }
                Component.onCompleted: {
                    knob.value = model.volumeNormalized * 100
                    knob.y = (model.volumeNormalized * faderArea.height) - height / 2
                }

                property real minimumValue: 0
                property real maximumValue: 100
                property bool dragging: false
                property real stepSize: 0.1
                property real value: model.volumeNormalized * 100

                y: (model.volumeNormalized * faderArea.height) - height / 2

                Repeater {
                    model: 2
                    Rectangle {
                        width: parent.width / 3.0
                        height: 2
                        radius: 1
                        color: "#2b2b2b"
                        y: parent.height / 2 - height / 2
                        x: index === 0 ? 0 : 2 * parent.width / 3.0
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton
                    preventStealing: true

                    property point pressGlobalPos

                    onPressed: {
                        if (clipArea && clipArea.globalPos)
                            pressGlobalPos = clipArea.globalPos()
                        knob.dragging = true
                        if (clipArea && clipArea.setOverrideCursor)
                            clipArea.setOverrideCursor(Qt.BlankCursor)
                    }

                    onPositionChanged: {
                        if (!knob.dragging || !(mouse.buttons & Qt.LeftButton))
                            return
                        if (!clipArea || !clipArea.globalPos)
                            return

                        var p = clipArea.globalPos()
                        var stepsDelta = p.y - pressGlobalPos.y
                        var raw = knob.value + stepsDelta * knob.stepSize

                        var stepIndex = Math.round(
                                    (raw - knob.minimumValue) / knob.stepSize)
                        var stepped = knob.minimumValue + stepIndex * knob.stepSize
                        stepped = Math.max(knob.minimumValue,
                                           Math.min(knob.maximumValue, stepped))

                        if (model.automatedVolumeActive) {
                            model.automatedVolumeEnabled = false
                        }

                        model.volumeNormalized = stepped / 100.0

                        if (clipArea.setCursorPosition)
                            clipArea.setCursorPosition(pressGlobalPos.x,
                                                       pressGlobalPos.y)
                    }

                    function finishDrag() {
                        if (!knob.dragging)
                            return
                        knob.dragging = false
                        if (clipArea && clipArea.setOverrideCursor)
                            clipArea.setOverrideCursor(Qt.ArrowCursor)

                        if (clipArea && clipArea.setCursorPosition
                                && pressGlobalPos) {
                            var gp = knob.mapToGlobal(knob.width / 2,
                                                      knob.height / 2)
                            clipArea.setCursorPosition(gp.x, gp.y)
                        }
                    }

                    onReleased: finishDrag()
                    onCanceled: finishDrag()
                }
            }
        }

        // ---- Meter ----
        Item {
            id: meterBoxItem
            width: root.width / 2
            height: parent.height

            Item {
                id: meterScale
                width: Global.scaleSize2(76)
                height: parent.height
                anchors.right: meterBox.left
                anchors.rightMargin: Global.scaleSize2(15)

                property var displayValues: [0, 3, 6, 10, 20, 30, 40, 45, 50, 60]
                property int lineLength: Global.scaleSize2(25)
                property real fontSize: topfontPixelSize
                property string fontFamilty: topFontFamily
                property color fontColor: theme.f1_

                Repeater {
                    model: meterScale.displayValues.length
                    delegate: Item {
                        width: meterScale.width
                        height: labelTemplate.implicitHeight

                        property real value: meterScale.displayValues[index]
                        y: Global.dbDisplayToYPixel(
                               value, meterScale.height) - height / 2

                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            width: meterScale.lineLength
                            height: 2
                            color: meterScale.fontColor
                        }

                        Text {
                            id: labelTemplate
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.children[0].left
                            anchors.rightMargin: Global.scaleSize2(10)
                            text: value
                            color: meterScale.fontColor
                            font.family: meterScale.fontFamilty
                            font.pixelSize: meterScale.fontSize
                        }
                    }
                }
            }

            Rectangle {
                id: meterBox
                width: meterRow.implicitWidth + 1 * Global.scaleSize2(
                           3) + (2 - 1) * Global.scaleSize2(3)
                height: parent.height
                radius: Global.scaleSize2(5)
                anchors.horizontalCenter: parent.horizontalCenter
                color: theme.bg5

                Rectangle {
                    width: parent.width
                    height: 2
                    color: theme.bg5
                    y: Global.dbDisplayToYPixel(
                           0, meterBoxItem.height) - height / 2
                    z: meterRow.z + 1
                }

                Row {
                    id: meterRow
                    anchors.fill: parent
                    anchors.margins: Global.scaleSize2(3)
                    spacing: Global.scaleSize2(3)

                    Repeater {
                        model: _clipArea.tracksModel.truePeakModel(
                                   _areaInfo.selectedTrackItem)
                        delegate: Item {
                            width: Global.scaleSize2(24)
                            height: parent.height
                            property real level: (index === 0) ? root.meterL : root.meterR
                            property real turePeak: MaximumTruePeak

                            onTurePeakChanged: {

                                root.maxTruePeak = turePeak
                            }

                            Rectangle {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                radius: 2
                                height: Math.floor(
                                            parent.height * (1 - PeakMeterLevel))

                                gradient: Gradient {
                                    GradientStop {
                                        position: 1.0
                                        color: "#15D0E9"
                                    }
                                    GradientStop {
                                        position: 0.0
                                        color: "#26E9CC"
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
