import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.2
import '.'
import 'qrc:/'
import "qrc:/Items"


Rectangle {
    id:                 record_send_btn // ------------------------------------------------------------------- 1
    // anchors {
    //     top:            tf_11.top
    //     bottom:         tf_11.bottom
    // }
    width:          scaleSize2(106)
    height: scaleSize2(37)
    gradient:       _3DGradient
    radius:         topRadius


    property Gradient _3DGradient: Gradient {
        GradientStop { position: 0.0;  color: didGradient ? "#C5C7C9" : theme.s11_Enable_FIL }
        GradientStop { position: 0.17; color: didGradient ? "#BDBEBF" : theme.s11_Enable_FIL }
        GradientStop { position: 0.5;  color: didGradient ? "#AAABAD" : theme.s11_Enable_FIL }
        GradientStop { position: 0.82; color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL }
        GradientStop { position: 1.0;  color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL }
    }

    Rectangle {
        id:         betweenLine_send_btn
        visible:    recordBtn.isClicked || sendBtn.isClicked
        enabled:    recordBtn.isClicked || sendBtn.isClicked
        x:          recordBtn.x + recordBtn.width
        height:     parent.height
        width:      topWidth
        z:          1
        color:      'black'//theme.s11_Enable_FIL
    }

    RowLayout {
        id: rr
        anchors.fill: parent
        spacing: topWidth

        TrackSubButton {
            id:                     recordBtn
            Layout.fillHeight:      true
            Layout.fillWidth:       true

            onLeft:                 true

            flColor: light ? (isClicked ? '#DE3C47' : 'transparent') : isClicked ? '#DE3C47' : theme.s11_Enable_FIL

            //                onIsClickedChanged: focus = true

            Image {
                id:                     recordImg
                anchors.centerIn:       parent
                sourceSize:             {
                    if(recordBtn.height %2 !== 0)
                        Qt.size(oddSize(parent.width*.3), oddSize(parent.width*.3))
                    else
                        Qt.size(evenSize(parent.width*.3), evenSize(parent.width*.3))
                }
                source:                 'qrc:/Resource/rec_icon.svg'
                visible:                false
                mipmap: true
            }

            ColorOverlay {
                id:                     colorImageRecord
                cached:                 true
                anchors.fill:           recordImg

                source:                 recordImg
                //                    clip:                   true
                //                    antialiasing:           true
                color:                  recordBtn.isClicked | light  ? theme.a1 : theme.a2
            }
            //                MouseArea {
            //                    anchors.fill: parent
            //                    onClicked: console.log('image: ' + recordImg.height + ', btn: '+ recordBtn.height + ', img w: ' +
            //                                           recordImg.width + ', btn w: '+ recordBtn.width)
            //                }
            onSendMessage: funcs.trackLighting(ind, 'select_btn')
        } // recordBtn.

        TrackSubButton {
            id:                     sendBtn
            Layout.fillHeight:      true
            Layout.fillWidth:       true
            flColor: light  ? (isClicked ? '#13C44B' : 'transparent') : isClicked ? '#13C44B' : theme.s11_Enable_FIL

            Image {
                id:                     sendImg
                anchors.centerIn:       parent
                sourceSize:             {
                    if(sendBtn.height %2 !== 0)
                        Qt.size(oddSize(parent.width*.6), oddSize(parent.width*.6))
                    else
                        Qt.size(evenSize(parent.width*.6), evenSize(parent.width*.6))
                }
                source:                 'qrc:/Resource/send_icon.svg'
                visible:                false
            }
            ColorOverlay {
                id:                     colorImageSend
                cached:                 true
                anchors.fill:           sendImg

                source:                 sendImg
                clip:                   true
                antialiasing:           true
                color:                  sendBtn.isClicked | light  ? theme.a1 : theme.a2
            }
            //                MouseArea {
            //                    anchors.fill: parent
            //                    onClicked: console.log('image: ' + sendImg.height + ', btn: '+ sendBtn.height)
            //                }
            onSendMessage: funcs.trackLighting(ind, 'select_btn')
        } // sendBtn.
    }

    Rectangle{
        anchors {
            fill:       parent
            topMargin:  -1
            bottomMargin: -1
        }
        color:              'transparent'
        border.color:       theme.b10_Disable_BRD
        border.width:       topWidth
        radius:             topRadius
    }
}
