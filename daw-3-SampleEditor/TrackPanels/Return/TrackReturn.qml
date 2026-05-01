import QtQuick 2.12
import '.'
import 'qrc:/'
import "qrc:/Items"

Rectangle {
    id: root
    color: light ? theme.bg1 : theme.bg2

    // height: isCollapsed ? trackAudio.height : 0
    // width: trackListView.width

    property int ind: index

    width: parent.width
    height:  isCollapsed ?
        rootMainTrack.height + scaleSize2(5)*0 + (automationChildren > 0 & showAutomation ? automationListView.height + scaleSize2(15) : 0)
               : 0
    clip: true

    MouseArea {id: ma0; anchors.bottom: parent.bottom; anchors.top: parent.top }

    // ----------------------------------------------------------------------------------------------------------------------------------  properties:
    property bool splitStereo: false

    property bool didGradient: light//| automationBtn.didShadow | viewBtn.didShadow | recordBtn.didShadow | sendBtn.didShadow

    property bool didFold:     rowsExpanded < 1 ? true : false

    property alias draggableSize: collapsPanel.width

    property bool allow2clear: false

    // Automation
    // property alias automationRun: automationBtn.isClicked
    property bool showAutomation: false
    property bool inAutomationMenu: false
    property bool inAutomationChild: false

    property int _rowsExpanded: -1

    property bool inTrack: false

    property int channels:      8


    property real _refSize: refSize
    on_RefSizeChanged: seperator.y = (rowsExpanded + 1) * (scaleSize2(37) + scaleSize2(15)) + scaleSize2(10) - seperator.height
    // ---------------------------------------------------------------------------------------------------------------------  properties.
    Collapse {
        id: collapsPanel
    }

    Item
    {
        id: rootMainTrack
        height:  (rowsExpanded+1) * (scaleSize2(37) + scaleSize2(15)) + scaleSize2(10) //- scaleSize2(5)
        clip: true

        MouseArea {
            id: maRoot
            anchors.fill: parent
            anchors.topMargin: row1.height
            anchors.bottomMargin: scaleSize2(5) + 1
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            // property bool allow2clear: false

            onClicked: {
                if(groupTrackLight.length === 1 & !holdCtrl & !holdShift & allow2clear)
                {
                    groupTrackLight.length = 0
                    funcs.offLightAllTracks()
                }
            }

            onPressed: {
                parent.focus = true
                root.focus = true

                if(mouse.button === Qt.RightButton)
                    funcs.trackLighting_rightclick(ind, 'btn')
                else
                    funcs.trackLighting(ind, 'btn')

                if (mouse.button === Qt.RightButton) {
                    positionInWindow = mapToItem(track_list.contentItem, mouse.x, mouse.y)
                    trackRightClick.numberItem = ind
                    trackRightClick.visble_grouper = funcs.allow2MakeGroup()
                    trackRightClick.visble_ungrouper = groupTrackLight.length === 1 & trackReturnModel.get(groupTrackLight[0]).type === 'Group'
                    trackRightClick.visible = true

                    if(rightPanel.visible)
                        rightPanel.visible = false
                }
                else
                    trackRightClick.visible = false
            }
        }

        Row1 {
            id: row1
        }


        Row2 {
            id: row2
            anchors.top: row1.bottom; anchors.topMargin: scaleSize2(15)
        }


        AutomationRow {
            id:             automationRow
            anchors {
                top:        row2.bottom
                topMargin:  scaleSize2(15)
            }
        }




        Channels {
            id: channelRows
            anchors {
                top: parent.top
                bottom: parent.bottom; bottomMargin: scaleSize2(5)
            }
        }
    }



    Seperator {
        id: seperator
    }



    AutomationListView {
        id: automationListView
        anchors {
            top: seperator.bottom; topMargin: scaleSize2(15)            
        }
    }

    Rectangle {
        visible: coverBound
        anchors.fill: parent

        color: 'transparent'
        border.color: theme.betweenRec_dragging_o20
        border.width: scaleSize2(5)
    }



    // Rectangle {z: 50
    //     anchors.fill: automationListView
    //     color: 'red'; opacity: 0.6
    // }


    // -------------------------------------------------------------------------------------------------------------------  Function:
    function changeRowCounter(num) {
        seperator.y = scaleSize2(37)*num + scaleSize2(15)*num + scaleSize2(10) - seperator.height

        var _i = Math.round((seperator.y - scaleSize2(10)) / (scaleSize2(37) + scaleSize2(15))) - 1
        if(_i !== rowsExpanded)
            rowFunc(_i)
    }

    // onRowsExpandedChanged: {//console.log('----------------0-----------------------> ', rowsExpanded, spanVert.main, trackReturnModel.get(ind).rowsExpanded)
//    onRowCounterChanged: {//console.log('\u001b[1;32m row counter  track ', ind, 'is changed to ' , rowCounter ,'\u001b[1;35m')
    function rowFunc(rowCounter) {
        trackReturnModel.setProperty(index, 'rowsExpanded', rowCounter)
        var arr = []
        if(type === 'Group' & trackReturnModel.get(ind).rowsExpanded === 1) {
            trackReturnModel.setProperty(ind, 'imgURL', 'qrc:/Resource/folder_off_icon.svg')

            for(var i = 1; i <= trackReturnModel.get(ind).sub_set_children.count; i++)
            {
                if(trackReturnModel.get(ind + i).type === 'Group')
                {
                    trackReturnModel.setProperty(ind + i, 'isCollapsed', true)
                }

                arr.length = 0
                if(trackReturnModel.get(ind + i).type === 'audio')
                {
                    for(var j = 0; j < trackReturnModel.get(ind + i).in_set_parent.count; j++)
                    {
                        for(var k = 0; k < trackReturnModel.count; k++)
                        {
                            if(trackReturnModel.get(ind + i).in_set_parent.get(j).member === trackReturnModel.get(k).pretext)
                            {
                                if(trackReturnModel.get(k).rowsExpanded > 0)
                                arr.push(true)
                                else arr.push(false)
                            }
                        }
                    }
                }

                if(arr.indexOf(false) === -1) trackReturnModel.setProperty(ind + i, 'isCollapsed', true)


            }// main for.
            trackListView.positionViewAtIndex(ind, ListView.Visible)

            funcs.offLightAllTracks()
        }

        if(rowCounter === 0) showAutomation = false
        else showAutomation = true
        // console.log('----------------1-----------------------> ', rowsExpanded, spanVert.main, trackReturnModel.get(ind).rowsExpanded)
    }

    function getRandomInt(max) {
      return Math.floor(Math.random() * max);
    }

    function change_rowExpanding()
    {
        if(rowsExpanded > 0)
        {
            _rowsExpanded = rowsExpanded
            // rowsExpanded = 0
            trackReturnModel.setProperty(ind, 'rowsExpanded', 0)
            // spanVert.yIndicator = 0
            changeRowCounter(1)

            if(type === 'Group')
            {
                trackReturnModel.setProperty(ind, 'imgURL', 'qrc:/Resource/folder_on_icon.svg')

                for(var i = 1; i <= trackReturnModel.get(ind).sub_set_children.count; i++)
                {
                    trackReturnModel.setProperty(ind + i, 'isCollapsed', false)

                    /*if(trackReturnModel.get(ind + i).type === 'Group')
                    trackReturnModel.setProperty(ind + i, 'imgURL', 'qrc:/Resource/folder_on_icon.svg')*/
                }

                funcs.offLightAllTracks()
            }
        }
        else
        {
            trackReturnModel.setProperty(ind, 'rowsExpanded', _rowsExpanded)
            changeRowCounter(_rowsExpanded+1)

            if(type === 'Group')
            {
                trackReturnModel.setProperty(ind, 'imgURL', 'qrc:/Resource/folder_off_icon.svg')

                for(i = 1; i <= sub_set_children.count; i++)
                {
                    // if(trackReturnModel.get(ind + i).type !== 'Group')
                    trackReturnModel.setProperty(ind + i, 'isCollapsed', true)
                    /*if(trackReturnModel.get(ind + i).rowsExpanded < 1)
                        trackReturnModel.setProperty(ind + i, 'rowsExpanded', 1)*/

                    /*if(trackReturnModel.get(ind + i).type === 'Group')
                        trackReturnModel.setProperty(ind + i, 'imgURL', 'qrc:/Resource/folder_off_icon.svg')*/
                }

                funcs.offLightAllTracks()
            }
        }
    }
























    states: [State {
            name: "Right"
            when:!globalValues.tracksOnLeft

            AnchorChanges {
                target: collapsPanel
                anchors.right: root.right
            }

            AnchorChanges {
                target: ma0
                anchors.left: parent.left
                anchors.right: collapsPanel.left
            }

            AnchorChanges {
                target: rootMainTrack
                anchors.left: root.left
                anchors.right: collapsPanel.left
            }
            PropertyChanges {
                target: rootMainTrack
                anchors.leftMargin: 0
                anchors.rightMargin: scaleSize2(10)
            }

            PropertyChanges {
                target: maRoot
                anchors.rightMargin: -scaleSize2(10)
                anchors.leftMargin: 0
            }

            AnchorChanges {
                target: row1
                anchors.left: channelRows.right
                anchors.right: parent.right
            }

            AnchorChanges {
                target: row2
                anchors.left: undefined
                anchors.right: row1.right
            }

            AnchorChanges {
                target: automationRow
                anchors.right: rootMainTrack.right
            }

            AnchorChanges {
                target: channelRows
                anchors.left: rootMainTrack.left
            }

            AnchorChanges {
                target: automationListView
                anchors.right: rootMainTrack.right
            }




            AnchorChanges {
                target: seperator
                anchors.right: automationChildren > 0 & showAutomation ? rootMainTrack.right : parent.right
                anchors.left: parent.left
            }
            PropertyChanges {
                target: seperator
                anchors.rightMargin: automationChildren > 0 & showAutomation ? -scaleSize2(10) : 0
                anchors.leftMargin: 0
            }
        },

        State {
            name: "Left"
            when: globalValues.tracksOnLeft

            AnchorChanges {
                target: collapsPanel
                anchors.left: root.left
            }

            AnchorChanges {
                target: ma0
                anchors.left: collapsPanel.right
                anchors.right: parent.right
            }

            AnchorChanges {
                target: rootMainTrack
                anchors.right: root.right
                anchors.left: collapsPanel.right
            }
            PropertyChanges {
                target: rootMainTrack
                anchors.rightMargin: 0
                anchors.leftMargin: scaleSize2(10)
            }

            PropertyChanges {
                target: maRoot
                anchors.rightMargin: 0
                anchors.leftMargin: -scaleSize2(10)
            }

            AnchorChanges {
                target: row1
                anchors.left: parent.left
                anchors.right: channelRows.left
            }

            AnchorChanges {
                target: row2
                anchors.left: row1.left
                anchors.right: undefined
            }

            AnchorChanges {
                target: automationRow
                anchors.left: rootMainTrack.left
            }

            AnchorChanges {
                target: channelRows
                anchors.right: rootMainTrack.right
            }

            AnchorChanges {
                target: automationListView
                anchors.left: rootMainTrack.left
            }





            AnchorChanges {
                target: seperator
                anchors.right: parent.right
                anchors.left: automationChildren > 0 & showAutomation ? rootMainTrack.left : parent.left
            }
            PropertyChanges {
                target: seperator
                anchors.rightMargin: 0
                anchors.leftMargin: automationChildren > 0 & showAutomation ? -scaleSize2(10) : 0
            }

        }]

}
