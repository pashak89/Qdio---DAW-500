import QtQuick 2.12
import QtGraphicalEffects 1.12
 import QtQuick.Controls 2.15
Rectangle {
    id: root
    smooth: true
    color: 'transparent'

    signal sendMessage(string msg, int compId)
    signal sigClicked

    //------ save:
    property var obj: null
    property bool is_ValueChanged: false
    property real previousValue: 0
    property bool firstValueClick: true
    property bool afterLoaded: false
    property bool canUndo: true
    property int value: 0

    property bool onLeft: false

    property bool didShadow: false

    property real fontSize: topfontPixelSize

    property real thisRadius: topRadius

    property real marginText: 1

    property Item parentBtn: null
    property bool isParentBtn: false

    property color brColor: theme.b10_Disable_BRD
    property color flColor: isClicked ? theme.b11_Enable1_FIL : theme.b10_Disable_FIL

    property bool isClicked: value
    property string name: ''

    CustomRectangle {
        id: backg
        width: Math.round(parent.width) //+1
        height: Math.round(parent.height) //+1
        color: flColor
        corners: onLeft ? [Qt.AlignLeft | Qt.AlignTop
                           | Qt.AlignBottom] : [Qt.AlignRight | Qt.AlignTop | Qt.AlignBottom]
        radius: topRadius


    }

    MouseArea {
        id: mArea
        anchors.fill: parent
        enabled: {
            if (isParentBtn)
                parentBtn.isClicked ? true : false
            else
                parent.enabled
        }

        onPressed: {


            mArea.focus = true
            root.focus = true

            value = isClicked
            previousValue = !isClicked
            sendMessage('pressed', 1)
            sigClicked()
        }
    }

    onFocusChanged: {
        if (mArea.focus || root.focus)
            didShadow = true
        else
            didShadow = false
    }
}
