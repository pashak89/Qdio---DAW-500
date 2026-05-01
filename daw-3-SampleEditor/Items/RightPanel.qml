import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.2

Rectangle {
    id: root

    signal action(string text)

    property var rightModules: rightPanel.rightModules
    property var leftModules: rightPanel.leftModules
    radius: topRadius

    color: bgColr

    property color lineClr: '#FFFFFF'

    property color bgColr: '#2F3032'

    property Item panel: null

    property bool isLine: false

    property bool twoGroups: false

    property string firstGroup: ''
    property string rightFirstGroup: ''

    property int parentHeight: {
        if (twoGroups) {
            rightModules.length * cellHeight + cellHeight + splitter.height
                    + (rightModules.length - 1) * smallSpace + bigSpace * 2
        } else {
            rightModules.length * cellHeight + (rightModules.length - 1) * smallSpace
        }
    }
    function resize() {

        if (twoGroups) {
            parentHeight = leftModules.length * rPanel.cellHeight + rPanel.cellHeight
                    + splitter.height + (leftModules.length - 1) * smallSpace + bigSpace * 2
        } else {
            parentHeight = leftModules.length * cellHeight + (leftModules.length - 1) * smallSpace
        }
    }

    property int widthSize: testSize.contentWidth + testSize2.contentWidth
    property int heightSize: testSize.contentHeight

    property int parentWidth: scaleSize2(480) //widthSize + scaleSize2(50)

    property int margin: topRadius + 4 * topRadius * 1 / leftModules.length

    property int cellHeight: heightSize

    property int smallSpace: 2

    property int bigSpace: scaleSize2(7)

    // SIGNAL :
    signal messageCall(string msg, int compId)

    ColumnLayout {
        id: col
        y: margin

        spacing: bigSpace

        ShortcutRow {
            visible: twoGroups
            Layout.preferredHeight: cellHeight
            Layout.preferredWidth: parentWidth
            leftTitle: firstGroup
            rightTitle: rightFirstGroup
            originParent: root

            onTriggered: {

                if (leftTitle === 'Split Stereo Pan')
                    panel.splitStereo = true
                if (leftTitle === 'Stereo Pan Mode')
                    panel.splitStereo = false

                rightPanel.close()
                //                messageCall(leftTitle, 1)
            }
        }

        Rectangle {
            id: splitter
            visible: twoGroups
            Layout.preferredHeight: 1
            Layout.preferredWidth: parentWidth
            color: lineClr
        }

        ColumnLayout {
            id: col2
            spacing: smallSpace
            Repeater {
                model: leftModules
                delegate: ShortcutRow {
                    Layout.preferredHeight: cellHeight
                    Layout.preferredWidth: parentWidth
                    leftTitle: leftModules[index] === undefined ? '' : leftModules[index]
                    rightTitle: rightModules[index] === undefined ? '' : rightModules[index]
                    originParent: root

                    onTriggered: {

                        action(leftTitle)
                        rightPanel.close()
                    }
                }
            }
        }
    }

    //    ShortcutRow {
    //        id: testSize
    //        visible: false
    //        height: cellHeight
    //        width: 10
    //        leftTitle: 'Show Automation in New Lane'
    //        rightTitle: 'rightFirstGroup'
    //        originParent: root
    //    }
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
        text: rightModules[rightModules.length - 1]
    }

    Rectangle {
        anchors.fill: root
        //        anchors.margins: -1
        radius: root.radius
        color: 'transparent'
        border.color: lineClr
    }
}
