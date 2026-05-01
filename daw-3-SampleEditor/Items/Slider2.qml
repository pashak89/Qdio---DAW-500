import QtQuick 2.0

// Minimal slider implementation
Rectangle {
    id: root

    property int value: main * stepSize

    property int minimumValue: 0
    property int maximumValue: setScale()
    property int stepSize: 1

    property int main: 0

    property int minimumSize: 1

    property int maxWidth: 100

    property int xx: 1

    property int spotWidth: maxWidth - snapper.width

//    onSpotWidthChanged: console.log('spotWidth: ' + spotWidth)

    function setScale() {
        xx = (slider.width - minimumSize) / stepSize
        return xx
    }


//    color: 'transparent'
    Rectangle {
        id: slider

        anchors.right: parent.right


//        x: 80

        width: 10//maxWidth-70
        height: parent.height
        color: 'red'//'transparent'

        Rectangle { id: snapper; width: minimumSize + value; height: parent.height; color: 'yellow'//'transparent'
            //        onWidthChanged: console.log('www: ' + (maxWidth-width))
        }


        //        Rectangle {
        //            anchors.right: parent.right
        //            y: 70
        //            width: slider.width - snapper.width
        //            height: parent.height
        //            color: 'salmon'
        //        }


        Rectangle {
            id: indicator
            y: 0
            x: maxWidth -10
            height: parent.height - 2
            width: 10
            color: 'blue'//"transparent"

        }

        MouseArea {
            anchors.fill: parent

            drag {
                target: indicator
                axis: Drag.XAxis
                minimumX: 0
                maximumX: slider.width + maxWidth - indicator.width
            }

            onPositionChanged: {
                main = indicator.x / slider.width * maximumValue
            }
        }

        Component.onCompleted: main = indicator.x / slider.width * maximumValue

    }
}
