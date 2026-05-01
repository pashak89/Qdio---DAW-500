import QtQuick 2.3
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: root
    width: scaleSize2(200)*4
    height: scaleSize2(50)

    RowLayout {
        anchors.fill: parent
        spacing: 10

        Button {
            id: options_btn
            Layout.fillHeight: true; Layout.fillWidth: true
            text: "options"

            onClicked: {
                var option_component = Qt.createComponent("Preference.qml")
                var option = option_component.createObject(root)
                option.show()
                focus = false
            }
            focus: false
        }

        Button {
            id: left2rightBtn
            Layout.fillHeight: true; Layout.fillWidth: true

            text: globalValues.tracksOnLeft ? 'left' : 'right'
            onClicked: globalValues.tracksOnLeft = !globalValues.tracksOnLeft
        }
        Button {
            Layout.fillHeight: true; Layout.fillWidth: true

            text: globalValues.masterOnTop ? 'master on top' : 'master on bottom'
            onClicked: globalValues.masterOnTop = !globalValues.masterOnTop
        }
    }

    Rectangle {height: scaleSize2(50); width: root.parent.width; color: 'teal'; z: -1}
}
