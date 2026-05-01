// TrackPanel/delegates/FxEditDelegate.qml
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import Global 1.0

Rectangle {
    id: root
    property var modelData_
    property var pluginPopup
    property var clipArea
    property var areaInfo

    height: Global.scaleSize2(57)
    width: parent ? parent.width : Global.scaleSize2(300)

    color: modelData_.enabled ? theme.mixerDD21_Select2_FIL : theme.mixerDD11_Select1_FIL
    radius: Global.radiusCal(3)

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: theme.mixerDD11_Select1_BRD
        border.width: Global.scaleSize2(3)
        radius: topRadius
    }

    RowLayout {
        anchors.fill: parent

        // left circle enable/disable
        Rectangle {
            Layout.preferredWidth: parent.height - Global.scaleSize2(19)
            Layout.preferredHeight: Layout.preferredWidth
            Layout.leftMargin: Global.scaleSize2(19)
            radius: width / 2
            color: modelData_.enabled ? theme.b31_Enable_FIL : theme.b300_Disable2_FIL
            border.color: theme.bg5
            border.width: 1

            MouseArea {
                anchors.fill: parent
                onClicked: modelData_.enabled = !modelData_.enabled
            }
        }

        // open/close plugin UI
        Item {
            Layout.preferredWidth: Global.scaleSize2(47)
            Layout.preferredHeight: Global.scaleSize2(47)
            Layout.leftMargin: Global.scaleSize2(3)

            Rectangle {
                anchors.fill: parent
                color: modelData_.controlVisible ? theme.b31_Enable_FIL : theme.b300_Disable2_FIL
                border.color: theme.b300_Disable2_BRD
                border.width: 1
                radius: Global.scaleSize2(5)
            }

            Image {
                id: icon
                anchors.fill: parent
                anchors.margins: Global.scaleSize2(7)
                source: "qrc:/Resource/ShowPlugin_Icon.svg"
                scale: 0.9

                ColorOverlay {
                    anchors.fill: icon
                    source: icon
                    color: theme.bg5
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (!clipArea || !areaInfo)
                        return
                    var trackIndex = areaInfo.selectedTrackItem
                    var eff = clipArea.tracksModel.effectChain(
                                trackIndex).getEffect(modelData_.name)
                    if (!eff)
                        return

                    if (modelData_.controlVisible === false)
                        eff.showControl()
                    else
                        eff.hideControl()
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }

        // right dropdown arrow (visual only in your original edit delegate)
        Item {
            Layout.preferredWidth: Global.scaleSize2(25)
            Layout.preferredHeight: Global.scaleSize2(25)
            Layout.rightMargin: Global.scaleSize2(20)
            Layout.alignment: Qt.AlignHCenter

            Image {
                id: icon2
                anchors.fill: parent
                source: "qrc:/resources/svg/track_list/Triangle.svg"
                smooth: true

                ColorOverlay {
                    anchors.fill: icon2
                    source: icon2
                    color: theme.button_unclicked_color
                }
            }

            MouseArea {
                anchors.fill: parent
                propagateComposedEvents: true
                onClicked: {

                    if (!pluginPopup)
                        return
                    if (pluginPopup.visible) {
                        pluginPopup.close()
                        return
                    }

                    pluginPopup.openBelow(root, Global.scaleSize2(2))
                }
            }
        }
    }
}
