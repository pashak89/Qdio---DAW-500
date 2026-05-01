import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Item"

Rectangle {
    id: root

    color: theme.bg2
    radius: scaleSize(10)

    property int indexTab: index

    Rectangle {
        visible: index === 0 | index === 3
        anchors {
            left: parent.left
            leftMargin: index === 3 ? height : 0
            right: parent.right
            rightMargin: index === 0 ? height : 0
        }

        height: scaleSize(10) + 2
        color: parent.color
    }

    property bool isCurrentTabItem: index === stack.currentIndex

    ListModel {
        id: filterImageModel

        ListElement {
            url: 'qrc:/Resource/EQ4_HighPass12dB.svg'
        }
        ListElement {
            url: 'qrc:/Resource/EQ4_Bell.svg'
        }
        ListElement {
            url: 'qrc:/Resource/EQ4_HighShelf.svg'
        }
        ListElement {
            url: 'qrc:/Resource/EQ4_LowPass12dB.svg'
        }
        ListElement {
            url: 'qrc:/Resource/EQ4_HighPass24dB.svg'
        }
        ListElement {
            url: 'qrc:/Resource/EQ4_Peak.svg'
        }
        ListElement {
            url: 'qrc:/Resource/EQ4_LowShelf.svg'
        }
        ListElement {
            url: 'qrc:/Resource/EQ4_LowPass24dB.svg'
        }
    }

    ColumnLayout {
        id: colMain
        anchors {
            fill: parent
            topMargin: scaleSize(15)
            margins: scaleSize(15)
            bottomMargin: scaleSize(10)
        }

        spacing: scaleSize(10)

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            RowLayout {
                id: row
                anchors.fill: parent
                spacing: scaleSize(5)

                Repeater {
                    model: filterVisualizer.filtersModel(indexTab)
                    delegate: colTFDialText
                }
            }
        }

        Item {
            Layout.preferredHeight: scaleSize(95)
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter

            GridLayout {
                id: grid
                columns: 4
                rows: 2
                anchors.fill: parent

                columnSpacing: scaleSize(27)
                rowSpacing: scaleSize(12)

                Repeater {
                    model: filterImageModel
                    delegate: filterComponent
                }
            }
        }
    }

    Component {
        id: filterComponent
        RadioButton {
            id: control
            checked: filterVisualizer.EQ4_FilterType === index
            Component.onCompleted: {
                if (filterVisualizer.EQ4_FilterType === 1 | filterVisualizer.EQ4_FilterType
                        === 2 | filterVisualizer.EQ4_FilterType === 6) {

                    filterVisualizer.setFilterEnabled(1, true)
                } else {

                    filterVisualizer.setFilterEnabled(1, false)
                }
            }

            onClicked: {
                focus = true
                filterVisualizer.saveUndoOperation()
                filterVisualizer.EQ4_FilterType = index
                if (index === 1 | index === 2 | index === 6) {

                    filterVisualizer.setFilterEnabled(1, true)
                } else {

                    filterVisualizer.setFilterEnabled(1, false)
                }
            }

            padding: 0
            Layout.fillHeight: true
            Layout.fillWidth: true

            indicator: Rectangle {
                id: rec
                anchors.fill: parent

                color: control.checked ? trackEnable ? theme.b11_Enable1_FIL : theme.b20_Disable_FIL : theme.b10_Disable_FIL

                radius: topRadius
                border {
                    width: topWidth
                    color: theme.b10_Disable_BRD
                }
            }

            contentItem: null

            Icon {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: scaleSize(2)
                size: scaleSize(44)
                source: url
                colorIcon: control.checked ? trackEnable ? theme.a1 : theme.a1 : theme.a2
            }

            Rectangle {
                visible: control.focus
                anchors.fill: rec
                color: theme.b10_Disable_BRD
                anchors.margins: -refSize
                radius: topRadius
                z: -1
            }
        }
    }

    Component {
        id: colTFDialText

        ColumnLayout {
            id: col
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: scaleSize(15)

            property double originalValue: model.value
            onOriginalValueChanged: {
                tf.value = model.value
                tf._value = tf.value

                if (model.variance) {

                    var new_val = (Math.log(model.value / tf.min) / Math.log(
                                       tf.max / tf.min))

                    var value1 = new_val * (tf.max - tf.min) + tf.min

                    dialt.value = value1
                } else {


                    dialt.value = model.value
                }

                var _decimal = cursorPositionClass.getDecimal(tf._value)

                tf.text = tf._value >= 1000 ? (tf._value / 1000).toFixed(
                                                  _decimal) : tf._value.toFixed(
                                                  _decimal)

            }

            TextFieldSyncDial {
                id: tf
                Layout.preferredHeight: scaleSize(40)
                Layout.preferredWidth: scaleSize(120)

                unit: model.unit

                property color flColor: theme.s11_Enable_FIL
                property color brColor: theme.s10_Disable_BRD

                max: model.max
                min: model.min
                isDecimal: model.decimal
                defaultValue: model.dValue

                //value: model.value
                variance: model.variance

                disable: !model.enabled
                onPressed: {
                    filterVisualizer.saveUndoOperation()
                }

                onMoved: {
                    var new_val = value

                    if (model.variance) {
                        var x = (value - tf.min) / (tf.max - tf.min)
                        new_val = tf.min * Math.pow(
                                    10, x * Math.log(
                                        tf.max / tf.min) / Math.log(10))
                    }

                    model.value = new_val
                    if (index === 0) {
                        filterVisualizer.EQ4_Freq = new_val
                    }
                    if (index === 1) {
                        filterVisualizer.EQ4_Gain = new_val
                    }
                    if (index === 2) {
                        filterVisualizer.EQ4_QFactor = new_val
                    }
                }
            }

            ArcDial {
                id: dialt
                Layout.fillHeight: true
                Layout.fillWidth: true

                property bool isEnabled: !model.active || (model.active
                                                           && model.automated)
                disable: !model.enabled
                defaultValue: model.dValue

                //value: model.value
                variance: model.variance

                min: model.min
                max: model.max

                onPressed: {
                    filterVisualizer.saveUndoOperation()
                }
                onMoved: {

                    var new_val = value

                    if (model.variance) {
                        var x = (value - tf.min) / (tf.max - tf.min)
                        new_val = tf.min * Math.pow(
                                    10, x * Math.log(
                                        tf.max / tf.min) / Math.log(10))
                    }

                    model.value = new_val
                    if (index === 0) {
                        filterVisualizer.EQ4_Freq = new_val
                    }
                    if (index === 1) {
                        filterVisualizer.EQ4_Gain = new_val
                    }
                    if (index === 2) {
                        filterVisualizer.EQ4_QFactor = new_val
                    }
                }

                isActive: model.active
                isAutomated: model.automated

                function callback(index, text) {

                    if (text === "Re-Enable Automation") {
                        model.enabled = true
                        filterVisualizer.setAutomatedEnabled(index, true)
                    }

                    if (text === "Show Automation") {
                        _clipArea.tracksModel.setAutomationLaneEnabled(
                                    trackIndex, true)

                        _clipArea.tracksModel.setAutomationMenuTitle(
                                    trackIndex, "EQ",
                                    model.nameFilter + model.filterType)
                    }

                    if (text === "Disable Automation") {

                        var new_val = dialt.value

                        if (model.variance) {
                            var x = (dialt.value - tf.min) / (tf.max - tf.min)
                            new_val = tf.min * Math.pow(
                                        10, x * Math.log(
                                            tf.max / tf.min) / Math.log(10))
                        }

                        model.value = new_val
                        if (index === 0) {
                            filterVisualizer.EQ4_Freq = new_val
                        }
                        if (index === 1) {
                            filterVisualizer.EQ4_Gain = new_val
                        }
                        if (index === 2) {
                            filterVisualizer.EQ4_QFactor = new_val
                        }

                        _clipArea.tracksModel.setAutomationEnabled(
                                    trackIndex, "EQ",
                                    model.nameFilter + model.filterType, false)
                    }
                }

                MouseArea {
                    anchors.fill: parent

                    acceptedButtons: Qt.RightButton
                    onClicked: {

                        positionInWindow = Qt.point(mouse.x, mouse.y)

                        rightPanel.leftModules = dialt.isEnabled ? ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation'] : ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                        rightPanel.isTwoGroups = false
                        rightPanel.firstStr = ''
                        rightPanel.cc = parent
                        rightPanel.index = index
                        rightPanel.callback = dialt.callback
                        rightPanel.parent = dialt

                        rightPanel.visible = true
                    }
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.topMargin: -scaleSize(70)

                InfoText {
                    id: title
                    text: model.nameFilter
                }
            }
        }
    }
}
