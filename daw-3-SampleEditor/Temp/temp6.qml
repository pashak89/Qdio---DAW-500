import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.2
import QtQuick.LocalStorage 2.0

Window {
    id: windowId
    width: 800
    height: 600
    title: "Drag & drop example"
    visible: true
    color: 'grey'

    GridLayout {
        id: grid
        columns: 1
        rowSpacing: 12

        Repeater {
            model: 7
        delegate: Rectangle {
            Layout.row: index; Layout.column: 0

            color: {
                if(index === 0) 'blue'
                else if(index === 1) 'salmon'
                else if(index === 2) 'yellow'
                else if(index === 3) 'brown'
                else if(index === 4) 'teal'
                else if(index === 5) 'lightblue'
                else if(index === 6) 'green'
                else 'red'
            }
            Layout.preferredWidth: 75
            Layout.preferredHeight: {
                if(index === 0) 55
                else if(index === 1) 42
                else if(index === 2) 75
                else if(index === 3) 36
                else if(index === 4) 25
                else if(index === 5) 85
                else if(index === 6) 17
                else 25
            }
            MouseArea{
                anchors.fill: parent
                onClicked: console.log(index)
            }
        }}

        Keys.onRightPressed: {
            grid.children[2].Layout.preferredHeight = 10
        }

        focus: true
        Keys.onSpacePressed: {
            grid.children[2].Layout.row = 4
            grid.children[4].Layout.row = 2
        }

        Keys.onTabPressed: {
            var it = check_child_num(2, 7)
            it.Layout.row = 3
            grid.children[3].Layout.row = 2
        }
    }

    function check_child_num(n, length) {
        var result = grid.children[n]
        if(grid.children[n].Layout.row !== n)
        for(var a = 0; a<length; a++)
            if(grid.children[a].Layout.row === n)
                result = grid.children[a]
        return result
    }

}
