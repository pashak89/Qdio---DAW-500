// TrackPanel/delegates/FxBlankDelegate.qml
import QtQuick 2.15
import Global 1.0

Rectangle {

    property var modelData_

    height: Global.scaleSize2(57)
    width: parent ? parent.width : Global.scaleSize2(300)
    color: theme.mixerDD100_DeActive_FIL
    radius: Global.scaleSize2(5)

    border.color: theme.mixerDD11_Select1_BRD
    border.width: pluginPopup_.visible ? Global.scaleSize2(3) : 0
}
