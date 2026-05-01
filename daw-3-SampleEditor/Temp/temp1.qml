
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
//import QtQuick.Extras 1.2
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.5
import '.'
import 'qrc:/Items'
import Tools 1.0

Window {
    id: track_list
    visible: true
    width: scaleSize2(1200)
    height: scaleSize2(1450) - 100
    title: qsTr("Track Panel")
    color: '#2F3032'

    minimumWidth: 200
    minimumHeight: 250

    // signals:
    signal globalPositionTrackChanged(var item, var position)

    property var globalPosition
    property var positionInWindow

    /* track UI: */
    property int firstNum: -1
    property int secondNum: -1
    property int firstLightTrackAudioOnShift: -1
    property int secondLightTrackAudioOnShift: -1

    property int firstLightTrackReturnOnShift: -1
    property int secondLightTrackReturnOnShift: -1

    property bool _2Left: true
    property bool _2Top: false
    property bool holdShift: false
    property bool holdCtrl: false
//    property bool solo_on_audio: false
    property bool solo_on_return: false

    property string topFontFamily: myFont2.name
    property int topfontPixelSize: scaleSize(32)
    property alias cursorPosition: cursorPositionClass
    property real topRadius: radiusCal(3)
    property int topWidth: resolution[0] >= 3840
                           & resolution[1] >= 2170 ? ((refSize < .83) ? 1 : (refSize < 1.12) ? 2 : 2) :
                                                      ((refSize < .83) ? 1 : (refSize < 1.12) ? 1 : 2)

    property var resolution: cursorPosition.getResolution()

    property variant groupTrackArray: []
    property variant groupTrackLight: []
    property int group_width: 0

    property real _brightness: option !== undefined ? option.brighnessMeasure : 0
    property var option
    property var option_component

    property bool _vis: option.visible
    on_VisChanged: if(_vis === false) trackListRoot.focus = true

    property int modulation_status: 1

    property real refSize: 1.0
    onRefSizeChanged: {
        if (rightPanel.visible)
            rightPanel.visible = false
        if (trackRightClick.visible)
            trackRightClick.visible = false
    }

    property real scale: cursorPosition.getScale()
    onWidthChanged: scale = cursorPosition.getScale()

    property real widthButtonTrack: oddSize(trackListButton.height * 0.65)

    Theme1 { id: theme1 }
    Theme2 { id: theme2 }
    property Item theme: theme1

    FontLoader { id: myFont; source: 'qrc:/font/Open_Sans/OpenSans-VariableFont.ttf' }
    FontLoader { id: myFont2; source: 'qrc:/font/SourceSansPro-Regular.ttf' }

    CursorPosition { id: cursorPositionClass }

    function oddSize(value) {
        value = Math.round(value)
        if (value % 2 !== 0)
            return value
        else
            return value - 1
    }

    function evenSize(value) {
        value = Math.round(value)
        if (value % 2 === 0)
            return value
        else
            return value - 1
    }

    function offLightAllTracks() {console.log('start offLightAllTracks func')
        if (trackRightClick.visible)
            trackRightClick.visible = false
        if (rightPanel.visible)
            rightPanel.visible = false

        /*for (var i = 0; i < trackAudioModel.count; i++) {
//            trackAudioModel.setProperty(i, "light", false)
//            repGrid.itemAt(i).on = false
            if(trackListView.itemAtIndex(i).on)
            trackListView.itemAtIndex(i).on = false
        }*/

        for (var i = 0; i < trackReturnModel.count; i++) {
            trackReturnModel.setProperty(i, "light", false)
        }

        trackMasterModel.setProperty(0, "light", false)
        groupTrackLight.length = 0;console.log('end offLightAllTracks func')
    }

    function trackLighting_rightclick(myIndex, btn) {
        if (trackRightClick.visible)
            trackRightClick.visible = false
        if (rightPanel.visible)
            rightPanel.visible = false

        firstLightTrackAudioOnShift = myIndex
        secondLightTrackAudioOnShift = -1

        for(var k = 0; k < trackAudioModel.count; k++)
        {
            trackAudioModel.setProperty(k, 'editName', false)
        }

        if(trackAudioModel.get(myIndex).light === false /*!trackListView.itemAtIndex(myIndex).on*/)
        {
            for(var i = 0; i < trackAudioModel.count; i++)
            {
//                trackListView.itemAtIndex(i).on = false
                trackAudioModel.setProperty(i, 'light', false)
            }
            trackAudioModel.setProperty(myIndex, 'light', true)
//            trackListView.itemAtIndex(myIndex).on = true
        }
        // array of tracks are light!
        var j = -1
        for(var z=0; z<trackAudioModel.count; z++)
            if(trackAudioModel.get(z).light/*trackListView.itemAtIndex(z).on*/)
            {
                j++
                groupTrackLight[j] = z
            }
        console.log('rightclick - groupTrackLight:', groupTrackLight.length, groupTrackLight[0])
    }

    function trackLighting(myIndex, btn) {console.log('trackLighting func')
//        console.log(trackAudioModel.get(myIndex).in_set_parent.count)
//        console.log(trackAudioModel.get(myIndex).sub_set_children.count)
        /*console.log('')
        if(trackAudioModel.get(myIndex).in_set_parent.count>0)
        for(var v=0; v<trackAudioModel.get(myIndex).in_set_parent.count; v++)
        console.log('inset member: ', trackAudioModel.get(myIndex).in_set_parent.get(v).member)
        console.log('')
        if(trackAudioModel.get(myIndex).sub_set_children.count>0)
        for(v=0; v<trackAudioModel.get(myIndex).sub_set_children.count; v++)
        console.log('subset member: ', trackAudioModel.get(myIndex).sub_set_children.get(v).member)
        console.log('')*/
//        console.log('lighting: ', trackListView.itemAtIndex(myIndex).on, 'in', myIndex)
        for(var k = 0; k < trackAudioModel.count; k++)
        {
            trackAudioModel.setProperty(k, 'editName', false)
        }

        if(!holdCtrl)
        {
            groupTrackLight.length = 0
            offLightAllTracks()
        } else {
            if (trackRightClick.visible)
                trackRightClick.visible = false
            if (rightPanel.visible)
                rightPanel.visible = false
        }


        if (holdShift) {
            if (firstLightTrackAudioOnShift === -1) {
                firstLightTrackAudioOnShift = myIndex
            } else {
                secondLightTrackAudioOnShift = myIndex
                if (secondLightTrackAudioOnShift === firstLightTrackAudioOnShift)
                    secondLightTrackAudioOnShift = -1
            }

            if (secondLightTrackAudioOnShift > firstLightTrackAudioOnShift &
                    secondLightTrackAudioOnShift !== -1 &
                    firstLightTrackAudioOnShift !== -1) {
                for (var i = firstLightTrackAudioOnShift; i <= secondLightTrackAudioOnShift; i++) {
                    /*if (trackListView.itemAtIndex(i).ind >= firstLightTrackAudioOnShift &
                            trackListView.itemAtIndex(i).ind <= secondLightTrackAudioOnShift) {
                        trackListView.itemAtIndex(i).on = true
                        //                        trackModel.setProperty(i, "light", true)
                    }*/
                    trackAudioModel.setProperty(i, 'light', true)
                }
            } else if (firstLightTrackAudioOnShift > secondLightTrackAudioOnShift
                       & secondLightTrackAudioOnShift !== -1
                       & firstLightTrackAudioOnShift !== -1) {
                for (i = secondLightTrackAudioOnShift; i <= firstLightTrackAudioOnShift; i++) {
                    /*if (trackListView.itemAtIndex(i).ind >= secondLightTrackAudioOnShift &
                            trackListView.itemAtIndex(i).ind <= firstLightTrackAudioOnShift) {
                        trackListView.itemAtIndex(i).on = true
                        //                        trackModel.setProperty(i, "light", true)
                    }*/
                    trackAudioModel.setProperty(i, 'light', true)
                }
            }
            else
            {
                firstLightTrackAudioOnShift = myIndex
                secondLightTrackAudioOnShift = -1

                trackAudioModel.setProperty(myIndex, 'light', true)
                /*for (i = 0; i < trackAudioModel.count; i++)
                    if (trackListView.itemAtIndex(i).ind === myIndex)
                    {
                        trackListView.itemAtIndex(myIndex).on = true
                    }*/
            }
        }
        else if(holdCtrl)
        {
            firstLightTrackAudioOnShift = myIndex
            secondLightTrackAudioOnShift = -1

            /*for (i = 0; i < trackAudioModel.count; i++)
                if (trackListView.itemAtIndex(i).ind === myIndex)
                {
//                    if(trackListView.itemAtIndex(i).on === false)
//                    trackListView.itemAtIndex(i).on = true
//                    else
//                    trackListView.itemAtIndex(i).on = false
                    trackListView.itemAtIndex(i).on = !trackListView.itemAtIndex(i).on
                    return
                }*/
//            trackListView.itemAtIndex(myIndex).on = !trackListView.itemAtIndex(myIndex).on
            if(trackAudioModel.get(myIndex).light)
                trackAudioModel.setProperty(myIndex, 'light', false)
            else if(trackAudioModel.get(myIndex).light === false)
                trackAudioModel.setProperty(myIndex, 'light', true)
            var rInd = groupTrackLight.indexOf(myIndex)
            groupTrackLight.splice(rInd, 1)
        }
        else {
            firstLightTrackAudioOnShift = myIndex
            secondLightTrackAudioOnShift = -1

            /*for(i = 0; i < trackAudioModel.count; i++)
                if(trackListView.itemAtIndex(i).ind === myIndex)
                {
                    trackListView.itemAtIndex(i).on = true
                }*/
//            if(trackAudioModel.get(myIndex).light === false /*!trackListView.itemAtIndex(myIndex).on*/)
//            {

//    //            trackListView.itemAtIndex(myIndex).on = true
//            }

            for(i = 0; i < trackAudioModel.count; i++)
            {
//                trackListView.itemAtIndex(i).on = false
                trackAudioModel.setProperty(i, 'light', false)
            }
            trackAudioModel.setProperty(myIndex, 'light', true)
        }

        // array of tracks are light!
        var j = -1
        for(i=0; i<trackAudioModel.count; i++)
            if(trackAudioModel.get(i).light)
            {
                j++
                groupTrackLight[j] = i
            }
        console.log('groupTrackLight:', groupTrackLight.length, groupTrackLight[0], groupTrackLight[1])// Group click debug
        console.log('--------------------------------------------------------------')
        console.log('  ', trackAudioModel.get(myIndex).pretext, '->')
        console.log('::', 'parents:')
        for(i=0;i<trackAudioModel.get(myIndex).in_set_parent.count;i++)
        console.log('           ',trackAudioModel.get(myIndex).in_set_parent.get(i).member)
        console.log('.')
        console.log('::', 'children:')
        for(i=0;i<trackAudioModel.get(myIndex).sub_set_children.count;i++)
        console.log('            ',trackAudioModel.get(myIndex).sub_set_children.get(i).member)
    }

    function trackReturnLighting(myIndex, btn) {
        offLightAllTracks()

        if (holdShift) {
            if (firstLightTrackReturnOnShift === -1) {
                firstLightTrackReturnOnShift = myIndex
            } else {
                secondLightTrackReturnOnShift = myIndex
                if (secondLightTrackReturnOnShift === firstLightTrackReturnOnShift)
                    secondLightTrackReturnOnShift = -1
            }

            if (secondLightTrackReturnOnShift > firstLightTrackReturnOnShift
                    & secondLightTrackReturnOnShift !== -1
                    & firstLightTrackReturnOnShift !== -1) {
                for (var i = firstLightTrackReturnOnShift; i
                     <= secondLightTrackReturnOnShift; i++) {
                    //                    if(repReturnTrack1.itemAt(i).ind_return >= firstLightTrackReturnOnShift &
                    //                            repReturnTrack1.itemAt(i).ind_return <= secondLightTrackReturnOnShift)
                    //                    {
                    trackReturnModel.setProperty(i, "light", true)
                    //                    }
                }
            } else if (firstLightTrackReturnOnShift > secondLightTrackReturnOnShift
                       & secondLightTrackReturnOnShift !== -1
                       & firstLightTrackReturnOnShift !== -1) {
                for (i = secondLightTrackReturnOnShift; i <= firstLightTrackReturnOnShift; i++) {
                    //                    if(repReturnTrack1.itemAt(i).ind_return >= secondLightTrackReturnOnShift &
                    //                            repReturnTrack1.itemAt(i).ind_return <= firstLightTrackReturnOnShift)
                    //                    {
                    trackReturnModel.setProperty(i, "light", true)
                    //                    }
                }
            }
            else
            {
                firstLightTrackReturnOnShift = myIndex
                secondLightTrackReturnOnShift = -1

                trackReturnModel.setProperty(myIndex, "light", true)
            }
        } else {
            firstLightTrackReturnOnShift = myIndex
            secondLightTrackReturnOnShift = -1

            trackReturnModel.setProperty(myIndex, "light", true)
        }
    }

    function trackMasterLighting(myIndex, btn) {
        offLightAllTracks()
        trackMasterModel.setProperty(0, "light", true)
    }

    function trackAudioSelection(myIndex, btn) {
        if (trackRightClick.visible)
            trackRightClick.visible = false
        if (rightPanel.visible)
            rightPanel.visible = false

        trackAudioModel.get(myIndex).selectSolo = !trackAudioModel.get(myIndex).selectSolo

        var i = 0;
        var j = 0;
        var e = 0;
/*
        if(holdCtrl)
            console.log('You hold Ctrl key in SOLO!!')
        else if(holdCtrl === false)
            console.log('You did not hold Ctrl key in SOLO!!')
*/
        if(!holdCtrl)
        for(i = 0; i < trackAudioModel.count; i++)
        {
            if(i !== myIndex /*& trackAudioModel.get(i).type !== 'Group'*/)
                trackAudioModel.setProperty(i, 'selectSolo', false)
            trackAudioModel.setProperty(i, 'partialySolo', false)
        }

        var children_index = []
        for(e = 0; e < trackAudioModel.count; e++)
        {
            if(trackAudioModel.get(e).type === 'audio' & trackAudioModel.get(e).selectSolo)
            {
                for(i = 0; i < trackAudioModel.get(e).in_set_parent.count; i++)
                {
                    for(j = 0; j < trackAudioModel.count; j++)
                    {
                        if(trackAudioModel.get(e).in_set_parent.get(i).member === trackAudioModel.get(j).pretext)
                        trackAudioModel.setProperty(j, 'partialySolo', true)
                    }
                }// partialySolo.

                for(i = 0; i < trackAudioModel.count; i++)
                if(trackAudioModel.get(i).type !== 'Group')
                trackAudioModel.setProperty(i, 'solo_on_audio', true)                            
            }// if it was audio.
        }
        for(e = 0; e < trackAudioModel.count; e++)
        {
            if(trackAudioModel.get(e).type === 'Group' & trackAudioModel.get(e).selectSolo)
            {
                trackAudioModel.setProperty(e, 'partialySolo', false)
//                trackAudioModel.setProperty(e, 'selectTrack', true)
                trackAudioModel.setProperty(e, 'solo_on_audio', false)
                for(i = 0; i < trackAudioModel.get(e).in_set_parent.count; i++)
                {
                    for(j = 0; j < trackAudioModel.count; j++)
                    {
                        if(trackAudioModel.get(e).in_set_parent.get(i).member === trackAudioModel.get(j).pretext)
                            trackAudioModel.setProperty(j, 'partialySolo', true)
                    }
                }// partialySolo.

                for(i = 1; i <= trackAudioModel.get(e).sub_set_children.count; i++)
                {
                    children_index.push(i + e)//; console.log(i+e)
//                    if(trackAudioModel.get(i).type !== 'Group')
//                    {
                    if(trackAudioModel.get(i + e).selectSolo === false)
                        trackAudioModel.setProperty(i + e, 'solo_on_audio', false)
                    else if(trackAudioModel.get(i + e).selectSolo & trackAudioModel.get(i + e).type !== 'Group')
                        trackAudioModel.setProperty(i + e, 'solo_on_audio', true)
//                        trackAudioModel.setProperty(i + e, 'selectTrack', true)
//                    }
                }
                // children.

                for(i = 0; i < trackAudioModel.count; i++)
                {
                    if((trackAudioModel.get(i).type !== 'Group' & children_index.indexOf(i) === -1) /*|
                            (trackAudioModel.get(i).selectSolo & children_index.indexOf(i) > -1 & trackAudioModel.get(i).type !== 'Group')*/)
                        trackAudioModel.setProperty(i, 'solo_on_audio', true)
                }
            }// if it was group.
        }// for.
//console.log('children_index: ', children_index)
        checkAllSoloTrackAudio_btns()
    }

    function checkAllSoloTrackAudio_btns()
    {
        let allOn = false
        for (var i = 0; i < trackAudioModel.count; i++) {
            if (trackAudioModel.get(i).selectSolo === false)
                allOn = false
            else {
                allOn = true
                return
            }
        }

        if (allOn === false) {
            for (i = 0; i < trackAudioModel.count; i++) {
                if (trackListView.itemAtIndex(i).selectTrackDependSolo)
                    trackAudioModel.setProperty(i, 'selectTrack', true)
                else
                    trackAudioModel.setProperty(i, 'selectTrack', false)

                trackAudioModel.setProperty(i, 'partialySolo', false)
            }
        }

        for(i = 0; i < trackAudioModel.count; i++)
        trackAudioModel.setProperty(i, 'solo_on_audio', allOn)
    }

    function trackReturnSelection(myIndex, btn) {
        if (trackRightClick.visible)
            trackRightClick.visible = false
        if (rightPanel.visible)
            rightPanel.visible = false

        let AllSolo_isOn = false
        var i = 0
        var j = 0

        if (holdCtrl) {
            if (trackReturnModel.get(myIndex).selectSolo) {
                trackReturnModel.setProperty(myIndex, 'selectSolo', false)
                checkAllSoloTrackReturn_btns()
            } else {
                trackReturnModel.setProperty(myIndex, 'selectSolo', true)
                solo_on_return = true
            }
        } // hold.
        else {
            if (trackReturnModel.get(myIndex).selectSolo) {
                for (j = 0; j < trackReturnModel.count; j++) {
                    trackReturnModel.setProperty(j, 'selectSolo', false)
                }
                checkAllSoloTrackReturn_btns()
            } else {
                for (j = 0; j < trackReturnModel.count; j++) {
                    trackReturnModel.setProperty(j, 'selectSolo', false)
                }
                trackReturnModel.setProperty(myIndex, 'selectSolo', true)
                solo_on_return = true
            } // else 2.
        } // else 1.
    }
    function checkAllSoloTrackReturn_btns() {
        let allOn = false
        for (var j = 0; j < trackReturnModel.count; j++) {
            if (trackReturnModel.get(j).selectSolo === false)
                allOn = false
            else {
                allOn = true
                return
            }
        }

        if (allOn === false)
            for (j = 0; j < trackReturnModel.count; j++) {
                if (trackReturnModel.get(j).selectTrackDependSolo)
                    trackReturnModel.setProperty(j, 'selectTrack', true)
                else
                    trackReturnModel.setProperty(j, 'selectTrack', false)
            }
        solo_on_return = allOn
        return allOn
    }

    function toLocalePosition(toItem, fromItem, position) {
        // return the local position if inside item, or null if outside
        var localPos = toItem.mapFromItem(fromItem, position.x, position.y)
        if (localPos.x >= 0 & localPos.y >= 0 & localPos.x <= toItem.width
                & localPos.y <= toItem.height) {
            return localPos
        }
        return null
    }

    function check_child_num(n) {
        var length = trackAudioModel.count
        var result = trackListView.children[n]
        if (trackListView.children[n].Layout.row !== n)
            for (var a = 0; a < length; a++)
                if (trackListView.children[a].Layout.row === n)
                    result = trackListView.children[a]
        return result
    }

    function radiusCal(value) {
        value = value * scale * refSize
        return Math.round(value)
    }

    function scaleSize(value) {
        value = Math.round(value * scale * refSize / 2)
        if (value % 2 !== 0)
            return value
        else
            return value - 1
    }

    function scaleSize2(value) {
        value = Math.round(value * scale * refSize / 2)

        return value
    }

    /* ------- tracks.  */
    ListModel {
        id: trackAudioModel

        ListElement {
            type: 'audio'
            pretext: 'audio 1'
            isCollapsed: true
            in_set_parent: [] // این لیست المنت زیر مجموعه ی این اعضااست
            sub_set_children: [] // گروپ و ترک هایی که زیر مجموعه ی این لیست المنت هستند

            selectTrack: true
            selectSolo: false
            partialySolo: false
            solo_on_audio: false
            selectTrackDependSolo: true

            coverBound: false

            editName: false

            inDraggingCounter: -1

            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'

            // Automation
            automationChildren: 0
            volumeAutomated: false
            panAutomated: false
            speakerOnAutomated: false
            automationList: []
        }

        ListElement {
            type: 'audio'
            pretext: 'audio 2'
            isCollapsed: true
            in_set_parent: []  //ListElement{jj:'first'}
            sub_set_children: []

            selectTrack: true
            selectSolo: false
            partialySolo: false
            solo_on_audio: false
            selectTrackDependSolo: true

            coverBound: false

            editName: false

            inDraggingCounter: -1

            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'

            // Automation
            automationChildren: 0
            volumeAutomated: false
            panAutomated: false
            speakerOnAutomated: false
            automationList: []
        }
        /*ListElement {
            type: 'Group'
            pretext: 'group 2'
            isCollapsed: true
            in_set_parent: []
            sub_set_children: [ ListElement{g:4} ]
            selectTrack: true
            selectSolo: false
            light: false
            channel_count: 8
            rowsExpanded: 1
        }

        ListElement {
            type: 'Group'
            pretext: 'group 1'
            isCollapsed: true
            in_set_parent: [ListElement{g:3}]
            sub_set_children: []
            selectTrack: true
            selectSolo: false
            light: false
            channel_count: 8
            rowsExpanded: 1
        }*/

        ListElement {
            type: 'audio'
            pretext: 'audio 3'
            isCollapsed: true
            in_set_parent: []//[ ListElement{g:3}, ListElement{g:4} ]
            sub_set_children: []

            selectTrack: true
            selectSolo: false
            partialySolo: false
            solo_on_audio: false
            selectTrackDependSolo: true

            coverBound: false

            editName: false

            inDraggingCounter: -1

            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'

            // Automation
            automationChildren: 0
            volumeAutomated: false
            panAutomated: false
            speakerOnAutomated: false
            automationList: []
        }

        ListElement {
            type: 'audio'
            pretext: 'audio 4'
            isCollapsed: true
            in_set_parent: []
            sub_set_children: []

            selectTrack: true
            selectSolo: false
            partialySolo: false
            solo_on_audio: false
            selectTrackDependSolo: true

            coverBound: false

            editName: false

            inDraggingCounter: -1

            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'

            // Automation
            automationChildren: 0
            volumeAutomated: false
            panAutomated: false
            speakerOnAutomated: false
            automationList: []
        }

        ListElement {
            type: 'audio'
            pretext: 'audio 5'
            isCollapsed: true
            in_set_parent: []
            sub_set_children: []

            selectTrack: true
            selectSolo: false
            partialySolo: false
            solo_on_audio: false
            selectTrackDependSolo: true

            coverBound: false

            editName: false

            inDraggingCounter: -1

            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'

            // Automation
            automationChildren: 0
            volumeAutomated: false
            panAutomated: false
            speakerOnAutomated: false
            automationList: []
        }

        ListElement {
            type: 'audio'
            pretext: 'audio 6'
            isCollapsed: true
            in_set_parent: []
            sub_set_children: []

            selectTrack: true
            selectSolo: false
            partialySolo: false
            solo_on_audio: false
            selectTrackDependSolo: true

            coverBound: false

            editName: false

            inDraggingCounter: -1

            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'

            // Automation
            automationChildren: 0
            volumeAutomated: false
            panAutomated: false
            speakerOnAutomated: false
            automationList: []
        }

        ListElement {
            type: 'audio'
            pretext: 'audio 7'
            isCollapsed: true
            in_set_parent: []
            sub_set_children: []

            selectTrack: true
            selectSolo: false
            partialySolo: false
            solo_on_audio: false
            selectTrackDependSolo: true

            coverBound: false

            editName: false

            inDraggingCounter: -1

            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'

            // Automation
            automationChildren: 0
            volumeAutomated: false
            panAutomated: false
            speakerOnAutomated: false
            automationList: []
        }
    }

    ListModel {
        id: trackMasterModel

        ListElement {
            selectTrack: true
            selectSolo: false
            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'
        }
    }

    ListModel {
        id: trackReturnModel

        ListElement {
            selectTrack: true
            selectSolo: false
            selectTrackDependSolo: true
            light: false
            channel_count: 8
            rowsExpanded: 1
            imgURL: 'qrc:/Resource/Triangle.svg'
        }

        /*ListElement {
            selectTrack: true
            selectSolo: false
            selectTrackDependSolo: true
            light: false
            channel_count: 8
            rowsExpanded: 1
        }

        ListElement {
            selectTrack: true
            selectSolo: false
            selectTrackDependSolo: true
            light: false
            channel_count: 8
            rowsExpanded: 1
        }*/
    }

    Button {
        id: options_btn
        width: scaleSize(200)
        height: scaleSize(50)
        z: 5
        text: "options"

        onClicked: {
            option_component = Qt.createComponent("Preference.qml")
            option = option_component.createObject(track_list)
            option.show()
            focus = false
        }
        focus: false
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -50
    }

    Button {z:5
        id: left2rightBtn
        height: options_btn.height
        width: scaleSize(200)
        anchors.right: options_btn.left
        anchors.rightMargin: 5
        text: _2Left ? 'left' : 'right'
        onClicked: _2Left = !_2Left
    }
    Button {z:5
        height: options_btn.height
        width: scaleSize(250)
        anchors.right: left2rightBtn.left
        anchors.rightMargin: 5
        text: _2Top ? 'master on top' : 'master on bottom'
        onClicked: _2Top = !_2Top
    }

    Popup {
        id: popup
        x: trackListButton.addTrackBtn.x + trackListButton.x
        y: trackListButton.addTrackBtn.y + trackListButton.addTrackBtn.height
           + 1 + trackListButton.y

        background: Rectangle {
            id: backAdd
            implicitWidth: Math.round(trackListButton.width * 0.7)
            implicitHeight: Math.round(implicitWidth * .85)
            radius: topRadius
            color: 'transparent'
        }

        contentItem: AddTrack {
            anchors.fill: backAdd
        }
        onOpenedChanged: {
            trackListButton.addTrackBtn.isClicked = opened ? true : false
        }
    }

    /*Rectangle {
        id: betweenRec
        z: trackListRoot.z + 1
        visible: ma4listview.halfDown | ma4listview.halfUp
        anchors {
            leftMargin: ma4listview.outBound ? 0 : trackAudioModel.get(ma4listview.indexDrag).in_set_parent.count * scaleSize2(50)
            left: trackListRoot.left
            right: trackListRoot.right
        }
        height: scaleSize2(5) //trackListView.spacing
        color: theme.betweenRec_dragging_o20
        y: trackListRoot.y
    }*/

    Rectangle {
        visible: false
        id: scoller
        anchors.left: trackListRoot.right
        anchors.leftMargin: 15
        y: _2Top & (trackListButton.masterIsOn
                    | trackListButton.returnIsOn) ? topGrid.height
                                                    + topGrid.rowSpacing : trackListRoot.y
        width: scaleSize2(15)
        height: scaleSize2(50)
        radius: 5
        MouseArea {
            anchors.fill: parent
            drag {
                target: parent
                axis: Drag.YAxis
                minimumY: _2Top & (trackListButton.masterIsOn | trackListButton.returnIsOn) ? topGrid.height + topGrid.rowSpacing : trackListRoot.y
                maximumY: track_list.height - 25
            }
        }
    }

    // ------------------------------------------------------------------------------------------------ C O M P O N E N T :
    Component {
        id: audioTrackComponent
        Rectangle {
            id: audioTrack
//             height: isCollapsed ? scaleSize2(18) + scaleSize2(37) + scaleSize2(56) * trackAudioModel.get(index).rowsExpanded + scaleSize2(5) : 0
            height: isCollapsed ? trackAudio.height : 0
            width: trackListView.width

            clip: true

            color: light ? theme.bg1 : theme.bg2

            property int ind: index

            property alias draggableArea: trackAudio.draggableSize // for dragging or scroll

            property alias automateRun: trackAudio.automationRun

            TrackAudio2 {
                id: trackAudio
                               onLeftSide: _2Left
                name: pretext //+ ' ' + (index + 1)
                numberAudio: audioTrack.ind
            }
            Rectangle {
                visible: coverBound
                anchors.fill: trackAudio

                color: 'transparent'
                border.color: theme.betweenRec_dragging_o20
                border.width: scaleSize2(5)
            }
        }
    }

    Component {
        id: returnTrackComponent
        Rectangle {
            id: returnTrack
            visible: trackListButton.returnIsOn
            enabled: trackListButton.returnIsOn
            Layout.fillWidth: true
            Layout.preferredHeight: scaleSize2(18) + scaleSize2(
                                        37) + scaleSize2(
                                        56) * trackReturnModel.get(
                                        index).rowsExpanded
            color: /*on*/ trackReturn.parentFocus ? theme.bg1 : theme.bg2

            clip: true

            property alias rowsExpanded: trackReturn.rowsExpanded
            onRowsExpandedChanged: trackReturnModel.setProperty(index,
                                                               'rowsExpanded',
                                                               rowsExpanded)

            property int ind_return: index
            property bool on: trackReturnModel.get(ind_return).light

            TrackReturn {
                id: trackReturn
                //                onLeftSide: _2Left
                numberAudio: returnTrack.ind_return
                didSelectSolo: trackReturnModel.get(ind_return).selectSolo
                didSelectTrack: trackReturnModel.get(ind_return).selectTrack
                onTrackSelectPressed: {
                    if (trackReturnModel.get(ind_return).selectTrackDependSolo)
                        trackReturnModel.setProperty(ind_return,
                                                     "selectTrackDependSolo",
                                                     false)
                    else
                        trackReturnModel.setProperty(ind_return,
                                                     "selectTrackDependSolo",
                                                     true)
                }
            }
        }
    }

    Component {
        id: masterTrackComponent
        Rectangle {
            id: masterTrack
            visible: trackListButton.masterIsOn
            enabled: trackListButton.masterIsOn
            Layout.fillWidth: true
            Layout.preferredHeight: scaleSize2(18) + scaleSize2(
                                        37) + scaleSize2(
                                        56) * trackMasterModel.get(
                                        0).rowsExpanded
            color: /*on*/ trackMaster.parentFocus ? theme.bg1 : theme.bg2

            clip: true
            property alias rowsExpanded: trackMaster.rowsExpanded
            onRowsExpandedChanged: trackMasterModel.setProperty(0,
                                                               'rowsExpanded',
                                                               rowsExpanded)

            property bool on: trackMasterModel.get(0).light

            TrackMaster {
                id: trackMaster
                //                onLeftSide: _2Left
            }
        }
    }

    // ------------------------------------------------------------------------------------------------ C O M P O N E N T.
    HeaderTracks {
        id: trackListButton
        z: trackListView.z + 1

        onTurnAutomations: {
            for(var i = 0; i < trackAudioModel.count; i++)
                trackListView.itemAtIndex(i).automateRun = on
        }
    }
    MouseArea {
        anchors.fill: parent
        onWheel: trackListView.flick(0, wheel.angleDelta.y * 5)
//        hoverEnabled: true // TODO fix focus for keys
//        onEntered: trackListRoot.focus = true // fot enabling key event
    }

    // ------------------------------------------------------------------------------------------Tracks:
    Rectangle {
        id: trackListRoot
        anchors.top: trackListButton.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: draggerWidth.x + draggerWidth.width //+ group_width

        color: 'transparent'
        //border.color: 'lightblue'
        clip: true

        property alias cursorTrackListRoot: mouseArea.cursorShape

        MouseArea { // for shape pointer
            id: mouseArea
            anchors.fill: parent
            onPressed: mouse.accepted = false
        }

//        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_Shift) {
                holdShift = true
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = true
            }
        }

        Keys.onReleased: {
            if (event.key === Qt.Key_Shift) {
                holdShift = false
            }
            if (event.key === Qt.Key_Control) {
                holdCtrl = false
            }
        }

        GridLayout {
            id: topGrid
            visible: _2Top
            enabled: _2Top
            width: parent.width
            columns: 1
            z: trackListView.z + 1

            rowSpacing: scaleSize2(10)

            Repeater {
                id: repMasterTrack1
                model: 1
                delegate: masterTrackComponent
            }

            Repeater {
                id: repReturnTrack1
                model: trackReturnModel
                delegate: returnTrackComponent
            }
        }

        ListView {
            id: trackListView;//focus: true
            z:  1//ma4listview.z+10///*ma4listview.isAncherDrag ? ma4listview.z :*/ ma4listview.z + 1
            clip: true
            // y: _2Top & (trackListButton.masterIsOn
            //             | trackListButton.returnIsOn) ? (topGrid.height + topGrid.rowSpacing) * 2
            //                                             - scoller.y : scoller.y * 0 + spacing
            width: parent.width
            height: Math.round(track_list.height * .7)//150 + 150 + 120
            spacing: scaleSize2(5)*0
            cacheBuffer: 1000

            flickableDirection: Flickable.VerticalFlick
            interactive: false
            boundsMovement: Flickable.StopAtBounds
            maximumFlickVelocity: 300
            property int upIndexItem:   indexAt(2, contentY)
            property int downIndexItem: indexAt(2, contentY+height)
            onContentYChanged: {delay2UpAutoScroll = 1; delay2DownAutoScroll = 1}
            onUpIndexItemChanged: delay2UpAutoScroll = 35
            onDownIndexItemChanged: delay2DownAutoScroll = 35

            model: trackAudioModel
            delegate: audioTrackComponent
        }
        GroupingMouseArea {
            // change RowLevel and autoscrll
            id: ma4listview
//          anchors.fill: gridTrackListView
            anchors {
                left:       trackListView.left
                top:        trackListView.top
                bottom:     trackListView.bottom
                bottomMargin: -scaleSize2(5)
            }
            onIsAncherDragChanged: trackListView.z = isAncherDrag ? 0 : 1
        }
        /*****/
        Rectangle {width: trackListView.width; height: scaleSize2(5);color: theme.betweenRec_dragging_o20; z: 1; visible: ma4listview.isTop}
        Rectangle {width: trackListView.width; height: scaleSize2(5); color: theme.betweenRec_dragging_o20; y: ma4listview.yBottom + trackListView.y - scaleSize2(5)
            z: 1; visible: ma4listview.isBottom}
        /*****/



        GridLayout {
            id: bottomGrid
            visible: !_2Top
            enabled: !_2Top

            /*Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            height: trackListRoot.height*/
            width: parent.width
            anchors.bottom: parent.bottom

            columns: 1

            rowSpacing: topGrid.rowSpacing

            //            parent: trackListRoot//gridTracks
            Repeater {
                id: repReturnTrack2
                model: trackReturnModel
                delegate: returnTrackComponent
            }

            Repeater {
                id: repMasterTrack2
                model: 1
                delegate: masterTrackComponent
            }
        }

        Rectangle {
            id: draggerWidth
            color: 'transparent'//'transparent' //'salmon'
            width: 10
            height: parent.height
            x: 350 + group_width
            z: 1

            MouseArea {
                id: ma
                anchors.fill: parent
                property int vMWidth: 8 * scaleSize2(8) + topWidth * 20

                hoverEnabled: true
                onEntered: cursorPosition.hCursorShape()
                onExited: if(!pressed) cursorPosition.onlyShowCursor()
                onPressed: cursorPosition.hCursorShape()
                onReleased: cursorPosition.onlyShowCursor()

                drag {
                    axis: Drag.XAxis
                    minimumX: scaleSize2(50) + 4 * scaleSize2(106) + scaleSize2(10) + 3 * scaleSize2(15) + ma.vMWidth + group_width // TODO 4 general resize
                    maximumX: 1000
                    target: draggerWidth
                }
            }
        }
    }

    property int delay2UpAutoScroll: 1
    property int delay2DownAutoScroll: 1
    property int requireSizeForAutoScroll: 35
    Timer {
        id: timer2Up
        interval: 10 + 10 * delay2UpAutoScroll
        repeat: true
        onTriggered: {
            if(!trackListView.atYBeginning)
            {
                trackListView.contentY--
            }
        }
    }
    Timer {
        id: timer2Down
        interval: 10 + 10 * delay2DownAutoScroll
        repeat: true
        onTriggered: {
            if(!trackListView.atYEnd)
            {
                trackListView.contentY++
            }
        }
    }

    // -----------------------------------------------------------------------------------------------------------  Right Click:
    function equalArr(ar1, ar2)
    {
      return ar1.sort().toString() === ar2.sort().toString()
    }
    function allow2MakeGroup_() {
        var pureArray = groupTrackLight.slice()
        var repeatMember0 = []
        for(var i = 0; i < groupTrackLight.length; i++)
        {
            if(trackAudioModel.get(groupTrackLight[i]).type === 'Group')
            {
                // subset_members = groupTrackLight[i] // index of Group
                for(var j = 0; j < trackAudioModel.get(groupTrackLight[i]).sub_set_children.count; j++)
                {
                    var child_exist_index0 = pureArray.indexOf(groupTrackLight[i] + j + 1)

                    if(child_exist_index0 > -1)
                    {
                        repeatMember0.push(child_exist_index0)
                        pureArray.splice(child_exist_index0, 1)
                    }

                }
            }// if Group.
        }// pure_audio_or_group.

        var result = true
        var resultArr = []
        var arr1 = []
        var arr2 = []

        if(pureArray.length === 1) {result = true; return result}
        else
        {
            for(i = 0; i < trackAudioModel.get(pureArray[0]).in_set_parent.count; i++)
            {
                arr1.push(trackAudioModel.get(pureArray[0]).in_set_parent.get(i).member)
            }
            for(i = 0; i < trackAudioModel.get(pureArray[0]).sub_set_children.count; i++)
            {
                arr1.push(trackAudioModel.get(pureArray[0]).sub_set_children.get(i).member)
            }

            for(var k = 1; k < pureArray.length; k++)
            {
                arr2.length = 0
                for(i = 0; i < trackAudioModel.get(pureArray[k]).in_set_parent.count; i++)
                {
                    arr2.push(trackAudioModel.get(pureArray[k]).in_set_parent.get(i).member)
                }
                for(i = 0; i < trackAudioModel.get(pureArray[k]).sub_set_children.count; i++)
                {
                    arr2.push(trackAudioModel.get(pureArray[k]).sub_set_children.get(i).member)
                }
                resultArr.push(equalArr(arr1, arr2))
            }
        }

        if(resultArr.indexOf(false) > -1) result = false
        else result = true

        console.log('***********************************: ', 'result: ', result)
        return result
    }

    function allow2MakeGroup() {
        var resultArr = []
        var result = true
        var childrenArr = []
        let checker = (arr, target) => target.every(v => arr.includes(v));
        for(var i = 0; i < trackAudioModel.count; i++)
        {
            if(trackAudioModel.get(i).type === 'Group' & groupTrackLight.indexOf(i) < 0)
            {
                for(var j = 1; j <= trackAudioModel.get(i).sub_set_children.count; j++)
                    childrenArr.push(i+j)

                if(groupTrackLight.length > childrenArr.length)
                {
                    for(var l = 0; l < groupTrackLight.length; l++)
                    for(var k = 0; k < childrenArr.length; k++)
                    {
                        if(childrenArr[k] > groupTrackLight[l] & childrenArr.indexOf(groupTrackLight[l]) < 0)
                            resultArr.push(!checker(groupTrackLight, childrenArr))
                    }
                }
            }
//            console.log('*groupTrackLight:', groupTrackLight, 'childrenArr:', childrenArr)
            childrenArr.length = 0
        }

        if(resultArr.indexOf(false) > -1) result = false
        else result = true        
//console.log('*: ', result)
        return result
    }

    Window {
        id: trackRightClick
        width: panel.width //Math.round(350 *0.75 * refSize)
        height: panel.height //Math.round(500 *0.75 * refSize)
        visible: false
        color: 'transparent'

        flags: Qt.Popup | Qt.Dialog | Qt.FramelessWindowHint

        x: positionInWindow !== undefined ? positionInWindow.x + track_list.x : 0
        y: positionInWindow !== undefined ? positionInWindow.y + track_list.y : 0

//        property alias cc: panel.panel
        property alias numberItem: panel.numberItem

        property bool visble_grouper: allow2MakeGroup()
        property bool visble_ungrouper: false

        TrackRightClick {
            id: panel
            isVisible_groupMaker: trackRightClick.visble_grouper
            isVisible_ungrouper: trackRightClick.visble_ungrouper
        }
    }

    Window {
        id: rightPanel
        visible: false
        width: rPanel.width
        height: rPanel.height
        color: 'transparent'

        flags: Qt.Popup | Qt.Dialog | Qt.FramelessWindowHint

        x: positionInWindow !== undefined ? positionInWindow.x + track_list.x : 0
        y: positionInWindow !== undefined ? positionInWindow.y + track_list.y : 0

        property var rightModules: ['', '']
        property var leftModules: ['Show Automation', 'Show Automation in New Lane']
        property alias panelWidth: rPanel.parentWidth
        property alias isTwoGroups: rPanel.twoGroups
        property alias firstStr: rPanel.firstGroup

        property alias cc: rPanel.panel

        RightPanel {
            id: rPanel
            property var rightModules: rightPanel.rightModules
            property var leftModules: rightPanel.leftModules
        }
    }










    //--------------------------------------------------------------------------------- Brightness:
    Rectangle {
        z: 1
        anchors.fill: trackListButton
        color: _brightness < 0 ? 'white' : 'black'
        opacity: Math.abs(_brightness) / 200
    }
    Rectangle {
        z: 1
        anchors.fill: trackListRoot
        color: _brightness < 0 ? 'white' : 'black'
        opacity: Math.abs(_brightness) / 200
    }
    //--------------------------------------------------------------------------------- Brightness.
}
