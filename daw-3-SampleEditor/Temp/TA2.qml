import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.2
import 'qrc:/Items'
import 'qrc:/TrackAudio/Automation/'

Item {
    id:             root

    property int channels:      8

    property int numberAudio: 1

    // anchors.left:   parent.left
    // anchors.right:  parent.right
    width: parent.width
    height:         rootMainTrack.height + (automationChildren > 0 & showAutomation ? automationListView.height + scaleSize2(15) : 0)

    property Item nameItem: textLabel

    property bool splitStereo: false

    property real _refSize: refSize
    on_RefSizeChanged: seperator.y = (rowsExpanded + 1) * (scaleSize2(37) + scaleSize2(15)) + scaleSize2(10) - seperator.height


    property alias automationRun: automationBtn.isClicked

    property int _rowsExpanded: -1

    property bool showAutomation: false


    property bool inTrack: false
    property bool inAutomationMenu: false
    property bool inAutomationChild: false
    property real tf3: 0
    onTf3Changed: {
        if(inAutomationMenu | inAutomationChild) {ttf.value = tf3; ttf.text = tf3}
    }

    property string name: 'Audio'

    property alias draggableSize: collapsPanel.width
    property alias volumSize: volumeMeter.width

    property bool didFold:      rowsExpanded < 1 ? true : false
    property bool didLight:    false
    property bool secondLevelLight: select_btn.didShadow | solo_btn.didShadow | tf2.didShadow | ttf.didShadow
    property bool firstLevelLight: didLight | automationBtn.didShadow | viewBtn.didShadow | recordBtn.didShadow | sendBtn.didShadow

    property bool onLeftSide:   true

    property alias colorPanel:  collapsPanel.color

//    property alias didSelectTrack:  select_btn.isClicked
//    property alias didSelectSolo:  solo_btn.isClicked

    property alias volumeMeterWidth: volumeMeter.width

//    property int group_childs: group_childs_func(pretext)

    property var colors: ['#FFCADA', '#F44336', '#FFDD9F', '#FF5722', '#F3C430', '#7C9712', '#61D67D', '#00CBE6', '#293AAE', '#9C27B0', '#808080',
        '#D694A3', '#E52729', '#FCBD87', '#CC5600', '#DAA521', '#57501B', '#007F3C', '#2166F3', '#232692', '#673AB7', '#D5BF96',
        '#FB939C', '#CA252B', '#FF7F50', '#B73F0B', '#FFC107', '#66CA43', '#065C37', '#03A9F4', '#4652AB', '#612652', '#B6863C',
        '#FE50B9', '#C6100A', '#E4715C', '#FEF167', '#BDDF43', '#48B30F', '#8AD8E4', '#729EF5', '#B97DDA', '#56253B', '#78450E',
        '#E91E63', '#950C04', '#FF9800', '#FEEB2C', '#94CF37', '#1C911C', '#30CFCA', '#5159DE', '#AC41EF', '#CFCECA', '#873E2B']

    states: [State {
            name: "Right"
            when:!onLeftSide

            /*AnchorChanges {
                target: draggableArea
                anchors.left: undefined
                anchors.right: parent.right
            }*/

            AnchorChanges {
                target: collapsPanel
                anchors.left: undefined
                anchors.right: parent.right
            }

            AnchorChanges {
                target: solo_btn
                anchors.left: undefined
                anchors.right: parent.right//collapsPanel.left
            }
            PropertyChanges {
                target: solo_btn
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(10)
            }

            AnchorChanges {
                target: select_btn
                anchors.left: undefined
                anchors.right: solo_btn.left
            }
            PropertyChanges {
                target: select_btn
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: name
                anchors.left: volumeMeter.right
                anchors.right: solo_btn.left
            }
            PropertyChanges {
                target: name
                anchors.leftMargin: scaleSize2(10)
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: tf_10
                anchors.left: undefined
                anchors.right: collapsPanel.left
            }
            PropertyChanges {
                target: tf_10
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(10)
            }

            AnchorChanges {
                target: tf_11
                anchors.left: undefined
                anchors.right: tf_10.left
            }
            PropertyChanges {
                target: tf_11
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: record_send_btn
                anchors.left: undefined
                anchors.right: tf_11.left
            }
            PropertyChanges {
                target: record_send_btn
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: auto_view_btn
                anchors.left: undefined
                anchors.right: record_send_btn.left
            }
            PropertyChanges {
                target: auto_view_btn
                anchors.leftMargin: undefined
                anchors.rightMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: volumeMeter
                anchors.right: undefined
                anchors.left: parent.left
            }
        },

        State {
            name: "Left"
            when: onLeftSide

            /*AnchorChanges {
                target: draggableArea
                anchors.right: undefined
                anchors.left: parent.left
            }*/

            AnchorChanges {
                target: collapsPanel
                anchors.right: undefined
                anchors.left: parent.left
            }

            AnchorChanges {
                target: select_btn
                anchors.right: undefined
                // anchors.left: collapsPanel.right
            }
            PropertyChanges {
                target: select_btn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(10)
            }

            AnchorChanges {
                target: solo_btn
                anchors.right: undefined
                anchors.left: select_btn.right
            }
            PropertyChanges {
                target: solo_btn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: name
                anchors.right: volumeMeter.left
                anchors.left: solo_btn.right
            }
            PropertyChanges {
                target: name
                anchors.rightMargin: scaleSize2(10)
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: tf_10
                anchors.right: undefined
                // anchors.left: collapsPanel.right
            }
            PropertyChanges {
                target: tf_10
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(10)
            }

            AnchorChanges {
                target: tf_11
                anchors.right: undefined
                anchors.left: tf_10.right
            }
            PropertyChanges {
                target: tf_11
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: record_send_btn
                anchors.right: undefined
                anchors.left: tf_11.right
            }
            PropertyChanges {
                target: record_send_btn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: auto_view_btn
                anchors.right: undefined
                anchors.left: record_send_btn.right
            }
            PropertyChanges {
                target: auto_view_btn
                anchors.rightMargin: undefined
                anchors.leftMargin: scaleSize2(15)
            }

            AnchorChanges {
                target: volumeMeter
                anchors.left: undefined
                anchors.right: rootMainTrack.right
            }
        }]

    property bool didGradient: light//| automationBtn.didShadow | viewBtn.didShadow | recordBtn.didShadow | sendBtn.didShadow
    property Gradient _3DGradient: Gradient {
        GradientStop { position: 0.0;  color: didGradient ? "#C5C7C9" : theme.s11_Enable_FIL }
        GradientStop { position: 0.17; color: didGradient ? "#BDBEBF" : theme.s11_Enable_FIL }
        GradientStop { position: 0.5;  color: didGradient ? "#AAABAD" : theme.s11_Enable_FIL }
        GradientStop { position: 0.82; color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL }
        GradientStop { position: 1.0;  color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL }
    }

    function changeRowCounter(num) {
        seperator.y = scaleSize2(37)*num + scaleSize2(15)*num + scaleSize2(10) - seperator.height

        var _i = Math.round((seperator.y - scaleSize2(10)) / (scaleSize2(37) + scaleSize2(15))) - 1
        if(_i !== rowsExpanded)
            rowFunc(_i)
    }

    // onRowsExpandedChanged: {//console.log('----------------0-----------------------> ', rowsExpanded, spanVert.main, trackAudioModel.get(ind).rowsExpanded)
//    onRowCounterChanged: {//console.log('\u001b[1;32m row counter  track ', ind, 'is changed to ' , rowCounter ,'\u001b[1;35m')
    function rowFunc(rowCounter) {
        trackAudioModel.setProperty(index, 'rowsExpanded', rowCounter)
        var arr = []
        if(type === 'Group' & trackAudioModel.get(ind).rowsExpanded === 1) {
            trackAudioModel.setProperty(ind, 'imgURL', 'qrc:/Resource/folder_off_icon.svg')

            for(var i = 1; i <= trackAudioModel.get(ind).sub_set_children.count; i++)
            {
                if(trackAudioModel.get(ind + i).type === 'Group')
                {
                    trackAudioModel.setProperty(ind + i, 'isCollapsed', true)
                }

                arr.length = 0
                if(trackAudioModel.get(ind + i).type === 'audio')
                {
                    for(var j = 0; j < trackAudioModel.get(ind + i).in_set_parent.count; j++)
                    {
                        for(var k = 0; k < trackAudioModel.count; k++)
                        {
                            if(trackAudioModel.get(ind + i).in_set_parent.get(j).member === trackAudioModel.get(k).pretext)
                            {
                                if(trackAudioModel.get(k).rowsExpanded > 0)
                                arr.push(true)
                                else arr.push(false)
                            }
                        }
                    }
                }

                if(arr.indexOf(false) === -1) trackAudioModel.setProperty(ind + i, 'isCollapsed', true)


            }// main for.
            trackListView.positionViewAtIndex(ind, ListView.Visible)

            offLightAllTracks()
        }

        if(rowCounter === 0) showAutomation = false
        else showAutomation = true

        // console.log('----------------1-----------------------> ', rowsExpanded, spanVert.main, trackAudioModel.get(ind).rowsExpanded)
    }

    function getRandomInt(max) {
      return Math.floor(Math.random() * max);
    }

    function change_rowExpanding()
    {
        if(trackAudioModel.get(ind).rowsExpanded > 0)
        {
            _rowsExpanded = rowsExpanded
            // rowsExpanded = 0
            trackAudioModel.setProperty(ind, 'rowsExpanded', 0)
            // spanVert.yIndicator = 0
            changeRowCounter(1)

            if(type === 'Group')
            {
                trackAudioModel.setProperty(ind, 'imgURL', 'qrc:/Resource/folder_on_icon.svg')

                for(var i = 1; i <= trackAudioModel.get(ind).sub_set_children.count; i++)
                {
                    trackAudioModel.setProperty(ind + i, 'isCollapsed', false)

                    /*if(trackAudioModel.get(ind + i).type === 'Group')
                    trackAudioModel.setProperty(ind + i, 'imgURL', 'qrc:/Resource/folder_on_icon.svg')*/
                }

                offLightAllTracks()
            }
        }
        else
        {
            trackAudioModel.setProperty(ind, 'rowsExpanded', _rowsExpanded)
            changeRowCounter(_rowsExpanded+1)

            if(type === 'Group')
            {
                trackAudioModel.setProperty(ind, 'imgURL', 'qrc:/Resource/folder_off_icon.svg')

                for(i = 1; i <= trackAudioModel.get(ind).sub_set_children.count; i++)
                {
                    if(trackAudioModel.get(ind + i).type !== 'Group')
                    trackAudioModel.setProperty(ind + i, 'isCollapsed', true)
                    /*if(trackAudioModel.get(ind + i).rowsExpanded < 1)
                        trackAudioModel.setProperty(ind + i, 'rowsExpanded', 1)*/

                    /*if(trackAudioModel.get(ind + i).type === 'Group')
                        trackAudioModel.setProperty(ind + i, 'imgURL', 'qrc:/Resource/folder_off_icon.svg')*/
                }

                offLightAllTracks()
            }
        }
    }

    // ----------------------------------------------------------------------------------------------------- span snap:

    Rectangle {
        id:         collapsPanel
        width:      type === 'Group' ? trackAudioModel.get(ind).rowsExpanded > 0 ?
                                           (in_set_parent.count+2) * scaleSize2(50) :
                                           (in_set_parent.count+1) * scaleSize2(50) :// group
        leftRec.visible ? (in_set_parent.count+1) * scaleSize2(50) : scaleSize2(50) // audio
            //type === 'Group' || in_set_parent.count > 0 ? (group_childs + in_set_parent.count) * 5 + scaleSize2(50) : scaleSize2(50)
        height:     parent.height
        color:      colors[getRandomInt(colors.length -1)]//Qt.rgba(Math.random(),Math.random(),Math.random(),1)//'#1791E9'

        Rectangle {
            id: leftRec
            visible: in_set_parent.count > 0
            width: scaleSize2(50) * in_set_parent.count
            height: parent.height
            color: light  ? theme.bg1 : theme.bg2
        }

        Image {
            id: fold
            anchors {
                top: parent.top
                topMargin: scaleSize2(15)
                right: type === 'Group' ? undefined : parent.right
                rightMargin: type === 'Group' ? 0 : scaleSize2(12)// - scaleSize2(23)
                left: type === 'Group' ? parent.left : undefined
                leftMargin: type === 'Group' ? scaleSize2(12) + in_set_parent.count * scaleSize2(50) /*+ leftRec.width*/ : undefined
            }

            source: imgURL

            sourceSize: Qt.size(scaleSize2(25), scaleSize2(25))
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    change_rowExpanding()

                    if(rowsExpanded === 0) showAutomation = false
                    else showAutomation = true

                    if(automationBtn.isClicked & rowsExpanded < 2 & showAutomation)
                        changeRowCounter(rowsExpanded+2)
                }
            }

            rotation: type === 'audio' ? (didFold ? -90 : 0) : 0
        }
    }
    // ----------------------------------------------------------------------------------------------------- span snap vertical

    Item {
    id: rootMainTrack
    // height: seperator.y + seperator.height //+ scaleSize2(5)
    height:  (rowsExpanded+1) * (scaleSize2(37) + scaleSize2(15)) + scaleSize2(10)
    // width: parent.width - collapsPanel.width - scaleSize2(10)

    anchors {
        left: collapsPanel.right; leftMargin: scaleSize2(10)
        right: parent.right
    }

    clip: true

    MouseArea {
        id: maRoot
        anchors.fill: parent; anchors.leftMargin: -scaleSize2(10)
        anchors.bottomMargin: scaleSize2(5) + 1
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property bool allow2clear: false

        onClicked: {
            if(groupTrackLight.length === 1 & !holdCtrl & !holdShift & allow2clear)
            {
                groupTrackLight.length = 0
                offLightAllTracks()
            }
        }

        onPressed: {
            parent.focus = true
            root.focus = true

            if(mouse.button === Qt.RightButton)
                trackLighting_rightclick(ind, 'btn')
            else
                trackLighting(ind, 'btn')

            if (mouse.button === Qt.RightButton) {
                positionInWindow = mapToItem(track_list.contentItem, mouse.x, mouse.y)
                trackRightClick.numberItem = ind
                trackRightClick.visble_grouper = allow2MakeGroup()
                trackRightClick.visble_ungrouper = groupTrackLight.length === 1 & trackAudioModel.get(groupTrackLight[0]).type === 'Group'
                trackRightClick.visible = true

                if(rightPanel.visible)
                    rightPanel.visible = false
            }
            else
                trackRightClick.visible = false
        }
    }

    // ------------------------------------------------------------------------------------------ item 1
    TrackButton {
        id:             select_btn
        anchors {
            top:        parent.top
            topMargin:  scaleSize2(10)
        }

        height:         scaleSize2(37)
        width:          scaleSize2(106)

        name:       numberAudio +1
        hasRightPanel: true

        isClicked: true

        fontColor:
        {
            if(!solo_on_audio && selectTrackDependSolo) theme.f4_
            else if(solo_on_audio && !selectTrackDependSolo) theme.f2_
            else if(solo_on_audio && selectTrackDependSolo) theme.f7_
            else if(!solo_on_audio && !selectTrackDependSolo) theme.f1_
        }

        fillStyle:
        {
            if(!solo_on_audio && selectTrackDependSolo) theme.b11_Enable1_FIL
            else if(solo_on_audio && !selectTrackDependSolo) theme.b10_Disable_FIL
            else if(solo_on_audio && selectTrackDependSolo) theme.b20_Disable_FIL
            else if(!solo_on_audio && !selectTrackDependSolo) theme.b10_Disable_FIL
        }
        brColor: selectTrack ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD

        onSendMessage: if(msg === 'released') {selectTrackDependSolo = !selectTrackDependSolo; console.log("selectTrackDependSolo: ", selectTrackDependSolo)
                       console.log('solo_on_audio:', solo_on_audio, ', selectTrack:', selectTrack)
                       }
    }

    // ------------------------------------------------------------------------------------------ item 2
    TrackButton {
        id:             solo_btn
        anchors {
            top:        select_btn.top
        }
        imgVisible: partialySolo
        height: scaleSize2(37)
        width:  scaleSize2(106)
        name:   'S'
        isSoloBtn:  true
        fontColor:  selectSolo/*isClicked*/ ? theme.f4_ : theme.f2_

        fillStyle:  selectSolo/*isClicked*/ ? theme.b12_Enable2_FIL : theme.b20_Disable_FIL
        brColor:    selectSolo/*isClicked*/ ? theme.b12_Enable2_BRD : theme.b20_Disable_BRD

        onSendMessage: if(msg === "released") trackAudioSelection(ind, solo_btn)
    }

    Rectangle {
        id:                 name//-----------------------------------------------------------------------
        anchors {
            top:            solo_btn.top
        }
        z:                  maRoot.z -1
        height:             scaleSize2(37)
        color:              'transparent'

        clip: true
        TextInputElide {
            id:         textLabel
            fullText:   root.name
            anchors.fill: parent

            didMarker: editName
            onDidMarkerChanged: focus = didMarker

            onFocusChanged: name.z = focus ? maRoot.z +1 : maRoot.z -1
        }
    }

    // ------------------------------------------------------------------------------------------ item 3
    Rectangle {
        id:             tf_10
        anchors {
            top:        select_btn.bottom
            topMargin:  scaleSize2(15)
        }

        height: scaleSize2(37)
        width: scaleSize2(106)

        color: 'transparent'

        TrackTextField2 {
            id: tf2
            anchors.fill: parent
            bigHeight: true

            enableRightClick: true
            shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
//            shortcutKeyTxt:     ['', '', '']
//            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
//            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
//            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']

            flColor: theme.s11_Enable_FIL
            brColor: theme.s11_Enable_BRD

            max: 6
            min: -70

            decimal: 1

            hasSlider: true
        }}

    // ------------------------------------------------------------------------------------------ item 4
    Rectangle {
        id:                 tf_11
        anchors {
            top:            tf_10.top
        }

        height: scaleSize2(37)
        width: scaleSize2(106)

        color:                  'transparent'
        radius:                 topRadius

        RowLayout {
            id: rowFirst
            visible: splitStereo
            anchors.fill: parent

            TrackTextField {
                Layout.fillHeight: true
                Layout.preferredWidth: Math.round(parent.width *0.45)
                Layout.alignment: Qt.AlignLeft
                bigHeight: true

                enableRightClick: true
                shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
    //            shortcutKeyTxt:     ['', '', '']
    //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
    //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
    //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
                splitStr: 'Stereo Pan Mode'
                is2Groups: true

                flColor: theme.s11_Enable_FIL
                brColor: theme.s11_Enable_BRD
                min: -50
                max: 50
                defaultValue: 0
                has2Slider: true
                decimal: 0
                text: value
            }

            TrackTextField {
                Layout.fillHeight: true
                Layout.preferredWidth: Math.round(parent.width *0.45)
                Layout.alignment: Qt.AlignLeft
                bigHeight: true

                enableRightClick: true
                shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
                moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
    //            shortcutKeyTxt:     ['', '', '']
    //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
    //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
    //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
                splitStr: 'Stereo Pan Mode'
                is2Groups: true

                flColor: theme.s11_Enable_FIL
                brColor: theme.s11_Enable_BRD
                min: -50
                max: 50
                defaultValue: 0
                has2Slider: true
                decimal: 0
                text: value
            }
        }

        TrackTextField {
            id: ttf
            visible: !splitStereo
            anchors.fill: parent
            bigHeight: true

            enableRightClick: true
            shortcutKeyTxt:     ['', '', '', 'Del', 'Ctrl + Del']
            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
//            shortcutKeyTxt:     ['', '', '']
//            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
//            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
//            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
            splitStr: 'Split Stereo Pan'
            is2Groups: true

            flColor: theme.s11_Enable_FIL
            brColor: theme.s11_Enable_BRD
            min: -50
            max: 50
            defaultValue: 0
            has2Slider: true
            decimal: 0
            text: value
            onValueChanged: {inTrack = true; inAutomationMenu = false; inAutomationChild = false ;tf3 = value.toFixed(0)}
        }}

    Rectangle {
        id:                 record_send_btn // ------------------------------------------------------------------- 1
        anchors {
            top:            tf_11.top
            bottom:         tf_11.bottom
        }
        width:          scaleSize2(106)
        gradient:       _3DGradient
        radius:         topRadius

        Rectangle {
            id:         betweenLine_send_btn
            visible:    recordBtn.isClicked || sendBtn.isClicked
            enabled:    recordBtn.isClicked || sendBtn.isClicked
            x:          recordBtn.x + recordBtn.width
            height:     parent.height
            width:      topWidth
            z:          1
            color:      theme.s11_Enable_FIL
        }

        RowLayout {
            id: rr
            anchors.fill: parent
            spacing: topWidth

            TrackSubButton {
                id:                     recordBtn
                Layout.fillHeight:      true
                Layout.fillWidth:       true

                onLeft:                 true

                flColor: light ? (isClicked ? '#DE3C47' : 'transparent') : isClicked ? '#DE3C47' : theme.s11_Enable_FIL

//                onIsClickedChanged: focus = true

                Image {
                    id:                     recordImg
                    anchors.centerIn:       parent
                    sourceSize:             {
                        if(recordBtn.height %2 !== 0)
                            Qt.size(oddSize(parent.width*.3), oddSize(parent.width*.3))
                        else
                            Qt.size(evenSize(parent.width*.3), evenSize(parent.width*.3))
                    }
                    source:                 'qrc:/Resource/rec_icon.svg'
                    visible:                false
                    mipmap: true
                }

                ColorOverlay {
                    id:                     colorImageRecord
                    cached:                 true
                    anchors.fill:           recordImg

                    source:                 recordImg
                    //                    clip:                   true
                    //                    antialiasing:           true
                    color:                  recordBtn.isClicked | light  ? theme.a1 : theme.a2
                }
                //                MouseArea {
                //                    anchors.fill: parent
                //                    onClicked: console.log('image: ' + recordImg.height + ', btn: '+ recordBtn.height + ', img w: ' +
                //                                           recordImg.width + ', btn w: '+ recordBtn.width)
                //                }
                onSendMessage: trackLighting(ind, select_btn)
            } // recordBtn.

            TrackSubButton {
                id:                     sendBtn
                Layout.fillHeight:      true
                Layout.fillWidth:       true
                flColor: light  ? (isClicked ? '#13C44B' : 'transparent') : isClicked ? '#13C44B' : theme.s11_Enable_FIL

                Image {
                    id:                     sendImg
                    anchors.centerIn:       parent
                    sourceSize:             {
                        if(sendBtn.height %2 !== 0)
                            Qt.size(oddSize(parent.width*.6), oddSize(parent.width*.6))
                        else
                            Qt.size(evenSize(parent.width*.6), evenSize(parent.width*.6))
                    }
                    source:                 'qrc:/Resource/send_icon.svg'
                    visible:                false
                }
                ColorOverlay {
                    id:                     colorImageSend
                    cached:                 true
                    anchors.fill:           sendImg

                    source:                 sendImg
                    clip:                   true
                    antialiasing:           true
                    color:                  sendBtn.isClicked | light  ? theme.a1 : theme.a2
                }
                //                MouseArea {
                //                    anchors.fill: parent
                //                    onClicked: console.log('image: ' + sendImg.height + ', btn: '+ sendBtn.height)
                //                }
                onSendMessage: trackLighting(ind, select_btn)
            } // sendBtn.
        }

        Rectangle{
            anchors {
                fill:       parent
                topMargin:  -1
                bottomMargin: -1
            }
            color:              'transparent'
            border.color:       theme.b10_Disable_BRD
            border.width:       topWidth
            radius:             topRadius
        }
    }


    Rectangle {
        id:                 auto_view_btn //---------------------------------------------------------------------- 2
        anchors {
            top:            record_send_btn.top
            bottom:         record_send_btn.bottom
        }

        width:        scaleSize2(106)

        gradient:     _3DGradient
        radius:       topRadius

        Rectangle {
            id:         betweenLine_auto_view
            visible:    automationBtn.isClicked || viewBtn.isClicked
            enabled:    automationBtn.isClicked || viewBtn.isClicked
            x:          automationBtn.x + automationBtn.width
            height:     parent.height
            width:      topWidth
            z:          1
            color:      theme.s11_Enable_FIL
        }

        RowLayout {
            anchors.fill: parent
            spacing: topWidth

            TrackSubButton {
                id:                     automationBtn
                Layout.fillHeight:      true
                Layout.fillWidth:       true

                flColor: light  ? (isClicked ? '#EFA441' : 'transparent') : isClicked ? '#EFA441' : theme.s11_Enable_FIL


                onIsClickedChanged: {
                    if(isClicked & rowsExpanded < 2)
                    {
                        changeRowCounter((rowsExpanded >= _rowsExpanded ? rowsExpanded + 2 : _rowsExpanded +1) )
                        showAutomation = true
                    }
                    else if(!isClicked & rowsExpanded > 1)
                    {
                        changeRowCounter(2)
                        showAutomation = false
                    }
                }

                onLeft:                 true

                Image {
                    id:                     automationImg
                    anchors.centerIn:       parent
//                    anchors.verticalCenterOffset: parent.height % 2 === 0 ? parent.height/2 > height ? 1 : -1 : 0
                    sourceSize:             {
                        if(automationBtn.height %2 !== 0)
                            Qt.size(oddSize(parent.width*.6), oddSize(parent.width*.6))
                        else
                            Qt.size(evenSize(parent.width*.6), evenSize(parent.width*.6))
                    }
                    source:                 'qrc:/Resource/automation_icon.svg'
                    visible:                false
                }
                ColorOverlay {
                    id:                     colorImageAutomation
                    cached:                 true
                    anchors.fill:           automationImg
                    source:                 automationImg
                    //                    clip:                   true
                    //                    antialiasing:           true
                    color:                  automationBtn.isClicked | light  ? theme.a1 : theme.a2
                }
                onSendMessage: trackLighting(ind, select_btn)
            } // automationBtn.

            TrackSubButton {
                id:                     viewBtn
                Layout.fillHeight:      true
                Layout.fillWidth:       true

                flColor: {
                    if(isClicked) '#8E7EE0'
                    else if (light ) 'transparent'
                    else theme.s11_Enable_FIL
                }

                Image {
                    id:                     viewImg
                    anchors.centerIn:       parent
//                    anchors.verticalCenterOffset: parent.height % 2 === 0 ? parent.height/2 > height ? 1 : -1 : 0
                    sourceSize:             {
                        if(viewBtn.height %2 !== 0)
                            Qt.size(oddSize(parent.width*.8), oddSize(parent.width*.8))
                        else
                            Qt.size(evenSize(parent.width*.8), evenSize(parent.width*.8))
                    }

                    source:                 'qrc:/Resource/object_icon.svg'
                    visible:                false
                }
                ColorOverlay {
                    id:                     colorImageView
                    cached:                 true
                    anchors.fill:           viewImg

                    source:                 viewImg
                    color:                  viewBtn.isClicked | light  ? theme.a1 : theme.a2
                }
                onSendMessage: trackLighting(ind, select_btn)
            } // viewBtn.
        }

        Rectangle {
            anchors {
                fill:       parent
                topMargin:  -1
                bottomMargin: -1
            }
            color:              'transparent'
            border.color:       theme.b10_Disable_BRD
            border.width:       topWidth//Math.min(topWidth *2, 2)
            radius:             topRadius
        }
    }

    // --------------------------------------------------------------------------------------------------------------------- automation Condition
    Automation {
        id:             automationCondition
        anchors {
            top:        auto_view_btn.bottom
            topMargin:  scaleSize2(15)
            left: parent.left
            // left:       collapsPanel.right
            // leftMargin: scaleSize2(10)
        }

        visible: automationBtn.isClicked

        height: scaleSize2(37)
        width:  scaleSize2(106)*4 + 3* scaleSize2(15)
    }

    Rectangle {
        id:             volumeMeter //----------------------------------------------------------------------------------------
        width:          channels * scaleSize2(8) + (topWidth*2)*2 + (channels-1)* (topWidth)
        // height:         parent.height
        anchors {
            top: parent.top
            bottom: parent.bottom
            bottomMargin:  scaleSize2(5)
            right: parent.right
        }

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
    }// rightVolume.

    /*Rectangle {//visible: false
        anchors.bottom: parent.bottom
        width:  parent.width
        height: scaleSize2(5)
        color:  '#2f3032'
        Rectangle {
            visible:        inDraggingCounter > -1
            anchors.fill:   parent
            anchors.leftMargin: type === "Group" ? (in_set_parent.count - inDraggingCounter+1) * scaleSize2(50) : (in_set_parent.count - inDraggingCounter) * scaleSize2(50)
            color:          inDraggingCounter > -1 ? theme.betweenRec_dragging_o20 : 'transparent'
        }
    }*/

    }// root main track



    Rectangle {
        id: seperator
        y: (rowsExpanded + 1) * (scaleSize2(37) + scaleSize2(15)) + scaleSize2(10) - height
        width:  automationChildren ? rootMainTrack.width + scaleSize2(10) : parent.width
        height: scaleSize2(5)
        color:  '#2f3032'

        // onYChanged: console.log('y: ', y)

        anchors.left: automationChildren & showAutomation ? rootMainTrack.left : parent.left
        anchors.leftMargin: automationChildren & showAutomation ? -scaleSize2(10) : 0

        MouseArea {
            anchors.fill: parent
            drag.axis: Drag.YAxis
            drag.minimumY: scaleSize2(37) + scaleSize2(10) + scaleSize2(15)
            drag.target: parent

            hoverEnabled: true

            onEntered: cursorPositionClass.vCursorShape()
            onExited: if(!pressed) cursorPositionClass.onlyShowCursor()
            onPressed: cursorPositionClass.vCursorShape()
            onReleased: cursorPositionClass.onlyShowCursor()

            onMouseYChanged: {
                if(pressed) {
                    parent.y = Math.round((parent.y - scaleSize2(10)) / (scaleSize2(37) + scaleSize2(15))) * (scaleSize2(37) + scaleSize2(15)) +
                              scaleSize2(10) - height

                    var _i = Math.round((seperator.y - scaleSize2(10)) / (scaleSize2(37) + scaleSize2(15))) - 1
                    _rowsExpanded = _i
                    if(_i !== rowsExpanded)
                        rowFunc(_i)
                }
            }
        }


        Rectangle {
            visible:        inDraggingCounter > -1 & automationChildren < 1
            anchors.fill:   parent
            anchors.leftMargin: type === "Group" ? (in_set_parent.count - inDraggingCounter+1) * scaleSize2(50) : (in_set_parent.count - inDraggingCounter) * scaleSize2(50)
            color:          inDraggingCounter > -1 ? theme.betweenRec_dragging_o20 : 'transparent'
        }
    }


    MouseArea {
        anchors.fill: automationListView; anchors.topMargin: -scaleSize2(15)
        property int fromIndex: -1
        property int toIndex: -1

        onPressed: {
            if(automationChildren > 1) cursorPosition.dragCursorShape()

            if(pressed & automationChildren > 1 & fromIndex === -1)
                fromIndex = automationListView.indexAt(10, mouse.y + automationListView.contentY)
        }

        onMouseYChanged:
        {
            var indexDrop = automationListView.indexAt(10, mouse.y + automationListView.contentY)
            if(pressed & automationChildren > 1 & fromIndex !== -1 & indexDrop !== fromIndex & indexDrop !== -1)
            {
                automationListView.automationIndexDrop = indexDrop
            }
            else
                automationListView.automationIndexDrop = -1
        }

        onReleased: {
            cursorPosition.onlyShowCursor()
            toIndex = automationListView.indexAt(10, mouse.y + automationListView.contentY)

            if(automationChildren > 1 & fromIndex > -1 & toIndex > -1 & fromIndex !== toIndex)
            {
                trackAudioModel.get(ind).automationList.move(fromIndex, toIndex, 1)
            }

            fromIndex = -1
            toIndex = -1
            automationListView.automationIndexDrop = -1
        }
    }

    ListView {
        id: automationListView
        anchors {
            top: seperator.bottom; topMargin: scaleSize2(15)
            left: rootMainTrack.left
        }

        property int automationIndexDrop: -1

        onFocusChanged: console.log('hihi', focus)
        width: parent.width
        height: contentItem.childrenRect.height

        flickableDirection: Flickable.VerticalFlick
        interactive: false
        boundsMovement: Flickable.StopAtBounds

        spacing: scaleSize2(15)

        model: trackAudioModel.get(ind).automationList
        delegate: AutomationChild { }
    }

}


