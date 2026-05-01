import QtQuick 2.12
import '.'
import 'qrc:/'
import "qrc:/Items"

Rectangle {
    id:             tf_10
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

        enableRightClick: true
        shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
        moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
        //            shortcutKeyTxt:     ['', '', '']
        //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
        //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
        //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']

        flColor: theme.s11_Enable_FIL
        brColor: theme.s11_Enable_BRD

        max: 6
        min: -70

        decimal: 1

        hasSlider: true
    }
}

