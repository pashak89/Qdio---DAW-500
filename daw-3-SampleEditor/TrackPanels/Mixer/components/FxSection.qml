// TrackPanel/components/FxSection.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import Global 1.0
import "../delegates"
import "../../../Items"

Rectangle {
    id: root

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
            text: "FX"
        }

        ReorderableListView {
            id: reorderView
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
                    id: loader
                    anchors.fill: parent
                    property var modelData_: modelData
                    sourceComponent: fxLabelComp
                    onLoaded: loader.item.modelData_ = modelData
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    propagateComposedEvents: true
                    onEntered: loader.sourceComponent = fxEditComp
                    onExited: loader.sourceComponent = fxLabelComp
                }
            }

            Component {
                id: fxLabelComp

                FxLabelDelegate {}
            }

            Component {
                id: fxEditComp

                FxEditDelegate {
                    pluginPopup: root.pluginPopup
                    clipArea: root.clipArea
                    areaInfo: root.areaInfo
                }
            }

            proxyDelegate: Component {

                FxLabelDelegate {

                    modelData_: proxyData
                }
            }
        }

        // blank row + hover row
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
