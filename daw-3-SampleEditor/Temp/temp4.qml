import QtQuick 2.12
import QtQuick.Window 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.2


Window {
    id: window
    title: qsTr("Test Crop")
    width: 800
    height: 800
    visible: true
    color: 'grey'

    Flickable {
        id: flickable
        width: 200
        height: 400
        contentHeight: cc.height

        anchors.right: parent.right

        pressDelay: -1000

        ScrollBar.vertical:  ScrollBar {
            id: vbar
//            hoverEnabled: true
//            active:  pressed
            orientation: Qt.Vertical
            size: 25
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }

        Rectangle {anchors.fill: parent; color: 'transparent'; border.color: 'blue'}

        clip: true

        Column {
            id: cc
            spacing: 15
            width: 300
            height: r1.height + r2.height + 15


            Rectangle {
                id: r1
                width: 150
                height: ss.value > 75 ? ss.value : 75
                color: 'transparent'
                border.color: 'lightblue'

                clip: true

                MouseArea {
                    anchors.fill: parent
                }

                Slider {
                    id: ss
                    width: parent.width
                    height: 250

                    snapMode: Slider.SnapAlways
                    stepSize: 15
                    orientation: Qt.Vertical
                    from: height
                    value: 0
                    to: 0
                    onValueChanged: value = r1.height - value > stepSize ? r1.height: value
                }
            }



            Rectangle {
                id: r2
                width: 150
                height: ss1.value > 75 ? ss1.value : 75
                color: 'transparent'
                border.color: 'lightblue'

                clip: true

                Slider {
                    id: ss1
                    width: parent.width
                    height: 250

                    snapMode: Slider.SnapAlways
                    stepSize: 15
                    orientation: Qt.Vertical
                    from: height
                    value: 0
                    to: 0
                    onValueChanged: value = r2.height - value > stepSize ? r2.height: value
                }
            }







        }
    }


}
