// TrackPanel/menus/RowMenu.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import Global 1.0

Menu {
    id: rowMenu

    property var theme
    property int topfontPixelSize
    property string topFontFamily

    background: Rectangle {
        implicitWidth: Global.scaleSize2(340)
        color: theme.w3_FILL
        radius: Global.scaleSize2(5)
        border.color: theme.w3_BRD
        border.width: Global.scaleSize2(3)
    }

    ButtonGroup {
        id: colorGroup
        exclusive: true
    }

    component ColorItem: MenuItem {
        id: menuItem
        property string name
        property color dotColor

        checkable: true
        ButtonGroup.group: colorGroup

        implicitWidth: Global.scaleSize2(220)
        implicitHeight: Global.scaleSize2(56)

        background: Rectangle {
            anchors.fill: parent
            color: menuItem.highlighted ? theme.bg6 : "transparent"
        }

        contentItem: Item {
            anchors.fill: parent

            Row {
                spacing: Global.scaleSize2(20)
                anchors.left: parent.left
                anchors.leftMargin: Global.scaleSize2(5)
                anchors.fill: parent

                Rectangle {
                    width: Global.scaleSize2(20)
                    height: Global.scaleSize2(20)
                    radius: width / 2
                    color: dotColor
                    border.width: dotColor === "transparent" ? 1 : 0
                    border.color: theme.bg5_new
                    y: (parent.height - height) / 2
                }

                Text {
                    text: name
                    color: menuItem.highlighted ? theme.f10_ : theme.f1_
                    font.family: topFontFamily
                    font.pixelSize: topfontPixelSize
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }
        }
    }

    ColorItem {
        name: "No Color"
        dotColor: "transparent"
    }
    ColorItem {
        name: "Red"
        dotColor: "#EE3743"
    }
    ColorItem {
        name: "Orange"
        dotColor: "#FFA138"
    }
    ColorItem {
        name: "Yellow"
        dotColor: "#FFEE00"
    }
    ColorItem {
        name: "Green"
        dotColor: "#1CE637"
    }
    ColorItem {
        name: "Blue"
        dotColor: "#3DB5FF"
    }
    ColorItem {
        name: "Pink"
        dotColor: "#D55CED"
    }
}
