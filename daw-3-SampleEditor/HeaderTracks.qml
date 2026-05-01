import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.2
import "qrc:/Items"

Rectangle {
    id: root

    y: 25

    height: scaleSize2(75)
    color: '#555658'

    property alias masterIsOn: masterBtn.isClicked
    property alias returnIsOn: sendBtn.isClicked
    property alias addTrackBtn: addTrackBtn
    property alias automationBtn: automationBtn

    Keys.onPressed: {
        if (event.key === Qt.Key_Shift) {
            holdShift = true
        }
        if (event.key === Qt.Key_Control) {
            holdCtrl = true
        }
    }

    Keys.onReleased: {
        if (event.key === Qt.Key_Shift) {
            holdShift = false
        }
        if (event.key === Qt.Key_Control) {
            holdCtrl = false
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.focus = true
    }

    TrackButton {
        id: addTrackBtn
        anchors.left: parent.left
        anchors.leftMargin: Math.round(parent.width * 0.015)
        anchors.verticalCenter: parent.verticalCenter

        width: scaleSize2(56)
        height: scaleSize2(56)

        Image {
            id: addTrackImg
            anchors.centerIn: parent

            sourceSize: {
                if (addTrackBtn.height % 2 !== 0)
                    Qt.size(oddSize(parent.width * .7),
                            oddSize(parent.width * .7))
                else
                    Qt.size(evenSize(parent.width * .7),
                            evenSize(parent.width * .7))
            }

            source: 'qrc:/Resource/add_icon.svg'
            visible: false
        }
        ColorOverlay {
            id: colorImageAddTrack
            cached: true
            anchors.fill: addTrackImg

            source: addTrackImg
            clip: true
            antialiasing: true
            color: addTrackBtn.isClicked /*|| popup.opened*/
                   ? theme.a1 : theme.a2
        }

        //        fillStyle: isClicked ? theme.b11_Enable1_FIL : theme.b10_Disable_FIL
        onIsClickedChanged: {
            if (isClicked /*&& !popup.opened*/
                    ) {
                popup.open()
                //                fillStyle = theme.b11_Enable1_FIL
            } else {
                popup.close()
                //                fillStyle = theme.b10_Disable_FIL
            }
        }
        Keys.onPressed: {
            if (event.key === Qt.Key_Shift) {
                holdShift = true
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = true
            }
        }

        Keys.onReleased: {
            if (event.key === Qt.Key_Shift) {
                holdShift = false
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = false
            }
        }
    }

    TrackButton {
        id: automationBtn
        anchors.right: sendBtn.left
        anchors.rightMargin: Math.round(parent.width * 0.015)
        anchors.verticalCenter: parent.verticalCenter

        width: scaleSize2(56)
        height: scaleSize2(56)
        isClicked: _clipArea.automationEnabled
        onIsClickedChanged: _clipArea.automationEnabled = !_clipArea.automationEnabled

        Image {
            id: automationImg
            anchors.centerIn: parent
            sourceSize: {
                if (automationBtn.height % 2 !== 0)
                    Qt.size(oddSize(parent.width * .7),
                            oddSize(parent.width * .7))
                else
                    Qt.size(evenSize(parent.width * .7),
                            evenSize(parent.width * .7))
            }
            source: 'qrc:/Resource/automation_icon.svg'
            visible: false
        }
        ColorOverlay {
            id: colorImageAutomation
            cached: true
            anchors.fill: automationImg

            source: automationImg
            clip: true
            antialiasing: true
            color: automationBtn.isClicked ? theme.a1 : theme.a2
        }
        Keys.onPressed: {
            if (event.key === Qt.Key_Shift) {
                holdShift = true
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = true
            }
        }

        Keys.onReleased: {
            if (event.key === Qt.Key_Shift) {
                holdShift = false
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = false
            }
        }
    } // automationBtn.

    TrackButton {
        id: masterBtn
        anchors.right: parent.right
        anchors.rightMargin: Math.round(parent.width * 0.02)
        anchors.verticalCenter: parent.verticalCenter

        width: scaleSize2(56)
        height: scaleSize2(56)

        isClicked: true

        Image {
            id: masterImg
            anchors.centerIn: parent
            sourceSize: {
                if (masterBtn.height % 2 !== 0)
                    Qt.size(oddSize(parent.width * .7),
                            oddSize(parent.width * .7))
                else
                    Qt.size(evenSize(parent.width * .7),
                            evenSize(parent.width * .7))
            }
            source: 'qrc:/Resource/master_icon.svg'
            visible: false
        }
        ColorOverlay {
            cached: true
            anchors.fill: masterImg

            source: masterImg
            clip: true
            antialiasing: true
            color: masterBtn.isClicked ? theme.a1 : theme.a2
        }
        Keys.onPressed: {
            if (event.key === Qt.Key_Shift) {
                holdShift = true
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = true
            }
        }

        Keys.onReleased: {
            if (event.key === Qt.Key_Shift) {
                holdShift = false
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = false
            }
        }
    } // masterBtn.

    TrackButton {
        id: sendBtn
        anchors.right: masterBtn.left
        anchors.rightMargin: Math.round(parent.width * 0.015)
        anchors.verticalCenter: parent.verticalCenter

        width: scaleSize2(56)
        height: scaleSize2(56)

        isClicked: true

        Image {
            id: sendImg
            anchors.centerIn: parent
            sourceSize: {
                if (sendBtn.height % 2 !== 0)
                    Qt.size(oddSize(parent.width * .7),
                            oddSize(parent.width * .7))
                else
                    Qt.size(evenSize(parent.width * .7),
                            evenSize(parent.width * .7))
            }
            source: 'qrc:/Resource/send_icon.svg'
            visible: false
        }
        ColorOverlay {
            id: colorImageSend
            cached: true
            anchors.fill: sendImg

            source: sendImg
            clip: true
            antialiasing: true
            color: sendBtn.isClicked ? theme.a1 : theme.a2
        }
        Keys.onPressed: {
            if (event.key === Qt.Key_Shift) {
                holdShift = true
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = true
            }
        }

        Keys.onReleased: {
            if (event.key === Qt.Key_Shift) {
                holdShift = false
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = false
            }
        }
    } // sendBtn.

    Popup {
        id: popup
        x: addTrackBtn.x + Math.round(addTrackBtn.width / 2)
        y: addTrackBtn.y + Math.round(addTrackBtn.width / 2)

        background: Rectangle {
            id: backAdd
            implicitWidth: Math.round(root.width * 0.33)
            implicitHeight: Math.round(root.width * .3)
            radius: topRadius
            color: 'transparent'
        }

        // contentItem: AddTrack {
        //     anchors.fill: backAdd
        // }
        onOpenedChanged: {
            parent.addTrackBtn.isClicked = opened ? true : false
        }
    }
}
