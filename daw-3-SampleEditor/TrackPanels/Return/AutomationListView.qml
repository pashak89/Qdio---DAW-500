import QtQuick 2.12
import 'Automation/'

Item {
    height: automationListViewPanel.height
    width: parent.width

    MouseArea {
        anchors.fill: automationListViewPanel; anchors.topMargin: -scaleSize2(15)
        property int fromIndex: -1
        property int toIndex: -1

        onPressed: {
            if(automationChildren > 1) cursorPosition.dragCursorShape()

            if(pressed & automationChildren > 1 & fromIndex === -1)
                fromIndex = automationListViewPanel.indexAt(10, mouse.y + automationListViewPanel.contentY)
        }

        onMouseYChanged:
        {
            var indexDrop = automationListViewPanel.indexAt(10, mouse.y + automationListViewPanel.contentY)
            if(pressed & automationChildren > 1 & fromIndex !== -1 & indexDrop !== fromIndex & indexDrop !== -1)
            {
                automationListViewPanel.automationIndexDrop = indexDrop
            }
            else
                automationListViewPanel.automationIndexDrop = -1
        }

        onReleased: {
            cursorPosition.onlyShowCursor()
            toIndex = automationListViewPanel.indexAt(10, mouse.y + automationListViewPanel.contentY)

            if(automationChildren > 1 & fromIndex > -1 & toIndex > -1 & fromIndex !== toIndex)
            {
                trackReturnModel.get(ind).automationList.move(fromIndex, toIndex, 1)
            }

            fromIndex = -1
            toIndex = -1
            automationListViewPanel.automationIndexDrop = -1
        }
    }

    ListView {
        id: automationListViewPanel

        property int automationIndexDrop: -1

        onFocusChanged: console.log('-*-', focus)
        width: parent.width
        height: contentItem.childrenRect.height

        flickableDirection: Flickable.VerticalFlick
        interactive: false
        boundsMovement: Flickable.StopAtBounds

        spacing: scaleSize2(15)

        model: trackReturnModel.get(ind).automationList
        delegate: AutomationChild { }
    }

}
