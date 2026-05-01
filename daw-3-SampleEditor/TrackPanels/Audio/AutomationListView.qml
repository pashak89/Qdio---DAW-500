import QtQuick 2.12
import "Automation/"

Rectangle {
    height: automationListViewPanel.height
    width: parent.width
    color: "transparent"

    MouseArea {
        anchors.fill: automationListViewPanel
        anchors.topMargin: -scaleSize2(15)
        property int fromIndex: -1
        property int toIndex: -1

        onPressed: {

            //            if (automationLaneCount > 1)
            //                cursorPosition.dragCursorShape()
            if (pressed & automationLaneCount > 1 & fromIndex === -1)
                fromIndex = automationListViewPanel.indexAt(
                            10, mouse.y + automationListViewPanel.contentY)
        }

        onMouseYChanged: {
            var indexDrop = automationListViewPanel.indexAt(
                        10, mouse.y + automationListViewPanel.contentY)
            if (pressed & automationLaneCount > 1 & fromIndex !== -1 & indexDrop
                    !== fromIndex & indexDrop !== -1) {
                automationListViewPanel.automationIndexDrop = indexDrop
            } else
                automationListViewPanel.automationIndexDrop = -1
        }

        onReleased: {
            cursorPosition.onlyShowCursor()
            toIndex = automationListViewPanel.indexAt(
                        10, mouse.y + automationListViewPanel.contentY)

            if (automationLaneCount > 1 & fromIndex > -1 & toIndex > -1 & fromIndex !== toIndex) {
                trackAudioModel.get(ind).automationList.move(fromIndex,
                                                             toIndex, 1)
            }

            fromIndex = -1
            toIndex = -1
            automationListViewPanel.automationIndexDrop = -1
        }
    }

    ListView {
        id: automationListViewPanel

        property int automationIndexDrop: -1
        height: contentItem.childrenRect.height
        onFocusChanged: console.log('-*-', focus)
        width: parent.width

        //height: parent.height
        flickableDirection: Flickable.VerticalFlick
        interactive: false
        boundsMovement: Flickable.StopAtBounds

        model: _clipArea.tracksModel.automationLaneModel(ind)
        delegate: AutomationChild {

            color: "transparent"
            trackIndex: ind

            //effectList: model.EffectList
        }
    }
}
