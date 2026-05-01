// TrackPanel/components/VolumeFields.qml
import QtQuick 2.15
import Global 1.0
import "../../../Items"

Row {
    id: root

    property var clipArea
    property real truePeak: 0.0

    spacing: Global.scaleSize2(8)
    width: parent.width
    height: Global.scaleSize2(56)

    Rectangle {
        width: (root.width - root.spacing) / 2
        height: Global.scaleSize2(37)
        color: "transparent"
        radius: topRadius

        TrackTextField2 {
            id: tf1
            anchors.fill: parent
            bigHeight: true
            isVolume: true

            max: 100
            min: 0
            decimal: 1
            hasSlider: true

            enableRightClick: false
            shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
            moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

            flColor: theme.s11_Enable_FIL
            brColor: theme.s11_Enable_BRD

            onSigPressed: clipArea.tracksModel.saveUndoNoramized(model.index,
                                                                 "Mixer",
                                                                 "Volume")

            onDidShadowChanged: {
                if (didShadow) {
                    model.automationMenuTitle = "Mixer,Volume"
                    clipArea.tracksModel.volumeAutomationSelected(model.index)
                }
            }

            property real automatedValue: model.automatedVolumeValue
            property real automatedNormalized: model.automatedVolumeNormalized
            property real automatedActive: model.automatedVolumeActive
            property real automatedEnabled: model.automatedVolumeEnabled
            property real normalized: model.volumeNormalized

            value: model.volumeNormalized * 100
            text: model.volumeValue.toFixed(decimal)
            defaultValue: model.volumeDefaultNormalized * 100

            onAutomatedEnabledChanged: {
                if (!model.automatedVolumeEnabled) {
                    tf1.shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                    tf1.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                } else {
                    tf1.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                    tf1.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                }
            }

            onAutomatedValueChanged: {
                if (model.automatedVolumeEnabled) {
                    tf1.value = model.automatedVolumeNormalized * 100
                    tf1.text = model.automatedVolumeValue.toFixed(decimal)
                }
            }

            onNormalizedChanged: {
                tf1.value = model.volumeNormalized * 100
                tf1.text = model.volumeValue.toFixed(decimal)
            }

            onSigValueChanged: {
                if (model.automatedVolumeActive) {
                    model.automatedVolumeEnabled = false
                    shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                    moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                }
                model.volumeNormalized = value / 100.0
                tf1.value = model.volumeNormalized * 100
                tf1.text = model.volumeValue.toFixed(decimal)
            }

            onAction: {
                if (text == "Re-Enable Automation")
                    model.automatedVolumeEnabled = true
                if (text == "Delete Automation")
                    model.clear = true
                if (text == "Show Automation")
                    model.automationLaneEnabled = true
                if (text == "Disable Automation") {
                    model.automatedVolumeEnabled = false
                    moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                    model.volumeNormalized = value / 100.0
                    tf1.value = model.volumeNormalized * 100
                    tf1.text = model.volumeValue.toFixed(decimal)
                }
            }
        }
    }

    Rectangle {
        width: (root.width - root.spacing) / 2
        height: Global.scaleSize2(37)
        color: "transparent"
        radius: topRadius

        TrackTextField2 {
            anchors.fill: parent
            bigHeight: true
            isVolume: true

            enableRightClick: false
            has2Slider: false
            hasSlider: false

            text: truePeak > 0 ? Math.abs((1 - Math.abs(
                                               truePeak)) * 100).toFixed(
                                     2) : "0.0"

            flColor: theme.s10_Disable_FIL
            brColor: theme.s11_Enable_BRD
        }
    }
}
