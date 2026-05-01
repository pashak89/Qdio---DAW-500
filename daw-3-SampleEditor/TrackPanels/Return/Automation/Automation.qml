import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.2
import 'qrc:/Items'
import 'qrc:/'

Rectangle {
    id:             automationCondition

    visible: automationBtn.isClicked

    height: scaleSize2(37)
    width:  scaleSize2(106)*4 + 3* scaleSize2(15)


    color: 'transparent'

    Rectangle {
        id: pluser
        height: parent.height; width: height
        radius: Math.round(height/2)
        border.color: automationMenu.displayMenu === 'None' | automationChildren >=3 ? "#737373" : theme.s11_Enable_BRD
        border.width: topWidth
        color: theme.s11_Enable_FIL

        Image {
            id: img_addAutomation
            anchors.centerIn: parent
            anchors.fill: parent
            anchors.margins: Math.round(parent.height/5)
            sourceSize:     Qt.size(height, height)
            source:         'qrc:/Resource/Plus.svg'
            fillMode:       Image.PreserveAspectFit
            antialiasing: false
            smooth: false
            clip: true
            mipmap: true

            ColorOverlay {
                id: cl
                anchors.fill: parent
                source: parent
                color: automationMenu.displayMenu === 'None' | automationChildren >=3 ? "#737373" : theme.tColor
                antialiasing: false
                smooth: false
            }
        }

        MouseArea {
            enabled: automationMenu.displayMenu !== 'None'
            anchors.fill: parent
            onClicked: {
                if(automationList.count < 3) {
                    automationList.append({displayText: automationMenu.displayMenu, size: 3 })
                    automationMenu.displayMenu = 'None'
                    trackReturnModel.setProperty(ind, "automationChildren", automationList.count)
                }
                //                        console.log('automationChildren, automationList.count', automationList.count)
            }
        }
    }

    AutomationDropMenu {
        id: automationMenu
        height: parent.height
        width: scaleSize2(106) * 2 + 2 * scaleSize2(15) + scaleSize2(53)
        isRootMenu: true
    }



    RowLayout {
        id: rowTextFields

        opacity: automationMenu.displayMenu !== 'None' ? 1 : 0
        enabled: automationMenu.displayMenu !== 'None'
        height: parent.height
        width: scaleSize2(106)

        Rectangle {
            visible: automationMenu.displayMenu === 'Volume'
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: 'transparent'

            TrackTextField2 {
                id: tfVolume
                anchors.fill: parent
                // bigHeight: true

                enableRightClick: true
                shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

                flColor: theme.s11_Enable_FIL
                brColor: theme.s11_Enable_BRD

                max: 6
                min: -70

                decimal: 1

                hasSlider: true
            }
        }

        // ------------------------------------------------------------------------------------------ item 4
        Rectangle {
            visible: automationMenu.displayMenu === 'Pan'
            Layout.fillHeight: true
            Layout.fillWidth: true

            color:                  'transparent'
            radius:                 topRadius

            RowLayout {
                id: rowFirst
                visible: splitStereo
                anchors.fill: parent

                TrackTextField {
                    id: tfPan1
                    Layout.fillHeight: true
                    Layout.preferredWidth: Math.round(parent.width *0.45)
                    Layout.alignment: Qt.AlignLeft
                    // bigHeight: true

                    enableRightClick: true
                    shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                    moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

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
                    id: tfPan2
                    Layout.fillHeight: true
                    Layout.preferredWidth: Math.round(parent.width *0.45)
                    Layout.alignment: Qt.AlignLeft
                    // bigHeight: true

                    enableRightClick: true
                    shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                    moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

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
                id: tfPan3Menu
                visible: !splitStereo
                anchors.fill: parent
                // bigHeight: true

                enableRightClick: true
                shortcutKeyTxt:   ['', '', '', 'Del', 'Ctrl + Del']
                moduls:           ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

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
                // onValueChanged: {inTrack = false; inAutomationMenu = true; inAutomationChild = false ;tf3 = value.toFixed(0)}

                onFocusChanged: console.log('fos: ', focus)
            }
        }
    }














    states: [State {
            name: "Right"
            when:!globalValues.tracksOnLeft

            AnchorChanges {
                target: pluser
                anchors.right: parent.right
            }

            AnchorChanges {
                target: automationMenu
                anchors.right: pluser.left
            }
            PropertyChanges {
                target: automationMenu
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: rowTextFields
                anchors.right: automationMenu.left
            }
            PropertyChanges {
                target: rowTextFields
                anchors.rightMargin: scaleSize2(15)
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: pluser
                anchors.left: parent.left
            }

            AnchorChanges {
                target: automationMenu
                anchors.left: pluser.right
            }
            PropertyChanges {
                target: automationMenu
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: rowTextFields
                anchors.left: automationMenu.right
            }
            PropertyChanges {
                target: rowTextFields
                anchors.leftMargin: scaleSize2(15)
            }
        }]

}
