import QtQuick 2.12
import QtGraphicalEffects 1.2

TextInput {
    id: root

    property string fullText: "audio "

    property bool  didMarker: focus

    property color bgColor: 'red'

    font.pixelSize:     elideText.fontSize
    font.letterSpacing: 0.5
    font.family:        topFontFamily
    font.weight:        Font.DemiBold

    text: focus ? fullText : ''

    selectByMouse:      true
    activeFocusOnPress: false

    selectedTextColor : color
    selectionColor :    'black'

    color: theme.tColor
    onFocusChanged: {
        if(focus) {
            root.forceActiveFocus()
        }
//        else {
//            trackAudioModel.setProperty(index, 'editName', false)
//        }
    }

    wrapMode: TextInput.NoWrap

    cursorDelegate: Rectangle {
        id:         markera
        visible:    root.focus
        anchors.verticalCenter: parent.verticalCenter
        width:      3; height: root.height
        radius:     1
        color:      'salmon'

        SequentialAnimation on color {
            loops: Animation.Infinite
            ColorAnimation { from: '#ff0000'; to: "#eaeaea"; duration: 350 }
            ColorAnimation { from: "#eaeaea"; to: '#ff0000'; duration: 350 }
        }
    }

    Component.onCompleted: ensureVisible(0)

//    GaussianBlur {
//        anchors.fill:   root
//        source:         root
//        radius:         0.5
//        samples:        Math.round(radius * 2+1)
//        cached:         true
//        visible:        root.focus
//    }

    onActiveFocusChanged: {
            // When we first gain focus, save the old text and select everything for clearing.
        if (activeFocus) {
            selectAll()
            delay(10, function() { root.selectAll() })
        }
    }

    onAccepted: {
        fullText =  root.text

        focus =     false

        ensureVisible(0)

        trackAudioModel.setProperty(index, 'editName', false)
    }
//    onTextEdited: {
//        selectAll()
//    }

    Timer { id: timer }

    function delay(delayTime, cb) {
        timer.interval = delayTime;
        timer.repeat = false;
        timer.triggered.connect(cb);
        timer.start();
    }

    InfoText_left {
        id:         elideText
        name:       root.fullText
        visible:    !root.focus
    }

}
