import QtQuick 2.12
import '.'
import 'qrc:/'
import "qrc:/Items"

Rectangle {
    height:             scaleSize2(37)
    color:              'transparent'

    clip: true
    TextInputElide {
        id:           textLabel
        fullText:     pretext
        anchors.fill: parent

        didMarker: editName
        onDidMarkerChanged: focus = didMarker
    }
}
