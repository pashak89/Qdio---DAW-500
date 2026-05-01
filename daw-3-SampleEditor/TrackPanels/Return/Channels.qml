import QtQuick 2.12
import QtQuick.Layouts 1.12
import '.'
import 'qrc:/'
import "qrc:/Items"

Rectangle {
    id:             volumeMeter //----------------------------------------------------------------------------------------
    width:          channels * scaleSize2(8) + (topWidth*2)*2 + (channels-1)* (topWidth)
    // height:         parent.height
    // anchors {
    //     top: parent.top
    //     bottom: parent.bottom
    //     bottomMargin:  scaleSize2(5)
    //     right: parent.right
    // }

    color:          'black'

    RowLayout {
        id: row
        anchors.fill:   parent
        anchors.margins: topWidth*2//+1
        spacing:        topWidth

        Repeater {
            model: channels
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: '#042327'

                Rectangle {
                    id: channel
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: Math.round(parent.height * .75)
                    color: '#15D0E9'
                } // rec1.
            }// rec2.



        }// repeater.
    }// row.

    MouseArea {
        anchors.fill: parent
    }
}
