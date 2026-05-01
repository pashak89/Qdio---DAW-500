import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.2
import TrackItem 1.0
import "qrc:/Items"
import "qrc:/"

Rectangle {
    id: automationCondition

    visible: automationBtn.isClicked

    height: scaleSize2(37)
    width: scaleSize2(106) * 4 + 3 * scaleSize2(15)

    color: 'transparent'

    Rectangle {
        id: pluser
        height: scaleSize2(37)
        width: scaleSize2(37)
        anchors.verticalCenter: parent.verticalCenter
        radius: Math.round(height / 2)
        border.color: theme.s11_Enable_BRD
        border.width: topWidth
        color: theme.s11_Enable_FIL

        Image {
            id: img_addAutomation
            anchors.centerIn: parent
            anchors.fill: parent
            anchors.margins: Math.round(parent.height / 5)
            sourceSize: Qt.size(height, height)
            source: 'qrc:/Resource/Plus.svg'
            fillMode: Image.PreserveAspectFit
            antialiasing: false
            smooth: false
            clip: true
            mipmap: true

            ColorOverlay {
                id: cl
                anchors.fill: parent
                source: parent
                color: automationMenuTitle === 'None' ? "#737373" : theme.tColor
                antialiasing: false
                smooth: false
            }
        }

        MouseArea {
            enabled: automationMenuTitle !== 'None'
            anchors.fill: parent
            onClicked: {

                var model_ = _clipArea.tracksModel.automationLaneModel(ind)

                model_.append(model.automationMenuTitle,
                              model.automationSubMenuTitle, 3)


                model.automationMenuTitle = 'None,None'
            }
        }
    }

    AutomationDropMenu {
        id: automationMenu

        width: scaleSize2(106) * 2 + 2 * scaleSize2(15) + scaleSize2(53)
        isRootMenu: true
        height: scaleSize2(37)

        anchors.verticalCenter: parent.verticalCenter
        displayMenu: automationSubMenuTitle
        //effectList: model.EffectList
    }

    RowLayout {
        id: rowTextFields

        opacity: automationMenuTitle !== 'None' ? 1 : 0
        enabled: automationMenuTitle !== 'None'

        width: scaleSize2(106)
        height: scaleSize2(37)

        anchors.verticalCenter: parent.verticalCenter
        Rectangle {
            visible: automationSubMenuTitle === 'KeyFrames'
            width: scaleSize2(106)
            height: scaleSize2(37)
            //gradient: _3DGradient
            radius: topRadius
            color: "transparent"
            RowLayout {
                anchors.fill: parent
                spacing: topWidth

                TrackButton {
                    id: bezierButton
                    imgVisible: false
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    property bool selected: keyFrameType === 1
                    name: 'B'
                    isSoloBtn: true
                    fontColor: selected /*isClicked*/
                               ? theme.f4_ : theme.f2_

                    fillStyle: selected /*isClicked*/
                               ? theme.b12_Enable2_FIL : theme.b20_Disable_FIL
                    brColor: selected /*isClicked*/
                             ? theme.b12_Enable2_BRD : theme.b20_Disable_BRD

                    onSendMessage: {

                        if (msg === "released") {

                            model.keyFrameType = 1

                        }
                    }
                }
                TrackButton {
                    id: linerButton
                    imgVisible: false
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    property bool selected: keyFrameType === 0
                    name: 'L'
                    isSoloBtn: true
                    fontColor: selected /*isClicked*/
                               ? theme.f4_ : theme.f2_

                    fillStyle: selected /*isClicked*/
                               ? theme.b12_Enable2_FIL : theme.b20_Disable_FIL
                    brColor: selected /*isClicked*/
                             ? theme.b12_Enable2_BRD : theme.b20_Disable_BRD

                    onSendMessage: {

                        if (msg === "released") {

                             model.keyFrameType = 0
                        }
                    }
                }
            }
        }
        Rectangle {
            visible: automationMenuTitle !== 'None'
                     && automationSubMenuTitle !== 'KeyFrames'
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: 'transparent'
            radius: topRadius

            TrackTextField2 {
                id: tfVolume
                name: automationSubMenuTitle

                anchors.fill: parent

                isPan: automationSubMenuTitle === 'Pan'
                isVolume: automationSubMenuTitle === 'Volume'
                isOnOff: automationSubMenuTitle === 'DeviceOn'
                         | automationSubMenuTitle === 'SpeakerOn'

                max: 100
                min: 0
                decimal: automationSubMenuTitle === 'Pan' ? 0 : 2
                has2Slider: automationSubMenuTitle === 'Pan'
                hasSlider: automationSubMenuTitle !== 'Pan'

                // bigHeight: true
                enableRightClick: true
                shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
                moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                flColor: theme.s11_Enable_FIL
                brColor: theme.s11_Enable_BRD

                property real automatedValue: model.automatedValue
                property real automatedNormalized: model.automatedNormalized
                property real automatedActive: model.automatedActive
                property real automatedEnabled: model.automatedEnabled
                property real normalized: model.normalized

                text: model.value.toFixed(decimal)
                defaultValue: model.defaultNormalized * 100
                value: model.normalized * 100

                onSigPressed: {
                    _clipArea.tracksModel.saveUndoNoramized(
                                ind, automationMenuTitle,
                                automationSubMenuTitle)
                }

                onAutomatedEnabledChanged: {

                    if (!model.automatedEnabled) {
                        shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                        moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                    } else {
                        tfVolume.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                        tfVolume.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                    }
                }
                onAutomatedNormalizedChanged: {

                    if (model.automatedEnabled) {

                        value = model.automatedNormalized * 100
                        text = model.automatedValue.toFixed(decimal)
                    } else {

                    }
                }

                onNormalizedChanged: {

                    value = model.normalized * 100
                    text = model.value.toFixed(decimal)
                }
                onSigValueChanged: {

                    if (model.automatedActive) {
                        if (model.automatedEnabled) {
                            model.automatedEnabled = false
                            shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                            moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                        }
                    }
                    model.normalized = value / 100.0

                    tfVolume.text = model.value.toFixed(decimal)
                    tfVolume.defaultValue = model.normalized * 100
                }

                onAction: {

                    if (text == "Re-Enable Automation") {

                        value = model.automatedNormalized * 100

                        model.automatedEnabled = true

                        tfVolume.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                        tfVolume.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                    }

                    if (text == "Delete Automation") {

                        //model.clear = true
                        _clipArea.tracksModel.automationClear(
                                    ind, automationMenuTitle,
                                    automationSubMenuTitle)
                    }
                }

                Rectangle {
                    z: 1000
                    visible: model.automatedActive
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 1
                    anchors.topMargin: 0
                    color: model.automatedEnabled ? "#EA2027" : "#808080"
                    width: scaleSize2(11)
                    height: scaleSize2(11)
                    radius: scaleSize2(11)
                }
            }
        }
    }

    states: [
        State {
            name: "Right"
            when: !globalValues.tracksOnLeft

            AnchorChanges {
                target: pluser
                anchors.right: parent.right
                anchors.left: undefined
            }

            AnchorChanges {
                target: automationMenu
                anchors.right: pluser.left
                anchors.left: undefined
            }
            PropertyChanges {
                target: automationMenu
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: undefined
            }

            AnchorChanges {
                target: rowTextFields
                anchors.right: automationMenu.left
                anchors.left: undefined
            }
            PropertyChanges {
                target: rowTextFields
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: undefined
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: pluser
                anchors.right: undefined
                anchors.left: parent.left
            }

            AnchorChanges {
                target: automationMenu
                anchors.right: undefined
                anchors.left: pluser.right
            }
            PropertyChanges {
                target: automationMenu
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: rowTextFields
                anchors.right: undefined
                anchors.left: automationMenu.right
            }
            PropertyChanges {
                target: rowTextFields
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }
        }
    ]
}
