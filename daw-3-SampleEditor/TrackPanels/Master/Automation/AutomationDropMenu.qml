import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import 'qrc:/Items'
import 'qrc:/'

Item {
    id: root

    Action { id: actionVolume; text: qsTr("Volume"); checked: isVolume_Automated }
    Action { id: actionPan; text: qsTr("Pan"); checked: isPan_Automated }
    Action { id: actionSpeakerOn; text: qsTr("Speacker On"); checked: isSpeackerOn_Automated }

    property bool isVolume_Automated: false
    // onIsVolume_AutomatedChanged: {
    //     if(isVolume_Automated) all.addAction(actionVolume)
    //     else all.removeAction(actionVolume)
    // }

    property bool isPan_Automated: false
    // onIsPan_AutomatedChanged: {
    //     if(isPan_Automated) all.addAction(actionPan)
    //     else all.removeAction(actionPan)
    // }

    property bool isSpeackerOn_Automated: false
    // onIsSpeackerOn_AutomatedChanged: {
    //     if(isSpeackerOn_Automated) all.addAction(actionSpeakerOn)
    //     else all.removeAction(actionSpeakerOn)
    // }

    property string displayMenu: qsTr("None")

    property bool isRootMenu: false

    property color brColorDisplay:  theme.s11_Enable_BRD
    property color brColor:         theme.a2
    property color flColor:         theme.s11_Enable_FIL

    property real fontSize:         topfontPixelSize
    property string fontFamilty:    topFontFamily
    property color fontColor:       theme.tColor

    property int space: scaleSize2(6)

    property int borderWidth: 1

    property var myTheme: theme
    onMyThemeChanged: {
        indicator.requestPaint()
    }

    focus: false
    property bool didShadow:   false
    // property bool popupFocus: popup.opened

    // onPopupFocusChanged: {
    //     if(!popupFocus) {
    //         didShadow = false
    //     }
    // }

    onActiveFocusChanged: { if(root.focus) didShadow = true }
    onDidShadowChanged: if(didShadow) focus = true


    function checkMenuIteration(idTxt) {
        var result = false
        for(var i = 0; i < automationChildren; i++) {
            result = trackMasterModel.get(ind).automationList.get(i).displayText === idTxt
            if(result) break
        }
        return result
    }


    Label { id: bigTxt ;visible: false; text: 'Show All Automated'; font { family: fontFamilty; pixelSize: fontSize; weight: Font.DemiBold }}
    Label { id: bigTxt2 ;visible: false; text: 'Speaker On'; font: bigTxt.font }


    Rectangle {
        id: menu

        anchors.fill: parent
        radius: topRadius

        color: flColor
        border.color: brColorDisplay
        border.width: topWidth

        Text {
            anchors {
                left: parent.left; leftMargin: scaleSize2(10)
            }

            text: displayMenu
            height: parent.height //- parent.height / 4
            width: parent.width - height
            font: bigTxt.font
            color: fontColor
            verticalAlignment: Text.AlignVCenter
        }

        Canvas {
            id: indicator

            anchors {
                right: parent.right
                rightMargin: scaleSize2(9)
                verticalCenter: parent.verticalCenter
            }

            width: scaleSize2(19)
            height: scaleSize2(11)
            contextType: "2d"

            onPaint: {
                context.reset();
                context.moveTo(0, 0);
                context.lineTo(width, 0);
                context.lineTo(width / 2, height);
                context.closePath();
                context.fillStyle = fontColor
                context.fill();
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: { didShadow = true ; mainMenu.open();  }
        }
    }


    //---------------------------------------------------------------------------------------------------------------------------- Main Popup

    Menu {
        id: mainMenu

        spacing: space

        onOpenedChanged: {
            if(opened) {
                didShadow = true

                // TODO 6 lines code is for test only
                /*all.removeAction(actionVolume)
                all.removeAction(actionPan)
                all.removeAction(actionSpeakerOn)

                if(isVolume_Automated) all.addAction(actionVolume)
                if(isPan_Automated) all.addAction(actionPan)
                if(isSpeackerOn_Automated) all.addAction(actionSpeakerOn)*/
            }

            else didShadow = false
            // if(!opened & isRootMenu) displayMenu = 'None'
        }

        x: menu.x - shadowRec.borderWidth/2
        y: menu.y + menu.height + 1

        // Action {
        //     text: qsTr("None")
        //     anchors.verticalCenter: parent.verticalCenter
        // }

        MenuItem {
            id: none
            text: 'None'
            height: scaleSize2(40)
            padding: 0

            enabled: isRootMenu

            contentItem:  Rectangle {
                height: scaleSize2(40); width: backgroundMenu.width
                color:  none.highlighted ? fontColor : flColor
                Text {
                    height: scaleSize2(40)
                    leftPadding: scaleSize2(10)
                    text: 'None'
                    font: bigTxt.font
                    opacity: enabled ? 1.0 : 0.3
                    color: none.highlighted ? flColor : fontColor
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }

            background: Rectangle {
                width: parent.width
                height: scaleSize2(40)
                opacity: enabled ? 1 : 0.3
                color: none.highlighted ? fontColor : flColor
            }

            onTriggered: displayMenu = text
        }



        Menu {
            title: "Mixer"

            spacing: space

            Action {
                text: qsTr("Volume")
                checked: isVolume_Automated
                onTriggered: {
                    displayMenu = text
                    if(!isRootMenu) trackMasterModel.get(ind).automationList.setProperty(index, 'displayText', displayMenu)
                }
            }
            Action {
                text: qsTr("Pan")
                checked: isPan_Automated
                onTriggered: {
                    displayMenu = text
                    if(!isRootMenu) trackMasterModel.get(ind).automationList.setProperty(index, 'displayText', displayMenu)
                }
            }
            Action {
                text: qsTr("Speaker On")
                checked: isSpeackerOn_Automated
                onTriggered: {
                    displayMenu = text
                    if(!isRootMenu) trackMasterModel.get(ind).automationList.setProperty(index, 'displayText', displayMenu)
                }
            }

            topPadding: topRadius
            bottomPadding: topRadius
            leftPadding: root.borderWidth
            rightPadding: root.borderWidth

            onOpened: {
                actionAt(0).enabled = !checkMenuIteration(actionAt(0).text)
                actionAt(1).enabled = !checkMenuIteration(actionAt(1).text)
                actionAt(2).enabled = !checkMenuIteration(actionAt(2).text)
            }

            delegate: MenuItem {
                id: menuItemMixer
                implicitWidth: 300
                implicitHeight: scaleSize2(40)

                indicator: Item {
                    implicitWidth: scaleSize2(40) /2
                    implicitHeight: scaleSize2(40)
                    Rectangle {
                    visible: menuItemMixer.checked
                    anchors {
                        left: parent.left; leftMargin: scaleSize2(12)
                        verticalCenter: parent.verticalCenter
                    }

                    width: scaleSize2(12)
                    height: width; radius: height/2
                    color: 'gold'
                    border.color: 'black'
                    border.width: root.borderWidth
                }}

                contentItem: Text {
                    leftPadding: menuItemMixer.indicator.width + scaleSize2(5)
                    text: menuItemMixer.text
                    font: bigTxt.font
                    opacity: enabled ? 1.0 : 0.3
                    color: menuItemMixer.highlighted ? flColor : fontColor
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    opacity: enabled ? 1 : 0.3
                    color: menuItemMixer.highlighted ? fontColor : flColor
                }
            }

            background: CustomRectangle {
                implicitWidth: bigTxt2.contentWidth + scaleSize2(15) * 3 + scaleSize2(40)/2
                implicitHeight: scaleSize2(40)
                color: flColor
                borderColor: brColor
                borderWidth: root.borderWidth
                corners: [
                        Qt.AlignLeft | Qt.AlignBottom,
                        Qt.AlignRight | Qt.AlignTop,
                        Qt.AlignRight | Qt.AlignBottom,
                    ]
                radius: topRadius
            }
        }


        /*MenuSeparator {
            padding: 0
            contentItem: Rectangle {
                implicitWidth: backgroundMenu.width
                implicitHeight: 1
                color: brColor
            }
        }*/


        /*Menu {
            id: all
            spacing: space
            title: qsTr("Show All Automated")

            // Action { text: qsTr("Volume"); checked: isVolume_Automated }
            // Action { text: qsTr("Pan"); checked: isPan_Automated }
            // Action { text: qsTr("Speacker On"); checked: isSpeackerOn_Automated }

            onOpenedChanged: if(!isVolume_Automated & !isPan_Automated & !isSpeackerOn_Automated) close()

            topPadding: topRadius
            bottomPadding: topRadius
            leftPadding: root.borderWidth
            rightPadding: root.borderWidth

            delegate: MenuItem {
                id: menuItemAutomated
                implicitWidth: 50
                implicitHeight: scaleSize2(40)

                indicator: Item {
                    implicitWidth: scaleSize2(40) /2
                    implicitHeight: scaleSize2(40)
                    Rectangle {
                    visible: menuItemAutomated.checked
                    anchors {
                        left: parent.left; leftMargin: scaleSize2(12)
                        verticalCenter: parent.verticalCenter
                    }

                    width: scaleSize2(12)
                    height: width; radius: height/2
                    color: 'gold'
                    border.color: 'black'
                    border.width: root.borderWidth
                }}

                contentItem: Text {
                    leftPadding: menuItemAutomated.indicator.width + scaleSize2(5)
                    text: menuItemAutomated.text
                    font: bigTxt.font
                    opacity: enabled ? 1.0 : 0.3
                    color: menuItemAutomated.highlighted ? flColor : fontColor
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    implicitWidth: parent.width
                    implicitHeight: parent.height
                    opacity: enabled ? 1 : 0.3
                    color: menuItemAutomated.highlighted ? fontColor : flColor
                }
            }

            background: CustomRectangle {
                implicitWidth: bigTxt2.contentWidth + scaleSize2(15) * 3 + scaleSize2(40)/2
                implicitHeight: scaleSize2(40)
                color: flColor
                borderColor: brColor
                borderWidth: root.borderWidth
                corners: [
                        Qt.AlignLeft | Qt.AlignBottom,
                        Qt.AlignRight | Qt.AlignTop,
                        Qt.AlignRight | Qt.AlignBottom,
                    ]
                radius: topRadius
            }
        }*/

        topPadding: topRadius
        bottomPadding: topRadius
        leftPadding: root.borderWidth
        rightPadding: root.borderWidth

        delegate: MenuItem {
            id: menuItem
            implicitHeight: scaleSize2(40)
            implicitWidth: backgroundMenu.width
            padding: 0

            onHighlightedChanged: menuItemArrow.requestPaint()

            arrow: Canvas {
                id: menuItemArrow

                visible: !isVolume_Automated & !isPan_Automated & !isSpeackerOn_Automated &
                         menuItem.text.indexOf('Show All Automated') >= 0 ? false : menuItem.text !== 'None'
                anchors {
                    right: parent.right
                    rightMargin: scaleSize2(15)
                    verticalCenter: parent.verticalCenter
                }

                width: scaleSize2(11)
                height: scaleSize2(19)
                contextType: "2d"

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();
                    ctx.moveTo(0, 0);
                    ctx.lineTo(width, height/2);
                    ctx.lineTo(0, height);
                    ctx.closePath();
                    ctx.fillStyle = menuItem.highlighted ? flColor : fontColor
                    ctx.fill();
                }
            }

            indicator: Item {
                implicitWidth: scaleSize2(40) /2
                implicitHeight: scaleSize2(40)
                Rectangle {
                visible: (isVolume_Automated | isPan_Automated | isSpeackerOn_Automated) & menuItem.text !== 'None'
                anchors {
                    left: parent.left; leftMargin: scaleSize2(12)
                    verticalCenter: parent.verticalCenter
                }

                width: scaleSize2(12)
                height: width; radius: height/2
                color: 'gold'
                border.color: 'black'
                border.width: root.borderWidth
            }}

            contentItem: Text {
                id: txtMenu
                height: scaleSize2(40)
                leftPadding: menuItem.text === 'None' ? scaleSize2(10) :
                                                        isVolume_Automated | isPan_Automated | isSpeackerOn_Automated ?
                                                            menuItem.indicator.width + scaleSize2(5) + scaleSize2(12) : scaleSize2(10)
                text: menuItem.text
                font: bigTxt.font
                opacity: enabled ? 1.0 : 0.3
                color: menuItem.highlighted ? flColor : fontColor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                width: parent.width
                height: scaleSize2(40)
                opacity: enabled ? 1 : 0.3
                color: menuItem.highlighted ? fontColor : flColor
            }
        }

        background: CustomRectangle {
            id: backgroundMenu
            implicitWidth: root.width + topWidth * 2//bigTxt.contentWidth + scaleSize2(11) + scaleSize2(15) *5 + root.borderWidth *2
            implicitHeight: scaleSize2(40)
            color: flColor
            borderColor: brColor
            borderWidth: root.borderWidth
            corners: Qt.AlignLeft | Qt.AlignRight | Qt.AlignBottom
            radius: topRadius
        }
    }


    Component {
        id: menuComponent
        MenuItem {
            id: menuItemComponent
            implicitWidth: 300
            implicitHeight: scaleSize2(40)

            indicator: Item {
                implicitWidth: scaleSize2(40) /2
                implicitHeight: scaleSize2(40)
                Rectangle {
                    visible: menuItemComponent.checked
                    anchors {
                        left: parent.left; leftMargin: scaleSize2(12)
                        verticalCenter: parent.verticalCenter
                    }

                    width: scaleSize2(12)
                    height: width; radius: height/2
                    color: 'gold'
                    border.color: 'black'
                    border.width: root.borderWidth
                }}

            contentItem: Text {
                leftPadding: menuItemComponent.indicator.width + scaleSize2(12)
                text: menuItemComponent.text
                font: bigTxt.font
                opacity: enabled ? 1.0 : 0.3
                color: menuItemComponent.highlighted ? flColor : fontColor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                implicitWidth: parent.width
                implicitHeight: parent.height
                opacity: enabled ? 1 : 0.3
                color: menuItemComponent.highlighted ? fontColor : flColor
            }
        }
    }


    CustomRectangle {
        id: shadowRec
        corners:         Qt.AlignLeft | Qt.AlignRight | Qt.AlignTop
        visible:         didShadow
        anchors.fill:    menu
        color:           'transparent'
        borderColor:    brColorDisplay
        borderWidth:    Math.min(topWidth *2, 2)
        anchors.margins: -1
        radius:          topRadius
        z: -1
    }

}
