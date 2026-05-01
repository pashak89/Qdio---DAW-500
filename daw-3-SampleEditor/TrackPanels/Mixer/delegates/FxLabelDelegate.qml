// TrackPanel/delegates/FxLabelDelegate.qml
import QtQuick 2.15
import Global 1.0
import "../../../Items"
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

Item {
    property var modelData_

    height: Global.scaleSize2(57)
    width: parent.width

    Rectangle {
        z: label.z + 1
        anchors.fill: parent
        color: "transparent"

        radius: topRadius
        visible: pluginPopup_.visible

        border.color: theme.mixerDD11_Select1_BRD
        border.width: Global.scaleSize2(3)
    }

    CustomLabel {
        id: label
        height: Global.scaleSize2(57)
        width: parent.width

        color: modelData_.enabled ? theme.mixerDD10_DeSelect_FIL : theme.mixerDD100_DeActive_FIL
        fontSize: topfontPixelSize
        fontFamilty: topFontFamily
        fontColor: modelData_.enabled ? theme.f1_ : theme.f5_
        radius: Global.scaleSize2(5)
        text: modelData_.name
        leftPaddingTxt: Global.scaleSize2(15)
        horizontalTextAlignment: Text.AlignHCenter
    }
}
