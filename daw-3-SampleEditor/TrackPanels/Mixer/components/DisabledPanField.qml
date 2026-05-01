// TrackPanel/components/DisabledPanField.qml
import QtQuick 2.15
import Global 1.0
import QtGraphicalEffects 1.15
import "../../../Items"

TrackButton {
    id: recBtn

    width: parent ? parent.width : Global.scaleSize2(300)
    height: Global.scaleSize2(37)

    fillStyle: isClicked ? theme.b80_enable_FIL : theme.b10_Disable_FIL
    brColor: isClicked ? theme.b80_enable_BRD : theme.b10_Disable_BRD

    Image {
        id: recImg
        anchors.centerIn: parent
        sourceSize: {
            if (recBtn.height % 2 !== 0)
                Qt.size(Global.oddSize(recBtn.height * .35),
                        Global.oddSize(recBtn.height * .35))
            else
                Qt.size(Global.evenSize(recBtn.height * .35),
                        Global.evenSize(recBtn.height * .35))
        }
        source: 'qrc:/Resource/rec_icon.svg'
        visible: false
    }
    ColorOverlay {
        cached: true
        anchors.fill: recImg

        source: recImg
        clip: true
        antialiasing: true
        color: recBtn.isClicked ? theme.button_clicked_color : theme.button_unclicked_color
    }
}
