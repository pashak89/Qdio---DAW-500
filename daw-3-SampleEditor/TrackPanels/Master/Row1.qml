import QtQuick 2.12
import '.'
import 'qrc:/'
import "qrc:/Items"

Item {

    // width: scaleSize2(10) + selectBtn.width + soloBtn.width + nameText.width + scaleSize2(15) * 2
    height: scaleSize2(10) + selectBtn.height

    Select {
        id: selectBtn
        anchors {
            top: parent.top; topMargin: scaleSize2(10)
        }
    }


    Solo {
        id: soloBtn
        anchors {
            top: selectBtn.top
        }
    }




    TrackName {
        id: nameText
        anchors {
            left: soloBtn.right; leftMargin: scaleSize2(15)
            top: selectBtn.top
        }
        width: 50

        z: editName ? 1 : 0
    }



    MouseArea {
        id: maRoot
        anchors.fill: nameText
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        // property bool allow2clear: false

        onClicked: {
            if(groupTrackLight.length === 1 & !holdCtrl & !holdShift & allow2clear)
            {
                groupTrackLight.length = 0
                funcs.offLightAllTracks()
            }
        }

        onPressed: {
            parent.focus = true
            root.focus = true

            if(mouse.button === Qt.RightButton)
                funcs.trackLighting_rightclick(ind, 'btn')
            else
                funcs.trackLighting(ind, 'btn')

            if (mouse.button === Qt.RightButton) {
                positionInWindow = mapToItem(track_list.contentItem, mouse.x, mouse.y)
                trackRightClick.numberItem = ind
                trackRightClick.visble_grouper = funcs.allow2MakeGroup()
                trackRightClick.visble_ungrouper = groupTrackLight.length === 1 & trackMasterModel.get(groupTrackLight[0]).type === 'Group'
                trackRightClick.visible = true

                if(rightPanel.visible)
                    rightPanel.visible = false
            }
            else
                trackRightClick.visible = false
        }
    }



    states: [State {
            name: "Right"
            when:!globalValues.tracksOnLeft

            AnchorChanges {
                target: selectBtn
                anchors.right: soloBtn.left
            }
            PropertyChanges {
                target: selectBtn
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: soloBtn
                anchors.right: parent.right
            }
            PropertyChanges {
                target: soloBtn
                anchors.rightMargin: 0
            }

            AnchorChanges {
                target: nameText
                anchors.right: selectBtn.left
                anchors.left: parent.left
            }
            PropertyChanges {
                target: nameText
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: scaleSize2(10)
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: selectBtn
                anchors.left: parent.left
            }
            PropertyChanges {
                target: selectBtn
                anchors.leftMargin: 0
            }

            AnchorChanges {
                target: soloBtn
                anchors.left: selectBtn.right
            }
            PropertyChanges {
                target: soloBtn
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: nameText
                anchors.right: parent.right
                anchors.left: soloBtn.right
            }
            PropertyChanges {
                target: nameText
                anchors.rightMargin: scaleSize2(10)
                anchors.leftMargin: scaleSize2(15)
            }

        }]

}
