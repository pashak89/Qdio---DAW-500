import QtQuick 2.12
import "."
import "qrc:/"
import "qrc:/Items"

Rectangle {
    id: tf_10

    // anchors {
    //     top:        select_btn.bottom
    //     topMargin:  scaleSize2(15)
    // }
    height: scaleSize2(37)
    width: scaleSize2(106)

    color: 'transparent'

    TrackTextField2 {
        id: tf2
        anchors.fill: parent
        bigHeight: true

        isVolume: true

        max: 100
        min: 0
        decimal: 1
        hasSlider: true

        enableRightClick: true
        shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
        moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

        //            shortcutKeyTxt:     ['', '', '']
        //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
        //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
        //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
        flColor: theme.s11_Enable_FIL
        brColor: theme.s11_Enable_BRD

        onSigPressed: {
            _clipArea.tracksModel.saveUndoNoramized(model.index,
                                                   "Mixer", "Volume")
        }

        onDidShadowChanged: {
            if (didShadow) {
                model.automationMenuTitle = "Mixer,Volume"
                _clipArea.tracksModel.volumeAutomationSelected(model.index)
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
                tf2.shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                tf2.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
            } else {
                tf2.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                tf2.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
            }
        }
        onAutomatedValueChanged: {

            if (model.automatedVolumeEnabled) {
                tf2.value = model.automatedVolumeNormalized * 100
                tf2.text = model.automatedVolumeValue.toFixed(decimal)
            } else {

            }
        }
        onNormalizedChanged: {

            tf2.value = model.volumeNormalized * 100
            tf2.text = model.volumeValue.toFixed(decimal)
        }

        onSigValueChanged: {            
            if (model.automatedVolumeActive) {
                model.automatedVolumeEnabled = false
                shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
            }

            model.volumeNormalized = value / 100.0
            tf2.value = model.volumeNormalized * 100
            tf2.text = model.volumeValue.toFixed(decimal)
        }

        onAction: {

            if (text == "Re-Enable Automation") {
                model.automatedVolumeEnabled = true
            }

            if (text == "Delete Automation") {
                model.clear = true
            }
            if (text == "Show Automation") {
                model.automationLaneEnabled = true
            }

            if (text == "Disable Automation") {
                model.automatedVolumeEnabled = false
                moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']

                model.volumeNormalized = value / 100.0
                tf2.value = model.volumeNormalized * 100
                tf2.text = model.volumeValue.toFixed(decimal)
            }
        }
    }

    Rectangle {
        id: automatedLight
        z: 1000
        visible: model.automatedVolumeActive
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1
        anchors.topMargin: 0
        color: model.automatedVolumeEnabled ? "#EA2027" : "#808080"
        width: scaleSize2(11)
        height: scaleSize2(11)
        radius: scaleSize2(11)
    }
}
