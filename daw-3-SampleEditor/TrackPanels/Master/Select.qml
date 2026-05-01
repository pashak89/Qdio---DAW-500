import QtQuick 2.12
import '.'
import 'qrc:/'
import "qrc:/Items"

TrackButton {
    id:             select_btn

    height:         scaleSize2(37)
    width:          scaleSize2(106)

    name:          ind +1
    hasRightPanel: true

    isClicked: true

    fontColor:
    {
        if(!solo_on_audio && selectTrackDependSolo) theme.f4_
        else if(solo_on_audio && !selectTrackDependSolo) theme.f2_
        else if(solo_on_audio && selectTrackDependSolo) theme.f7_
        else if(!solo_on_audio && !selectTrackDependSolo) theme.f1_
    }

    fillStyle:
    {
        if(!solo_on_audio && selectTrackDependSolo) theme.b11_Enable1_FIL
        else if(solo_on_audio && !selectTrackDependSolo) theme.b10_Disable_FIL
        else if(solo_on_audio && selectTrackDependSolo) theme.b20_Disable_FIL
        else if(!solo_on_audio && !selectTrackDependSolo) theme.b10_Disable_FIL
    }
    brColor: selectTrack ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD

    onSendMessage: if(msg === 'released') {selectTrackDependSolo = !selectTrackDependSolo; console.log("selectTrackDependSolo: ", selectTrackDependSolo)
                   console.log('solo_on_audio:', solo_on_audio, ', selectTrack:', selectTrack)
                   }
}
