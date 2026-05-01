import QtQuick 2.12
import '.'
import 'qrc:/'
import "qrc:/Items"

Item {

    height: volumeTextField.height
    width: volumeTextField.width * 4 + scaleSize2(15)*3

    Volume {
        id: volumeTextField
    }

    Pan {
        id: panTextField
    }

    SendRecord {
        id: sendRecordBtns
    }

    AutoView {
        id: autoViewBtns
    }



    states: [State {
            name: "Right"
            when:!globalValues.tracksOnLeft

            AnchorChanges {
                target: volumeTextField
                anchors.right: parent.right
            }

            AnchorChanges {
                target: panTextField
                anchors.right: volumeTextField.left
            }
            PropertyChanges {
                target: panTextField
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: sendRecordBtns
                anchors.right: panTextField.left
            }
            PropertyChanges {
                target: sendRecordBtns
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: autoViewBtns
                anchors.right: sendRecordBtns.left
            }
            PropertyChanges {
                target: autoViewBtns
                anchors.rightMargin: scaleSize2(15)
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: volumeTextField
                anchors.left: parent.left
            }

            AnchorChanges {
                target: panTextField
                anchors.left: volumeTextField.right
            }
            PropertyChanges {
                target: panTextField
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: sendRecordBtns
                anchors.left: panTextField.right
            }
            PropertyChanges {
                target: sendRecordBtns
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: autoViewBtns
                anchors.left: sendRecordBtns.right
            }
            PropertyChanges {
                target: autoViewBtns
                anchors.leftMargin: scaleSize2(15)
            }

        }]
}
