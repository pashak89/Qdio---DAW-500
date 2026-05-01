import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ".."

Item {
    id: root

    property bool didFocus: control.focus | tf.focus | tf.maFocus
    property color brColor: theme.s11_Enable_BRD
    property color flColor: control.checked
                            & trackEnable ? theme.s11_Enable_FIL : theme.s10_Disable_FIL
    property string text: "L"
    property int space: didFocus ? refSize + topWidth : topWidth

   // property alias value: tf.value
    property alias checked: control.checked
    property real value: 0
    onValueChanged: {
        tf.value = value
    }

    signal clicked
    signal moved(real value)
    signal pressed()

    Rectangle {
        id: background

        anchors {
            fill: parent
            leftMargin: radius
            rightMargin: radius
        }

        color: brColor
        radius: topRadius
    }

    Button {
        id: control
        width: Math.round(parent.width * .43) - space
        height: parent.height
        checkable: true
        checked: true

        background: Rectangle {
            color: control.checked ? trackEnable ? theme.b11_Enable1_FIL : theme.b20_Disable_FIL : theme.b10_Disable_FIL
            radius: topRadius
            Rectangle {
                anchors.right: parent.right
                height: parent.height
                width: topRadius
                color: parent.color
            }
        }

        contentItem: InfoText {
            anchors.fill: parent
            text: root.text
            color: control.checked ? trackEnable ? theme.f4_ : theme.f4_ : theme.f1_
        }

        onClicked: {
            control.focus = true
            root.clicked()
        }
    }

    Text_Field {
        id: tf

        width: parent.width - control.width - space
        height: parent.height
        anchors.right: parent.right

        hasSlider: true
        defaultValue: 100
        max: 100
        min: 0
        forSpeed: 500
        decimal: 0
        onMoved: {
            root.moved(value)
        }
        onPressed:{
            root.pressed()
        }
    }

    Rectangle {
        id: foreground

        anchors.fill: parent

        color: 'transparent'
        border {
            color: brColor
            width: topWidth
        }

        radius: topRadius
    }

    Rectangle {
        id: shadow
        z: -1
        visible: didFocus
        anchors.fill: parent
        anchors.margins: -refSize

        color: brColor
        radius: topRadius
    }
}
