import QtQuick 2.12
import QtGraphicalEffects 1.14

Rectangle {
    id: rootBtn
    smooth: true
    color: 'transparent'

    signal sendMessage(string msg, int compId)

    signal action(int index, string text)

    function callback(index, text) {
        action(index, text)
    }

    //------ save:
    property bool isPressed: false
    property var obj: null
    property bool is_ValueChanged: false
    property real previousValue: 0
    property bool firstValueClick: true
    property bool afterLoaded: false
    property bool canUndo: true
    property int value: 0

    // right panel
    property bool hasRightPanel: false
    property var shortcutKeyTxt: ['', '']
    property var moduls: ['Show Automation', 'Show Automation in New Lane']

    property bool didShadow: false
    property bool isSoloBtn: false
    property bool bigSize: false

    property real fontSize: topfontPixelSize

    property real thisRadius: topRadius

    property real marginText: 1
    property alias contentHSize: txtBtn.contentH
    property alias contentWSize: txtBtn.contentW

    property Item parentBtn: null
    property bool isParentBtn: false

    property color brColor: isClicked ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD //theme.b10_Disable_BRD
    property color flColor: "#262626"
    property color fontColor: isClicked ? theme.f4_ : theme.f1_

    property bool isClicked: value
    property string name: ''

    property alias fillStyle: borderRec.color

    property alias fontColor: txtBtn.colorTxt

    property alias contentHBtn: txtBtn.contentH

    property bool imgVisible: false

    Rectangle {
        id: borderRec

        width: Math.round(parent.width + (bigSize ? 1 : 0))
        height: Math.round(parent.height + (bigSize ? 1 : 0))

        color: {
            if (isParentBtn) {
                if (!parentBtn.isClicked)
                    "transparent"
                else if (parentBtn.isClicked && isClicked)
                    theme.b11_Enable1_FIL
                else
                    theme.b10_Disable_FIL
            } else
                isClicked ? theme.b11_Enable1_FIL : theme.b10_Disable_FIL
        }

        border.width: topWidth

        border.color: {
            if (isParentBtn) {
                if (!parentBtn.isClicked)
                    brColor
                else if (parentBtn.isClicked && isClicked)
                    theme.b11_Enable1_BRD
                else
                    theme.b10_Disable_BRD
            } else
                brColor //isClicked ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD
        }

        radius: thisRadius
    }

    Rectangle {
        id: recImg
        anchors.fill: parent
        anchors.margins: 1
        radius: topRadius
        color: '#FFA647'
        smooth: true
        visible: false
    }

    Image {
        id: mask
        anchors.fill: recImg
        fillMode: Image.Stretch
        source: 'qrc:/resources/svg/track_list/triangle_btn.svg'
        mipmap: true
        smooth: true
        visible: false
    }

    OpacityMask {
        visible: imgVisible
        anchors.fill: recImg
        source: recImg
        maskSource: mask
    }

    property alias hSize: txtBtn.contentH
    property alias fSize: txtBtn.fontSize
    InfoText2 {
        id: txtBtn
        name: rootBtn.name

        colorTxt: {
            if (isParentBtn) {
                if (!parentBtn.isClicked)
                    fontColor
                else if (parentBtn.isClicked && isClicked)
                    theme.f4_
                else
                    theme.f1_
            } else
                fontColor
        } // color.
        fontSize: rootBtn.fontSize
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
        onEntered: {

            _clipArea.setOverrideCursor(Qt.ArrowCursor)
        }
    }
    MouseArea {
        id: mArea
        anchors.fill: parent

        acceptedButtons: Qt.LeftButton | Qt.RightButton
        enabled: {
            if (isParentBtn)
                parentBtn.isClicked ? true : false
            else
                parent.enabled
        }

        onPressed: {
            rootBtn.isPressed = true
            if (hasRightPanel && mouse.button === Qt.RightButton) {
                rootBtn.focus = true
                mArea.focus = true
                if (mouse.button === Qt.RightButton) {
                    positionInWindow = Qt.point(mouse.x, mouse.y)

                    rightPanel.leftModules = moduls
                    rightPanel.rightModules = shortcutKeyTxt

                    rightPanel.cc = parent
                    rightPanel.visible = true
                    rightPanel.index = index
                    rightPanel.callback = rootBtn.callback
                    rightPanel.parent = rootBtn

                    if (trackRightClick.visible)
                        trackRightClick.visible = false
                } else
                    rightPanel.visible = false
            } else {
                if (hasRightPanel) {
                    if (rightPanel.visible)
                        rightPanel.visible = false
                }
                if (!isSoloBtn)
                    isClicked = !isClicked
                mArea.focus = true

                sendMessage("pressed", 1)
            }
        }

        onReleased: {

            rootBtn.isPressed = false
            if (mouse.button === Qt.LeftButton) {
                sendMessage("released", 0) //undoCtrl_button()
            }
        }

        onFocusChanged: {
            if (mArea.focus || rootBtn.focus)
                didShadow = true
            else {

                if (hasRightPanel) {
                    if (trackRightClick.visible)
                        trackRightClick.visible = false

                    rightPanel.visible = false
                }

                didShadow = false
            }
        }
    }

    Rectangle {
        visible: didShadow
        anchors.fill: borderRec
        color: theme.b10_Disable_BRD //'transparent'

        anchors.margins: -refSize
        radius: thisRadius
        z: -1
    }
}
