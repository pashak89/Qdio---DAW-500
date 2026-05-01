import QtQuick 2.3
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.12
import 'qrc:/Items'

Item {

Rectangle {
    id: root
    color: theme.riCli_txt_r3
    radius: topRadius
    anchors.fill: parent

    property int fontSize: topfontPixelSize

    function return2zeroValue() {
        audioTrackCount.value = 0
        midiTrackCount.value = 0
        returnTrackCount.value = 0
        ambisonicsTrackCount.value = 0
        videoTrackCount.value = 0
    }

    function calculateCountReturnTracks() {
        if(returnTrackCount.value + trackReturnModel.count > 26)
            return (26 - returnTrackCount.value - trackReturnModel.count)
        else
            return returnTrackCount.value
    }

    onVisibleChanged: if(visible) return2zeroValue()

    InfoText {
        id: label
        name: 'Add Track'
        leftPaddingTxt: Math.round(parent.width/2 - contentW/2)
        colorTxt: theme.riCli_txt_hover_r4
        fontSize: root.fontSize
//        z: 1
    }

    Rectangle {
        id: secondPart
        z: 1
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            leftMargin: 0
            rightMargin: 0
            bottomMargin: 0
            topMargin: label.contentH
        }

        radius: topRadius
        color: theme.riCli_fil_r2
        border.color: theme.riCli_brd_r1

        GridLayout {
            id: grid
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                leftMargin: Math.round(parent.width /15)
                rightMargin: Math.round(parent.width /15)
                bottomMargin: buttons.height + Math.round(parent.width /200)
                topMargin: Math.round(parent.width /15)
            }
            rows: 5
            columns: 2

            columnSpacing: scaleSize2(5)
            rowSpacing: scaleSize2(15)

            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                InfoText {
                    name: 'Audio Track:'
                    fontSize: root.fontSize
                    colorTxt: theme.tColor
                }
            }
            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                SpinBoxCustom {
                    id: audioTrackCount
                    width: scaleSize2(250)
                    height: parent.height
                    anchors.right: parent.right
                    value: 0
                  }
            }

            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                InfoText {
                    name: 'MIDI Track:'
                    fontSize: root.fontSize
                    colorTxt: theme.tColor
                }
            }
            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                SpinBoxCustom {
                    id: midiTrackCount
                    width: scaleSize2(250)
                    height: parent.height
                    anchors.right: parent.right
                    value: 0
                  }
            }

            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                InfoText {
                    name: 'Return Track:'
                    fontSize: root.fontSize
                    colorTxt: theme.tColor
                }
            }
            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                SpinBoxCustom {
                    id: returnTrackCount
                    width: scaleSize2(250)
                    height: parent.height
                    anchors.right: parent.right
                    value: 0
                    onValueChanged: value = root.calculateCountReturnTracks()
                  }
            }

            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                InfoText {
                    name: 'Ambisonics Track:'
                    fontSize: root.fontSize
                    colorTxt: theme.tColor
                }
            }
            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                SpinBoxCustom {
                    id: ambisonicsTrackCount
                    width: scaleSize2(250)
                    height: parent.height
                    anchors.right: parent.right
                    value: 0
                  }
            }

            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                InfoText {
                    name: 'Video Track:'
                    fontSize: root.fontSize
                    colorTxt: theme.tColor
                }
            }
            Rectangle {
                Layout.fillHeight: true; Layout.fillWidth: true
                color: 'transparent'
                SpinBoxCustom {
                    id: videoTrackCount
                    width: scaleSize2(250)
                    height: parent.height
                    anchors.right: parent.right
                    value: 0
                  }
            }
        }// GridLayout.

        Rectangle {
            id: buttons
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            height: root.height / 5
            color: 'transparent'

            focus: popup.opened

            RowLayout {
                anchors {
                    top:    parent.top
                    bottom: parent.bottom
                    left:   parent.left
                    right:  parent.right
                    rightMargin:    Math.round(root.width /8)
                    leftMargin:     Math.round(root.width /8)
                    topMargin:      Math.round(parent.height /4)
                    bottomMargin:   Math.round(parent.height /4)
                }

                spacing:  Math.round(root.width /5)

                TrackButton {
                    id:                     btn_cancel
                    Layout.fillHeight:      true
                    Layout.fillWidth:       true

                    name:       "Cancel"
                    fontSize: root.fontSize

                    fillStyle:  isClicked ? theme.b50_Disable_FIL : theme.b40_Disable_FIL
                    brColor:    isClicked ? theme.b50_Disable_BRD : theme.b40_Disable_BRD
                    fontColor:  isClicked? theme.riCli_txt_hover_r4 : theme.riCli_txt_r3

                    onSendMessage: {
                        if(msg === 'released')
                        {
                            popup.close()
                            isClicked = false
                        }
                    }
                }

                TrackButton {
                    id:                     btn_done
                    Layout.fillHeight:      true
                    Layout.fillWidth:       true

                    name:       "Done"
                    fontSize: root.fontSize

                    fillStyle:  isClicked ? theme.b11_Enable1_FIL: theme.b40_Disable_FIL
                    brColor:    isClicked ? theme.b11_Enable1_BRD : theme.b40_Disable_BRD

                    onSendMessage: {
                        if(msg === 'pressed')
                        {
                            if(audioTrackCount.value > 0)
                                for(var i = 0; i < audioTrackCount.value; i++)
                                trackAudioModel.append( {"selectTrack": true, "selectSolo":false,
                                                      "light": false, "channel_count":8, "rowsExpanded": 1} )
                            if(returnTrackCount.value > 0 )
                                for(i = 0; i < root.calculateCountReturnTracks(); i++)
                                trackReturnModel.append( {"selectTrack": true, "selectSolo":false, "selectTrackDependSolo": true,
                                                      "light": false, "channel_count":8, "rowsExpanded": 1} )
                        }

                        else if(msg === 'released')
                        {
                            popup.close()
                            focus = false
                            isClicked = false
                        }// onSendMessage.
                    }
                }
            }
        }
    }

    smooth: true
//    visible: false
}

DropShadow {
        anchors.fill: root
        horizontalOffset: 5
        verticalOffset: 5
        radius: 10.0
        samples: 21
        color: "#40000000"
        source: root
    }

}
