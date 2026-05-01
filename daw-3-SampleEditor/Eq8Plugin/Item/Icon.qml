import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

Item {
    id: root
    width: size
    height: width

    property string source: ''
    property real size: scaleSize(40)
    property color colorIcon: 'white'

    Image {
        id: icon
        source: root.source

        width: size
        height: width
        sourceSize: Qt.size(width, height)
        fillMode: Image.PreserveAspectFit

        smooth: true
        visible: false
    }
    ColorOverlay {
        anchors.fill: icon
        source: icon
        color: colorIcon
    }
}
