// SteppedValueScrubber.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: root

    property string text: ""

    property real fontSize: topfontPixelSize

    property real topPaddingTxt: 0
    property real bottomPaddingTxt: 0
    property real leftPaddingTxt: 0
    property real rightPaddingTxt: 0

    property bool right2Left: false

    // NEW: alignment parameters
    property int horizontalTextAlignment: Text.AlignHCenter // AlignLeft, AlignHCenter, AlignRight, AlignJustify
    property int verticalTextAlignment: Text.AlignVCenter // AlignTop, AlignVCenter, AlignBottom

    property string fontFamilty: topFontFamily
    property color fontColor: theme.f1_

    property real radius: 0

    property real value: 0
    property real minimumValue: 0
    property real maximumValue: 100
    property real stepSize: 1 // step size between min/max

    property string valueFormat: "%.0f" // how to format the number

    // how many pixels up/down per one step change
    property real pixelsPerStep: 8

    // --- internal ---
    readonly property real range: Math.max(1e-6, maximumValue - minimumValue)
    property bool isHovered: false
    property bool dragging: false

    // e.g. SteppedValueScrubber { clipArea: _clipArea }
    property var clipArea: _clipArea

    property color borderColor
    property color flColor
    property int topWidth
    property color barColor

    contentItem: Rectangle {
        anchors.fill: parent

        color: flColor
        radius: root.radius

        Rectangle {
            id: barFill

            radius: root.radius
            anchors.left: parent.left

            anchors.top: parent.top

            height: parent.height
            width: parent.width * Math.min(
                       1.0,
                       Math.max(0.0,
                                (root.value - root.minimumValue) / root.range))
            color: barColor
        }
        Rectangle {
            anchors.fill: parent

            color: "transparent"
            border.color: borderColor
            border.width: root.topWidth
            radius: root.radius
            visible: hovered
        }

        // Name / label
        Label {
            text: root.text
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color: fontColor
            horizontalAlignment: root.horizontalTextAlignment
            verticalAlignment: root.verticalTextAlignment
            opacity: !root.isHovered && !root.dragging ? 1.0 : 0.0

            visible: !isHovered && !dragging

            font {
                pixelSize: fontSize
                family: fontFamilty
                letterSpacing: 0.5
                weight: Font.DemiBold
            }
        }
        Label {
            id: valueLabel
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            color: fontColor
            horizontalAlignment: root.horizontalTextAlignment
            verticalAlignment: root.verticalTextAlignment
            visible: dragging
            text: root.value
            opacity: root.isHovered || root.dragging ? 1.0 : 0.0
            font {
                pixelSize: fontSize
                family: fontFamilty
                letterSpacing: 0.5
                weight: Font.DemiBold
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            preventStealing: true

            // global press point (fixed visual cursor position)
            property point pressGlobalPos
            // last total dy from press point (for incremental diff)
            property real lastDyFromPress: 0

            onEntered: root.isHovered = true
            onExited: root.isHovered = false

            onPressed: {
                if (root.clipArea && root.clipArea.globalPos) {
                    pressGlobalPos = root.clipArea.globalPos()
                    lastDyFromPress = 0
                }
                root.dragging = true

                // hide cursor
                if (root.clipArea && root.clipArea.setOverrideCursor)
                    root.clipArea.setOverrideCursor(Qt.BlankCursor)
            }

            onPositionChanged: {
                if (!root.dragging || !(mouse.buttons & Qt.LeftButton))
                    return
                if (!root.clipArea || !root.clipArea.globalPos)
                    return

                var p = root.clipArea.globalPos()

                var stepsDelta = p.y - pressGlobalPos.y

                var raw = root.value + stepsDelta * root.stepSize

                var stepIndex = Math.round(
                            (raw - root.minimumValue) / root.stepSize)
                var stepped = root.minimumValue + stepIndex * root.stepSize
                stepped = Math.max(root.minimumValue,
                                   Math.min(root.maximumValue, stepped))
                root.value = stepped

                // keep mouse visually at the press point
                if (root.clipArea.setCursorPosition)
                    root.clipArea.setCursorPosition(pressGlobalPos.x,
                                                    pressGlobalPos.y)
            }

            function finishDrag() {
                if (!root.dragging)
                    return
                root.dragging = false

                // restore cursor shape
                if (root.clipArea && root.clipArea.setOverrideCursor)
                    root.clipArea.setOverrideCursor(Qt.ArrowCursor)

                // keep cursor at press point when releasing (as requested)
                if (root.clipArea && root.clipArea.setCursorPosition
                        && pressGlobalPos)
                    root.clipArea.setCursorPosition(pressGlobalPos.x,
                                                    pressGlobalPos.y)
            }

            onReleased: finishDrag()
            onCanceled: finishDrag()
        }
    }
}
