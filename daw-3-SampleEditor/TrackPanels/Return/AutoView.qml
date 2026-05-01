import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.2
import '.'
import 'qrc:/'
import "qrc:/Items"

Rectangle {
    id:                 auto_view_btn //---------------------------------------------------------------------- 2
    // anchors {
    //     top:            record_send_btn.top
    //     bottom:         record_send_btn.bottom
    // }

    width:        scaleSize2(106)
    height: scaleSize2(37)
    gradient:     _3DGradient
    radius:       topRadius

    property bool _automationTurn: automationTurn
    on_AutomationTurnChanged: automationBtn.isClicked = _automationTurn


    property Gradient _3DGradient: Gradient {
        GradientStop { position: 0.0;  color: didGradient ? "#C5C7C9" : theme.s11_Enable_FIL }
        GradientStop { position: 0.17; color: didGradient ? "#BDBEBF" : theme.s11_Enable_FIL }
        GradientStop { position: 0.5;  color: didGradient ? "#AAABAD" : theme.s11_Enable_FIL }
        GradientStop { position: 0.82; color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL }
        GradientStop { position: 1.0;  color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL }
    }

    Rectangle {
        id:         betweenLine_auto_view
        visible:    automationBtn.isClicked || viewBtn.isClicked
        enabled:    automationBtn.isClicked || viewBtn.isClicked
        x:          automationBtn.x + automationBtn.width
        height:     parent.height
        width:      topWidth
        z:          1
        color:      'black'//theme.s11_Enable_FIL
    }

    RowLayout {
        anchors.fill: parent
        spacing: topWidth

        TrackSubButton {
            id:                     automationBtn
            Layout.fillHeight:      true
            Layout.fillWidth:       true

            flColor: light  ? (isClicked ? '#EFA441' : 'transparent') : isClicked ? '#EFA441' : theme.s11_Enable_FIL

//            isClicked: automationTurn

            onIsClickedChanged: {
                trackReturnModel.setProperty(ind, 'automationTurn', isClicked)
                if(isClicked & rowsExpanded < 2)
                {
                    changeRowCounter((rowsExpanded >= _rowsExpanded ? rowsExpanded + 2 : _rowsExpanded +1) )
                    showAutomation = true
                }
                else if(!isClicked & rowsExpanded > 1)
                {
                    changeRowCounter(2)
                    showAutomation = false
                }
            }

            onLeft:                 true

            Image {
                id:                     automationImg
                anchors.centerIn:       parent
                //                    anchors.verticalCenterOffset: parent.height % 2 === 0 ? parent.height/2 > height ? 1 : -1 : 0
                sourceSize:             {
                    if(automationBtn.height %2 !== 0)
                        Qt.size(oddSize(parent.width*.6), oddSize(parent.width*.6))
                    else
                        Qt.size(evenSize(parent.width*.6), evenSize(parent.width*.6))
                }
                source:                 'qrc:/Resource/automation_icon.svg'
                visible:                false
            }
            ColorOverlay {
                id:                     colorImageAutomation
                cached:                 true
                anchors.fill:           automationImg
                source:                 automationImg
                //                    clip:                   true
                //                    antialiasing:           true
                color:                  automationBtn.isClicked | light  ? theme.a1 : theme.a2
            }
            onSendMessage: funcs.trackLighting(ind, 'select_btn')
        } // automationBtn.

        TrackSubButton {
            id:                     viewBtn
            Layout.fillHeight:      true
            Layout.fillWidth:       true

            flColor: {
                if(isClicked) '#8E7EE0'
                else if (light ) 'transparent'
                else theme.s11_Enable_FIL
            }

            Image {
                id:                     viewImg
                anchors.centerIn:       parent
                //                    anchors.verticalCenterOffset: parent.height % 2 === 0 ? parent.height/2 > height ? 1 : -1 : 0
                sourceSize:             {
                    if(viewBtn.height %2 !== 0)
                        Qt.size(oddSize(parent.width*.8), oddSize(parent.width*.8))
                    else
                        Qt.size(evenSize(parent.width*.8), evenSize(parent.width*.8))
                }

                source:                 'qrc:/Resource/object_icon.svg'
                visible:                false
            }
            ColorOverlay {
                id:                     colorImageView
                cached:                 true
                anchors.fill:           viewImg

                source:                 viewImg
                color:                  viewBtn.isClicked | light  ? theme.a1 : theme.a2
            }
            onSendMessage: funcs.trackLighting(ind, 'select_btn')
        } // viewBtn.
    }

    Rectangle {
        anchors {
            fill:       parent
            topMargin:  -1
            bottomMargin: -1
        }
        color:              'transparent'
        border.color:       theme.b10_Disable_BRD
        border.width:       topWidth//Math.min(topWidth *2, 2)
        radius:             topRadius
    }
}
