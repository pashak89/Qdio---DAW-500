import QtQuick 2.12
import QtQuick.Layouts 1.12
import '.'
import 'qrc:/'
import "qrc:/Items"

Rectangle {
    id:                 tf_11
    // anchors {
    //     top:            tf_10.top
    // }

    height: scaleSize2(37)
    width: scaleSize2(106)

    color:                  'transparent'
    radius:                 topRadius

    RowLayout {
        id: rowFirst
        visible: splitStereo
        anchors.fill: parent

        TrackTextField {
            Layout.fillHeight: true
            Layout.preferredWidth: Math.round(parent.width *0.45)
            Layout.alignment: Qt.AlignLeft
            bigHeight: true

            enableRightClick: true
            shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
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
            Layout.preferredWidth: Math.round(parent.width *0.45)
            Layout.alignment: Qt.AlignLeft
            bigHeight: true

            enableRightClick: true
            shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
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

    TrackTextField {
        id: ttf
        visible: !splitStereo
        anchors.fill: parent
        bigHeight: true

        enableRightClick: true
        shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
        moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
        //            shortcutKeyTxt:     ['', '', '']
        //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
        //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
        //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
        splitStr: 'Split Stereo Pan'
        is2Groups: true

        flColor: theme.s11_Enable_FIL
        brColor: theme.s11_Enable_BRD
        min: -50
        max: 50
        defaultValue: 0
        has2Slider: true
        decimal: 0
        text: value
        onValueChanged: {inTrack = true; inAutomationMenu = false; inAutomationChild = false}
    }
}
