import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import ".."

Rectangle {
    id: root

    property color brColor: theme.s11_Enable_BRD
    property bool checked: false
    signal clicked
    color: theme.b10_Disable_BRD
    border.color: brColor
    border.width: topWidth
    radius: topRadius

    property int pad: topWidth

    // Shadows
    CustomRectangle {
        visible: control.focus
        anchors.fill: control
        anchors.margins: -refSize - pad
        color: theme.b10_Disable_BRD
        corners: [Qt.AlignLeft | Qt.AlignTop, Qt.AlignLeft | Qt.AlignBottom]
        radius: topRadius
        z: 1
    }
    CustomRectangle {
        visible: dropMenu.focus
        anchors.fill: dropMenu
        anchors.margins: -refSize - pad
        color: theme.b10_Disable_BRD
        corners: [Qt.AlignRight | Qt.AlignTop, Qt.AlignRight | Qt.AlignBottom]
        radius: topRadius
        z: 1
    }

    Button {
        id: control
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            leftMargin: pad
        }

        height: parent.height - 2 * pad
        width: height
        checkable: true
        checked: root.checked

        background: Rectangle {
            id: rec
            color: root.checked ? theme.b21_Enable_FIL : theme.b20_Disable_FIL

            radius: topRadius

            Rectangle {
                anchors.right: parent.right
                width: topRadius
                height: parent.height
                color: parent.color
            }
        }

        display: AbstractButton.IconOnly
        icon.source: 'qrc:/Resource/tune.svg'
        icon.color: '#1E1E1E'
        icon.height: height
        icon.width: width
        icon.name: "tune effect"
        padding: 0

        signal action(int index, string text)

        function callback(index, text) {
            action(index, text)
        }

        property var moduls: !model.deviceOnAutomated
                             && model.deviceOnActive ? ['Show Automation', 'Show Automation in New Lane', 'Re-Enable Automation', 'Delete Automation', 'Disable Automation'] : ['Show Automation', 'Show Automation in New Lane', 'Delete Automation', 'Disable Automation']

        MouseArea {
            id: maControl
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked: {
                control.focus = true

                if (mouse.button === Qt.LeftButton) {
                    root.clicked()
                } else if (mouse.button === Qt.RightButton) {

                    positionInWindow = Qt.point(mouse.x, mouse.y)

                    rightPanel.leftModules = control.moduls
                    rightPanel.isTwoGroups = false
                    rightPanel.firstStr = ''
                    rightPanel.cc = parent
                    rightPanel.parent = control
                    rightPanel.visible = true
                    rightPanel.callback = control.callback

                    //                    positionInWindow = mapToItem(track_list.contentItem,
                    //                                                 mouse.x, mouse.y)
                    //                    rightPanel.visible = true
                    //                    if (trackRightClick.visible)
                    //                        trackRightClick.visible = false
                }
            }
        }

        z: dropMenu.focus ? 0 : 1

        property bool deviceOnValue: filterVisualizer.deviceOnValue
        property bool deviceOnAutomated: filterVisualizer.deviceOnAutomated

        onDeviceOnValueChanged: {            
            filterVisualizer.enabled = deviceOnValue
            model.enabled = checked
        }

        onDeviceOnAutomatedChanged: {

            if (filterVisualizer.deviceOnValue) {
                filterVisualizer.enabled = true
                model.enabled = true
            } else {
                filterVisualizer.enabled = false
                model.enabled = false
            }

            if (!deviceOnAutomated) {

                control.moduls = ['Show Automation', 'Show Automation in New Lane', 'Re-Enable Automation', 'Delete Automation', 'Disable Automation']
            } else {

                control.moduls = ['Show Automation', 'Show Automation in New Lane', 'Delete Automation', 'Disable Automation']
            }
        }

        onAction: {

            if (text === "Show Automation") {
                clipArea.tracksModel.setAutomationLaneEnabled(trackIndex, true)

                clipArea.tracksModel.setAutomationMenuTitle(trackIndex, "EQ",
                                                            "DeviceOn")
            }
            if (text === "Delete Automation") {
                clipArea.tracksModel.automationClear(trackIndex, "EQ",
                                                     "DeviceOn")
            }

            if (text === "Disable Automation") {
                clipArea.tracksModel.setAutomationLaneEnabled(trackIndex, false)
            }

            if (text == "Re-Enable Automation") {
                filterVisualizer.deviceOnAutomatedEnable()
                control.moduls = ['Show Automation', 'Show Automation in New Lane', 'Delete Automation', 'Disable Automation']
            }
        }

        // filterVisualizer.setFilterEnabled(1, true)
        Rectangle {
            id: automatedLight
            z: 1000
            visible: filterVisualizer.deviceOnActive
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 1
            anchors.topMargin: 0
            color: filterVisualizer.deviceOnAutomated ? "#EA2027" : "#808080"
            width: scaleSize2(11)
            height: scaleSize2(11)
            radius: scaleSize2(11)
        }
    }

    ComboBoxItem {
        id: dropMenu
        anchors {
            right: parent.right
            rightMargin: pad
            left: control.right
            leftMargin: pad
            verticalCenter: parent.verticalCenter
        }

        height: parent.height - 2 * pad

        onDownChanged: if (down)
                           focus = true

        z: control.focus ? 0 : 1
    }

    Rectangle {
        id: spaceBetween
        anchors {
            left: control.right
        }
        width: pad
        height: parent.height
        // color: brColor
        color: 'transparent'
    }

    // Window {
    //     id: popup
    //     x: parent.x + maControl.mouseX
    //     y: parent.y +  maControl.mouseY
    //     width: scaleSize(550)
    //     height: scaleSize(150)
    //     // modal: true
    //     // focus: true
    // }
}
