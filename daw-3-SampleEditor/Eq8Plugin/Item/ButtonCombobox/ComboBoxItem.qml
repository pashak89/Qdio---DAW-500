import QtQuick 2.12
import QtQuick.Controls 2.15

import "qrc:/Items/"

ComboBox {
    id: control
    model: ["Equalizer", "Effect1", "Effect2"]

    delegate: ItemDelegate {
        id: delegate
        height: scaleSize(50)
        width: control.width
        contentItem: InfoText_left {
            anchors.verticalCenter: parent.verticalCenter
            name: modelData
            leftPaddingTxt: scaleSize(13)
            // fontSize: parent.height - scaleSize(20)
            colorTxt: control.highlightedIndex === index ? 'black' : 'white'
        }

        highlighted: control.highlightedIndex === index
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: scaleSize(24)
        height: scaleSize(16)
        contextType: "2d"

        property color triColor: theme.dd21_enable_tri
        onTriColorChanged: requestPaint()
        Component.onCompleted: {
            _areaInfo.zoomFactorChanged.connect(requestPaint)
        }
        Connections {
            target: control
            function onPressedChanged() {
                canvas.requestPaint()
            }
        }

        onPaint: {
            context.reset()
            context.moveTo(0, 0)
            context.lineTo(width, 0)
            context.lineTo(width / 2, height)
            context.closePath()
            context.fillStyle = triColor
            context.fill()
        }
    }

    contentItem: InfoText_left {
        leftPaddingTxt: scaleSize(20)
        name: control.displayText
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 40
        radius: topRadius
        color: theme.tab11_Enable_FIL

        Rectangle {
            width: parent.radius
            height: parent.height
            color: parent.color
        }

        Rectangle {
            visible: control.down
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: parent.radius
            height: width
            color: parent.color
        }
    }

    popup: Popup {
        y: control.height + refSize * 2
        width: control.width + refSize * 2
        implicitHeight: contentItem.implicitHeight + refSize * 2
        padding: 0

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.popup.visible ? control.delegateModel : null
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator {}


            /*opacity: control.down ? 1 : 0
            Behavior on opacity { NumberAnimation {duration: 200; easing.type: Easing.OutCurve} }
            scale: control.down ? 1 : 0.7
            Behavior on scale { NumberAnimation {duration: 200; easing.type: Easing.OutCurve} }*/
        }

        background: Rectangle {
            border.color: brColor
            radius: topRadius

            color: 'black'
            border.width: topWidth

            Rectangle {
                width: parent.width
                height: parent.radius
                color: parent.color
            }


            /*opacity: control.down ? 1 : 0
            Behavior on opacity { NumberAnimation {duration: 200; easing.type: Easing.OutCurve} }
            scale: control.down ? 1 : 0.7
            Behavior on scale { NumberAnimation {duration: 200; easing.type: Easing.OutCurve} }*/
        }
    }
}
