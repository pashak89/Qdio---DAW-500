import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import TrackItem 1.0
import "qrc:/Items"
import "qrc:/TrackPanels/Audio"
import QtQml.Models 2.15

Item {
    id: root

    property string displayMenu: "Volume"

    property bool isRootMenu: false

    property color brColorDisplay: theme.s11_Enable_BRD
    property color brColor: theme.a2
    property color flColor: theme.s11_Enable_FIL

    property real fontSize: topfontPixelSize
    property string fontFamilty: topFontFamily
    property color fontColor: theme.tColor

    property int space: scaleSize2(6)

    property int borderWidth: 1

    property var myTheme: theme
    onMyThemeChanged: {
        indicator.requestPaint()
    }

    function menuItemVisible() {
        return false
    }

    signal sigChanged

    focus: false
    property bool didShadow: false

    onActiveFocusChanged: {
        if (root.focus)
            didShadow = true
    }
    onDidShadowChanged: if (didShadow)
                            focus = true

    function isAutomated(menu, subMenu) {

        return _clipArea.tracksModel.isAutomated(ind, menu, subMenu)
    }
    function isSelected(menu, subMenu) {

        return _clipArea.tracksModel.isSelected(ind, menu, subMenu)
    }

    function isEnabled(menu, subMenu) {

        return _clipArea.tracksModel.isEnabled(ind, menu, subMenu)
    }

    Label {
        id: bigTxt
        visible: false
        text: 'Show All Automated'
        font {
            family: fontFamilty
            pixelSize: fontSize
            weight: Font.DemiBold
        }
    }
    Label {
        id: bigTxt2
        visible: false
        text: 'Speaker On'
        font: bigTxt.font
    }

    Rectangle {
        id: menu
        enabled: isRootMenu
        anchors.fill: parent
        radius: topRadius

        color: flColor
        border.color: brColorDisplay
        border.width: topWidth

        Text {
            enabled: isRootMenu
            anchors {
                left: parent.left
                leftMargin: scaleSize2(10)
            }

            text: isRootMenu ? automationSubMenuTitle : AutomationLaneSubMenuTitle
            height: parent.height //- parent.height / 4
            width: parent.width - height
            font: bigTxt.font
            color: fontColor
            verticalAlignment: Text.AlignVCenter
        }

        Canvas {
            id: indicator
            visible: isRootMenu

            anchors {
                right: parent.right
                rightMargin: scaleSize2(9)
                verticalCenter: parent.verticalCenter
            }

            width: scaleSize2(19)
            height: scaleSize2(11)
            contextType: "2d"

            onPaint: {
                context.reset()
                context.moveTo(0, 0)
                context.lineTo(width, 0)
                context.lineTo(width / 2, height)
                context.closePath()
                context.fillStyle = fontColor
                context.fill()
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                didShadow = true
                mainMenu.open()
            }
        }
    }

    Component {
        id: menuComponent
        Menu {
            id: multiEqMenu
            spacing: space

            topPadding: topRadius
            bottomPadding: topRadius
            leftPadding: root.borderWidth
            rightPadding: root.borderWidth

            property ListModel effectMenuModel: ListModel {}
            function updateCheckedState() {
                for (var i = 0; i < effectMenuModel.count; i++) {
                    var item = effectMenuModel.get(i)
                    effectMenuModel.setProperty(i, "checked",
                                                isAutomated(multiEqMenu.title,
                                                            item.name))

                    effectMenuModel.setProperty(i, "active",
                                                !isSelected(multiEqMenu.title,
                                                            item.name))

                    effectMenuModel.setProperty(i, "enabled",
                                                isEnabled(multiEqMenu.title,
                                                          item.name))
                }
            }

            onOpened: {
                multiEqMenu.updateCheckedState()
            }
            Repeater {
                model: effectMenuModel
                MenuItem {
                    id: multiEqEffect
                    width: 300
                    text: model.name
                    height: scaleSize2(40)
                    enabled: model.active
                    checkable: false
                    indicator: Item {
                        implicitWidth: scaleSize2(40) / 2
                        implicitHeight: scaleSize2(40)
                        Rectangle {
                            visible: model.checked
                            anchors {
                                left: parent.left
                                leftMargin: scaleSize2(12)
                                verticalCenter: parent.verticalCenter
                            }

                            width: scaleSize2(12)
                            height: width
                            radius: height / 2
                            color: model.enabled ? 'gold' : 'gray'
                            border.color: 'black'
                            border.width: root.borderWidth
                        }
                    }

                    contentItem: Text {
                        leftPadding: multiEqEffect.indicator.width + scaleSize2(
                                         5)
                        text: multiEqEffect.text
                        font: bigTxt.font
                        opacity: enabled ? 1.0 : 0.3
                        color: multiEqEffect.highlighted ? flColor : fontColor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }

                    onTriggered: {


                        if (isRootMenu) {
                            automationMenuTitle = multiEqMenu.title + "," + model.name
                        } else {
                            //this value is not changable for now

                        }
                    }
                }
            }
            background: CustomRectangle {
                implicitWidth: bigTxt2.contentWidth + scaleSize2(
                                   15) * 5 + scaleSize2(40) / 2
                implicitHeight: scaleSize2(40)
                color: flColor
                borderColor: brColor
                borderWidth: root.borderWidth
                corners: [Qt.AlignLeft | Qt.AlignBottom, Qt.AlignRight
                    | Qt.AlignTop, Qt.AlignRight | Qt.AlignBottom]
                radius: topRadius
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------- Main Popup
    function addMenuItem(title, actions) {
        var menuItem = menuComponent.createObject(mainMenu, {
                                                      "title": title
                                                  })

        for (var j = 0; j < actions.length; j++) {
            let currentActionText = actions[j]

            menuItem.effectMenuModel.insert(j, {
                                                "name": currentActionText,
                                                "checked": false,
                                                "active": true,
                                                "enabled": true
                                            })
        }
        mainMenu.addMenu(menuItem)
    }

    Connections {
        target: _clipArea.tracksModel.effectMenuModel(ind)

        onRowsInserted: {


            for (var row = first; row <= last; row++) {

                var title = _clipArea.tracksModel.effectMenuModel(
                            ind).getTitle(row)
                var actions = _clipArea.tracksModel.effectMenuModel(
                            ind).getActions(row)

                addMenuItem(title, actions)
            }
        }

        onRowsRemoved: {


            for (var row = first; row <= last; row++) {
                // this 3 is beacuse of this (first: Volume, second: Pan, third: AudioObject)
                var item = mainMenu.menuAt(row + 3)
                mainMenu.removeMenu(item)
            }
        }
    }

    Component.onCompleted: {

        for (var i = 0; i < _clipArea.tracksModel.effectMenuModel(
                 ind).count; i++) {
            var title = _clipArea.tracksModel.effectMenuModel(ind).getTitle(i)
            var actions = _clipArea.tracksModel.effectMenuModel(
                        ind).getActions(i)
            addMenuItem(title, actions)
        }
    }
    Menu {
        id: mainMenu

        spacing: space

        x: menu.x - shadowRec.borderWidth / 2
        y: menu.y + menu.height + 1

        MenuItem {
            id: none
            text: 'None'
            height: scaleSize2(40)
            padding: 0

            contentItem: Rectangle {
                height: scaleSize2(40)
                width: backgroundMenu.width
                color: none.highlighted ? fontColor : flColor
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

            onTriggered: {

                if (isRootMenu) {
                    automationMenuTitle = "None,None"
                } else {
                    //this value is not changable for now
                }
            }
        }
        property string automationSubMenuTitle: automationSubMenuTitle
        ListModel {
            id: menuModel
            ListElement {
                name: "Volume"
                checked: false
                active: true
                enabled: true
            }
            ListElement {
                name: "Pan"
                checked: false
                active: true
                enabled: true
            }
            ListElement {
                name: "SpeakerOn"
                checked: false
                active: true
                enabled: true
            }
        }
        ListModel {
            id: audioObjectModel
            ListElement {
                name: "KeyFrames"
                checked: false
                active: true
                enabled: true
            }
        }

        function updateCheckedState() {
            for (var i = 0; i < menuModel.count; i++) {
                var item = menuModel.get(i)
                menuModel.setProperty(i, "checked", isAutomated("Mixer",
                                                                item.name))

                menuModel.setProperty(i, "active", !isSelected("Mixer",
                                                               item.name))

                menuModel.setProperty(i, "enabled", isEnabled("Mixer",
                                                              item.name))
            }
        }
        onAutomationSubMenuTitleChanged: updateCheckedState()
        Component.onCompleted: updateCheckedState()

        Menu {
            title: "Mixer"

            spacing: space

            onOpened: {
                mainMenu.updateCheckedState()
            }

            Repeater {
                model: menuModel
                MenuItem {
                    id: menuItemMixer
                    text: model.name
                    height: scaleSize2(40)
                    width: 300
                    enabled: model.active
                    checkable: false
                    indicator: Item {
                        implicitWidth: scaleSize2(40) / 2
                        implicitHeight: scaleSize2(40)
                        Rectangle {
                            visible: model.checked
                            anchors {
                                left: parent.left
                                leftMargin: scaleSize2(12)
                                verticalCenter: parent.verticalCenter
                            }

                            width: scaleSize2(12)
                            height: width
                            radius: height / 2
                            color: model.enabled ? 'gold' : 'gray'
                            border.color: 'black'
                            border.width: root.borderWidth
                        }
                    }
                    contentItem: Text {
                        leftPadding: menuItemMixer.indicator.width + scaleSize2(
                                         5)
                        text: menuItemMixer.text
                        font: bigTxt.font
                        opacity: enabled ? 1.0 : 0.3
                        color: menuItemMixer.highlighted ? flColor : fontColor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }

                    background: Rectangle {
                        width: parent.width
                        height: scaleSize2(40)
                        opacity: enabled ? 1 : 0.3
                        color: menuItemMixer.highlighted ? fontColor : flColor
                    }

                    onTriggered: {


                        if (isRootMenu) {
                            automationMenuTitle = "Mixer" + "," + model.name
                        } else {
                             //this value is not changable for now
                        }
                    }
                }
            }

            topPadding: topRadius
            bottomPadding: topRadius
            leftPadding: root.borderWidth
            rightPadding: root.borderWidth

            background: CustomRectangle {
                implicitWidth: bigTxt2.contentWidth + scaleSize2(
                                   15) * 3 + scaleSize2(40) / 2
                implicitHeight: scaleSize2(40)
                color: flColor
                borderColor: brColor
                borderWidth: root.borderWidth
                corners: [Qt.AlignLeft | Qt.AlignBottom, Qt.AlignRight
                    | Qt.AlignTop, Qt.AlignRight | Qt.AlignBottom]
                radius: topRadius
            }
        }

        Menu {
            title: "AudioObject"

            spacing: space

            Repeater {
                model: audioObjectModel
                MenuItem {
                    id: menuItemAudioObject
                    text: model.name
                    height: scaleSize2(40)
                    width: 300
                    enabled: model.active
                    checkable: false
                    indicator: Item {
                        implicitWidth: scaleSize2(40) / 2
                        implicitHeight: scaleSize2(40)
                        Rectangle {
                            visible: model.checked
                            anchors {
                                left: parent.left
                                leftMargin: scaleSize2(12)
                                verticalCenter: parent.verticalCenter
                            }

                            width: scaleSize2(12)
                            height: width
                            radius: height / 2
                            color: model.enabled ? 'gold' : 'gray'
                            border.color: 'black'
                            border.width: root.borderWidth
                        }
                    }
                    contentItem: Text {
                        leftPadding: menuItemAudioObject.indicator.width + scaleSize2(
                                         5)
                        text: menuItemAudioObject.text
                        font: bigTxt.font
                        opacity: enabled ? 1.0 : 0.3
                        color: menuItemAudioObject.highlighted ? flColor : fontColor
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }

                    background: Rectangle {
                        width: parent.width
                        height: scaleSize2(40)
                        opacity: enabled ? 1 : 0.3
                        color: menuItemAudioObject.highlighted ? fontColor : flColor
                    }

                    onTriggered: {
                        if (isRootMenu) {
                            automationMenuTitle = "AudioObject" + "," + model.name
                        } else {
                             //this value is not changable for now
                        }
                    }
                }
            }
            topPadding: topRadius
            bottomPadding: topRadius
            leftPadding: root.borderWidth
            rightPadding: root.borderWidth


            background: CustomRectangle {
                implicitWidth: bigTxt2.contentWidth + scaleSize2(
                                   15) * 3 + scaleSize2(40) / 2
                implicitHeight: scaleSize2(40)
                color: flColor
                borderColor: brColor
                borderWidth: root.borderWidth
                corners: [Qt.AlignLeft | Qt.AlignBottom, Qt.AlignRight
                    | Qt.AlignTop, Qt.AlignRight | Qt.AlignBottom]
                radius: topRadius
            }
        }
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

                visible: !menuItemVisible() & menuItem.text.indexOf(
                             'Show All Automated') >= 0 ? false : menuItem.text !== 'None'
                anchors {
                    right: parent.right
                    rightMargin: scaleSize2(15)
                    verticalCenter: parent.verticalCenter
                }

                width: scaleSize2(11)
                height: scaleSize2(19)
                contextType: "2d"

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.moveTo(0, 0)
                    ctx.lineTo(width, height / 2)
                    ctx.lineTo(0, height)
                    ctx.closePath()
                    ctx.fillStyle = menuItem.highlighted ? flColor : fontColor
                    ctx.fill()
                }
            }

            indicator: Item {
                implicitWidth: scaleSize2(40) / 2
                implicitHeight: scaleSize2(40)
                Rectangle {
                    visible: (menuItemVisible()) & menuItem.text !== 'None'
                    anchors {
                        left: parent.left
                        leftMargin: scaleSize2(12)
                        verticalCenter: parent.verticalCenter
                    }

                    width: scaleSize2(12)
                    height: width
                    radius: height / 2
                    color: 'gold'
                    border.color: 'black'
                    border.width: root.borderWidth
                }
            }

            contentItem: Text {
                id: txtMenu
                height: scaleSize2(40)
                leftPadding: menuItem.text
                             === 'None' ? scaleSize2(10) : menuItemVisible(
                                              ) ? menuItem.indicator.width + scaleSize2(
                                                      5) + scaleSize2(
                                                      12) : scaleSize2(10)
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
            implicitWidth: root.width + topWidth * 2 //bigTxt.contentWidth + scaleSize2(11) + scaleSize2(15) *5 + root.borderWidth *2
            implicitHeight: scaleSize2(40)
            color: flColor
            borderColor: brColor
            borderWidth: root.borderWidth
            corners: Qt.AlignLeft | Qt.AlignRight | Qt.AlignBottom
            radius: topRadius
        }
    }

    CustomRectangle {
        id: shadowRec
        corners: Qt.AlignLeft | Qt.AlignRight | Qt.AlignTop
        visible: didShadow
        anchors.fill: menu
        color: brColorDisplay
        anchors.margins: -refSize
        radius: topRadius
        z: -1
    }
}
