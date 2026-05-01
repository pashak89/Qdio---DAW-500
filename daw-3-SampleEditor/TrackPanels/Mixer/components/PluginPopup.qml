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

    property var theme
    property int topfontPixelSize
    property string topFontFamily

    // these are used by your TreeViews
    property var clipArea
    property var areaInfo

    visible: false
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    width: Global.scaleSize2(650)
    height: mainCol.implicitHeight

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

    background: Rectangle {
        radius: Global.scaleSize2(5)
        color: theme.w3_FILL
        border.color: theme.w3_BRD
        border.width: Global.scaleSize2(3)
    }

    contentItem: Column {
        id: mainCol
        anchors.fill: parent
        spacing: 0

        // --- top bar with search ---
        Item {
            width: parent.width
            height: Global.scaleSize2(56) + Global.scaleSize2(20)

            Rectangle {
                anchors.left: parent.left
                anchors.right: icon.left
                anchors.leftMargin: Global.scaleSize2(15)
                anchors.rightMargin: Global.scaleSize2(8)
                anchors.verticalCenter: parent.verticalCenter
                height: Global.scaleSize2(56)
                radius: Global.scaleSize2(5)
                color: theme.bg5_new

                TextArea {
                    anchors.fill: parent
                    color: "#c7c7c7c7"
                    font.pixelSize: topfontPixelSize
                    verticalAlignment: Qt.AlignVCenter
                    font.family: topFontFamily
                    clip: true
                    cursorVisible: true
                    placeholderText: "Search"
                    topPadding: 0
                    bottomPadding: 0
                    leftPadding: Global.scaleSize2(8)
                    rightPadding: Global.scaleSize2(8)
                }
            }

            Image {
                id: icon
                source: "qrc:/Resource/search_icon.svg"
                smooth: true
                fillMode: Image.PreserveAspectFit
                width: Global.scaleSize2(56)
                height: Global.scaleSize2(56)
                anchors.right: parent.right
                anchors.rightMargin: Global.scaleSize2(15)
                anchors.verticalCenter: parent.verticalCenter

                ColorOverlay {
                    anchors.fill: parent
                    source: parent
                    color: theme.button_unclicked_color
                }
            }
        }

        Rectangle {
            height: 1
            color: theme.s7
            width: parent.width - 2
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Item {
            id: colorBar
            width: parent.width
            height: Global.scaleSize2(56) + 2 * Global.scaleSize2(10)

            // Tweakables
            property int padding: Global.scaleSize2(5)
            property int dotSize: Global.scaleSize2(56)
            property int dotSpacing: Global.scaleSize2(20)
            property color barColor: theme.w3_FILL
            property color borderColor: "#00000000"
            property real radius: Global.scaleSize2(5)

            // Selection + colors
            property int selectedIndex: 0 // ✅ first selected by default
            property var colors: ["#EE3743", "#FFA138", "#FFEE00", "#1CE637", "#3DB5FF", "#D55CED"]
            signal colorSelected(int index, color c)

            Rectangle {
                id: bar

                height: parent.height
                implicitWidth: colorBar.padding * 2 + (colorBar.colors.length * colorBar.dotSize)
                               + ((colorBar.colors.length - 1) * colorBar.dotSpacing)

                color: colorBar.barColor
                radius: colorBar.radius
                antialiasing: true

                anchors.left: parent.left
                anchors.leftMargin: Global.scaleSize2(5)

                Row {
                    id: dotRow

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: colorBar.dotSpacing

                    Repeater {
                        model: colorBar.colors.length

                        Item {
                            id: cell
                            width: colorBar.dotSize
                            height: colorBar.dotSize

                            readonly property bool isSelected: index === colorBar.selectedIndex

                            // selection highlight (rounded square)
                            Rectangle {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter

                                width: cell.isSelected ? (colorBar.dotSize) : 0
                                height: cell.isSelected ? (colorBar.dotSize) : 0
                                radius: colorBar.radius
                                color: theme.bg6

                                antialiasing: true
                                opacity: cell.isSelected ? 1 : 0

                                Behavior on width {
                                    NumberAnimation {
                                        duration: 120
                                    }
                                }
                                Behavior on height {
                                    NumberAnimation {
                                        duration: 120
                                    }
                                }
                                Behavior on opacity {
                                    NumberAnimation {
                                        duration: 120
                                    }
                                }
                            }

                            // dot itself
                            Rectangle {
                                anchors.centerIn: parent
                                width: Global.scaleSize2(25)
                                height: Global.scaleSize2(25)
                                radius: width / 2
                                color: colorBar.colors[index]
                                antialiasing: true
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    colorBar.selectedIndex = index
                                    colorBar.colorSelected(
                                                index, colorBar.colors[index])
                                }
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            height: 1
            color: theme.s7
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 2
        }
        // --- tabs (VST2/VST3) ---
        Item {
            id: vstTabs
            width: parent.width
            height: Global.scaleSize2(56) + 2 * Global.scaleSize2(10)

            property int selectedIndex: 0
            property var tabs: ["VST2", "VST3"]

            Row {
                anchors.verticalCenter: parent.verticalCenter
                spacing: Global.scaleSize2(45)
                anchors.left: parent.left
                anchors.leftMargin: Global.scaleSize2(15)

                Repeater {
                    model: vstTabs.tabs.length
                    delegate: Item {
                        width: tabText.implicitWidth + Global.scaleSize2(25)
                        height: tabText.implicitHeight + Global.scaleSize2(10)

                        Rectangle {
                            anchors.fill: parent
                            radius: Global.scaleSize2(5)
                            color: vstTabs.selectedIndex === index ? theme.bg6 : "transparent"
                        }

                        Text {
                            id: tabText
                            text: vstTabs.tabs[index]
                            anchors.centerIn: parent
                            color: vstTabs.selectedIndex === index ? theme.f10_ : theme.f1_
                            font.pixelSize: topfontPixelSize
                            font.family: topFontFamily
                            font.weight: Font.DemiBold
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                vstTabs.selectedIndex = index
                                stack.currentIndex = index
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            height: 1
            color: theme.s7
            width: parent.width - 2
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // --- No Plugin label row ---
        Item {
            width: parent.width
            height: Global.scaleSize2(56) + 2 * Global.scaleSize2(10)

            Text {
                text: "No Plugin"
                anchors.left: parent.left
                anchors.leftMargin: Global.scaleSize2(15)
                anchors.verticalCenter: parent.verticalCenter
                color: theme.f1_
                font.pixelSize: topfontPixelSize
                font.family: topFontFamily
                font.weight: Font.DemiBold
            }
        }

        Rectangle {
            height: 1
            color: theme.s7
            width: parent.width - 2
            anchors.horizontalCenter: parent.horizontalCenter
        }

        StackLayout {
            id: stack
            width: parent.width
            implicitHeight: Global.scaleSize2(700)

            // VST2
            TreeView {
                width: parent.width
                implicitHeight: Global.scaleSize2(700)
                model: vst3.vst2modules
                selectionEnabled: true
                hoverEnabled: true

                highlight: Rectangle {
                    color: "transparent"
                }
                hoverColor: theme.bg6
                selectedItemColor: theme.s7
                handleColor: theme.s7

                onChildSelectedIndexChanged: {
                    var i = vst3.vst2modules.getTreeItemIndex(
                                childSelectedIndex)
                    var path = vst3.vst2modules.getTreeItemPath(
                                childSelectedIndex)
                    var name = vst3.vst3modules.getTreeItemName(
                                childSelectedIndex)
                    clipArea.addVst3Effect(areaInfo.selectedTrackItem,
                                           path, name, i)
                }

                contentItem: Item {

                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        color: currentRow.isHoveredIndex ? theme.f10_ : theme.f1_
                        text: currentRow.currentData
                        font.pixelSize: topfontPixelSize
                        font.family: topFontFamily
                        font.weight: Font.DemiBold
                    }
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        hoverEnabled: true
                        onClicked: {
                            if (mouse.button === Qt.RightButton) {
                                var p = mapToItem(null, mouse.x, mouse.y)

                                rowMenu.popup(item, mouse.x, mouse.y)
                            }
                        }
                    }
                }
            }

            // VST3
            TreeView {
                id: vst3Tree
                width: parent.width
                implicitHeight: Global.scaleSize2(700)
                model: vst3.vst3modules
                selectionEnabled: true
                hoverEnabled: true

                highlight: Rectangle {
                    color: "transparent"
                }
                hoverColor: theme.bg6
                selectedItemColor: theme.s7
                handleColor: theme.s7

                onChildSelectedIndexChanged: {

                    var i = vst3.vst3modules.getTreeItemIndex(
                                childSelectedIndex)
                    var path = vst3.vst3modules.getTreeItemPath(
                                childSelectedIndex)
                    var name = vst3.vst3modules.getTreeItemName(
                                childSelectedIndex)

                    clipArea.addVst3Effect(areaInfo.selectedTrackItem,
                                           path, name, i)
                }

                contentItem: Item {
                    Text {
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        color: currentRow.isHoveredIndex ? theme.f10_ : theme.f1_
                        text: currentRow.currentData
                        font.pixelSize: topfontPixelSize
                        font.family: topFontFamily
                        font.weight: Font.DemiBold
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        hoverEnabled: false
                        propagateComposedEvents: true
                        onClicked: {
                            if (mouse.button === Qt.RightButton) {
                                var p = mapToItem(null, mouse.x, mouse.y)

                                rowMenu.popup(item, mouse.x, mouse.y)
                            }
                        }
                    }
                }
            }
        }
    }

    RowMenu {
        id: rowMenu
        theme: popup.theme
        topfontPixelSize: popup.topfontPixelSize
        topFontFamily: popup.topFontFamily
    }
}
