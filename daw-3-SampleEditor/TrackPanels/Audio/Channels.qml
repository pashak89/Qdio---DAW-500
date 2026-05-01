import QtQuick 2.12
import QtQuick.Layouts 1.12
import "."
import "qrc:/"
import "qrc:/Items"

Rectangle {
    id: volumeMeter //----------------------------------------------------------------------------------------
    width: 2 * scaleSize2(8) + (topWidth * 2) * 2 + (channels - 1) * (topWidth)
    height: parent.height
    color: 'black'

    RowLayout {
        id: row
        anchors.fill: parent
        anchors.margins: topWidth * 2 //+1
        spacing: topWidth

        Repeater {

            model: _clipArea.tracksModel.truePeakModel(ind)

            delegate: Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: '#042327'

                property bool muted: model.Muted

                Rectangle {
                    width: parent.width
                    height: 2
                    y: 0.15 * parent.height
                    color: 'black'
                }

                Rectangle {

                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: Math.floor(parent.height * (1 - AverageMeterLevel))
                    color: muted ? '#6F6F6F' : '#15D0E9'
                }

                Rectangle {
                    id: rect1
                    width: parent.width
                    height: PeakMeterPeakLevel >= 1 ? 0 : 1
                    y: Math.floor(parent.height * (PeakMeterPeakLevel))

                    color: muted ? '#6F6F6F' : '#15D0E9'
                }
                Rectangle {

                    width: parent.width

                    height: Math.floor(
                                parent.height * ((1 - PeakMeterLevel) - (1 - AverageMeterLevel)))
                    y: Math.floor(parent.height * (PeakMeterLevel))
                    color: muted ? '#383838' : '#0F707D'
                }
            }
        }
    }
    MouseArea {
        anchors.fill: parent
    }
}
