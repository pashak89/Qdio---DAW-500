import QtQuick 2.12
import "."
import "qrc:/"
import "qrc:/Items"

TrackButton {
    id: solo_btn

    imgVisible: partialySolo
    height: scaleSize2(37)
    width: scaleSize2(106)
    name: 'S'
    isSoloBtn: true
    fontColor: model.solo /*isClicked*/
               ? theme.f4_ : theme.f1_

    fillStyle: model.solo /*isClicked*/
               ? theme.b12_Enable2_FIL : theme.b10_Disable_FIL
    brColor: model.solo /*isClicked*/
             ? theme.b12_Enable2_BRD : theme.b10_Disable_BRD

    onSendMessage: {

        if (msg === "released") {

            clipArea.tracksModel.toggleSolo(index)
            //trackAudioSelection(index, solo_btn)
        }
    }
}
