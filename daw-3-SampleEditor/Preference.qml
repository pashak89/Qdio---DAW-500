import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import 'qrc:/Items/'

Window  {
    id: options_win
    title: "Preferences"
    width: 500
    height: 200
    color: 'black'

    x: 20
    y: 120

    property alias brighnessMeasure: brghtn.value


    /*TextField {
        id: tfSave
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        text: '1'
    }

    Button {
        anchors.bottom: parent.bottom
        text: 'saves'
        onClicked: console.log(actionSave[undoManager.saveIndex].name)
    }*/

    Button {
        id: them1
        text: 'Apply Theme 1'
        onClicked: theme = theme1
    }
    Button {
        id: them2
        text: 'Apply Theme 2'
        anchors.top: them1.bottom
        anchors.topMargin: 15
        onClicked: theme = theme2
    }

    Rectangle {
        id: zoomDisplay_btn
        width: (65)
        height: (20)
        color: "#484e52"

        anchors{
            centerIn: parent
            horizontalCenterOffset: width-80
//            verticalCenterOffset: -height+80
        }

        border{
            width: 1
            color: "#1c1f21"
        }

        TrackTextField {
            id: size_NumberBox
            anchors.fill: parent
            enableFocus: false

            max: 150
            min: 50
            defaultValue: 100
            canUndo: false
            fontSize: 13

            onValueChanged: {
                refSize = (value/100)
            }
        }

    }

    Rectangle {
        width: (65)
        height: (20)
        color: "teal"

        anchors{
            centerIn: parent
            horizontalCenterOffset: width
//            verticalCenterOffset: height+80
        }

        border{
            width: 1
            color: "#1c1f21"
        }

        TrackTextField {
            id: brghtn
            anchors.fill: parent
enableFocus: false
            fontSize: 13

            max: 100
            min: -100
            defaultValue: 0
            canUndo: false
            isInf: false

            onValueChanged: globalValues.brightness = value
        }
    }

//    property string parentFont: ''

    FontLoader{ id: ableton1; source: 'qrc:/Resource/font/SourceSansPro-Regular.ttf' }
    FontLoader{ id: ableton2; source: 'qrc:/Resource/font/OpenSans-Regular.ttf' }
    FontLoader{ id: ableton3; source: 'qrc:/Resource/font/other/IBMPlexSansThai-Regular.ttf' }
    FontLoader{ id: ableton4; source: 'qrc:/Resource/font/other/Inconsolata-VariableFont_wdth,wght.ttf' }
    FontLoader{ id: ableton5; source: 'qrc:/Resource/font/other/Karla-VariableFont_wght.ttf' }
    FontLoader{ id: ableton6; source: 'qrc:/Resource/font/other/Mada-Regular.ttf' }
    FontLoader{ id: ableton7; source: 'qrc:/Resource/font/other/PlusJakartaSans-VariableFont_wght.ttf' }
    FontLoader{ id: ableton8; source: 'qrc:/Resource/font/other/SourceSans3-VariableFont_wght.ttf' }
    FontLoader{ id: ableton9; source: 'qrc:/Resource/font/other/SourceSerifPro-Regular.ttf' }
    FontLoader{ id: ableton10; source: 'qrc:/Resource/font/other/WorkSans-VariableFont_wght.ttf' }

    Grid {
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: 80
        rows: 2
        columns: 5

        columnSpacing: 2
        rowSpacing: 2

        Button { text: 'Sans Pro'; onClicked: topFontFamily = ableton1.name }
        Button { text: 'Open Sans'; onClicked: topFontFamily = ableton2.name }
        Button { text: 'Font 3'; onClicked: topFontFamily = ableton3.name }
        Button { text: 'Font 4'; onClicked: topFontFamily = ableton4.name }
        Button { text: 'Font 5'; onClicked: topFontFamily = ableton5.name }
        Button { text: 'Font 6'; onClicked: topFontFamily = ableton6.name }
        Button { text: 'Font 7'; onClicked: topFontFamily = ableton7.name }
        Button { text: 'Font 8'; onClicked: topFontFamily = ableton8.name }
        Button { text: 'Font 9'; onClicked: topFontFamily = ableton9.name }
        Button { text: 'Font 10'; onClicked: topFontFamily = ableton10.name }
    }


    // onVisibilityChanged: console.log('hhhhhhhhhhhhhhhhhhhhhhhhh', visible)

}
