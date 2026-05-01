// TrackPanel/components/PluginPopup.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import Global 1.0
import "../menus"
import "../../../Items"

Popup {
    id: popup

    // These are used by your TreeViews
    property var clipArea
    property var areaInfo
    property int outputChannelsCount: model.outputChannelsCount

    signal channelCountChanged(int value)
    onOutputChannelsCountChanged: {
        channelListModel.setChannels(outputChannelsCount)
    }
    Component.onCompleted: {
        channelListModel.setChannels(outputChannelsCount)
    }

    visible: false
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    width: Global.scaleSize2(350)
    height: mainCol.implicitHeight

    background: Rectangle {
        radius: Global.scaleSize2(5)
        color: theme.w3_FILL
        border.color: theme.w3_BRD
        border.width: Global.scaleSize2(3)
    }

    function openBelow(item, yOffsetPx) {
        if (!item)
            return
        var overlayItem = popup.parent
        if (!overlayItem)
            return

        var p = item.mapToItem(overlayItem, 0, item.height)
        popup.x = p.x
        popup.y = p.y + (yOffsetPx || 0)

        popup.open()
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
    }

    contentItem: Column {
        id: mainCol
        anchors.fill: parent
        spacing: 0

        CustomLabel {
            fontSize: topfontPixelSize
            fontFamilty: topFontFamily
            fontColor: theme.f1_
            height: Global.scaleSize2(56)
            width: parent.width
            radius: topRadius
            text: "Settings..."
            horizontalTextAlignment: Text.AlignLeft
            leftPaddingTxt: Global.scaleSize2(20)
        }

        Rectangle {
            height: 1
            color: theme.s7
            width: parent.width - 2
            anchors.horizontalCenter: parent.horizontalCenter
        }

        CustomLabel {
            fontSize: topfontPixelSize
            fontFamilty: topFontFamily
            fontColor: theme.f1_
            height: Global.scaleSize2(56)
            width: parent.width
            radius: topRadius
            text: "Ext. Out"
            horizontalTextAlignment: Text.AlignLeft
            leftPaddingTxt: Global.scaleSize2(45)
        }
        Rectangle {
            height: 1
            color: theme.s7
            width: parent.width - 2
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Column {
            height: implicitHeight + Global.scaleSize2(10)
            width: parent.width
            Repeater {
                id: repeaterMainColumn
                model: channelListModel

                delegate: Rectangle {
                    height: Global.scaleSize2(49)
                    width: parent.width
                    color: mouseArea.containsMouse ? theme.bg6 : "transparent"
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {

                            channelCountChanged(value)

                            channelListModel.setChannels(value)
                        }
                    }
                    Image {
                        id: icon

                        source: "qrc:/Resource/checkmark_icon.svg"
                        smooth: true

                        visible: model.checked // ✅ THIS IS THE FIX

                        height: Global.scaleSize2(32)
                        width: Global.scaleSize2(32)
                        anchors.right: label.left
                        anchors.rightMargin: Global.scaleSize2(10)
                        anchors.verticalCenter: parent.verticalCenter

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: mouseArea.containsMouse ? theme.s3 : theme.s7
                        }
                    }

                    CustomLabel {
                        id: label
                        fontSize: topfontPixelSize
                        fontFamilty: topFontFamily
                        fontColor: mouseArea.containsMouse ? theme.f10_ : theme.f1_
                        height: Global.scaleSize2(49)
                        width: parent.width - Global.scaleSize2(45)
                        radius: topRadius
                        text: name
                        horizontalTextAlignment: Text.AlignLeft
                        anchors.verticalCenter: parent.verticalCenter

                        anchors.left: parent.left
                        anchors.leftMargin: Global.scaleSize2(45)
                    }
                }
            }
        }
    }
}
