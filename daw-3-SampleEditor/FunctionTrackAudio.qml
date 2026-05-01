import QtQuick 2.3

Item {

    function offLightAllTracks() {console.log('start offLightAllTracks func')
//        if (trackRightClick.visible)
//            trackRightClick.visible = false
//        if (rightPanel.visible)
//            rightPanel.visible = false

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
        return
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
//        for(var i = 0; i < trackAudioModel.count; i++)
//        {
//            if(trackAudioModel.get(i).type === 'Group' & groupTrackLight.indexOf(i) < 0)
//            {
//                for(var j = 1; j <= trackAudioModel.get(i).sub_set_children.count; j++)
//                    childrenArr.push(i+j)

//                if(groupTrackLight.length > childrenArr.length)
//                {
//                    for(var l = 0; l < groupTrackLight.length; l++)
//                    for(var k = 0; k < childrenArr.length; k++)
//                    {
//                        if(childrenArr[k] > groupTrackLight[l] & childrenArr.indexOf(groupTrackLight[l]) < 0)
//                            resultArr.push(!checker(groupTrackLight, childrenArr))
//                    }
//                }
//            }
////            console.log('*groupTrackLight:', groupTrackLight, 'childrenArr:', childrenArr)
//            childrenArr.length = 0
//        }

        if(resultArr.indexOf(false) > -1) result = false
        else result = true
//console.log('*: ', result)
        return result
    }

}
