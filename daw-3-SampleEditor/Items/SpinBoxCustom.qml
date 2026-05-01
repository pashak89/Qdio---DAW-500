import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

SpinBox {
     id: control
     value: 50
     editable: true

     property color fillColor: theme.b60_enable_FIL
     property color brdColor: theme.b60_enable_BRD

//     contentItem: TextInput {
//         z: 2
//         text: control.textFromValue(control.value, control.locale)

//         font: control.font
//         color: "#21be2b"
//         selectionColor: "#21be2b"
//         selectedTextColor: "#ffffff"
//         horizontalAlignment: Qt.AlignHCenter
//         verticalAlignment: Qt.AlignVCenter

//         readOnly: !control.editable
//         validator: control.validator
//         inputMethodHints: Qt.ImhFormattedNumbersOnly
//     }

     contentItem: Rectangle {         
         color: fillColor
         anchors.fill: parent
         anchors.rightMargin: parent.height + 3
         anchors.leftMargin: parent.height + 3
         radius: topRadius
         border.color: brdColor
         border.width: topWidth

         InfoText2 {
//             anchors.topMargin: parent.height / 4 - contentH / 4
//             leftPaddingTxt: width / 2 - contentW / 2
             name: control.textFromValue(control.value, control.locale)
//             fontSize: control.fontSize
         }
     }

     up.indicator: Rectangle {
         id:            addBtn
         x:             control.mirrored ? 0 : parent.width - width
         height:        parent.height
         width:         height
         color:         control.up.pressed ? theme.b51_Enable_FIL : theme.b50_Disable_FIL
         border.color:  control.up.pressed ? theme.b51_Enable_BRD : theme.b50_Disable_BRD

         Image {
             id:                    addImg
             anchors.centerIn:      parent

             sourceSize:            Qt.size(oddSize(parent.width*.5), oddSize(parent.width*.5))
             source:                'qrc:/Resource/add_icon.svg'
             visible:               false
         }
         ColorOverlay {
             id:                    colorAddImg
             cached:                true
             anchors.fill:          addImg

             source:                addImg
             clip:                  true
             antialiasing:          true
             color:                 control.up.pressed ? theme.riCli_txt_r3 : theme.riCli_txt_hover_r4
         }
     }
//     up.indicator: TrackButton {
//         id: addBtn
//         x: control.mirrored ? 0 : parent.width - width
//         height: parent.height
//         width: height

//         fillStyle: control.up.pressed ? theme.b11_Enable1_FIL : theme.b10_Disable_FIL
////         brdColor:  control.up.pressed ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD

//         Image {
//             id:                    addImg
//             anchors.centerIn:      parent

//             sourceSize:            Qt.size(oddSize(parent.width*.5), oddSize(parent.width*.5))
//             source:                'qrc:/Resource/add_icon.svg'
//             visible:               false
//         }
//         ColorOverlay {
//             id:                    colorAddImg
//             cached:                true
//             anchors.fill:          addImg

//             source:                addImg
//             clip:                  true
//             antialiasing:          true
//             color:                 control.up.pressed ? theme.a1 : theme.a2
//         }

////         fillStyle:  control.up.pressed ? 'red': 'green'
////         brColor:    'red'
//         onIsClickedChanged: control.value++
//     }


     down.indicator: Rectangle {
         id:        minusBtn
         x:         control.mirrored ? parent.width - width : 0
         height:    parent.height
         width:     height
         color:     control.down.pressed ? theme.b51_Enable_FIL : theme.b50_Disable_FIL
         border.color: control.down.pressed ? theme.b51_Enable_BRD : theme.b50_Disable_BRD

         Image {
             id:                    minusImg
             anchors.centerIn:      parent

             sourceSize:            Qt.size(oddSize(parent.width*.5), oddSize(parent.width*.65))
             source:                'qrc:/Resource/minus.svg'
             visible:               false
         }
         ColorOverlay {
             id:                    colorMinusImg
             cached:                true
             anchors.fill:          minusImg

             source:                minusImg
             clip:                  true
             antialiasing:          true
             color:                 control.down.pressed ? theme.riCli_txt_r3 : theme.riCli_txt_hover_r4
         }
     }

     /*down.indicator: TrackButton {
         id:        minusBtn
         x:         control.mirrored ? parent.width - width : 0
         height:    parent.height
         width:     height

         fillStyle: control.down.pressed ? theme.b11_Enable1_FIL : theme.b10_Disable_FIL
//         brdColor:  control.down.pressed ? theme.b11_Enable1_BRD : theme.b10_Disable_BRD

         Image {
             id:                    minusImg
             anchors.centerIn:      parent

             sourceSize:            Qt.size(oddSize(parent.width*.5), oddSize(parent.width*.65))
             source:                'qrc:/Resource/minus.svg'
             visible:               false
         }
         ColorOverlay {
             id:                    colorMinusImg
             cached:                true
             anchors.fill:          minusImg

             source:                minusImg
             clip:                  true
             antialiasing:          true
             color:                 control.down.pressed ? theme.a1 : theme.a2
         }
         onIsClickedChanged:        control.value--
     }*/

     background: null
 }
