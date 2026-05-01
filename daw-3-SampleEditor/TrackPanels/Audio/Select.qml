import QtQuick 2.12
import "."
import "qrc:/"
import "qrc:/Items"

TrackButton {
    id: select_btn

    height: scaleSize2(37)
    width: scaleSize2(106)
    property int activated: 0
    property bool mute: false

    name: ind + 1
    hasRightPanel: true

    isClicked: true

    fontColor: {

        if (activated === 1) {
            theme.f4_
        } else if (activated === 0) {
            theme.f2_
        } else {
            theme.f7_
        }
    }

    fillStyle: {

        if (activated === 1) {
            theme.b11_Enable1_FIL
        } else if (activated === 0) {
            theme.b10_Disable_FIL
        } else {
            theme.b20_Disable_FIL
        }
    }
    brColor: activated ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD

    onSendMessage: {
        if (msg === 'pressed') {
            clipArea.tracksModel.saveUndoNoramized(model.index, "Mixer",
                                                   "SpeakerOn")
        }
        if (msg === 'released') {

            model.automationMenuTitle = "Mixer,SpeakerOn"
            clipArea.tracksModel.speakerOnAutomationSelected(model.index)
            clipArea.tracksModel.toggleTrackActivator(model.index)
        }
    }
}
