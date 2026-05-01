import QtQuick 2.12
import QtQuick.Controls 2.15

TabButton {
    id: root

    height: parent.height
    anchors.top: parent.top
    checked: filterVisualizer.EQ4_activeBand === index
    onCheckedChanged: {
        console.log(filterVisualizer.EQ4_activeBand,index)

    }

    background: Rectangle {
        id: background
        color: root.checked ? theme.bg2 : 'transparent'

        radius: topRadius
    }
    onClicked: {
        filterVisualizer.saveUndoOperation()
        filterVisualizer.EQ4_activeBand = index
    }

    contentItem: Item {
        anchors.fill: background

        Button {
            id: control
            anchors {
                left: parent.left
                leftMargin: scaleSize(14)
                right: parent.right
                rightMargin: scaleSize(14)
                bottom: parent.bottom
                bottomMargin: scaleSize(25)
                top: parent.top
                topMargin: scaleSize(12)
            }

            property bool isChecked: model.Enabled

            //Component.onCompleted: isChecked = root.checked
            background: Rectangle {

                radius: topRadius
                border {
                    width: topWidth
                    color: theme.b10_Disable_BRD
                }
                color: control.isChecked ? trackEnable ? theme.b11_Enable1_FIL : theme.b20_Disable_FIL : theme.b10_Disable_FIL

                Rectangle {
                    visible: control.focus & (control.isChecked
                                              | (root.checked & !control.isChecked))
                    anchors.fill: parent
                    color: theme.b10_Disable_BRD
                    anchors.margins: -refSize
                    radius: topRadius
                    z: -1
                }
            }

            InfoText {
                text: root.text
                color: control.isChecked ? theme.f4_ : theme.f1_
            }

            onPressed: {

                filterVisualizer.saveUndoOperation()
                filterVisualizer.setBandEnable(index, !control.isChecked)
            }
            onReleased: {
                if (control.isChecked) {
                    filterVisualizer.EQ4_activeBand = index
                }
            }

            onClicked: control.focus = true
        }
    }

    // bottom corners
    Rectangle {
        id: cornerRec

        anchors {
            left: parent.left
            leftMargin: -background.radius
            right: parent.right
            rightMargin: -background.radius
            bottom: parent.bottom
            bottomMargin: -background.radius
        }

        height: topRadius * 3
        color: background.color
    }

    Rectangle {
        visible: root.checked
        anchors {
            right: cornerRec.right
            rightMargin: -background.radius
            bottom: parent.bottom
            bottomMargin: background.radius
        }

        width: radius * 2
        height: width
        radius: topRadius
        color: theme.bg1
    }

    Rectangle {
        visible: root.checked
        anchors {
            left: cornerRec.left
            leftMargin: -background.radius
            bottom: parent.bottom
            bottomMargin: background.radius
        }

        width: radius * 2
        height: width
        radius: topRadius
        color: theme.bg1
    }
}
