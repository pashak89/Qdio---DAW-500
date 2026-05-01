// TrackPanel/components/TrackButtonsRow.qml
import QtQuick 2.15
import Global 1.0
import "../../Audio"

Row {
    id: root
    property var clipArea

    spacing: Global.scaleSize2(8)
    width: parent.width
    height: Global.scaleSize2(37)

    //anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
    Select {
        id: selectBtn
        name: model.index + 1
        x: 0
        width: (root.width - root.spacing) / 2
        height: Global.scaleSize2(37)
        y: root.height / 2 - selectBtn.height / 2

        moduls: (!model.automatedSpeakerOnEnabled
                 && model.automatedSpeakerOnActive) ? ['Show Automation', 'Show Automation in New Lane', 'Re-Enable Automation', 'Delete Automation'] : ['Show Automation', 'Show Automation in New Lane', 'Delete Automation']

        activated: model.trackActivated
        mute: model.speakerOnValue === 0

        property real automatedValue: model.automatedSpeakerOnValue
        property real automatedNormalized: model.automatedSpeakerOnNormalized
        property real automatedActive: model.automatedSpeakerOnActive
        property real automatedEnabled: model.automatedSpeakerOnEnabled
        property real normalized: model.speakerOnNormalized

        onNormalizedChanged: selectBtn.mute = model.speakerOnValue === 0
        Rectangle {
            visible: model.automatedSpeakerOnActive
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.leftMargin: 1
            color: model.automatedSpeakerOnEnabled ? "#EA2027" : "#808080"
            width: Global.scaleSize2(11)
            height: Global.scaleSize2(11)
            radius: Global.scaleSize2(11)
        }

        onAction: {
            if (text == "Re-Enable Automation") {
                model.automatedEnabled = true
                selectBtn.moduls
                        = ['Show Automation', 'Show Automation in New Lane', 'Delete Automation']
            }
            if (text == "Delete Automation")
                model.clear = true
        }
    }

    Solo {
        width: (root.width - root.spacing) / 2
        x: selectBtn.width + root.spacing
        height: Global.scaleSize2(37)
        y: root.height / 2 - selectBtn.height / 2
    }
}
