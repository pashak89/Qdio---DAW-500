import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.2
import Tools 1.0
import 'qrc:/Automation/'


Window {
    id: root
    width: 700
    height: 600
    title: "Drag & drop example"
    visible: true

    color: 'grey'

    property bool isVolume_Automated: true
    property bool isPan_Automated: !false
    property bool isSpeackerOn_Automated: false

    property int topRadius: 5
    property color flColor: '#2F2F2F'
    property color brColor: fontColor
    property color fontColor: '#EAEAEA'
    property int space: 10
    property int borderWidth: 1


    Label { id: bigTxt ;visible: false; text: 'Show All Automated'; font { pixelSize: 25} }
    Label { id: bigTxt2 ;visible: false; text: 'Speacker On'; font { pixelSize: 25} }


    Button {
        id: fileButton
        text: "File"
        onClicked: {
            menu.open()
        }

        Menu {
            id: menu

            spacing: 25//space * 5

            x: 50
            y: 100

            Action { text: qsTr("None") }

            Menu {
                title: "Mixer"

                spacing: space

                Action { text: qsTr("Volume"); checked: isVolume_Automated }
                Action { text: qsTr("Pan"); checked: isPan_Automated }
                Action { text: qsTr("Speacker On"); checked: isSpeackerOn_Automated }

                topPadding: topRadius
                bottomPadding: topRadius
                leftPadding: root.borderWidth
                rightPadding: root.borderWidth

                delegate: MenuItem {
                    id: menuItemMixer
                    implicitWidth: 300
                    implicitHeight: 40

                    indicator: Item {
                        implicitWidth: parent.height
                        implicitHeight: parent.height
                        Rectangle {
                        visible: menuItemMixer.checked
                        height: Math.round(parent.height/2.5)
                        width: height
                        radius: height / 2
                        color: 'gold'
                        border.color: 'black'
                        border.width: 2
                        anchors.centerIn: parent
                    }}

                    contentItem: Text {
                        leftPadding: menuItemMixer.indicator.width
                        text: menuItemMixer.text
                        font { pixelSize: 25 }
                        opacity: enabled ? 1.0 : 0.3
                        color: menuItemMixer.highlighted ? flColor : fontColor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }

                    background: Rectangle {
                        implicitWidth: parent.width
                        implicitHeight: parent.height
                        opacity: enabled ? 1 : 0.3
                        color: menuItemMixer.highlighted ? fontColor : flColor
                    }
                }

                background: CustomRectangle {
                    implicitWidth: bigTxt2.contentWidth + 1.5* implicitHeight
                    implicitHeight: 40
                    color: flColor
                    borderColor: brColor
                    borderWidth: root.borderWidth
                    corners: [
                            Qt.AlignLeft | Qt.AlignBottom,
                            Qt.AlignRight | Qt.AlignTop,
                            Qt.AlignRight | Qt.AlignBottom,
                        ]
                    radius: topRadius
                }
            }


            MenuSeparator {
                padding: 0
                contentItem: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 1
                    color: brColor
                }
            }


            Menu {
                id: all
                spacing: space
                title: qsTr("Show All Automated")

                Action { text: qsTr("Volume"); checked: isVolume_Automated }
                Action { text: qsTr("Pan"); checked: isPan_Automated }
                Action { text: qsTr("Speacker On"); checked: isSpeackerOn_Automated }

                onOpenedChanged: if(!isVolume_Automated & !isPan_Automated & !isSpeackerOn_Automated) close()

                topPadding: topRadius
                bottomPadding: topRadius
                leftPadding: root.borderWidth
                rightPadding: root.borderWidth

                delegate: MenuItem {
                    id: menuItemAutomated
                    implicitWidth: 50
                    implicitHeight: 40

                    indicator: Item {
                        implicitWidth: parent.height
                        implicitHeight: parent.height
                        Rectangle {
                        visible: menuItemAutomated.checked
                        height: Math.round(parent.height/2.5)
                        width: height
                        radius: height / 2
                        color: 'gold'
                        border.color: 'black'
                        border.width: 2
                        anchors.centerIn: parent
                    }}

                    contentItem: Text {
                        leftPadding: menuItemAutomated.indicator.width
                        text: menuItemAutomated.text
                        font { pixelSize: 25 }
                        opacity: enabled ? 1.0 : 0.3
                        color: menuItemAutomated.highlighted ? flColor : fontColor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }

                    background: Rectangle {
                        implicitWidth: parent.width
                        implicitHeight: parent.height
                        opacity: enabled ? 1 : 0.3
                        color: menuItemAutomated.highlighted ? fontColor : flColor
                    }
                }

                background: CustomRectangle {
                    implicitWidth: bigTxt2.contentWidth + 1.5* implicitHeight
                    implicitHeight: 40
                    color: flColor
                    borderColor: brColor
                    borderWidth: root.borderWidth
                    corners: [
                            Qt.AlignLeft | Qt.AlignBottom,
                            Qt.AlignRight | Qt.AlignTop,
                            Qt.AlignRight | Qt.AlignBottom,
                        ]
                    radius: topRadius
                }
            }

            topPadding: topRadius
            bottomPadding: topRadius
            leftPadding: root.borderWidth
            rightPadding: root.borderWidth

            delegate: MenuItem {
                id: menuItem
                implicitHeight: 40
                implicitWidth: 100

                onHighlightedChanged: menuItemArrow.requestPaint()

                arrow: Canvas {
                    id: menuItemArrow
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 40
                    height: 40
                    visible: menuItem.subMenu
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.fillStyle = menuItem.highlighted ? flColor : fontColor
                        ctx.moveTo(15, 15)
                        ctx.lineTo(width - 15, height / 2)
                        ctx.lineTo(15, height - 15)
                        ctx.closePath()
                        ctx.fill()
                    }
                }

                indicator: Item {
                    implicitWidth: 40
                    implicitHeight: 40
                    Rectangle {
                    visible: (isVolume_Automated | isPan_Automated | isSpeackerOn_Automated) & menuItem.text !== 'None'
                    height: Math.round(parent.height/2.5)
                    width: height
                    radius: height / 2
                    color: 'gold'
                    border.color: 'black'
                    border.width: 2
                    anchors.centerIn: parent
                }}

                contentItem: Text {
                    id: txtMenu
                    height: 40
                    leftPadding: menuItem.indicator.width
                    text: menuItem.text
                    font { pixelSize: 25 }
                    opacity: enabled ? 1.0 : 0.3
                    color: menuItem.highlighted ? flColor : fontColor
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    width: parent.width
                    height: parent.height
                    opacity: enabled ? 1 : 0.3
                    color: menuItem.highlighted ? fontColor : flColor
                }
            }

            background: CustomRectangle {
                id: backgroundMenu
                implicitWidth: bigTxt.contentWidth + 2.5* implicitHeight
                implicitHeight: 40
                color: flColor
                borderColor: brColor
                borderWidth: root.borderWidth
                corners: Qt.AlignLeft | Qt.AlignRight | Qt.AlignBottom
                radius: topRadius
            }
        }


    }












}
