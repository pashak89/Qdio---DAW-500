import QtQuick 2.12
import QtQuick.Layouts 1.12
import "."
import "qrc:/"
import "qrc:/Items"

Rectangle {
    id: tf_11

    // anchors {
    //     top:            tf_10.top
    // }
    height: scaleSize2(37)
    width: scaleSize2(106)

    color: 'transparent'

    //visible: truePeakModel > 0
    radius: topRadius

    RowLayout {
        id: rowFirst
        visible: splitStereo
        anchors.fill: parent

        TrackTextField {
            Layout.fillHeight: true
            Layout.preferredWidth: Math.round(parent.width * 0.45)
            Layout.alignment: Qt.AlignLeft
            bigHeight: true

            enableRightClick: true
            shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
            moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
            //            shortcutKeyTxt:     ['', '', '']
            //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
            //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
            //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
            splitStr: 'Stereo Pan Mode'
            is2Groups: true

            flColor: theme.s11_Enable_FIL
            brColor: theme.s11_Enable_BRD
            min: -50
            max: 50
            defaultValue: 0
            has2Slider: true
            decimal: 0
            text: value
        }

        TrackTextField {
            Layout.fillHeight: true
            Layout.preferredWidth: Math.round(parent.width * 0.45)
            Layout.alignment: Qt.AlignLeft
            bigHeight: true

            enableRightClick: true
            shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
            moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
            //            shortcutKeyTxt:     ['', '', '']
            //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
            //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
            //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
            splitStr: 'Stereo Pan Mode'
            is2Groups: true

            flColor: theme.s11_Enable_FIL
            brColor: theme.s11_Enable_BRD
            min: -50
            max: 50
            defaultValue: 0
            has2Slider: true
            decimal: 0
            text: value
        }
    }

    TrackTextField2 {
        id: tf2
        anchors.fill: parent
        bigHeight: true

        name: "pan"
        isPan: true
        isVolume: false

        max: 100
        min: 0
        decimal: 0
        has2Slider: true
        hasSlider: false

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
                                                   "Mixer", "Pan")
        }

        onDidShadowChanged: {
            if (didShadow) {

                model.automationMenuTitle = "Mixer,Pan"
                _clipArea.tracksModel.panAutomationSelected(model.index)
            }
        }

        property real automatedValue: model.automatedPanValue
        property real automatedNormalized: model.automatedPanNormalized
        property real automatedActive: model.automatedPanActive
        property real automatedEnabled: model.automatedPanEnabled
        property real normalized: model.panNormalized

        defaultValue: model.panDefaultNormalized * 100
        value: model.panNormalized * 100
        text: model.panValue.toFixed(decimal)



        onAutomatedEnabledChanged: {

            if (!model.automatedPanEnabled) {
                tf2.shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                tf2.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
            } else {
                tf2.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                tf2.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
            }
        }
        onAutomatedValueChanged: {

            if (model.automatedPanEnabled) {
                value = model.automatedPanNormalized * 100
                text = model.automatedPanValue.toFixed(decimal)
            } else {

            }
        }
        onNormalizedChanged: {

            value = model.panNormalized * 100
            text = model.panValue.toFixed(decimal)
        }

        onSigValueChanged: {

            if (model.automatedPanActive) {
                model.automatedPanEnabled = false
                shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
            }

            model.panNormalized = value / 100.0

            tf2.value = model.panNormalized * 100
            tf2.text = model.panValue.toFixed(decimal)
        }

        onAction: {

            if (text == "Re-Enable Automation") {
                model.automatedPanEnabled = true
            }

            if (text == "Delete Automation") {
                model.clear = true
            }

            if (text == "Show Automation") {
                model.automationLaneEnabled = true
            }
            if (text == "Disable Automation") {
                model.automatedPanEnabled = false
                moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']

                model.volumeNormalized = value / 100.0
                tf2.value = model.volumeNormalized * 100
                tf2.text = model.volumeValue.toFixed(decimal)
            }
        }
    }

    Rectangle {
        z: 1000
        visible: model.automatedPanActive
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 1
        anchors.topMargin: 0
        color: model.automatedPanEnabled ? "#EA2027" : "#808080"

        width: scaleSize2(11)
        height: scaleSize2(11)
        radius: scaleSize2(11)
    }
}
