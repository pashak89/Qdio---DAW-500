// TrackPanel/components/FxSection.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import Global 1.0
import QtGraphicalEffects 1.15

import "../delegates"
import "../../../Items"

Rectangle {
    id: root

    property var effectModel
    property bool draggingEnabled
    property var ioPopup
    property var clipArea
    property var areaInfo

    property int outputChannelsCount: model.outputChannelsCount

    color: theme.bg5
    radius: topRadius
    width: parent ? parent.width : Global.scaleSize2(300)
    height: contentCol.implicitHeight + 2 * Global.scaleSize2(11)
    onOutputChannelsCountChanged: {

        console.log("outputChannelsCount", outputChannelsCount)
        channelListModel.setChannels(outputChannelsCount)
    }
    Component.onCompleted: {
        channelListModel.setChannels(outputChannelsCount)
    }
    ListModel {
        id: channelListModel

        ListElement {
            name: "Stereo"
            value: 2
            checked: false
        }
        ListElement {
            name: "5.0"
            value: 5
            checked: false
        }
        ListElement {
            name: "5.1"
            value: 6
            checked: false
        }
        ListElement {
            name: "7.1"
            value: 8
            checked: false
        }
        ListElement {
            name: "7.1.4"
            value: 12
            checked: false
        }
        ListElement {
            name: "9.1"
            value: 10
            checked: false
        }
        ListElement {
            name: "9.1.4"
            value: 14
            checked: false
        }
        ListElement {
            name: "22.2"
            value: 24
            checked: false
        }

        // Select item by real channel count
        function setChannels(outputChannelsCount) {
            for (var i = 0; i < count; i++) {
                setProperty(i, "checked", get(i).value === outputChannelsCount)
            }
        }

        // Returns selected item { name, value, checked }
        function getChecked() {
            for (var i = 0; i < count; i++) {
                if (get(i).checked)
                    return get(i)
            }
            return null
        }

        function getCheckedName() {
            for (var i = 0; i < count; i++) {
                if (get(i).checked)
                    return get(i).name
            }
            return ""
        }
    }

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
            text: "I/O"
        }

        CustomLabel {

            height: Global.scaleSize2(57)
            width: parent.width

            color: theme.mixerDD10_DeSelect_FIL
            fontSize: topfontPixelSize
            fontFamilty: topFontFamily
            fontColor: theme.f1_
            radius: Global.scaleSize2(5)
            text: "Ext. In"
            leftPaddingTxt: Global.scaleSize2(15)
            horizontalTextAlignment: Text.AlignHCenter
        }

        Item {
            height: Global.scaleSize2(57)
            width: parent.width
            Loader {
                id: loader
                anchors.fill: parent
                sourceComponent: fxLabelComp
            }

            HoverHandler {

                acceptedDevices: PointerDevice.Mouse

                onHoveredChanged: {

                    if (hovered) {
                        loader.sourceComponent = fxEditComp
                    } else {
                        loader.sourceComponent = fxLabelComp
                    }
                }
            }
        }
    }

    Component {
        id: fxEditComp

        Item {
            id: delegateRoot

            width: parent.width
            height: Global.scaleSize2(57)

            Rectangle {
                id: borderRec
                z: label.z + 1
                anchors.fill: parent

                color: "transparent"

                radius: topRadius

                border.color: theme.mixerDD11_Select1_BRD
                border.width: Global.scaleSize2(3)
            }

            CustomLabel {
                id: label
                height: Global.scaleSize2(57)
                width: parent.width

                color: theme.mixerDD10_DeSelect_FIL
                fontSize: topfontPixelSize
                fontFamilty: topFontFamily
                fontColor: theme.f1_
                radius: Global.scaleSize2(5)
                text: channelListModel.getCheckedName()
                leftPaddingTxt: Global.scaleSize2(15)
                horizontalTextAlignment: Text.AlignHCenter

                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
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

                        if (!ioPopup)
                            return
                        if (ioPopup.visible) {
                            ioPopup.close()
                            return
                        }

                        ioPopup.openBelow(delegateRoot, Global.scaleSize2(5))
                    }
                }
            }
        }
    }

    Component {
        id: fxLabelComp

        Item {
            width: parent.width
            height: Global.scaleSize2(57)

            Rectangle {
                id: borderRec
                z: label.z + 1
                anchors.fill: parent

                color: "transparent"
                visible: ioPopup.visible

                radius: topRadius

                border.color: theme.mixerDD11_Select1_BRD
                border.width: Global.scaleSize2(3)
            }

            CustomLabel {
                id: label
                height: Global.scaleSize2(57)
                width: parent.width

                color: theme.mixerDD10_DeSelect_FIL
                fontSize: topfontPixelSize
                fontFamilty: topFontFamily
                fontColor: theme.f1_
                radius: Global.scaleSize2(5)
                text: channelListModel.getCheckedName()
                leftPaddingTxt: Global.scaleSize2(15)
                horizontalTextAlignment: Text.AlignHCenter
            }

            Item {
                id: dropdownButton
                anchors.right: parent.right
                anchors.rightMargin: Global.scaleSize2(20)
                anchors.verticalCenter: parent.verticalCenter

                width: Global.scaleSize2(25)
                height: Global.scaleSize2(25)
                visible: ioPopup.visible
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

                        if (!ioPopup)
                            return
                        if (ioPopup.visible) {
                            ioPopup.close()
                            return
                        }

                        ioPopup.openBelow(delegateRoot, Global.scaleSize2(5))
                    }
                }
            }
        }
    }
}
