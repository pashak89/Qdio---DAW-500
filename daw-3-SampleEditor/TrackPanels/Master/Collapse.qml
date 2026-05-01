import QtQuick 2.12
import QtQuick.Controls 2.12
import '.'
import 'qrc:/'
import "qrc:/Items"

Rectangle {
    id:         root
    width:      type === 'Group' ? constWidth + (rowsExpanded > 0 ? 2 : 1) * scaleSize2(50) :
                                   constWidth + scaleSize2(50) // audio

    height:     parent.height //- scaleSize2(5)
    color:      colors[getRandomInt(colors.length -1)]


    property int constWidth: scaleSize2(50) * in_set_parent.count

    property var colors: ['#FFCADA', '#F44336', '#FFDD9F', '#FF5722', '#F3C430', '#7C9712', '#61D67D', '#00CBE6', '#293AAE', '#9C27B0', '#808080',
        '#D694A3', '#E52729', '#FCBD87', '#CC5600', '#DAA521', '#57501B', '#007F3C', '#2166F3', '#232692', '#673AB7', '#D5BF96',
        '#FB939C', '#CA252B', '#FF7F50', '#B73F0B', '#FFC107', '#66CA43', '#065C37', '#03A9F4', '#4652AB', '#612652', '#B6863C',
        '#FE50B9', '#C6100A', '#E4715C', '#FEF167', '#BDDF43', '#48B30F', '#8AD8E4', '#729EF5', '#B97DDA', '#56253B', '#78450E',
        '#E91E63', '#950C04', '#FF9800', '#FEEB2C', '#94CF37', '#1C911C', '#30CFCA', '#5159DE', '#AC41EF', '#CFCECA', '#873E2B']

    Rectangle {
        id: leftRec
        width: constWidth
        height: parent.height
        color: light  ? theme.bg1 : theme.bg2
    }

    Image {
        id: fold
        anchors {
            top: parent.top
            topMargin: scaleSize2(15)
        }

        source: imgURL

        sourceSize: Qt.size(scaleSize2(25), scaleSize2(25))
        MouseArea {
            anchors.fill: parent
            onClicked:
            {
                var r = rowsExpanded
                change_rowExpanding()

                if(r !== rowsExpanded & rowsExpanded === 0) showAutomation = false
                else if(rowsExpanded > 0) showAutomation = true

                if(automationTurn & rowsExpanded < 2 & showAutomation)
                    changeRowCounter(rowsExpanded+2)

                if(r === rowsExpanded & rowsExpanded === 0)
                {
                    if(automationChildren > 0)
                    {
                        showAutomation = true
                        changeRowCounter(rowsExpanded+3)
                    }
                    else
                        changeRowCounter(rowsExpanded+2)
                }
            }
        }

        rotation: type === 'audio' ? (didFold ? -90 : 0) : 0
    }

    states: [State {
            name: "Right"
            when:!globalValues.tracksOnLeft

            AnchorChanges {
                target: leftRec
                anchors.right: parent.right
            }

            AnchorChanges {
                target: fold
                anchors.right: type === 'Group' ? leftRec.left : undefined
                anchors.left: type === 'Group' ? undefined : parent.left
            }
            PropertyChanges {
                target: fold
                anchors.rightMargin: type === 'Group' ? scaleSize2(12) : undefined
                anchors.leftMargin: type === 'Group' ? undefined : scaleSize2(12)
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: leftRec
                anchors.left: parent.left
            }

            AnchorChanges {
                target: fold
                anchors.right: type === 'Group' ? undefined : parent.right
                anchors.left: type === 'Group' ? leftRec.right : undefined
            }
            PropertyChanges {
                target: fold
                anchors.rightMargin: type === 'Group' ? undefined : scaleSize2(12)
                anchors.leftMargin: type === 'Group' ? scaleSize2(12) : undefined
            }
        }]
}
