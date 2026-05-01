// TrackPanel/components/SendsSection.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import Global 1.0
import "../delegates"
import "../../../Items"

Rectangle {
    id: root

    property string title
    property var effectModel
    property bool draggingEnabled
    property var pluginPopup
    property var clipArea
    property var areaInfo

    color: theme.bg5
    radius: topRadius
    width: parent ? parent.width : Global.scaleSize2(300)
    height: contentCol.implicitHeight + 2 * Global.scaleSize2(11)

    Column {
        id: contentCol
        width: parent.width - Global.scaleSize2(21)
        anchors.centerIn: parent
        spacing: Global.scaleSize2(10)

        CustomLabel {
            fontSize: topfontPixelSize
            fontFamilty: topFontFamily
            fontColor: theme.f9_
            height: Global.scaleSize2(38)
            width: parent.width
            radius: topRadius
            text: root.title
        }

        ReorderableListView {
            width: parent.width
            radius: topRadius

            model: root.effectModel
            rowHeight: Global.scaleSize2(57)

            draggingEnabled: root.draggingEnabled
            spacing: Global.scaleSize2(5)
            longPressMs: 350
            proxyTextRole: "name"

            contentDelegate_: Item {
                anchors.fill: parent

                Loader {
                    id: sendsLoader
                    anchors.fill: parent
                    property var modelData_: modelData
                    sourceComponent: sendsComp
                    onLoaded: sendsLoader.item.modelData_ = modelData
                }

                HoverHandler {
                    acceptedDevices: PointerDevice.Mouse

                    onHoveredChanged: {
                        if (hovered) {
                            sendsLoader.sourceComponent = sendsComp
                        }
                    }
                }
            }

            Component {
                id: sendsComp
                SendsLabelDelegate {
                    pluginPopup: root.pluginPopup
                    clipArea: root.clipArea
                    areaInfo: root.areaInfo
                }
            }

            proxyDelegate: Component {
                CustomLabel {
                    property var modelData_
                    height: Global.scaleSize2(57)
                    color: modelData_.enabled ? theme.mixerDD10_DeSelect_FIL : theme.mixerDD100_DeActive_FIL
                    fontSize: root.topfontPixelSize
                    fontFamilty: root.topFontFamily
                    fontColor: modelData_.enabled ? theme.f1_ : theme.f5_
                    radius: Global.scaleSize2(5)
                    width: parent.width
                    text: modelData_.name
                    leftPaddingTxt: Global.scaleSize2(15)
                    horizontalTextAlignment: Text.AlignLeft
                }
            }
        }

        Item {
            width: parent.width
            height: Global.scaleSize2(57)

            Loader {
                id: blankLoader
                anchors.fill: parent
                sourceComponent: blankComp
            }

            HoverHandler {
                acceptedDevices: PointerDevice.Mouse
                onHoveredChanged: blankLoader.sourceComponent = hovered ? hoverComp : blankComp
            }

            // reuse FX blank/hover visuals
            Component {
                id: blankComp
                FxBlankDelegate {}
            }
            Component {
                id: hoverComp
                FxHoverDelegate {

                    pluginPopup: root.pluginPopup
                }
            }
        }
    }
}
