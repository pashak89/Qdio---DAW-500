import QtQuick 2.12

MouseArea {
    id: root
    // TODO check it for  تداخل
    width: trackListView.width//isAncherDrag ? trackListView.width : (scaleSize2(50) + trackAudio.draggableSize)

    /*property int someWidth: indexPressed > -1 ? (trackAudioModel.get(groupTrackLight[0]).in_set_parent.count+1) * scaleSize2(50) +
                                                trackAudioModel.get(indexPressed).sub_set_children.count * 5*0
                                              : 0*/
    property int i: 0
    property int j: 0
    property int k: 0

    property int indexPressed: -1
    property int indexDrag: -1
    property int indexReleased: -1
    property int indexReleased_rowsExpanded: -1
    property int indexDesired: -1
    property int new_indexReleased: -1
    property int new_indexDesired: -1
    property int index_between_1: -1
    property int index_between_2: -1
    property int dragIndexRaw: -1

    property int ystart: -1

    property variant removeArray: []

    // property var all_move_unique_index: []
    property var clearing_insets_member_pretext: []
    property var all_movement_member_index: []
    property var all_movement_member_pretext: []
    property var index_lightGroup_in_moveGroup: []
    property var pure_audio_or_group: []
    property var odl_groupTrackLight: []
    property var groupTrackLight_pretext: []
    property var notArrayParent_pretext: []
    property var notArrayParent_index: []
    property var allParents_index: []
    property var allParents_pretext: []

    property var newGuideParent_pretext
    property int newGuideParent_index: -1

    property var pretext_indexReleased
    property var pretext_indexDesired
    property var child_exist_index

    property bool isPosMouseChange: false
    property bool allow2clear: false
    property bool halfUp: false
    property bool halfDown: false
    property bool notMoving: false
    property bool didIntoGroup: false
    property bool didAllowOut: false
    property bool outBound: false
    property bool isAncherDrag: false
    property bool isMemberOfGroup: memberOfGroup()
    property bool isTop: false
    property bool isBottom: false
    property real yBottom: 0


    /// F U N C T I O N ::_
    // --------------------------------------------------------------------------------------------------------------------------------- R E T U R N - PROP:
    function return4property() {
        didAllowOut = allow2out()
        if(!didAllowOut |
                indexPressed < 0 | indexReleased < 0 |
                groupTrackArray.length === 0 |
                didIntoGroup | outBound)
        {console.log('return 0');return true}

        if(indexPressed === indexReleased & trackAudioModel.get(indexPressed).inDraggingCounter < 1)
        {console.log('return 1');return true}

        if(isMemberOfGroup)
        {console.log('return member');return true}

        if(indexDesired === 0 & indexReleased === 0 & dragIndexRaw > 0)
        {console.log('return 2- clicked', 'groupTrackArray.length: ', groupTrackArray.length)
            removeArray.length = 0
            var removeList_pretext = []
            for(var v = 0; v < pure_audio_or_group.length; v++)
            {
                for(var u = 0; u < trackAudioModel.get(pure_audio_or_group[v]).in_set_parent.count; u++)
                    removeList_pretext.push(trackAudioModel.get(pure_audio_or_group[v]).in_set_parent.get(u).member)
                trackAudioModel.get(pure_audio_or_group[v]).in_set_parent.clear()
            }
            for(var a = 0; a < all_movement_member_index.length; a++)
            {
                for(var b = 0; b < removeList_pretext.length; b++)
                {
                    for(v = 0; v < trackAudioModel.get(all_movement_member_index[a]).in_set_parent.count ; v++)
                    {
                        if(trackAudioModel.get(all_movement_member_index[a]).in_set_parent.get(v).member === removeList_pretext[b])
                            removeArray.push(v);
                    }
                }

                removeArray.sort(function(aa, bb) { return aa - bb; });

                for(b = 0; b < removeArray.length; b++)
                {
                    trackAudioModel.get(all_movement_member_index[a]).in_set_parent.remove(removeArray[b], 1)
                    for(var j = 0; j < removeArray.length; j++)
                        removeArray[j] = removeArray[j]-1
                }
                removeArray.length = 0;
            }
            halfUp = false
            halfDown = false
            isAncherDrag = false
            return true
        }
        return false
    }// -------------------------------------------------------------------------------------------------------------------------------- R E T U R N - PROP.

    // --------------------------------------------------------------------------------------------------------------------------------- R E M O V I N G -subset-inset:
    function remove_subset_inset() {
        removeArray.length = 0

        // from parents:
        for(i = 0; i < trackAudioModel.count; i++)
        {
            for(j = 0; j < trackAudioModel.get(i).sub_set_children.count; j++)
            {
                for(k = 0; k < all_movement_member_index.length; k++)
                {
                    if(all_movement_member_pretext[k] === trackAudioModel.get(i).sub_set_children.get(j).member
                            & all_movement_member_pretext.indexOf(trackAudioModel.get(i).pretext) < 0)
                    {
                        /*if(notArrayParent_index.length > 0 & indexPressed === indexReleased)
                        {
                            if(notArrayParent_index.indexOf(i) > -1)
                                removeArray.push(j)
                        }
                        else*/ removeArray.push(j)
                    }
                }
            }
//            for(k = 0; k < all_movement_member_index.length; k++)
//            if(findIndex(trackAudioModel.get(i).sub_set_children, all_movement_member_pretext[k], 'member') !== null &
//                    all_movement_member_pretext.indexOf(trackAudioModel.get(i).pretext) < 0)
//            removeArray.push(j)

            removeArray.sort(function(a, b) { return a - b; });

            for(j = 0; j < removeArray.length; j++)
            {
                trackAudioModel.get(i).sub_set_children.remove(removeArray[j], 1)
                for(var sj = 0; sj < removeArray.length; sj++)
                    removeArray[sj] = removeArray[sj]-1
            }
//            for(j = removeArray.length-1; j > -1; j--)
//            {
//                trackAudioModel.get(i).sub_set_children.remove(removeArray[j], 1)
//            }
            removeArray.length = 0;
        }// from parents.


        for(i = 0; i < all_movement_member_index.length; i++)
        {
            for(k = 0; k < trackAudioModel.get(all_movement_member_index[i]).in_set_parent.count; k++)
            {
                for(j = 0; j < clearing_insets_member_pretext.length; j++)
                {
                    if(clearing_insets_member_pretext[j] === trackAudioModel.get(all_movement_member_index[i]).in_set_parent.get(k).member)
                        /*if(notArrayParent_pretext.length > 0 & indexPressed === indexReleased)
                        {
                            if(notArrayParent_pretext.indexOf(clearing_insets_member_pretext[j]) > -1)
                                removeArray.push(k)
                        }
                        else*/ removeArray.push(k)
                }
            }
//            for(k = 0; k < clearing_insets_member_pretext.length; k++)
//            if(findIndex(trackAudioModel.get(all_movement_member_index[i]).in_set_parent, clearing_insets_member_pretext[k], 'member') !== null)
//            removeArray.push(j)

            removeArray.sort(function(a, b) { return a - b; });

            for(k = 0; k < removeArray.length; k++)
            {

                trackAudioModel.get(all_movement_member_index[i]).in_set_parent.remove(removeArray[k], 1)
                for(j = 0; j < removeArray.length; j++)
                    removeArray[j] = removeArray[j]-1
            }
            removeArray.length = 0;
//            for(j = removeArray.length-1; j > -1; j--)
//            {
//                trackAudioModel.get(all_movement_member_index[i]).in_set_parent.remove(removeArray[j], 1)
//            }
//            removeArray.length = 0;
        }// from childrens.
    }// -------------------------------------------------------------------------------------------------------------------------------- R E M O V I N G -subset-inset.

    // --------------------------------------------------------------------------------------------------------------------------------- A D D or C O P Y:
    function add_Copy() {
        var parent_on_released_pretext = []
        var parent_on_released_index = []

//                console.log('new_indexReleased:', new_indexReleased, ', indexReleased:', indexReleased)
        for(i = 0; i < trackAudioModel.get(indexReleased).in_set_parent.count; i++)
        {
            //if(notArrayParent_pretext.indexOf(trackAudioModel.get(indexReleased).in_set_parent.get(i).member) < 0)
            parent_on_released_pretext.push(trackAudioModel.get(indexReleased).in_set_parent.get(i).member)
        }
        for(i = 0; i < trackAudioModel.count;i++)
        {
            for(j = 0; j < parent_on_released_pretext.length; j++)
            if(trackAudioModel.get(i).pretext === parent_on_released_pretext[j])
            parent_on_released_index.push(i)
        }


//console.log("parents:", parent_on_released_pretext, parent_on_released_index, "children:", all_movement_member_pretext, all_movement_member_index)
        for(j = 0; j < parent_on_released_index.length; j++)
        {
            for(i = 0; i < all_movement_member_pretext.length; i++)
            {
                if(/*notArrayParent_index.indexOf(parent_on_released_index[j]) < 0 &*/ parent_on_released_index[j] !== all_movement_member_index[i] &
                        findIndex(trackAudioModel.get(parent_on_released_index[j]).sub_set_children, all_movement_member_pretext[i], 'member') === null)
                trackAudioModel.get(parent_on_released_index[j]).sub_set_children.append({member: all_movement_member_pretext[i]})
            }
        }

        for(i = 0; i < all_movement_member_index.length; i++)
        {//console.log('parent_on_released_pretext:', parent_on_released_pretext)
            for(j = 0; j < parent_on_released_pretext.length; j++)
            {
                if(/*notArrayParent_pretext.indexOf(parent_on_released_pretext[j]) < 0 & */all_movement_member_pretext[i] !== parent_on_released_pretext[j] &
                        findIndex(trackAudioModel.get(all_movement_member_index[i]).in_set_parent, parent_on_released_pretext[j], 'member') === null)
                trackAudioModel.get(all_movement_member_index[i]).in_set_parent.append({member: parent_on_released_pretext[j]})
            }
        }


       // add2child_parent()

//console.log('dncjskdnksdvkjsbv', trackAudioModel.get(new_indexReleased).type, halfDown, halfUp)
        if((trackAudioModel.get(indexReleased).type === 'Group' & halfDown) |
                (trackAudioModel.get(indexReleased).type === 'Group' & !halfDown & !halfUp))
        {//console.log('Group & halfDown')
            for(i = 0; i < all_movement_member_index.length; i++)
            {
                trackAudioModel.get(all_movement_member_index[i]).in_set_parent.append({member: trackAudioModel.get(indexReleased).pretext})
            }
            for(i = 0; i < all_movement_member_pretext.length; i++)
            {
                trackAudioModel.get(indexReleased).sub_set_children.append({member: all_movement_member_pretext[i]})
            }
        }
    }// -------------------------------------------------------------------------------------------------------------------------------- A D D or C O P Y.

    // --------------------------------------------------------------------------------------------------------------------------------- SET EXPENED LINE:
    function set_expendedLine() {
        if(trackAudioModel.get(indexReleased).type === 'Group' & indexReleased_rowsExpanded > 0)
        {//console.log('o0oo0o0o0o0o0ooo0o0ooo0oo0ooo0000oooo0')
            /*for(i = 0; i < all_movement_member_index.length; i++)
            {
                trackAudioModel.setProperty(all_movement_member_index[i], 'isCollapsed', true)
//                if(trackAudioModel.)
//                trackAudioModel.setProperty(all_movement_member_index[i], 'rowsExpanded', 1)
            }*/
        }
        else if(trackAudioModel.get(indexReleased).type === 'Group' & indexReleased_rowsExpanded === 0)
        {//console.log('o0oo0o0o0o0o-------0ooo0000oooo0')
            for(i = 0; i < all_movement_member_index.length; i++)
            {
                if(halfDown)
                trackAudioModel.setProperty(all_movement_member_index[i], 'isCollapsed', false)
//                        trackAudioModel.setProperty(all_movement_member_index[i], 'rowsExpanded', 0)
            }
        }
    }// -------------------------------------------------------------------------------------------------------------------------------- SET EXPENED LINE.

    // --------------------------------------------------------------------------------------------------------------------------------- END GROUPING:
    function endGrouping() {
        halfUp = false
        halfDown = false

        var vResult = 0
        for(i = 0; i < trackAudioModel.count; i++)
        {
            if(trackAudioModel.get(i).type === 'Group' & trackAudioModel.get(i).in_set_parent.count > vResult)
                vResult = trackAudioModel.get(i).in_set_parent.count
        }
        group_width = scaleSize2(50) * (vResult+1)

        isAncherDrag = false
    }// -------------------------------------------------------------------------------------------------------------------------------- END GROUPING.

    // --------------------------------------------------------------------------------------------------------------------------------  ARRAY DECK:
    function arrayDeck() {
        odl_groupTrackLight.length = 0
        odl_groupTrackLight = groupTrackLight.slice()

        pure_audio_or_group = groupTrackLight.slice()
        var repeatMember = []
        for(i = 0; i < groupTrackLight.length; i++)
        {
            if(trackAudioModel.get(groupTrackLight[i]).type === 'Group')
            {
                // subset_members = groupTrackLight[i] // index of Group
                for(var j = 0; j < trackAudioModel.get(groupTrackLight[i]).sub_set_children.count; j++)
                {
                    var child_exist_index = pure_audio_or_group.indexOf(groupTrackLight[i] + j + 1)

                    if(child_exist_index > -1)
                    {
                        repeatMember.push(child_exist_index)
                        pure_audio_or_group.splice(child_exist_index, 1)
                    }

                }
            }// if Group.
        }// pure_audio_or_group.
//console.log('groupTrackLight:', groupTrackLight)
        groupTrackLight_pretext.length = 0
        for(i = 0; i < groupTrackLight.length; i++)
        {
            groupTrackLight_pretext.push( trackAudioModel.get(groupTrackLight[i]).pretext )
        }

        all_movement_member_index.length = 0
        for(i = 0; i < pure_audio_or_group.length; i++)
        {
            all_movement_member_index.push(pure_audio_or_group[i])

            for(j = 1; j <= trackAudioModel.get(pure_audio_or_group[i]).sub_set_children.count; j++) {
                all_movement_member_index.push(pure_audio_or_group[i] +j)
            }
        }//console.log('pure_audio_or_group:', pure_audio_or_group)

        all_movement_member_pretext.length = 0
        for(i = 0; i < all_movement_member_index.length; i++)
        {
            all_movement_member_pretext.push(trackAudioModel.get(all_movement_member_index[i]).pretext)
        }//console.log('all_movement_member_pretext:', all_movement_member_pretext)
        clearing_insets_member_pretext.length = 0
        for(i = 0; i < pure_audio_or_group.length; i++)
        {
            for(j = 0; j < trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.count; j++)
            clearing_insets_member_pretext.push(trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.get(j).member)
        }
    }// -------------------------------------------------------------------------------------------------------------------------------- ARRAY DECK.

    // --------------------------------------------------------------------------------------------------------------------------------- R E T U R N -MOVING:
    function return4moving() {
        didAllowOut = allow2out()
        if(!didAllowOut |
                (indexPressed === indexReleased | indexPressed < 0 | indexReleased < 0 | groupTrackLight.indexOf(indexDesired) > -1) & outBound === false |
                (indexPressed === indexDesired & groupTrackLight.length === 1 & outBound === false) |
                (trackAudioModel.get(all_movement_member_index[0]).type === 'Group' & all_movement_member_index.indexOf(indexDesired) > -1 & outBound === false) |
                (outBound === false & dragIndexRaw === -1 & indexDrag === trackAudioModel.count-1) )
            return true

        if(isMemberOfGroup)
        {console.log('return member');return true}

        return false
    }// -------------------------------------------------------------------------------------------------------------------------------- R E T U R N -MOVING.

    // --------------------------------------------------------------------------------------------------------------------------------- M O V E M E N T:
    function moveTracks() {console.log('***------           indexDesired: ', indexDesired, ', -----all_movement_member_index: ', all_movement_member_index)
        // MOVE all into the top
        for(i = 0; i < all_movement_member_index.length; i++)
        {
            trackAudioModel.move(all_movement_member_index[i], i, 1)
        }

        var under_tracks = 0
        for(i = 0; i < all_movement_member_index.length; i++)
            if(all_movement_member_index[i] >= indexDesired)
                under_tracks++
        console.log('***------           under_tracks: ', under_tracks)
        trackAudioModel.move(0, indexDesired - all_movement_member_index.length + under_tracks, all_movement_member_index.length)
    }// -------------------------------------------------------------------------------------------------------------------------------- M O V E M E N T.

    // --------------------------------------------------------------------------------------------------------------------------------- R E A R R A Y:
    function rearray() {
        groupTrackLight.length = 0;// console.log('in reArray; groupTrackLight_pretext: ', groupTrackLight_pretext)
        for(i = 0; i < trackAudioModel.count; i++)
        {
            for(j = 0; j < groupTrackLight_pretext.length; j++)
            {
                if(groupTrackLight_pretext[j] === trackAudioModel.get(i).pretext)
                    groupTrackLight.push(i)
            }
        }
        groupTrackLight = groupTrackLight.sort(function (a, b) {  return a - b;  });
//console.log('in reArray; groupTrackLight: ', groupTrackLight)
        pure_audio_or_group = groupTrackLight.slice()
        for( i = 0; i < groupTrackLight.length; i++)
        {
            if(trackAudioModel.get(groupTrackLight[i]).type === 'Group')
            {
                for(j = 0; j < trackAudioModel.get(groupTrackLight[i]).sub_set_children.count; j++)
                {
                    child_exist_index = pure_audio_or_group.indexOf(groupTrackLight[i] + j + 1)
                    if(child_exist_index > -1)
                        pure_audio_or_group.splice(child_exist_index, 1)
                }
            }// if Group.
        }// pure_audio_or_group.
//console.log('pure:', pure_audio_or_group)
        clearing_insets_member_pretext.length = 0
        for(i = 0; i < pure_audio_or_group.length; i++)
        {
            for(j = 0; j < trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.count; j++)
            clearing_insets_member_pretext.push(trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.get(j).member)
        }

        all_movement_member_index.length = 0
        for(i = 0; i < pure_audio_or_group.length; i++)
        {
            all_movement_member_index.push(pure_audio_or_group[i])

            for(j = 1; j <= trackAudioModel.get(pure_audio_or_group[i]).sub_set_children.count; j++) {
                all_movement_member_index.push(pure_audio_or_group[i] + j)
            }
        }//console.log('all_move_member_index:', all_movement_member_index)

        all_movement_member_pretext.length = 0
        for(i = 0; i < all_movement_member_index.length; i++)
        {
            all_movement_member_pretext.push(trackAudioModel.get(all_movement_member_index[i]).pretext)
        }//console.log('all_move_member_pretext:', all_movement_member_pretext)

        for(i = 0; i < trackAudioModel.count; i++)
        {
            if(pretext_indexDesired === trackAudioModel.get(i).pretext)
                new_indexDesired = i
        }
        for(i = 0; i < trackAudioModel.count; i++)
        {
            if(pretext_indexReleased === trackAudioModel.get(i).pretext)
                new_indexReleased = i
        }
    } // ------------------------------------------------------------------------------------------------------------------------------- R E A R R A Y.

    // --------------------------------------------------------------------------------------------------------------------------------- F I N D Parent:
    function findNotParent(index, level) {
        if(level === 0) return []
        var levelArr = []
        var notLevelArr = []

        if(level > 0)
        for(var i = 0; i < trackAudioModel.get(dragIndexRaw).in_set_parent.count; i++)
            allParents_pretext.push(trackAudioModel.get(dragIndexRaw).in_set_parent.get(i).member)

        for(var p = 0; p < trackAudioModel.get(index).in_set_parent.count; p++)
        {//console.log('levelArr:',trackAudioModel.get(index).in_set_parent.get(p).member)
            var parentIndex = findIndex(trackAudioModel, trackAudioModel.get(index).in_set_parent.get(p).member, 'pretext')
            if(groupTrackLight.indexOf(parentIndex) > -1)
                if(index > parentIndex)
                index = parentIndex
        }

        for(p = 0; p < trackAudioModel.get(index).in_set_parent.count; p++)
        {//console.log('levelArr:',trackAudioModel.get(index).in_set_parent.get(p).member)
            parentIndex = findIndex(trackAudioModel, trackAudioModel.get(index).in_set_parent.get(p).member, 'pretext')
            //console.log('0levelArr:', _level)
            levelArr.push(parentIndex)
        }
        levelArr.sort(function (a, b) {return b - a});
//console.log('1levelArr:', levelArr)
        for(p = 0; p < level; p++)
        {
            notLevelArr.push(levelArr[p])
        }
        return notLevelArr
    }
    function findNotParent2(index) {
        var level = 0

        let curArr = listModel2Array(trackAudioModel.get(dragIndexRaw).in_set_parent)
        let nextArr = listModel2Array(trackAudioModel.get(dragIndexRaw+1).in_set_parent)
        let difference = curArr.filter(x => !nextArr.includes(x))



        let const_ = trackAudioModel.get(dragIndexRaw).type === 'Group' ?
                trackAudioModel.get(dragIndexRaw).in_set_parent.count + (trackAudioModel.get(dragIndexRaw).rowsExpanded > 0 ? 2 : 1):
                trackAudioModel.get(dragIndexRaw).in_set_parent.count

        var floorMouseX = Math.floor(mouseX / scaleSize2(50))
        var m = -1
        if(!globalValues.tracksOnLeft)
        {
            m = mouseX - (trackListView.width - const_ * scaleSize2(50))
            if(m >= 0)
            floorMouseX = const_ - Math.ceil(m / scaleSize2(50))
            else floorMouseX = const_ + 1
        }




        if(difference === trackAudioModel.get(dragIndexRaw).in_set_parent.count)
        {
            level = floorMouseX
        }
        else if(difference === 0)
        {
            index = -1
        }
        else {
            let mouseScale = floorMouseX
            let limit = trackAudioModel.get(dragIndexRaw).in_set_parent.count - difference.length
            if(mouseScale <= trackAudioModel.get(dragIndexRaw).in_set_parent.count & mouseScale >= limit)
            level = mouseScale
            else if(mouseScale > trackAudioModel.get(dragIndexRaw).in_set_parent.count)
            index = -1
            else if(mouseScale < limit)
            level = limit
        }
//        console.log('level: ', level, ', index', index)

        index--
        if(trackAudioModel.get(index) !== undefined)
        while(trackAudioModel.get(index).type !== 'Group' & index > -1)
        {
            if(trackAudioModel.get(index-1) !== undefined)
            index--//;console.log('index1: ', index)
        }

        if(trackAudioModel.get(index) !== undefined)
        while(trackAudioModel.get(index).in_set_parent.count !== level & index > -1)
        {
            if(trackAudioModel.get(index-1) !== undefined)
            index--//;console.log('index2: ', index)
        }

        var arr = []
        arr[0] = index; arr[1] = trackAudioModel.get(index).pretext
       // console.log('arr: ', arr)
        return arr
    }// -------------------------------------------------------------------------------------------------------------------------------- F I N D Parent.

    // --------------------------------------------------------------------------------------------------------------------------------- C O N D I T I O N:
    function firstRow() {
        if((dragIndexRaw < 0 & halfUp))
        {console.log('return dragIndexRaw- released', 'groupTrackArray.length: ', groupTrackArray.length, ', groupTrackLight:', groupTrackLight)
            moveTracks()
            rearray()
            removeArray.length = 0
            var removeList_pretext = []
            for(i = 0; i < pure_audio_or_group.length; i++)
            {
                for(j = 0; j < trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.count; j++)
                    removeList_pretext.push(trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.get(j).member)
                trackAudioModel.get(pure_audio_or_group[i]).in_set_parent.clear()
            }console.log('-> removeList_pretext:', removeList_pretext, ', pure_audio_or_group:', pure_audio_or_group)
            for(i = 0; i < all_movement_member_index.length; i++)
            {
                for(j = 0; j < removeList_pretext.length; j++)
                {
                    for(k = 0; k < trackAudioModel.get(all_movement_member_index[i]).in_set_parent.count ; k++)
                    {
                        if(trackAudioModel.get(all_movement_member_index[i]).in_set_parent.get(k).member === removeList_pretext[j])
                            removeArray.push(k);
                    }
                }

                removeArray.sort(function(a, b) { return a - b; });

                for(k = 0; k < removeArray.length; k++)
                {
                    trackAudioModel.get(all_movement_member_index[i]).in_set_parent.remove(removeArray[k], 1)
                    for(j = 0; j < removeArray.length; j++)
                        removeArray[j] = removeArray[j]-1
                }
                removeArray.length = 0
            }
            removeArray.length = 0
            //remove from parents
            var indx = 0
            var child_idx = 0
            for(i = 0; i < removeList_pretext.length; i++)
            {
                indx = findIndex(trackAudioModel, removeList_pretext[i], 'pretext')
                for(j = 0; j < all_movement_member_pretext.length; j++) {
                    child_idx = findIndex(trackAudioModel.get(indx).sub_set_children, all_movement_member_pretext[j], 'member')
                    trackAudioModel.get(indx).sub_set_children.remove(child_idx, 1)
                }
            }

            halfUp = false
            halfDown = false
            isAncherDrag = false
            return true
        }
        return false
    }
    function endRow() {
        removeArray.length = 0
        if(outBound)
        {console.log('outBound: ', outBound, 'return end.', 'all_movement_member_pretext: ', all_movement_member_pretext, ', groupTrackLight:', groupTrackLight)
            console.log('->clearing_insets_member_pretext:', clearing_insets_member_pretext)// removing in children
            for(i = 0; i < all_movement_member_index.length; i++)
            {
                for(j = 0; j < trackAudioModel.get(all_movement_member_index[i]).in_set_parent.count; j++)
                for(k = 0; k < clearing_insets_member_pretext.length; k++)
                if(trackAudioModel.get(all_movement_member_index[i]).in_set_parent.get(j).member === clearing_insets_member_pretext[k])
                    removeArray.push(j)

                removeArray.sort(function(a, b) { return a - b; });

                for(j = 0; j < removeArray.length; j++) {
                    trackAudioModel.get(all_movement_member_index[i]).in_set_parent.remove(removeArray[j], 1)
                    for(var r = 0; r < removeArray.length; r++)
                        removeArray[r] = removeArray[r]-1
                }
                removeArray.length = 0;
            }            

            // removing in parents
            for(i = 0; i < trackAudioModel.count; i++)
            {
                for(j = 0; j < trackAudioModel.get(i).sub_set_children.count; j++)
                {
                    for(k = 0; k < all_movement_member_index.length; k++)
                    {
                        if(all_movement_member_pretext[k] === trackAudioModel.get(i).sub_set_children.get(j).member
                                & all_movement_member_pretext.indexOf(trackAudioModel.get(i).pretext) < 0)
                        {
                            removeArray.push(j)
                        }
                    }
                }

                removeArray.sort(function(a, b) { return a - b; });

                for(j = 0; j < removeArray.length; j++)
                {
                    trackAudioModel.get(i).sub_set_children.remove(removeArray[j], 1)
                    for(r = 0; r < removeArray.length; r++)
                        removeArray[r] = removeArray[r]-1
                }
                removeArray.length = 0;
            }

            // moving :
            for(i = 0; i < all_movement_member_index.length; i++)
            {   // MOVE all into the top
                trackAudioModel.move(all_movement_member_index[i], i, 1)
            }
            var endIndex = trackAudioModel.count - 1
            /*for(i = 1; i <= all_movement_member_index.length; i++)
                trackAudioModel.move(0, endIndex - all_movement_member_index.length + i, all_movement_member_index.length)*/
            trackAudioModel.move(0, trackAudioModel.count - all_movement_member_index.length, all_movement_member_index.length)
            // moving.

            return true
        }// outbound.
        return false
    }

    function memberOfGroup() {
        var alls = []
        var alls_pretext = []
        var result = false
        for(var i = 0; i < groupTrackLight.length; i++)
        {
            alls.push(groupTrackLight[i])

            for(var j = 1; j <= trackAudioModel.get(groupTrackLight[i]).sub_set_children.count; j++) {
                alls.push(groupTrackLight[i] +j)
            }
        }// TODO optimise
        for(var a = 0; a < alls.length; a++)
            alls_pretext.push(trackAudioModel.get(a).pretext)

        if(alls.indexOf(dragIndexRaw) > -1 & indexPressed !== dragIndexRaw)
        {
            var number = 0
            /*for(var k = 0; k < trackAudioModel.get(dragIndexRaw).in_set_parent.count; k++)
            {
                if(alls_pretext.indexOf(trackAudioModel.get(dragIndexRaw).in_set_parent.get(k).member) > -1)
                    number++
            }
            if(number > auxMemberGroup() | halfUp)
                result = true
            else result = false*/
            /*result = true
            if(trackAudioModel.get(dragIndexRaw).inDraggingCounter > -1)
                result = false*/

            let const_ = trackAudioModel.get(dragIndexRaw).type === 'Group' ?
                    trackAudioModel.get(dragIndexRaw).in_set_parent.count + (trackAudioModel.get(dragIndexRaw).rowsExpanded > 0 ? 2 : 1):
                    trackAudioModel.get(dragIndexRaw).in_set_parent.count

            var floorMouseX = Math.floor(mouseX / scaleSize2(50))
            var m = -1
            if(!globalValues.tracksOnLeft)
            {
                m = mouseX - (trackListView.width - const_ * scaleSize2(50))
                if(m >= 0)
                floorMouseX = const_ - Math.ceil(m / scaleSize2(50))
                else floorMouseX = const_ + 1
            }


            if(floorMouseX >= trackAudioModel.get(dragIndexRaw).in_set_parent.count | halfUp)
                result = true


            else if(halfDown & trackAudioModel.get(dragIndexRaw).in_set_parent.count === trackAudioModel.get(dragIndexRaw+1).in_set_parent.count)
                result = true

            else result = false
        }
        return result
    }
    function auxMemberGroup() {
        if(dragIndexRaw < trackAudioModel.count - 1)
        {
            var curArr = listModel2Array(trackAudioModel.get(dragIndexRaw).in_set_parent)
            var nextArr = listModel2Array(trackAudioModel.get(dragIndexRaw+1).in_set_parent)
            var difference = curArr.filter(x => !nextArr.includes(x))

            var limit_draggingCounter = trackAudioModel.get(dragIndexRaw).in_set_parent.count - Math.floor(mouseX / scaleSize2(50))
            limit_draggingCounter = limit_draggingCounter > 0 ? limit_draggingCounter : 0

            return Math.min(limit_draggingCounter, difference.length)
        }
        else if(dragIndexRaw === trackAudioModel.count - 1)
        {
            limit_draggingCounter = trackAudioModel.get(dragIndexRaw).in_set_parent.count - Math.floor(mouseX / scaleSize2(50))
            return limit_draggingCounter
        }
    }

    function result_release_drag() {
       if(halfDown & indexReleased > indexPressed){
            return indexReleased + 1
        }
        else if(halfDown & indexReleased < indexPressed)
        {
            return  indexReleased + 1
        }
        else return indexReleased
    }
    function allow2out() {
        var allowArr = []
        var allShouldMove_pretext = []
        var temArr = []
        var counter = 0

        var pure_arr = []
        pure_arr = groupTrackLight.slice()
        var repeatMember = []
        for(i = 0; i < groupTrackLight.length; i++)
        {
            if(trackAudioModel.get(groupTrackLight[i]).type === 'Group')
            {
                // subset_members = groupTrackLight[i] // index of Group
                for(j = 0; j < trackAudioModel.get(groupTrackLight[i]).sub_set_children.count; j++)
                {
                    child_exist_index = pure_arr.indexOf(groupTrackLight[i] + j + 1)

                    if(child_exist_index > -1)
                    {
                        repeatMember.push(child_exist_index)
                        pure_arr.splice(child_exist_index, 1)
                    }

                }
            }// if Group.
        }// pure_audio_or_group.

        for(i = 0; i < pure_arr.length; i++){
            allShouldMove_pretext.push(trackAudioModel.get(pure_arr[i]).pretext)

            for(j = 0; j < trackAudioModel.get(pure_arr[i]).sub_set_children.count; j++)
                allShouldMove_pretext.push(trackAudioModel.get(pure_arr[i]).sub_set_children.get(j).member)
        }


//                for(j = 0; j < pure_audio_or_group.length; j++)
////                if(trackAudioModel.get(pure_audio_or_group[j]).type === 'Group')
//                for(i = 0; i < pure_audio_or_group[j]; i++)
//                {
//                    if(trackAudioModel.get(i).sub_set_children.count === allShouldMove_pretext.length)
//                    {
//                        if(allShouldMove_pretext.indexOf(trackAudioModel.get(i+1).pretext) > -1)
//                            allowArr.push(false)
//                        else
//                            allowArr.push(true)
//                    }
//                }



        for(i = 0; i < trackAudioModel.count; i++)
        {
            if(trackAudioModel.get(i).type === 'Group')
            {


                temArr.length = 0
                for(j = 0; j < trackAudioModel.get(i).sub_set_children.count; j++)
                temArr.push(trackAudioModel.get(i).sub_set_children.get(j).member)

                counter = 0
                for(k = 0; k < allShouldMove_pretext.length; k++)
                if(temArr.indexOf(allShouldMove_pretext[k]) > -1)
                {
                    counter++
                }

                if(trackAudioModel.get(i).sub_set_children.count === counter & allShouldMove_pretext.indexOf(trackAudioModel.get(i).pretext) === -1)
                    allowArr.push(false)
                else
                    allowArr.push(true)


            }
        }

        if(allowArr.indexOf(false) > -1) return false
        else return true;
    }
    function allow2out_dragging() {
        var allowArr = []
        var allShouldMove_pretext = []
        var temArr = []
        var counter = 0

        var _pure_audio_or_group = groupTrackLight.slice()
        var repeatMember = []
        for(i = 0; i < groupTrackLight.length; i++)
        {
            if(trackAudioModel.get(groupTrackLight[i]).type === 'Group')
            {
                // subset_members = groupTrackLight[i] // index of Group
                for(j = 0; j < trackAudioModel.get(groupTrackLight[i]).sub_set_children.count; j++)
                {
                    child_exist_index = _pure_audio_or_group.indexOf(groupTrackLight[i] + j + 1)

                    if(child_exist_index > -1)
                    {
                        repeatMember.push(child_exist_index)
                        _pure_audio_or_group.splice(child_exist_index, 1)
                    }

                }
            }// if Group.
        }// pure_audio_or_group.

        for(i = 0; i < _pure_audio_or_group.length; i++){
            allShouldMove_pretext.push(trackAudioModel.get(_pure_audio_or_group[i]).pretext)

            for(j = 0; j < trackAudioModel.get(_pure_audio_or_group[i]).sub_set_children.count; j++)
                allShouldMove_pretext.push(trackAudioModel.get(_pure_audio_or_group[i]).sub_set_children.get(j).member)
        }

        /*for(i = 0; i < _pure_audio_or_group[0]; i++)
        {
            if(trackAudioModel.get(i).sub_set_children.count === allShouldMove_pretext.length)
            {
                if(allShouldMove_pretext.indexOf(trackAudioModel.get(i+1).pretext) > -1)
                    allowArr.push(false)
                else
                    allowArr.push(true)
            }
        }*/

        for(i = 0; i < trackAudioModel.count; i++)
        {
            if(trackAudioModel.get(i).type === 'Group')
            {


                temArr.length = 0
                for(j = 0; j < trackAudioModel.get(i).sub_set_children.count; j++)
                temArr.push(trackAudioModel.get(i).sub_set_children.get(j).member)

                counter = 0
                for(k = 0; k < allShouldMove_pretext.length; k++)
                if(temArr.indexOf(allShouldMove_pretext[k]) > -1)
                {
                    counter++
                }

                if(trackAudioModel.get(i).sub_set_children.count === counter & allShouldMove_pretext.indexOf(trackAudioModel.get(i).pretext) === -1)
                    allowArr.push(false)
                else
                    allowArr.push(true)


            }
        }

        if(allowArr.indexOf(false) > -1) return false
        else return true;
    }
    // --------------------------------------------------------------------------------------------------------------------------------- C O N D I T I O N.
    /// F U N C T I O N.

    onPressed:
    {
        isAncherDrag = true
        ystart = mouseY
        indexPressed = trackListView.indexAt(2, mouseY + trackListView.contentY)
        if(groupTrackLight.length === 1) allow2clear = true
        else allow2clear = false

        if (groupTrackLight.indexOf(indexPressed) >= 0) { notMoving = true; return }
        else funcs.trackLighting(indexPressed, 'btn')
    }
onMouseYChanged: {//console.log('on y changed')}
    //onPositionChanged: {console.log('onPositionChanged')
        var vv = 0
        var jj = 0
        for(var i = 0; i < trackAudioModel.count; i++)
            if(trackAudioModel.get(i).isCollapsed)
            {
                vv += scaleSize2(18) + scaleSize2(37) + scaleSize2(56) * trackAudioModel.get(i).rowsExpanded+scaleSize2(5)
                jj++
            }
        outBound = vv+(jj-1)*trackListView.spacing < mouseY | mouseY > trackListView.height
        isBottom = outBound
        yBottom = vv
        //                console.log('ii================', vv+6*trackListView.spacing, mouseY)                       

        dragIndexRaw = trackListView.indexAt(2, mouseY + trackListView.contentY)


        if(isMemberOfGroup)
        {/*trackListRoot.cursorTrackListRoot = Qt.ForbiddenCursor; */


            _clipArea.setOverrideCursor(Qt.ForbiddenCursor)
            cursorPosition.forbiddenCursorShape()

        }

        if(dragIndexRaw > -1) indexDrag = dragIndexRaw
        if(indexPressed === indexDrag)
            isPosMouseChange = false
        else
            isPosMouseChange = true


        /*if(dragIndexRaw > -1)
        trackAudioModel.setProperty(dragIndexRaw,'inDraggingCounter', false)
        if(dragIndexRaw > 0)
        trackAudioModel.setProperty(dragIndexRaw -1,'inDraggingCounter', false)*/
        for(k = 0; k < trackAudioModel.count; k++)
            trackAudioModel.setProperty(k,'inDraggingCounter', -1)


        if(indexDrag > -1 & Math.abs(ystart-mouseY) > 2)
        {
            if(!isMemberOfGroup)
            {/*trackListRoot.cursorTrackListRoot = Qt.DragCopyCursor;*/

                _clipArea.setOverrideCursor(Qt.DragCopyCursor)

            }

            var itemY = trackListView.itemAtIndex(indexDrag).height + trackListView.itemAtIndex(indexDrag).y /*+ trackListRoot.y*/ - trackListView.contentY
            var itemY_2 = Math.round(trackListView.itemAtIndex(indexDrag).height / 2) /*2*/ + trackListView.itemAtIndex(indexDrag).y - trackListView.contentY
            var posy


            index_between_1 = -1
            index_between_2 = -1

            if(itemY_2 > mouseY & allow2out())
            {
                notMoving = false
                for(j = 0; j < trackAudioModel.count; j++)
                    trackAudioModel.setProperty(j, 'coverBound', false)
                // half up
                halfUp = true
                halfDown = false
                if(dragIndexRaw === -1) isTop = true


                if(dragIndexRaw > 0)
                {
                    let curArr = listModel2Array(trackAudioModel.get(dragIndexRaw-1).in_set_parent)
                    let nextArr = listModel2Array(trackAudioModel.get(dragIndexRaw).in_set_parent)
                    let difference = curArr.filter(x => !nextArr.includes(x))
//console.log('*^', difference.length)
                    if(difference.length === trackAudioModel.get(dragIndexRaw-1).in_set_parent.count)
                    trackAudioModel.setProperty(dragIndexRaw -1,'inDraggingCounter', trackAudioModel.get(dragIndexRaw-1).in_set_parent.count)
                    else
                    trackAudioModel.setProperty(dragIndexRaw -1,'inDraggingCounter', 0)
                }


                index_between_1 = indexDrag
                index_between_2 = indexDrag-1
                //                        console.log('up', index_between_1, index_between_2, ',  posy', posy)
                indexDesired = index_between_1
                if(index_between_1 === 0) indexDesired = 0
            }
            else if(itemY_2 <= mouseY & allow2out())
            {
                notMoving = false
                // half down
                halfUp = false
                halfDown = true
                isTop = false

                if(dragIndexRaw > -1)
                {
                    if(dragIndexRaw < trackAudioModel.count - 1)
                    {                //console.log('number: ', Math.floor(mouse.x / scaleSize2(50)))
                        let curArr = listModel2Array(trackAudioModel.get(dragIndexRaw).in_set_parent)
                        let nextArr = listModel2Array(trackAudioModel.get(dragIndexRaw+1).in_set_parent)
                        let difference = curArr.filter(x => !nextArr.includes(x))

                        let const_ = trackAudioModel.get(dragIndexRaw).type === 'Group' ?
                                trackAudioModel.get(dragIndexRaw).in_set_parent.count + (trackAudioModel.get(dragIndexRaw).rowsExpanded > 0 ? 2 : 1):
                                trackAudioModel.get(dragIndexRaw).in_set_parent.count

                        var floorMouseX = Math.floor(mouse.x / scaleSize2(50))
                        var m = -1
                        if(!globalValues.tracksOnLeft)
                        {
                            m = mouse.x - (trackListView.width - const_ * scaleSize2(50))
                            if(m >= 0)
                            floorMouseX = const_ - Math.ceil(m / scaleSize2(50))
                            else floorMouseX = const_ + 1
                        }
                        // console.log('floorMouseX: ', floorMouseX, m)

                        var limit_draggingCounter = trackAudioModel.get(dragIndexRaw).in_set_parent.count - floorMouseX
                        limit_draggingCounter = limit_draggingCounter > 0 ? limit_draggingCounter : 0

                        trackAudioModel.setProperty(dragIndexRaw,'inDraggingCounter', limit_draggingCounter/*Math.min(limit_draggingCounter, difference.length)*/)
                    }
                    else if(dragIndexRaw === trackAudioModel.count - 1)
                    {
                        limit_draggingCounter = trackAudioModel.get(dragIndexRaw).in_set_parent.count - floorMouseX
                        limit_draggingCounter = limit_draggingCounter > 0 ? limit_draggingCounter : 0

                        trackAudioModel.setProperty(dragIndexRaw,'inDraggingCounter', limit_draggingCounter)
                    }


                }

                index_between_1 = indexDrag
                index_between_2 = indexDrag+1
                //                        console.log('down', index_between_1, index_between_2, ',  posy', posy)
                indexDesired = index_between_2
                //                        if(index_between_1 === trackAudioModel.count - 1) indexDesired = trackAudioModel.count
                if(trackAudioModel.get(indexDrag).rowsExpanded === 0 & trackAudioModel.get(indexDrag).type === 'Group' &
                        /*halfDown === false &*/ dragIndexRaw > -1)
                    trackAudioModel.setProperty(indexDrag, 'coverBound', true)
                if(dragIndexRaw < 0) trackAudioModel.setProperty(indexDrag, 'coverBound', false)
                //                        console.log('hi', indexDrag, trackAudioModel.get(indexDrag).rowsExpanded, trackAudioModel.get(indexDrag).type, trackAudioModel.get(indexDrag).in_set_parent.count)
            }

            if(!allow2out_dragging())
            {
                /*trackListRoot.cursorTrackListRoot = Qt.ForbiddenCursor;*/

                _clipArea.setOverrideCursor(Qt.ForbiddenCursor)

            }
        }

        if(mouseY < requireSizeForAutoScroll & trackListView.contentY > 0)
        {
            timer2Down.stop()
            if(!trackListView.atYBeginning)
            {
                trackListView.contentY--
            }
            timer2Up.restart()
        }
        else if(mouseY > (height - requireSizeForAutoScroll))
        {
            timer2Up.stop()
            if(!trackListView.atYEnd)
            {
                trackListView.contentY++
            }
            timer2Down.restart()
        }
        else {
            timer2Up.stop()
            timer2Down.stop()
        }
    }

    onDoubleClicked: {
        if(groupTrackLight.length === 1 & !holdCtrl & !holdShift & !isPosMouseChange & allow2clear)
        {
            trackAudioModel.setProperty(indexPressed, 'light', false)
            groupTrackLight.length = 0
            funcs.offLightAllTracks()
        }
    }
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    onReleased: {
        isTop = false
        isBottom = false
        notArrayParent_index.length = 0
        notArrayParent_pretext.length = 0
        allParents_index.length = 0
        allParents_pretext.length = 0
        if(indexDrag > -1)
        notArrayParent_index = findNotParent(indexDrag, trackAudioModel.get(indexDrag).inDraggingCounter)
        for(i = 0; i < notArrayParent_index.length; i++)
            notArrayParent_pretext.push(trackAudioModel.get(notArrayParent_index[i]).pretext)
//        console.log(':::::', trackAudioModel.get(indexDrag).inDraggingCounter, 'notArrayParent_index:', notArrayParent_index, 'notArrayParent_pretext:', notArrayParent_pretext)
//console.log('drag counter: ', trackAudioModel.get(indexDrag).inDraggingCounter)

        if(notArrayParent_index.length > 0)
        {
            var guideArr = findNotParent2(indexDrag)

            newGuideParent_index = guideArr[0]
            newGuideParent_pretext = guideArr[1]
            if(newGuideParent_index > -1)
            guideLineMethod()

            for(i = 0; i < trackAudioModel.count; i++)
                trackAudioModel.setProperty(i, 'coverBound', false)
            /*trackListRoot.cursorTrackListRoot = Qt.ArrowCursor;*/

            _clipArea.setOverrideCursor(Qt.ArrowCursor)



            timer2Up.stop()
            timer2Down.stop()
            all_movement_member_index.length = 0
            all_movement_member_pretext.length = 0
            pure_audio_or_group.length = 0

    //console.log('indexDrag: ', indexDrag, ', indexDesired: ', indexDesired)
            indexReleased = indexDrag
            /*if(indexReleased < 0) {
                for(k = 0; k < trackAudioModel.count; k++)
                    trackAudioModel.setProperty(k,'inDraggingCounter', -1)
                return
            }*/
            if(indexReleased > -1)
            indexReleased_rowsExpanded = trackAudioModel.get(indexReleased).rowsExpanded

            if(indexDesired === -1 ) indexDesired = indexReleased

            if(indexReleased > -1)
            pretext_indexReleased = trackAudioModel.get(indexReleased).pretext
            pretext_indexDesired = halfDown ? trackAudioModel.get(indexDesired-1).pretext : trackAudioModel.get(indexDesired).pretext

            if(newGuideParent_index > -1)
            initialValue()
            if(newGuideParent_index > -1)
            return;
        }
//console.log(':::::::::::::::::::::::::::::::::::::::::::::::::::::::::::')

//        console.log('trackAudioModel.get(indexDrag).inDraggingCounter: ', trackAudioModel.get(indexDrag).inDraggingCounter)


        _clipArea.coverBound = false
        /*trackListRoot.cursorTrackListRoot = Qt.ArrowCursor;*/

        _clipArea.setOverrideCursor(Qt.ArrowCursor)
        cursorPosition.onlyShowCursor()

        timer2Up.stop()
        timer2Down.stop()
        all_movement_member_index.length = 0
        all_movement_member_pretext.length = 0
        pure_audio_or_group.length = 0

//console.log('indexDrag: ', indexDrag, ', indexDesired: ', indexDesired)
        indexReleased = indexDrag
        /*if(indexReleased < 0) {
            for(k = 0; k < trackAudioModel.count; k++)
                trackAudioModel.setProperty(k,'inDraggingCounter', -1)
            return
        }*/
        if(indexReleased > -1)
        indexReleased_rowsExpanded = trackAudioModel.get(indexReleased).rowsExpanded

        if(indexDesired === -1 ) indexDesired = indexReleased

        if(indexReleased > -1)
        pretext_indexReleased = trackAudioModel.get(indexReleased).pretext
        pretext_indexDesired = halfDown ? trackAudioModel.get(indexDesired-1).pretext : trackAudioModel.get(indexDesired).pretext

        arrayDeck()

        var retProperty = return4property()
        var retEnd = endRow()
        var retFirst = firstRow()
        if(retProperty | retEnd | retFirst)
        {console.log('return: ', retProperty, retEnd , retFirst)
            initialValue()
            rearray()
        }
        else
        {//console.log('->-<:', 'clearing_insets_member_pretext:', clearing_insets_member_pretext)
            for(var tt = 0; tt < all_movement_member_index.length; tt++)
            {
                console.log('0-0',trackAudioModel.get(all_movement_member_index[tt]).pretext, trackAudioModel.get(all_movement_member_index[tt]).isCollapsed, trackAudioModel.get(all_movement_member_index[tt]).rowsExpanded)
            }

            remove_subset_inset()
            add_Copy()
            set_expendedLine()
            endGrouping()

            for(tt = 0; tt < all_movement_member_index.length; tt++)
            {
                console.log('0-1',trackAudioModel.get(all_movement_member_index[tt]).pretext, trackAudioModel.get(all_movement_member_index[tt]).isCollapsed, trackAudioModel.get(all_movement_member_index[tt]).rowsExpanded)
            }
        }

        var retMovement = return4moving()
        if(retMovement | retEnd)
        {
            halfDown = false
            halfUp = false
            isAncherDrag = false
        }
        else
        {
            moveTracks()
            rearray()
        }
//        console.log('::groupTrackLight:', groupTrackLight)
        for(k = 0; k < trackAudioModel.count; k++)
            trackAudioModel.setProperty(k,'inDraggingCounter', -1)
    }// finish.


    function initialValue() {
        halfDown = false
        halfUp = false
        isAncherDrag = false
        for(k = 0; k < trackAudioModel.count; k++)
            trackAudioModel.setProperty(k,'inDraggingCounter', -1)
    }

    // --------------------------------------------------------------------------------------------------------------------------------- T O O L S:
    function _findIndex(model, criteria) {
        for(var i = 0; i < model.count; ++i) if (criteria(model.get(i))) return i
        return null
    }
    function _findElementModel(model, criteria) {
        for(var i = 0; i < model.count; ++i) if (criteria(model.get(i))) return model.get(i)
        return null
    }
    function findIndex(model, str, attribute){
        if(model.count < 1) return null
        return _findIndex(model, function(item) { return item[attribute] === str })
    }
    function findElementModel(model, str, attribute) {
        if(model.count < 1) return null
        return _findElementModel(model, function(item) { return item[attribute] === str })
    }
    function listModel2Array(model) {
        var arr = []
        for(var i = 0; i < model.count; i++)
        {
            arr.push(model.get(i).member)
        }
        return arr
    }

    function add2child_parent() {
        if(allParents_pretext.length === 0) return

        for(j = 0; j < trackAudioModel.count; j++)
            if(allParents_pretext.indexOf(trackAudioModel.get(j).pretext) > -1)
            allParents_index.push(j)

        for(j = 0; j < allParents_index.length; j++)
        {
            for(i = 0; i < all_movement_member_pretext.length; i++)
            {
                if(notArrayParent_index.indexOf(allParents_index[j]) < 0 & allParents_index[j] !== all_movement_member_index[i] &
                        findIndex(trackAudioModel.get(allParents_index[j]).sub_set_children, all_movement_member_pretext[i], 'member') === null)
                trackAudioModel.get(allParents_index[j]).sub_set_children.append({member: all_movement_member_pretext[i]})
            }
        }

        for(i = 0; i < all_movement_member_index.length; i++)
        {
            for(j = 0; j < allParents_pretext.length; j++)
            {
                if(notArrayParent_pretext.indexOf(allParents_pretext[j]) < 0 & allParents_pretext[j] !== all_movement_member_pretext[i] &
                        findIndex(trackAudioModel.get(all_movement_member_index[i]).in_set_parent, allParents_pretext[j], 'member') === null)
                trackAudioModel.get(all_movement_member_index[i]).in_set_parent.append({member: allParents_pretext[j]})
            }
        }
    }
    // --------------------------------------------------------------------------------------------------------------------------------- T O O L S.

    function guideLineMethod() {//console.log('in guiding method!')
        arrayDeck()

        var newParents_index = []
        var newParents_pretext = []
        for(i = 0; i < trackAudioModel.get(newGuideParent_index).in_set_parent.count; i++)
            newParents_pretext.push(trackAudioModel.get(newGuideParent_index).in_set_parent.get(i).member)
        for(i = 0; i < trackAudioModel.count; i++)
        {
            for(j = 0; j < newParents_pretext.length; j++)
            if(trackAudioModel.get(i).pretext === newParents_pretext[j])
                newParents_index.push(i)
        }console.log("::.::", newGuideParent_index, newGuideParent_pretext, newParents_index, newParents_pretext)
        // parents.

        remove_subset_inset()

        // ADD or COPY:
        for(i = 0; i < all_movement_member_index.length; i++)
        {
            for(j = 0; j < newParents_pretext.length; j++)
            {
                trackAudioModel.get(all_movement_member_index[i]).in_set_parent.append({member: newParents_pretext[j]})
                trackAudioModel.get(newParents_index[j]).sub_set_children.append({member: all_movement_member_pretext[i]})
            }
        }
        // ADD or COPY.


        moveTracks()
        rearray()
    }

}
