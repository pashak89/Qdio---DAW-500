import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2

Rectangle {
    id: root
    width: parentWidth
    height: parentHeight + margin * 2

    radius: topRadius

    color: bgColr

    signal renameCall(string msg, int compId)
    signal messageCall(string msg, int compId)
    signal addGroupTrack
    signal action(string text)

    property color lineClr: Qt.darker('#FFFFFF', 1.5)

    property color bgColr: '#2F3032'

    property Item panel: null

    property int numberItem: -1

    property bool isLine: false

    property bool twoGroups: false

    property string firstGroup: ''
    property string rightFirstGroup: ''

    property string newMember: ''

    property int parentHeight: columnMain.height

    property variant parentOfGroup_pretext: []

    property bool isVisible_groupMaker: true
    property bool isVisible_ungrouper: true

    property int parentWidth: 11 * cellHeight + 2 * scaleSize2(
                                  15) + 10 * bigSpace

    property int margin: topRadius + Math.round(4 * topRadius * 1 / 5)

    property int cellHeight: heightSize

    property int smallSpace: 2

    property int bigSpace: scaleSize2(7)

    property int widthSize: testSize.contentWidth + testSize2.contentWidth
    property int heightSize: testSize.contentHeight

    property ListModel copy_inset
    property ListModel copy_subset

    // sub_set_children به والدها در قسمت (numberItem) اضافه کردن فرزند یعنی
    function set_subSet_on_parent() {
        for (var i = 0; i < trackAudioModel.get(
                 numberItem).in_set_parent.count; i++)
            for (var j = 0; j < trackAudioModel.count; j++)
                // find pretext inset of this child in parent
                // focus on child
                if (/*child*/ trackAudioModel.get(numberItem).in_set_parent.get(
                            i).member === trackAudioModel.get(j).pretext) {
                    trackAudioModel.get(j).sub_set_children.append({
                                                                       "member": newMember
                                                                   })
                    // focus on parent

                    /*for(var k = 0; k < trackAudioModel.get(j).sub_set_children.count; k++)
                    {
                        // find index of parent
                        if(trackAudioModel.get(j).sub_set_children.get(k).member === trackAudioModel.get(numberItem).pretext)
                        {
                            // add new group into subset of parent
                            trackAudioModel.get(j).sub_set_children.insert(k, {member:newMember})
                            return
                        }
                    }*/
                }
    }

    // in_set_parent به فرزندها در قسمت (numberItem) اضافه کردن والد یعنی
    function set_inSet_on_child() {
        for (var i = 0; i < trackAudioModel.get(
                 numberItem).sub_set_children.count; i++)
            for (var j = 0; j < trackAudioModel.count; j++)
                // find pretext subset of this parent in child
                // focus on parent
                if (/*parent*/ trackAudioModel.get(
                            numberItem).sub_set_children.get(
                            i).member === trackAudioModel.get(j).pretext) {
                    trackAudioModel.get(j).in_set_parent.append({
                                                                    "member": newMember
                                                                })
                    // focus on child

                    /*for(var k = 0; k < trackAudioModel.get(j).in_set_parent.count; k++) //child
                {
                    // find index of child
                    if(trackAudioModel.get(j).in_set_parent.get(k).member === trackAudioModel.get(numberItem).pretext)
                    {
                        // add new group into inset of child
                        trackAudioModel.get(j).in_set_parent.insert(k+1, {member:newMember})
                        return
                    }
                }*/
                }
    }

    ColumnLayout {
        id: columnMain
        y: margin
        spacing: bigSpace
        antialiasing: true

        ColumnLayout {
            spacing: smallSpace
            antialiasing: true

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Cut'
                rightTitle: 'Ctrl+X'
                onTriggered: {

                }
            } // --------------------------------------------------------

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Copy'
                rightTitle: 'Ctrl+C'
                onTriggered: {

                }
            } // --------------------------------------------------------

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Paste'
                rightTitle: 'Ctrl+V'
                onTriggered: {

                }
            } // --------------------------------------------------------

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Delete'
                rightTitle: 'Del'
                onTriggered: {
                    action('Delete')
                }
            } // --------------------------------------------------------

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Duplicate'
                rightTitle: 'Ctrl+D'
                onTriggered: {

                }
            } // --------------------------------------------------------

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Rename'
                rightTitle: 'Ctrl+R'

                onTriggered: {
                    trackRightClick.close()
                    renameCall('rename', numberItem)
                    trackAudioModel.setProperty(numberItem, 'editName', true)
                }
            } // --------------------------------------------------------

            // -----------------------------------------------------------------1
            Rectangle {
                id: splitter
                Layout.preferredHeight: 1
                Layout.preferredWidth: parentWidth
                color: lineClr
                antialiasing: true
            }

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Freeze'
                rightTitle: 'Ctrl+F'
                onTriggered: {

                }
            }

            // -----------------------------------------------------------------2
            Rectangle {
                id: splitter2
                Layout.preferredHeight: 1
                Layout.preferredWidth: parentWidth
                color: lineClr
            }

            ShortcutRow {
                visible: isVisible_groupMaker
                id: groupMaker
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Group Tracks'
                rightTitle: 'Ctrl+G'
                onTriggered: {
                    if (trackAudioModel.get(
                                numberItem).in_set_parent.count >= 5) {
                        trackRightClick.close()
                        return
                    }
                    // یک عضو تک

                    /*for(var i = 0; i < groupTrackLight.length; i++)
                {
                    for(var j = 0; j < trackAudioModel.get(groupTrackLight[i]).in_set_parent.count; j++)
                    {
                        for(var z = 0; z < trackAudioModel.count; z++)
                        {
                            if(trackAudioModel.get(groupTrackLight[i]).in_set_parent.get(j).member === trackAudioModel.get(z).pretext &
                                    trackAudioModel.get(z).type === 'Group' &
                                    groupTrackLight.indexOf(z) < 0 & // not contains
                                    trackAudioModel.get(z).sub_set_children.count - trackAudioModel.get(groupTrackLight[i]).sub_set_children.count === 1)
                            {
                                return;
                            }
                            else if(trackAudioModel.get(groupTrackLight[i]).in_set_parent.get(j).member === trackAudioModel.get(z).pretext &
                                    trackAudioModel.get(z).type === 'Group' &
                                    trackAudioModel.get(groupTrackLight[i]).type === 'Group' &
                                    groupTrackLight.indexOf(z) < 0 & // not contains
                                    trackAudioModel.get(z).sub_set_children.count - 1 === trackAudioModel.get(groupTrackLight[i]).sub_set_children.count)
                            {console.log('return 2-2')
                                halfUp = false
                                halfDown = false
                                return;
                            }
                        }
                    }
                }*/
                    // R E T U R N.
                    console.log('0-groupTrackLight', groupTrackLight,
                                groupTrackLight.length)
                    newMember = 'group ' + (groupTrackArray.length + 1)
                    groupTrackArray.push(newMember)
                    if (groupTrackLight.length === 1) {
                        // actions for other:
                        // /parent
                        if (trackAudioModel.get(
                                    numberItem).in_set_parent.count) {
                            set_subSet_on_parent()
                        }
                        // /child
                        if (trackAudioModel.get(
                                    numberItem).sub_set_children.count) {
                            set_inSet_on_child()
                        }

                        // other.

                        //                    copy_inset = trackAudioModel.get(numberItem).in_set_parent
                        //                    copy_subset = trackAudioModel.get(numberItem).sub_set_children
                        //                    copy_subset.insert(0, {member:trackAudioModel.get(numberItem).pretext})
                        trackAudioModel.get(numberItem).in_set_parent.append({
                                                                                 "member": newMember
                                                                             })

                        trackAudioModel.insert(numberItem, {
                                                   "isCollapsed": true,
                                                   "type": 'Group',
                                                   "pretext": newMember,
                                                   "in_set_parent": [],
                                                   "sub_set_children": [],
                                                   "selectTrack": true,
                                                   "selectSolo": false,
                                                   "partialySolo": false,
                                                   "solo_on_audio": false,
                                                   "selectTrackDependSolo": true,
                                                   "light": false,
                                                   "editName": false,
                                                   "inDraggingCounter": -1,
                                                   "coverBound": false,
                                                   "channel_count": 8,
                                                   "rowsExpanded": 1,
                                                   "imgURL": 'qrc:/Resource/folder_off_icon.svg',
                                                   "automationTurn"// Automation
                                                   : false,
                                                   "automationChildren": 0,
                                                   "volumeAutomated": false,
                                                   "panAutomated": false,
                                                   "speakerOnAutomated": false,
                                                   "automationList": []
                                               })
                        // copy subset if group
                        for (var i = 0; i < trackAudioModel.get(
                                 numberItem + 1).sub_set_children.count; i++) {
                            trackAudioModel.get(
                                        numberItem).sub_set_children.insert(i, {
                                                                                "member": trackAudioModel.get(numberItem + 1).sub_set_children.get(i).member
                                                                            })
                        }
                        trackAudioModel.get(
                                    numberItem).sub_set_children.insert(0, {
                                                                            "member": trackAudioModel.get(numberItem + 1).pretext
                                                                        })
                        // copy inset
                        for (i = 0; i < trackAudioModel.get(
                                 numberItem + 1).in_set_parent.count - 1; i++) {
                            trackAudioModel.get(
                                        numberItem).in_set_parent.insert(i, {
                                                                             "member": trackAudioModel.get(numberItem + 1).in_set_parent.get(i).member
                                                                         })
                        }
                        console.log('01-groupTrackLight', groupTrackLight,
                                    groupTrackLight.length)
                    } // single track lighted.

                    var vResult = 0
                    for (i = 0; i < trackAudioModel.count; i++) {
                        if (trackAudioModel.get(
                                    i).type === 'Group' & trackAudioModel.get(
                                    i).in_set_parent.count > vResult)
                            vResult = trackAudioModel.get(i).in_set_parent.count
                    }
                    console.log('02-groupTrackLight', groupTrackLight,
                                groupTrackLight.length)
                    group_width = scaleSize2(50) * (vResult + 1)
                    console.log('1-groupTrackLight', groupTrackLight,
                                groupTrackLight.length)
                    if (groupTrackLight.length === 1) {
                        trackRightClick.close()
                        addGroupTrack()
                        //                    funcs.offLightAllTracks()
                        funcs.trackLighting(numberItem, 'btn')
                        return
                    }
                    console.log('----------------------------------------------------- M U L T I',
                                groupTrackLight, groupTrackLight.length)

                    /// multi track highlighted
                    // Before Movement:
                    // before created
                    //                    for(i = 0; i < groupTrackLight.length; i++)
                    //                        trackAudioModel.get(groupTrackLight[i]).in_set_parent.append({member:newMember})
                    var group_focus = groupTrackLight.slice(0, 1)
                    trackAudioModel.insert(group_focus, {
                                               "isCollapsed": true,
                                               "type": 'Group',
                                               "pretext": newMember,
                                               "in_set_parent": [],
                                               "sub_set_children": [],
                                               "selectTrack": true,
                                               "selectSolo": false,
                                               "partialySolo": false,
                                               "solo_on_audio": false,
                                               "selectTrackDependSolo": true,
                                               "light": false,
                                               "editName": false,
                                               "inDraggingCounter": -1,
                                               "coverBound": false,
                                               "channel_count": 8,
                                               "rowsExpanded": 1,
                                               "imgURL": 'qrc:/Resource/folder_off_icon.svg',
                                               "automationTurn": false,
                                               "automationChildren": 0,
                                               "volumeAutomated": false,
                                               "panAutomated": false,
                                               "speakerOnAutomated": false,
                                               "automationList": []
                                           })
                    // rearray groupTrackLight
                    for (i = 0; i < groupTrackLight.length; i++)
                        groupTrackLight[i] = groupTrackLight[i] + 1

                    // copy subset in this Group

                    /*for(var j = 0; j < groupTrackLight.length; j++)
                        for(i = 0; i <trackAudioModel.get(groupTrackLight[j]).sub_set_children.count; i++)
                        {
                            trackAudioModel.get(group_focus).sub_set_children.
                            append({member:trackAudioModel.get(groupTrackLight[j]).sub_set_children.get(i).member})
                        }
                    for(i = 0; i < groupTrackLight.length; i++)
                        trackAudioModel.get(group_focus).sub_set_children.insert(0, {member:trackAudioModel.get(groupTrackLight[i]).pretext})*/

                    // copy inset of first
                    for (i = 0; i < trackAudioModel.get(
                             groupTrackLight[0]).in_set_parent.count; i++) {
                        trackAudioModel.get(
                                    group_focus).in_set_parent.insert(i, {
                                                                          "member": trackAudioModel.get(groupTrackLight[0]).in_set_parent.get(i).member
                                                                      })
                    }
                    // Before Movement.
                    var pure_audio_or_group = groupTrackLight.slice()
                    var repeatMember = []
                    for (i = 0; i < groupTrackLight.length; i++) {
                        if (trackAudioModel.get(
                                    groupTrackLight[i]).type === 'Group') {
                            // var subset_members = groupTrackLight[i] // index of Group
                            for (var j = 0; j < trackAudioModel.get(
                                     groupTrackLight[i]).sub_set_children.count; j++) {
                                var child_exist_index = pure_audio_or_group.indexOf(
                                            groupTrackLight[i] + j + 1)
                                if (child_exist_index > -1) {
                                    //console.log("0-groupTrackLight:", groupTrackLight)
                                    repeatMember.push(child_exist_index)
                                    pure_audio_or_group.splice(
                                                child_exist_index, 1)
                                    //console.log("1-groupTrackLight:", groupTrackLight)
                                }
                            }
                        } // if Group.
                    } // pure_audio_or_group.

                    // TODO Clean code
                    var new_groupTrackLight = groupTrackLight.slice()
                    console.log("2-groupTrackLight:", groupTrackLight)
                    groupTrackLight = pure_audio_or_group.slice()
                    // console.log("new_groupTrackLight:", new_groupTrackLight)
                    var preTextArray_groupLight = []
                    for (i = 0; i < new_groupTrackLight.length; i++)
                        preTextArray_groupLight.push(
                                    trackAudioModel.get(
                                        new_groupTrackLight[i]).pretext)

                    // [] Movement:
                    // first array of group light is ok.
                    var ii = 0
                    var startMove = 0

                    /*if(trackAudioModel.get(groupTrackLight[0]).type === 'audio' )
                    {
                    for(i = 0+1; i < groupTrackLight.length; i++)
                    {
                        if(trackAudioModel.get(groupTrackLight[i]).type === 'audio' )
                        {
//                            if(groupTrackLight[i] !== groupTrackLight[0]+i+ii)
                            trackAudioModel.move(groupTrackLight[i], groupTrackLight[0]+ii +1, 1)
                            ii++

                        } else if(trackAudioModel.get(groupTrackLight[i]).type === 'Group' )
                        {
                            trackAudioModel.move(groupTrackLight[i], groupTrackLight[0]+ii +1, 1)
                            for(j = 0; j < trackAudioModel.get(groupTrackLight[i]).sub_set_children.count; j++)
                            {
                                ii++
                                trackAudioModel.move(groupTrackLight[i]+1+j, groupTrackLight[0]+ii +1, 1)
                            }
                        }
                    }
                    } else if(trackAudioModel.get(groupTrackLight[0]).type === 'Group' )
                    {*/
                    for (i = 0 + 1; i < groupTrackLight.length; i++) {
                        startMove += trackAudioModel.get(
                                    groupTrackLight[0] + startMove).sub_set_children.count
                                + 1 //(next place)

                        trackAudioModel.move(
                                    groupTrackLight[i],
                                    groupTrackLight[0] + startMove,
                                    trackAudioModel.get(
                                        groupTrackLight[i]).sub_set_children.count + 1)
                    }
                    //                    }

                    // Movement.

                    // [] Rearray groupTrackLight
                    groupTrackLight.length = 0

                    /*if(repeatMember.length === 0)
                    for(i = 1; i < groupTrackLight.length; i++)
                        groupTrackLight[i] = groupTrackLight[0]+i*/
                    // console.log("preTextArray_groupLight:", preTextArray_groupLight)
                    for (i = 0; i < preTextArray_groupLight.length; i++) {
                        for (j = 0; j < trackAudioModel.count; j++) {
                            if (preTextArray_groupLight[i] === trackAudioModel.get(
                                        j).pretext) {
                                groupTrackLight.push(j)
                            }
                        }
                    }
                    //                    new_groupTrackLight.length = 0
                    pure_audio_or_group = groupTrackLight.slice()
                    repeatMember = []
                    for (i = 0; i < groupTrackLight.length; i++) {
                        if (trackAudioModel.get(
                                    groupTrackLight[i]).type === 'Group') {
                            // subset_members = groupTrackLight[i] // index of Group
                            for (j = 0; j < trackAudioModel.get(
                                     groupTrackLight[i]).sub_set_children.count; j++) {
                                child_exist_index = pure_audio_or_group.indexOf(
                                            groupTrackLight[i] + j + 1)

                                if (child_exist_index > -1) {
                                    repeatMember.push(child_exist_index)
                                    pure_audio_or_group.splice(
                                                child_exist_index, 1)
                                }
                            }
                        } // if Group.
                    } // pure_audio_or_group.

                    // New Arrays:
                    var clearing_insets_member_pretext = []
                    for (i = 0; i < pure_audio_or_group.length; i++) {
                        for (j = 0; j < trackAudioModel.get(
                                 pure_audio_or_group[i]).in_set_parent.count; j++)
                            clearing_insets_member_pretext.push(
                                        trackAudioModel.get(
                                            pure_audio_or_group[i]).in_set_parent.get(
                                            j).member)
                    }
                    console.log('clearing_insets_member_pretext:',
                                clearing_insets_member_pretext)
                    var all_movement_member_index = []
                    ii = 0
                    for (i = 0; i < pure_audio_or_group.length; i++) {
                        all_movement_member_index.push(pure_audio_or_group[i])

                        for (j = 1; j <= trackAudioModel.get(
                                 pure_audio_or_group[i]).sub_set_children.count; j++) {
                            all_movement_member_index.push(
                                        pure_audio_or_group[i] + j)
                        }
                    }

                    var all_movement_member_pretext = []
                    for (i = 0; i < all_movement_member_index.length; i++) {
                        all_movement_member_pretext.push(
                                    trackAudioModel.get(
                                        all_movement_member_index[i]).pretext)
                    }

                    // [] change detail of track model of group light:
                    // Removing :

                    /*for(i = 0; i < pure_audio_or_group.length; i++)
                    {
                        for(j = 0; j < trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.count; j++)
                        clearing_inset.push(trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.get(j))
                    }

                    // for itself from others
                    for(z = 0; z < pure_audio_or_group.length); z++)
                    for(i = 0; i < trackAudioModel.get(pure_audio_or_group[z]).in_set_parent.count; i++)
                    for(j = 0; j < trackAudioModel.count; j++)
                    {
                        // if(trackAudioModel.get(pure_audio_or_group[z]).in_set_parent.get(i).member === trackAudioModel.get(j).pretext) // & j !== group_focus
                        for(k = 0; k < trackAudioModel.get(j).sub_set_children.count; k++)
                        if(trackAudioModel.get(j).sub_set_children.get(k).member === trackAudioModel.get(pure_audio_or_group[z]).pretext)
                        trackAudioModel.get(j).sub_set_children.remove(k, 1)
                    }
                    // for childs: // remove from other tracks
                    for(var a = 0; a < pure_audio_or_group.length; a++)
                    for(i = 0; i < trackAudioModel.get(apure_audio_or_group[a]).sub_set_children.count; i++)
                    {
                        for(j = 0; j < trackAudioModel.count; j++)
                        for(k = 0; k < trackAudioModel.get(j).sub_set_children.count; k++)
                        if(trackAudioModel.get(j).sub_set_children.get(k).member === trackAudioModel.get(pure_audio_or_group[a]).sub_set_children.get(i).member & j < pure_audio_or_group[a])
                        trackAudioModel.get(j).sub_set_children.remove(k, 1)
                    }

                    // remove from itselvs
                    for(var a = 0; a < pure_audio_or_group.length; a++)
                    trackAudioModel.get(j).in_set_parent.clear()

                    // remove from children
                    for(var a = 0; a < pure_audio_or_group.length; a++)
                    if(trackAudioModel.get(pure_audio_or_group[a]).type === "Group")
                    for(i = 0; i < trackAudioModel.get(pure_audio_or_group[a]).sub_set_children.count; i++){

                        trackAudioModel.get(pure_audio_or_group[a] + i +1).in_set_parent.clear()
                        trackAudioModel.get(pure_audio_or_group[a] + i +1).in_set_parent.append({member:trackAudioModel.get(pure_audio_or_group[a]).pretext})
                    }*/

                    // R E M O V I N G :
                    var removeArray = [] // from parent
                    for (i = 0; i < trackAudioModel.count; i++) {
                        for (j = 0; j < trackAudioModel.get(
                                 i).sub_set_children.count; j++) {
                            for (var z = 0; z < all_movement_member_index.length; z++) {
                                if (all_movement_member_pretext[z] === trackAudioModel.get(
                                            i).sub_set_children.get(
                                            j).member & all_movement_member_pretext.indexOf(
                                            trackAudioModel.get(
                                                i).pretext) < 0) {
                                    removeArray.push(j)
                                }
                            }
                        }
                        removeArray.sort(function (aa, bb) {
                            return aa - bb
                        })


                        /*for(b = 0; b < removeArray.length; b++)
                        {
                            trackAudioModel.get(all_movement_member_index[a]).in_set_parent.remove(removeArray[b], 1)
                            for(var j = 0; j < removeArray.length; j++)
                                removeArray[j] = removeArray[j]-1
                        }*/
                        for (j = 0; j < removeArray.length; j++) {
                            console.log('parent:',
                                        trackAudioModel.get(i).pretext,
                                        trackAudioModel.get(
                                            i).sub_set_children.count)
                            trackAudioModel.get(i).sub_set_children.remove(
                                        removeArray[j], 1)
                            for (var a = 0; a < removeArray.length; a++)
                                removeArray[a] = removeArray[a] - 1
                            console.log('parent:',
                                        trackAudioModel.get(i).pretext,
                                        trackAudioModel.get(
                                            i).sub_set_children.count)
                        }
                        removeArray.length = 0
                    }

                    removeArray.length = 0
                    for (j = 0; j < clearing_insets_member_pretext.length; j++)
                        for (i = 0; i < all_movement_member_index.length; i++) {
                            for (z = 0; z < trackAudioModel.get(
                                     all_movement_member_index[i]).in_set_parent.count; z++) {
                                if (clearing_insets_member_pretext[j] === trackAudioModel.get(
                                            all_movement_member_index[i]).in_set_parent.get(
                                            z).member)
                                    removeArray.push(z)
                            }

                            removeArray.sort(function (aa, bb) {
                                return aa - bb
                            })

                            for (var b = 0; b < removeArray.length; b++) {
                                console.log('child:',
                                            trackAudioModel.get(i).pretext,
                                            trackAudioModel.get(
                                                i).sub_set_children.count)
                                trackAudioModel.get(
                                            all_movement_member_index[a]).in_set_parent.remove(
                                            removeArray[b], 1)
                                for (var sj = 0; sj < removeArray.length; sj++)
                                    removeArray[sj] = removeArray[sj] - 1
                                console.log('child:',
                                            trackAudioModel.get(i).pretext,
                                            trackAudioModel.get(
                                                i).sub_set_children.count)
                            }
                            removeArray.length = 0
                        }
                    // R E M O V I N G.

                    // A D D _or_ C O P Y :
                    console.log('all_movement_member_index: ',
                                all_movement_member_index,
                                ', all_movement_member_pretext:',
                                all_movement_member_pretext)
                    for (i = 0; i < all_movement_member_index.length; i++) {
                        trackAudioModel.get(
                                    group_focus).sub_set_children.append({
                                                                             "member": all_movement_member_pretext[i]
                                                                         })
                    }
                    //console.log( "parentOfGroup_pretext:",parentOfGroup_pretext)
                    for (i = 0; i < all_movement_member_index.length; i++) {
                        trackAudioModel.get(
                                    all_movement_member_index[i]).in_set_parent.append({
                                                                                           "member": newMember
                                                                                       })
                    }

                    /*for(i = 0; i < all_movement_member_index.length; i++)
                    {
                        for(j = 0; j < trackAudioModel.get(group_focus).in_set_parent.count; j++)
                        {

                            trackAudioModel.get(all_movement_member_index[i]).in_set_parent.append(
                                        {member: trackAudioModel.get(group_focus).in_set_parent.get(j).member})
                        }
                    }*/
                    parentOfGroup_pretext.length = 0
                    for (i = 0; i < trackAudioModel.get(
                             group_focus).in_set_parent.count; i++)
                        parentOfGroup_pretext.push(
                                    trackAudioModel.get(
                                        group_focus).in_set_parent.get(
                                        i).member)
                    console.log('parentOfGroup_pretext:', parentOfGroup_pretext)
                    for (i = 0; i < trackAudioModel.count; i++) {
                        for (j = 0; j < parentOfGroup_pretext.length; j++) {
                            if (parentOfGroup_pretext[j] === trackAudioModel.get(
                                        i).pretext) {
                                trackAudioModel.get(i).sub_set_children.append({
                                                                                   "member": trackAudioModel.get(group_focus).pretext
                                                                               })
                                for (var k = 0; k < trackAudioModel.get(
                                         group_focus).sub_set_children.count; k++) {
                                    trackAudioModel.get(
                                                i).sub_set_children.append({
                                                                               "member": trackAudioModel.get(group_focus).sub_set_children.get(k).member
                                                                           })
                                }
                            }
                        }
                    }
                    // A D D _or_ C O P Y.

                    // check children and parent with first child of new group:
                    var _insets_pretext = []
                    var _members_pretext = []
                    var _childOfParent_pretext = []

                    for (i = 0; i < trackAudioModel.get(
                             group_focus[0] + 1).in_set_parent.count; i++) {
                        _insets_pretext.push(
                                    trackAudioModel.get(
                                        group_focus[0] + 1).in_set_parent.get(
                                        i).member)
                    }
                    for (i = 0; i < all_movement_member_index.length; i++) {
                        for (j = 0; j < trackAudioModel.get(
                                 all_movement_member_index[i]).in_set_parent.count; j++) {
                            _members_pretext.push(
                                        trackAudioModel.get(
                                            all_movement_member_index[i]).in_set_parent.get(
                                            j).member)
                        }

                        for (j = 0; j < _insets_pretext.length; j++) {
                            if (_members_pretext.indexOf(
                                        _insets_pretext[j]) < 0) {
                                trackAudioModel.get(
                                            all_movement_member_index[i]).in_set_parent.append({
                                                                                                   "member": _insets_pretext[j]
                                                                                               })
                            }
                        }
                        _members_pretext.length = 0

                        // check children of parents:
                        for (k = 0; k < trackAudioModel.count; k++) {
                            if (_insets_pretext.indexOf(trackAudioModel.get(
                                                            k).pretext) > 0) {
                                for (j = 0; j < trackAudioModel.get(
                                         k).sub_set_children.count; j++) {
                                    _childOfParent_pretext.push(
                                                trackAudioModel.get(
                                                    k).sub_set_children.get(
                                                    j).member)
                                }

                                for (j = 0; j < all_movement_member_pretext; j++) {
                                    if (_childOfParent_pretext.indexOf(
                                                all_movement_member_pretext[j]) < 0) {
                                        trackAudioModel.get(
                                                    k).sub_set_children.append({
                                                                                   "member": all_movement_member_pretext[j]
                                                                               })
                                    }
                                }
                            }
                            _childOfParent_pretext.length = 0
                        }
                        // check children of parents.
                    }
                    // check children and parent with first child of new group.

                    // modify insets of tracks if parent under the child otherwise track top of its parent:
                    var counterInset = 0
                    var indexInset = -1
                    var indexSubset = -1
                    for (i = 0; i < all_movement_member_index.length; i++) {
                        //remove:
                        removeArray.length = 0
                        for (var l = 0; l < trackAudioModel.get(
                                 all_movement_member_index[i]).in_set_parent.count
                             - counterInset * 0; l++) {
                            // find index of inset member
                            for (var h = 0; h < trackAudioModel.count; h++) {
                                if (trackAudioModel.get(
                                            h).pretext === trackAudioModel.get(
                                            all_movement_member_index[i]).in_set_parent.get(
                                            l).member) {
                                    indexInset = h
                                }
                            }

                            //for(var d = 0; d < trackAudioModel.get(all_movement_member_index[i]).in_set_parent.count; d++)
                            //  console.log('-+-+:',trackAudioModel.get(all_movement_member_index[i]).in_set_parent.get(d).member)
                            if (all_movement_member_index[i] < indexInset) {
                                //console.log('=++',trackAudioModel.get(all_movement_member_index[i]).pretext, indexInset, l, trackAudioModel.get(all_movement_member_index[i]).in_set_parent.count)
                                removeArray.push(l)
                                //                                counterInset++
                            }
                            indexInset = -1
                        }

                        for (l = removeArray.length - 1; l > -1; l--) {
                            trackAudioModel.get(
                                        all_movement_member_index[i]).in_set_parent.remove(
                                        removeArray[l], 1)
                        }
                        // remove.

                        // add:
                        var parents_pretext = []
                        for (l = 0; l < trackAudioModel.get(
                                 all_movement_member_index[i]).sub_set_children.count; l++) {
                            // find index of subset member
                            for (j = 0; j < trackAudioModel.count; j++) {
                                if (trackAudioModel.get(
                                            j).pretext === trackAudioModel.get(
                                            all_movement_member_index[i]).sub_set_children.get(
                                            l).member) {
                                    indexSubset = j
                                    for (h = 0; h < trackAudioModel.get(
                                             indexSubset).in_set_parent.count; h++) {
                                        parents_pretext.push(
                                                    trackAudioModel.get(
                                                        indexSubset).in_set_parent.get(
                                                        h).member)
                                    }

                                    if (parents_pretext.indexOf(
                                                trackAudioModel.get(
                                                    all_movement_member_index[i]).pretext) < 0)
                                        trackAudioModel.get(
                                                    indexSubset).in_set_parent.append({
                                                                                          "member": trackAudioModel.get(all_movement_member_index[i]).pretext
                                                                                      })

                                    parents_pretext.length = 0
                                }
                            }
                        }
                        // add.
                    }

                    // modify.
                    for (i = 0; i < trackAudioModel.count; i++) {
                        if (trackAudioModel.get(
                                    i).type === 'Group' & trackAudioModel.get(
                                    i).in_set_parent.count > vResult)
                            vResult = trackAudioModel.get(i).in_set_parent.count
                    }
                    group_width = scaleSize2(50) * (vResult + 1)
                    console.log('vResult:', vResult)

                    trackRightClick.close()
                    addGroupTrack()

                    //                funcs.offLightAllTracks()
                    funcs.trackLighting(group_focus[0], 'btn')
                } // triggered.
            }

            Rectangle {
                visible: isVisible_groupMaker
                id: splitter3
                Layout.preferredHeight: 1
                Layout.preferredWidth: parentWidth
                color: lineClr
            }

            // -----------------------------------------------------------------3
            ShortcutRow {
                visible: isVisible_ungrouper
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Ungroup Tracks'
                rightTitle: ''
                onTriggered: {
                    funcs.offLightAllTracks()

                    var removeParent = -1

                    for (var i = 1; i <= trackAudioModel.get(
                             numberItem).sub_set_children.count; i++) {
                        for (var j = 0; j < trackAudioModel.get(
                                 i + numberItem).in_set_parent.count; j++) {
                            if (trackAudioModel.get(
                                        i + numberItem).in_set_parent.get(
                                        j).member === trackAudioModel.get(
                                        numberItem).pretext)
                                removeParent = j
                        }

                        if (removeParent > -1)
                            trackAudioModel.get(
                                        i + numberItem).in_set_parent.remove(
                                        removeParent, 1)
                        removeParent = -1

                        trackAudioModel.setProperty(i + numberItem,
                                                    'isCollapsed', true)
                        if (trackAudioModel.get(
                                    i + numberItem).rowsExpanded < 1)
                            trackAudioModel.setProperty(i + numberItem,
                                                        'rowsExpanded', 1)
                    }

                    // from parents:
                    for (i = 0; i < trackAudioModel.get(
                             numberItem).in_set_parent.count; i++) {
                        for (j = 0; j < numberItem; j++) {
                            for (var k = 0; k < trackAudioModel.get(
                                     j).sub_set_children.count; k++) {
                                if (trackAudioModel.get(j).sub_set_children.get(
                                            k).member === trackAudioModel.get(
                                            numberItem).pretext)
                                    trackAudioModel.get(
                                                j).sub_set_children.remove(k, 1)
                            }
                        }
                    }

                    trackAudioModel.remove(numberItem, 1)

                    var vResult = 0
                    for (i = 0; i < trackAudioModel.count; i++) {
                        if (trackAudioModel.get(
                                    i).type === 'Group' & trackAudioModel.get(
                                    i).in_set_parent.count > vResult)
                            vResult = trackAudioModel.get(i).in_set_parent.count
                    }
                    group_width = scaleSize2(50) * (vResult + 1)

                    trackRightClick.close()
                }
            }

            // -----------------------------------------------------------------2
            Rectangle {
                id: splitter4_
                visible: isVisible_ungrouper
                Layout.preferredHeight: 1
                Layout.preferredWidth: parentWidth
                color: lineClr
            }

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Insert Audio Track'
                rightTitle: ''
                onTriggered: {
                    funcs.offLightAllTracks()

                    var inset_pretext = []

                    // make Name:
                    var j = 1
                    for (var i = 0; i < trackAudioModel.count; i++) {
                        if (trackAudioModel.get(i).type === 'audio') {
                            j++
                        }
                    }
                    var audioName = 'audio ' + j
                    // make name.


                    /* :::  F O R top of other audio, old method:
                    // find place of into origin parent:
                    var place = numberItem + 1
                    if(trackAudioModel.get(numberItem).type === 'Group')
                        place = numberItem
                    var ij = 0
                    if(trackAudioModel.get(numberItem).in_set_parent.count) {
                        ij = 1
                        while(trackAudioModel.get(numberItem-ij).type !== 'Group')
                        {
                            ij++
                        }
                        place = numberItem - ij +1
                    }
                    // find place.

                    if(trackAudioModel.get(numberItem).in_set_parent.count)
                    for(i = 0; i < trackAudioModel.get(place).in_set_parent.count; i++)
                    {
                        inset_pretext.push(trackAudioModel.get(place).in_set_parent.get(i).member)
                    }


                    trackAudioModel.insert(place,
                                           {isCollapsed: true, type: 'audio', pretext: audioName,
                                               in_set_parent: [],
                                               sub_set_children: [],
                                               selectTrack: true, selectSolo: false, partialySolo: false, solo_on_audio: false, selectTrackDependSolo: true, light: false,
                                               coverBound: false, channel_count: 8, rowsExpanded: 1, imgURL: 'qrc:/Resource/Triangle.svg'
                                           })

                    if(inset_pretext.length & trackAudioModel.get(place-1).type === 'Group' & trackAudioModel.get(place-1).rowsExpanded < 1)
                        trackAudioModel.setProperty(place, 'isCollapsed', false)

                    // copy on parent and inserted track
                    for(i = 0; i < inset_pretext.length; i++)
                    {
                        for(var k = 0; k < trackAudioModel.count; k++){
                            if(trackAudioModel.get(k).pretext === inset_pretext[i])
                                trackAudioModel.get(k).sub_set_children.append({member: audioName})
                        }
                        trackAudioModel.get(place).in_set_parent.append({member: inset_pretext[i]})
                    }
                    */

                    //:::  F O R bottom of other audio, new method:
                    var place = numberItem + 1
                    // find place:
                    if (trackAudioModel.get(numberItem).type === 'Group') {
                        place = numberItem + trackAudioModel.get(
                                    numberItem).sub_set_children.count + 1
                    }

                    // find place.
                    if (trackAudioModel.get(numberItem).in_set_parent.count)
                        for (i = 0; i < trackAudioModel.get(
                                 numberItem).in_set_parent.count; i++) {
                            inset_pretext.push(
                                        trackAudioModel.get(
                                            numberItem).in_set_parent.get(
                                            i).member)
                        }

                    trackAudioModel.insert(place, {
                                               "isCollapsed": true,
                                               "type": 'audio',
                                               "pretext": audioName,
                                               "in_set_parent": [],
                                               "sub_set_children": [],
                                               "selectTrack": true,
                                               "selectSolo": false,
                                               "partialySolo": false,
                                               "solo_on_audio": false,
                                               "selectTrackDependSolo": true,
                                               "light": false,
                                               "editName": false,
                                               "inDraggingCounter": -1,
                                               "coverBound": false,
                                               "channel_count": 8,
                                               "rowsExpanded": 1,
                                               "imgURL": 'qrc:/Resource/Triangle.svg',
                                               "automationTurn": false,
                                               "automationChildren": 0,
                                               "volumeAutomated": false,
                                               "panAutomated": false,
                                               "speakerOnAutomated": false,
                                               "automationList": []
                                           })

                    if (inset_pretext.length & trackAudioModel.get(
                                place - 1).type === 'Group' & trackAudioModel.get(
                                place - 1).rowsExpanded < 1)
                        trackAudioModel.setProperty(place, 'isCollapsed', false)

                    // copy on parent and inserted track
                    for (i = 0; i < inset_pretext.length; i++) {
                        for (var k = 0; k < trackAudioModel.count; k++) {
                            if (trackAudioModel.get(
                                        k).pretext === inset_pretext[i])
                                trackAudioModel.get(k).sub_set_children.append({
                                                                                   "member": audioName
                                                                               })
                        }
                        trackAudioModel.get(place).in_set_parent.append({
                                                                            "member": inset_pretext[i]
                                                                        })
                    }

                    trackListView.positionViewAtIndex(place, ListView.Contain)

                    trackRightClick.close()

                    funcs.offLightAllTracks()
                    funcs.trackLighting(place, 'btn')
                }
            }

            // -----------------------------------------------------------------2
            Rectangle {
                id: splitter4
                Layout.preferredHeight: 1
                Layout.preferredWidth: parentWidth
                color: lineClr
            }
            // colors
            Rectangle {
                id: colors
                property int mySpace: bigSpace
                Layout.preferredHeight: cellHeight * 5 + colors.mySpace * 6
                Layout.preferredWidth: parentWidth

                color: bgColr
                GridLayout {
                    id: gridColors
                    columns: 11
                    rows: 5
                    rowSpacing: colors.mySpace
                    columnSpacing: colors.mySpace
                    anchors {
                        fill: parent
                        margins: colors.mySpace
                        leftMargin: scaleSize2(15)
                        rightMargin: scaleSize2(15)
                    }

                    Repeater {
                        model: ['#FFCADA', '#F44336', '#FFDD9F', '#FF5722', '#F3C430', '#7C9712', '#61D67D', '#00CBE6', '#293AAE', '#9C27B0', '#808080', '#D694A3', '#E52729', '#FCBD87', '#CC5600', '#DAA521', '#57501B', '#007F3C', '#2166F3', '#232692', '#673AB7', '#D5BF96', '#FB939C', '#CA252B', '#FF7F50', '#B73F0B', '#FFC107', '#66CA43', '#065C37', '#03A9F4', '#4652AB', '#612652', '#B6863C', '#FE50B9', '#C6100A', '#E4715C', '#FEF167', '#BDDF43', '#48B30F', '#8AD8E4', '#729EF5', '#B97DDA', '#56253B', '#78450E', '#E91E63', '#950C04', '#FF9800', '#FEEB2C', '#94CF37', '#1C911C', '#30CFCA', '#5159DE', '#AC41EF', '#CFCECA', '#873E2B']
                        delegate: Rectangle {
                            color: modelData
                            //                            Layout.fillHeight: true
                            //                            Layout.fillWidth: true
                            Layout.preferredHeight: cellHeight
                            Layout.preferredWidth: cellHeight

                            border.color: bgColr
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    panel.colorPanel = color
                                    trackRightClick.close()
                                }
                                onEntered: parent.border.color = color
                                onExited: parent.border.color = bgColr
                            }
                        }
                    }
                }
            } // ---------------------------------------------------------------------- 4
            // colors.
            Rectangle {
                id: splitter5
                Layout.preferredHeight: 1
                Layout.preferredWidth: parentWidth
                color: lineClr
                antialiasing: true
            }

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'More Colors'
                onTriggered: {

                }
            } // ---------------------------------------------------------------------- 5

            Rectangle {
                id: splitter6
                Layout.preferredHeight: 1
                Layout.preferredWidth: parentWidth
                color: lineClr
                antialiasing: true
            }

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Randomize Track Color'
                onTriggered: {

                }
            }

            ShortcutRow {
                Layout.preferredHeight: cellHeight
                Layout.preferredWidth: parentWidth
                originParent: root
                leftTitle: 'Assign Track Color to Clips'
                onTriggered: {

                }
            }
            // ---------------------------------------------------------------------- 6
        } // column.
    } // Main column.

    Text {
        id: testSize
        visible: false
        font.family: topFontFamily
        font.pixelSize: topfontPixelSize
        text: 'Show Automation in New Lane'
    }

    Text {
        id: testSize2
        visible: false
        font.family: topFontFamily
        font.pixelSize: topfontPixelSize
        text: 'Ctrl + G'
    }

    Rectangle {
        anchors.fill: root
        //        anchors.margins: -1
        radius: root.radius
        color: 'transparent'
        border.color: '#FFFFFF' //lineClr
    }
    //            Component.onCompleted: { console.log(container.height); console.log(columnMain.height) }
}
