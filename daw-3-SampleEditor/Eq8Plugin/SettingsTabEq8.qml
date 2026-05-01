import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "StackItemBar"
import "Item"
import "Item/ButtonTextField"

Item {
    id: root

    property alias tabIndex: bar.currentIndex

    TabBar {
        id: bar
        width: parent.width
        height: scaleSize(75)
        currentIndex: filterVisualizer.EQ4_activeBand

        background: Rectangle {
            color: theme.bg1
        }
        clip: true

        Repeater {
            model: filterVisualizer.bandModel
            delegate: TabButtonItem {
                text: index + 1
            }
        }
    }

    StackLayout {
        id: stack
        width: parent.width
        height: root.height - bar.height + topRadius - scaleSize(65)

        anchors.bottom: parent.bottom
        anchors.bottomMargin: scaleSize(65)

        currentIndex: bar.currentIndex

        Repeater {
            model: 4
            delegate: StackBarItem {}
        }
    }

    RowLayout {
        id: row

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        height: scaleSize(42)

        Button_TextField {
            id: tfBtn_left
            text: "L"
            Layout.fillHeight: true
            Layout.fillWidth: true
            value: _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                       name).leftWetLevel * 100
            checked: _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                         name).leftWetDryEnabled
            onMoved: {

                _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                            name).leftWetLevel = value / 100.0

                if (join_btn.checked) {

                    _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                                name).rightWetLevel = value / 100.0
                }
            }

            onPressed: {
                filterVisualizer.saveUndoOperation(1)
            }

            onClicked: {
                if (!tfBtn_left.checked) {
                    join_btn.checked = false
                }
                filterVisualizer.saveUndoOperation(1)
                _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                            name).leftWetDryEnabled = tfBtn_left.checked
            }
        }

        Button {
            id: join_btn
            Layout.fillHeight: true
            Layout.preferredWidth: Math.round(height * 1.2)
            checkable: true
            checked: true

            property double maxValue

            background: Rectangle {
                id: rec
                color: join_btn.checked ? trackEnable ? theme.b11_Enable1_FIL : theme.b20_Disable_FIL : theme.b10_Disable_FIL

                radius: topRadius
                border {
                    width: topWidth
                    color: theme.b10_Disable_BRD
                }
                Icon {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: 1
                    source: 'qrc:/Resource/Stereo_Icon.svg'
                    colorIcon: join_btn.checked ? trackEnable ? theme.a1 : theme.a1 : theme.a2

                    size: {
                        if (join_btn.height % 2 !== 0)
                            oddSize(join_btn.width * .7)
                        else
                            evenSize(join_btn.width * .7)
                    }
                }

                Rectangle {
                    visible: join_btn.focus
                    anchors.fill: parent
                    color: theme.b10_Disable_BRD
                    anchors.margins: -refSize
                    radius: topRadius
                    z: -1
                }
            }

            onClicked: {
                join_btn.focus = true

                maxValue = Math.max(
                            _clipArea.tracksModel.effectChain(
                                trackIndex).getEffect(name).leftWetLevel,
                            _clipArea.tracksModel.effectChain(
                                trackIndex).getEffect(name).rightWetLevel)

                _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                            name).rightWetDryEnabled = true
                _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                            name).leftWetDryEnabled = true

                _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                            name).leftWetLevel = join_btn.maxValue
                _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                            name).rightWetLevel = join_btn.maxValue
            }
        }

        Button_TextField {
            id: tfBtn_right
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: "R"
            value: _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                       name).rightWetLevel * 100
            checked: _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                         name).rightWetDryEnabled
            onMoved: {

                _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                            name).rightWetLevel = value / 100.0

                if (join_btn.checked) {
                    _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                                name).leftWetLevel = value / 100.0
                }
            }
            onPressed: {
                filterVisualizer.saveUndoOperation(1)
            }

            onClicked: {
                if (!tfBtn_right.checked) {
                    join_btn.checked = false

                    //filtersModel.setProperty(1, 'adjust', false)
                }
                filterVisualizer.saveUndoOperation(1)
                _clipArea.tracksModel.effectChain(trackIndex).getEffect(
                            name).rightWetDryEnabled = tfBtn_right.checked
            }
        }
    }
}
