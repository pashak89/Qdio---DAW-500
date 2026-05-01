import QtQuick 2.12
import '.'
import 'qrc:/'
import "qrc:/Items"

TrackButton {
    id:             solo_btn

    imgVisible: partialySolo
    height: scaleSize2(37)
    width:  scaleSize2(106)
    name:   'S'
    isSoloBtn:  true
    fontColor:  selectSolo/*isClicked*/ ? theme.f4_ : theme.f2_

    fillStyle:  selectSolo/*isClicked*/ ? theme.b12_Enable2_FIL : theme.b20_Disable_FIL
    brColor:    selectSolo/*isClicked*/ ? theme.b12_Enable2_BRD : theme.b20_Disable_BRD

    onSendMessage: if(msg === "released") funcs.trackAudioSelection(ind, solo_btn)
}
