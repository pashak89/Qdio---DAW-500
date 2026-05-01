// TrackPanel/components/PanField.qml
import QtQuick 2.15
import Global 1.0
import "../../../Items"

Rectangle {
    id: root

    property var clipArea
    width: parent.width
    height: Global.scaleSize2(86)
    color: "transparent"
    radius: topRadius

    TrackTextField2 {
        id: tf2
        height: Global.scaleSize2(37)
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        bigHeight: true

        name: "pan"
        isPan: true
        isVolume: false

        max: 100
        min: 0
        decimal: 0
        has2Slider: true
        hasSlider: false

        property real automatedValue: model.automatedPanValue
        property real automatedNormalized: model.automatedPanNormalized
        property real automatedActive: model.automatedPanActive
        property real automatedEnabled: model.automatedPanEnabled
        property real normalized: model.panNormalized
        property string automationMenuTitle: model.automationMenuTitle
        property real panDefaultNormalized: model.panDefaultNormalized
        property real panValue: model.panValue
        property real automationLaneEnabled: model.automationLaneEnabled

        enableRightClick: false
        shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
        moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

        flColor: theme.s11_Enable_FIL
        brColor: theme.s11_Enable_BRD

        onSigPressed: clipArea.tracksModel.saveUndoNoramized(model.index,
                                                             "Mixer", "Pan")
        onDidShadowChanged: {
            if (didShadow) {
                model.automationMenuTitle = "Mixer,Pan"
                clipArea.tracksModel.panAutomationSelected(index)
            }
        }

        defaultValue: panDefaultNormalized * 100
        value: normalized * 100
        text: panValue.toFixed(decimal)

        onAutomatedEnabledChanged: {
            if (!automatedPanEnabled) {
                tf2.shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                tf2.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
            } else {
                tf2.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                tf2.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
            }
        }

        onAutomatedValueChanged: {
            if (automatedEnabledd) {
                value = automatedNormalized * 100
                text = automatedValue.toFixed(decimal)
            }
        }

        onNormalizedChanged: {
            value = normalized * 100
            text = panValue.toFixed(decimal)
        }

        onSigValueChanged: {
            if (automatedActive) {
                automatedEnabled = false
                shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
            }

            model.normalized = value / 100.0
            tf2.value = normalized * 100
            tf2.text = panValue.toFixed(decimal)
        }

        onAction: {
            if (text == "Re-Enable Automation")
                model.automatedEnabled = true
            if (text == "Delete Automation")
                model.clear = true
            if (text == "Show Automation")
                model.automationLaneEnabled = true
            if (text == "Disable Automation") {
                model.automatedEnabled = false
                moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                model.normalized = value / 100.0
                tf2.value = normalized * 100
                tf2.text = panValue.toFixed(decimal)
            }
        }
    }
}
