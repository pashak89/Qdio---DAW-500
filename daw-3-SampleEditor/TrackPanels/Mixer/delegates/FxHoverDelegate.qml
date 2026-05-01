// TrackPanel/delegates/FxHoverDelegate.qml
import QtQuick 2.15
import QtGraphicalEffects 1.15
import Global 1.0

Rectangle {
    id: delegateRoot

    property var modelData_

    property var pluginPopup
    // PluginPopup instance to open

    // match row sizing
    width: parent ? parent.width : Global.scaleSize2(300)
    height: Global.scaleSize2(57)

    color: theme.mixerDD11_Select1_FIL

    Rectangle {
        id: borderRec
        z: delegateRoot.z + 1
        anchors.fill: parent
        color: "transparent"
        border.color: theme.mixerDD11_Select1_BRD
        border.width: Global.scaleSize2(3)
        radius: topRadius
    }

    Item {
        id: dropdownButton
        anchors.right: parent.right
        anchors.rightMargin: Global.scaleSize2(20)
        anchors.verticalCenter: parent.verticalCenter

        width: Global.scaleSize2(25)
        height: Global.scaleSize2(25)

        Image {
            id: icon
            anchors.fill: parent
            source: "qrc:/resources/svg/track_list/Triangle.svg"
            smooth: true

            ColorOverlay {
                anchors.fill: icon
                source: icon
                color: theme.button_unclicked_color
            }
        }

        MouseArea {
            anchors.fill: parent
            propagateComposedEvents: true
            onPressed: {

                if (!pluginPopup)
                    return
                if (pluginPopup.visible) {
                    pluginPopup.close()
                    return
                }

                pluginPopup.openBelow(delegateRoot, Global.scaleSize2(2))
            }
        }
    }
}
