import QtQuick 2.12
import QtQuick.Layouts 1.12

Rectangle {
    signal triggered(string msg, int compId)

    property color backgroundHoverColor: '#BCE4FF'
    property color backgroundDefaultColor: bgColr

    property color txtHoverColor: bgColr
    property color txtDefaultColor: 'white'

    property string leftTitle: ''
    property string rightTitle: ''

    property Item originParent: null

    Layout.fillHeight: true
    Layout.fillWidth: true
//    Layout.topMargin: originParent.height / 100
//    Layout.bottomMargin: originParent.height / 100

    property int leftWidth: leftTxt.contentW
    property int rightWidth: rightTxt.contentW


    color: 'transparent'
    InfoText_left {
        id: leftTxt
        anchors.leftMargin: scaleSize2(15)//Math.round(originParent.width / 20)

//        anchors.topMargin: parent.height / 2 - contentH /2

//        leftPaddingTxt: Math.round(originParent.width / 25)
        name: leftTitle
        colorTxt: txtDefaultColor
    }
    InfoText2 {
        id: rightTxt
        anchors.rightMargin: scaleSize2(15)//Math.round(originParent.width / 20)
//        anchors.topMargin: parent.height / 2 - contentH /2
        right2Left: true
//        rightPaddingTxt: Math.round(originParent.width / 25)
        name: rightTitle
        colorTxt: txtDefaultColor
    }
    MouseArea {
        anchors.fill: parent; hoverEnabled: true
        onEntered: { parent.color = backgroundHoverColor; leftTxt.colorTxt = txtHoverColor; rightTxt.colorTxt = txtHoverColor }
        onExited: {parent.color = backgroundDefaultColor; leftTxt.colorTxt = txtDefaultColor; rightTxt.colorTxt = txtDefaultColor }
        onClicked: triggered("clicked",1)
    }
}
