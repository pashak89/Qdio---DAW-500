import QtQuick 2.12
import "."
import "qrc:/"
import "qrc:/Items"

Item {


    height: volumeTextField.height
    width: volumeTextField.width * 4 + scaleSize2(15) * 3

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

    states: [
        State {
            name: "Right"
            when: !globalValues.tracksOnLeft

            AnchorChanges {
                target: volumeTextField
                anchors.right: parent.right
                anchors.left: undefined
            }

            AnchorChanges {
                target: panTextField
                anchors.right: volumeTextField.left
                anchors.left: undefined
            }
            PropertyChanges {
                target: panTextField
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: undefined
            }

            AnchorChanges {
                target: sendRecordBtns
                anchors.right: panTextField.left
                anchors.left: undefined
            }
            PropertyChanges {
                target: sendRecordBtns
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: undefined
            }

            AnchorChanges {
                target: autoViewBtns
                anchors.right: sendRecordBtns.left
                anchors.left: undefined
            }
            PropertyChanges {
                target: autoViewBtns
                anchors.rightMargin: scaleSize2(15)
                anchors.leftMargin: undefined
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: volumeTextField
                anchors.right: undefined
                anchors.left: parent.left
            }

            AnchorChanges {
                target: panTextField
                anchors.right: undefined
                anchors.left: volumeTextField.right
            }
            PropertyChanges {
                target: panTextField
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: sendRecordBtns
                anchors.right: undefined
                anchors.left: panTextField.right
            }
            PropertyChanges {
                target: sendRecordBtns
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: autoViewBtns
                anchors.right: undefined
                anchors.left: sendRecordBtns.right
            }
            PropertyChanges {
                target: autoViewBtns
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }
        }
    ]
}
