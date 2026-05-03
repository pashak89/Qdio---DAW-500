import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.2
import "."
import "qrc:/"
import "qrc:/Items"

Rectangle {
    id: auto_view_btn //---------------------------------------------------------------------- 2

    width: scaleSize2(106)
    height: scaleSize2(37)
    gradient: _3DGradient
    radius: topRadius

    property Gradient _3DGradient: Gradient {
        GradientStop {
            position: 0.0
            color: didGradient ? "#C5C7C9" : theme.s11_Enable_FIL
        }
        GradientStop {
            position: 0.17
            color: didGradient ? "#BDBEBF" : theme.s11_Enable_FIL
        }
        GradientStop {
            position: 0.5
            color: didGradient ? "#AAABAD" : theme.s11_Enable_FIL
        }
        GradientStop {
            position: 0.82
            color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL
        }
        GradientStop {
            position: 1.0
            color: didGradient ? "#ADAEB0" : theme.s11_Enable_FIL
        }
    }

    Rectangle {
        id: betweenLine_auto_view
        visible: automationBtn.isClicked || viewBtn.isClicked
        enabled: automationBtn.isClicked || viewBtn.isClicked
        x: automationBtn.x + automationBtn.width
        height: parent.height
        width: topWidth
        z: 1
        color: theme.s11_Enable_FIL
    }

    RowLayout {
        anchors.fill: parent
        spacing: topWidth

        TrackSubButton {
            id: automationBtn
            Layout.fillHeight: true
            Layout.fillWidth: true

            flColor: selectedTrack ? (isClicked ? '#EFA441' : 'transparent') : isClicked ? '#EFA441' : theme.s11_Enable_FIL

            isClicked: model.automationLaneEnabled
            onIsClickedChanged: {
                console.log("isClicked", isClicked)
            }

            onSigClicked: {
                model.automationLaneEnabled = !model.automationLaneEnabled
            }

            onLeft: true

            Image {
                id: automationImg
                anchors.centerIn: parent

                sourceSize: {
                    if (automationBtn.height % 2 !== 0)
                        Qt.size(oddSize(parent.width * .6),
                                oddSize(parent.width * .6))
                    else
                        Qt.size(evenSize(parent.width * .6),
                                evenSize(parent.width * .6))
                }
                source: 'qrc:/Resource/automation_icon.svg'
                visible: false
            }
            ColorOverlay {
                id: colorImageAutomation
                cached: true
                anchors.fill: automationImg
                source: automationImg
                color: automationBtn.isClicked | model.selectedTrack ? theme.a1 : theme.a2
            }
        }

        TrackSubButton {
            id: viewBtn
            Layout.fillHeight: true
            Layout.fillWidth: true
            isClicked: model.objectEnabled
            flColor: {
                if (isClicked)
                    '#8E7EE0'
                else if (selectedTrack)
                    'transparent'
                else
                    theme.s11_Enable_FIL
            }

            Image {
                id: viewImg
                anchors.centerIn: parent

                sourceSize: {
                    if (viewBtn.height % 2 !== 0)
                        Qt.size(oddSize(parent.width * .8),
                                oddSize(parent.width * .8))
                    else
                        Qt.size(evenSize(parent.width * .8),
                                evenSize(parent.width * .8))
                }

                source: 'qrc:/Resource/object_icon.svg'
                visible: false
            }
            ColorOverlay {
                id: colorImageView
                cached: true
                anchors.fill: viewImg

                source: viewImg
                color: viewBtn.isClicked | model.selectedTrack ? theme.a1 : theme.a2
            }
            onSendMessage: {


                    model.objectEnabled = !model.objectEnabled



                    clipArea.add3dObject(index, model.objectEnabled)


                    objectCreator.setKeyFrames(
                                index, clipArea.tracksModel.keyFrames(index))

                    if (typeof scene3D !== 'undefined' && scene3D)
                        scene3D.setEntityVisible(index, model.objectEnabled)

            }

            //onSendMessage: funcs.trackLighting(ind, 'select_btn')
        }
    }

    Rectangle {
        anchors {
            fill: parent
            topMargin: -1
            bottomMargin: -1
        }
        color: 'transparent'
        border.color: theme.b10_Disable_BRD
        border.width: topWidth //Math.min(topWidth *2, 2)
        radius: topRadius
    }
}
