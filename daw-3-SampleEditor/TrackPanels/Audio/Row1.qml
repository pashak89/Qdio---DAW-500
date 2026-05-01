import QtQuick 2.12
import "."
import "qrc:/"
import "qrc:/Items"

Item {

    height: scaleSize2(37)

    Select {
        id: selectBtn
        anchors {
            top: parent.top
            bottom: parent.bottom
        }

        moduls: !model.automatedSpeakerOnEnabled
                && model.automatedSpeakerOnActive ? ['Show Automation', 'Show Automation in New Lane', 'Re-Enable Automation', 'Delete Automation'] : ['Show Automation', 'Show Automation in New Lane', 'Delete Automation']

        activated: model.trackActivated
        mute: model.speakerOnValue === 0

        property real automatedValue: model.automatedSpeakerOnValue
        property real automatedNormalized: model.automatedSpeakerOnNormalized
        property real automatedActive: model.automatedSpeakerOnActive
        property real automatedEnabled: model.automatedSpeakerOnEnabled
        property real normalized: model.speakerOnNormalized

        onNormalizedChanged: {
            selectBtn.mute = model.speakerOnValue === 0
        }

        onAutomatedEnabledChanged: {

            if (!model.automatedSpeakerOnEnabled) {
                selectBtn.mute = model.speakerOnValue === 0
                selectBtn.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
            } else {
                selectBtn.mute = model.speakerOnValue === 0
                selectBtn.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
            }
        }
        onAutomatedValueChanged: {
            if (model.automatedSpeakerOnEnabled) {

                selectBtn.mute = model.speakerOnValue === 0
            } else {

            }
        }

        Rectangle {
            id: automatedLight
            z: 1000
            visible: model.automatedSpeakerOnActive
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 1
            anchors.topMargin: 0
            color: model.automatedSpeakerOnEnabled ? "#EA2027" : "#808080"
            width: scaleSize2(11)
            height: scaleSize2(11)
            radius: scaleSize2(11)
        }

        onAction: {

            if (text == "Re-Enable Automation") {
                model.automatedEnabled = true
                selectBtn.moduls
                        = ['Show Automation', 'Show Automation in New Lane', 'Delete Automation']
            }

            if (text == "Delete Automation") {
                model.clear = true
            }
        }

        anchors.verticalCenter: parent.verticalCenter
    }

    Solo {
        id: soloBtn

        anchors {
            top: parent.top
            bottom: parent.bottom
        }
    }
    Row {
        id: row
        anchors.left: soloBtn.right
        anchors.leftMargin: 2
        anchors {
            top: parent.top
            bottom: parent.bottom
        }

        Text {
            text: model.inputChannelsCount
        }
        Text {
            text: ","
        }

        Text {
            text: model.outputChannelsCount
        }
    }

    //    TrackName {
    //        id: nameText
    //        anchors.verticalCenter: parent.verticalCenter
    //        anchors {
    //            left: soloBtn.right; leftMargin: scaleSize(15)
    //            top: selectBtn.top
    //        }
    //        width: 50
    //        z: editName ? 1 : 0
    //    }
    //    MouseArea {
    //        id: maRoot
    //        anchors.fill: nameText
    //        acceptedButtons: Qt.LeftButton | Qt.RightButton

    //        // property bool allow2clear: false
    //        onClicked: {
    //            if (groupTrackLight.length === 1 & !holdCtrl & !holdShift & allow2clear) {
    //                groupTrackLight.length = 0
    //                funcs.offLightAllTracks()
    //            }
    //        }

    //        onPressed: {
    //            parent.focus = true
    //            root.focus = true

    //            if (mouse.button === Qt.RightButton) {

    //                //funcs.trackLighting_rightclick(ind, 'btn')
    //            } else {
    //                model.selectedTrack = true
    //            }

    //            if (mouse.button === Qt.RightButton) {
    //                positionInWindow = mapToItem(track_list.contentItem,
    //                                             mouse.x, mouse.y)
    //                trackRightClick.numberItem = ind
    //                trackRightClick.visble_grouper = funcs.allow2MakeGroup()
    //                trackRightClick.visble_ungrouper = groupTrackLight.length === 1
    //                        & trackAudioModel.get(
    //                            groupTrackLight[0]).type === 'Group'
    //                trackRightClick.visible = true

    //                if (rightPanel.visible)
    //                    rightPanel.visible = false
    //            } else
    //                trackRightClick.visible = false
    //        }
    //    }
    states: [
        State {
            name: "Right"
            when: !globalValues.tracksOnLeft

            AnchorChanges {
                target: selectBtn
                anchors.right: soloBtn.left
                anchors.left: undefined
            }
            PropertyChanges {
                target: selectBtn
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: undefined
            }

            AnchorChanges {
                target: soloBtn
                anchors.right: parent.right
                anchors.left: undefined
            }
            PropertyChanges {
                target: soloBtn
                anchors.rightMargin: 0
                anchors.leftMargin: undefined
            }

            //            AnchorChanges {
            //                target: nameText
            //                anchors.right: selectBtn.left
            //                anchors.left: parent.left
            //            }
            //            PropertyChanges {
            //                target: nameText
            //                anchors.rightMargin: scaleSize2(15)
            //                anchors.leftMargin: scaleSize2(10)
            //            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: selectBtn
                anchors.right: undefined
                anchors.left: parent.left
            }
            PropertyChanges {
                target: selectBtn
                anchors.rightMargin: undefined
                anchors.leftMargin: 0
            }

            AnchorChanges {
                target: soloBtn
                anchors.right: undefined
                anchors.left: selectBtn.right
            }
            PropertyChanges {
                target: soloBtn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            //            AnchorChanges {
            //                target: nameText
            //                anchors.right: parent.right
            //                anchors.left: soloBtn.right
            //            }
            //            PropertyChanges {
            //                target: nameText
            //                anchors.rightMargin: scaleSize2(10)
            //                anchors.leftMargin: scaleSize2(15)
            //            }
        }
    ]
}
