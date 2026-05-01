import QtQuick 2.12

Rectangle {
    id: root
    width: 380
    height: 520

    color: "#6f6f6f" // main background (grey)

    property int topBarH: 46
    property int colorRowH: 32
    property int tabRowH: 40
    property int headerH: 34

    implicitWidth: contentCol.implicitWidth
    implicitHeight: contentCol.implicitHeight
    // ---------- Top search bar ----------
    Rectangle {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.topBarH
        color: "#1b1b1b"

        Rectangle {
            id: searchBox
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            height: 28
            radius: 3
            color: "#101010"
            border.color: "#2a2a2a"

            TextInput {
                id: searchInput
                anchors.left: parent.left
                anchors.right: searchIcon.left
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                color: "#d0d0d0"
                font.pixelSize: 14
                clip: true
                text: ""
                // placeholder
                Text {
                    visible: searchInput.text.length === 0
                             && !searchInput.activeFocus
                    text: "Search"
                    color: "#7d7d7d"
                    anchors.verticalCenter: searchInput.verticalCenter
                    anchors.left: searchInput.left
                }
            }

            // magnifier icon (simple)
            Item {
                id: searchIcon
                width: 22
                height: 22
                anchors.right: parent.right
                anchors.rightMargin: 6
                anchors.verticalCenter: parent.verticalCenter

                Canvas {
                    anchors.fill: parent
                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        ctx.strokeStyle = "#cfcfcf"
                        ctx.lineWidth = 2
                        ctx.beginPath()
                        ctx.arc(9, 9, 6, 0, Math.PI * 2)
                        ctx.stroke()
                        ctx.beginPath()
                        ctx.moveTo(14, 14)
                        ctx.lineTo(20, 20)
                        ctx.stroke()
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }

    // ---------- Color dots row + "Buttons" ----------
    Rectangle {
        id: colorRow
        anchors.top: topBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.colorRowH
        color: "#2a2a2a"

        Row {
            id: dotsRow
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            spacing: 12

            Repeater {
                model: ["#ff3b30", // red
                    "#ff9500", // orange
                    "#ffcc00", // yellow
                    "#34c759", // green
                    "#007aff", // blue
                    "#af52de" // purple
                ]
                delegate: Rectangle {
                    width: 14
                    height: 14
                    radius: 7
                    color: modelData
                    border.color: "#1a1a1a"
                    border.width: 1
                }
            }
        }

        Text {
            text: "Buttons"
            color: "#bdbdbd"
            font.pixelSize: 14
            anchors.right: parent.right
            anchors.rightMargin: 14
            anchors.verticalCenter: parent.verticalCenter
        }

        // optional tiny icon at far right
        Rectangle {
            width: 18
            height: 18
            radius: 3
            color: "transparent"
            border.color: "#9a9a9a"
            anchors.right: parent.right
            anchors.rightMargin: 6
            anchors.verticalCenter: parent.verticalCenter
            opacity: 0.7
        }
    }

    // ---------- Tabs row (EQ / Delay / Reverb) ----------
    Rectangle {
        id: tabsRowBg
        anchors.top: colorRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.tabRowH
        color: "#3a3a3a"

        property int selectedIndex: 2
        property var labels: ["EQ", "Delay", "Reverb"]

        Row {
            id: tabsRow
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 8
            spacing: 8

            Repeater {
                model: tabsRowBg.labels.length
                delegate: Rectangle {
                    property bool selected: index === tabsRowBg.selectedIndex
                    width: (index === 0 ? 44 : (index === 1 ? 72 : 88))
                    height: 26
                    radius: 3
                    color: selected ? "#e8e8e8" : "#4a4a4a"
                    border.color: "#1f1f1f"

                    Text {
                        anchors.centerIn: parent
                        text: tabsRowBg.labels[index]
                        color: selected ? "#111111" : "#e0e0e0"
                        font.pixelSize: 16
                        font.bold: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: tabsRowBg.selectedIndex = index
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }
        }

        Text {
            text: "Buttons"
            color: "#9f9f9f"
            font.pixelSize: 12
            anchors.left: tabsRow.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            opacity: 0.9
        }
    }

    // ---------- "No Plugin" header ----------
    Rectangle {
        id: header
        anchors.top: tabsRowBg.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: root.headerH
        color: "#4e4e4e"
        border.color: "#2a2a2a"

        Text {
            text: "No Plugin"
            color: "#e8e8e8"
            font.pixelSize: 16
            font.bold: true
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    // ---------- Content area with ListView placeholder ----------
    Rectangle {
        id: content
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#7a7a7a"

        // a real ListView (empty) + centered label like the screenshot
        ListView {
            id: listView
            anchors.fill: parent
            model: 0
            delegate: Item {
                width: listView.width
                height: 40
            }
        }

        Text {
            text: "ListView"
            color: "#1a1a1a"
            font.pixelSize: 14
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            opacity: 0.9
        }
    }
}
