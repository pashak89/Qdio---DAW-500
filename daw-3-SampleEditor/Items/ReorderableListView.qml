import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    clip: false

    // ---- API ----
    property var model: null
    property int rowHeight: 54
    property real spacing: 8
    property int longPressMs: 350
    property bool draggingEnabled: false
    property int topPadding: 0
    property int bottomPadding: 0

    property string proxyTextRole: "name"

    implicitHeight: topPadding + listView.contentHeight + bottomPadding
    height: implicitHeight

    // User-provided delegate content (visual)
    property Component contentDelegate_: null
    property Component proxyDelegate: null

    signal moved(int from, int to)

    property real radius: 0
    property color indicatorColor: "#00d1ff"
    property color proxyBorderColor: "#00d1ff"
    property color proxyColor: "#2b2b2b"

    QtObject {
        id: dragCtl

        property bool dragging: false
        property point dragPos: Qt.point(0, 0) // in root coords

        property int sourceIndex: -1
        property int targetInsert: -1 // 0..count insertion index
        property var draggedData: ({}) // only for proxy visuals

        function clamp(v, lo, hi) {
            return Math.max(lo, Math.min(hi, v))
        }

        function beginDrag(index, data, posInRoot) {
            if (dragging || !root.model)
                return
            if (index < 0 || index >= root.model.count)
                return

            dragging = true
            sourceIndex = index
            draggedData = data
            dragPos = posInRoot

            updateWhileDragging()
        }

        function updateDrag(posInRoot) {
            if (!dragging)
                return
            dragPos = posInRoot
            updateWhileDragging()
        }

        function computeInsertionIndexLinear(yInView) {
            if (!root.model)
                return 0

            if (yInView < 0)
                return 0
            if (yInView > listView.height)
                return root.model.count

            var cy = listView.contentY + yInView

            for (var i = 0; i < root.model.count; ++i) {
                var it = listView.itemAtIndex(i)
                if (!it)
                    continue

                var mid = it.y + it.height / 2
                if (cy < mid)
                    return i
            }
            return root.model.count
        }

        function computeInsertionIndex() {
            if (!root.model)
                return 0

            var p = listView.mapFromItem(root, dragPos.x, dragPos.y)

            if (p.y < 0)
                return 0
            if (p.y > listView.height)
                return root.model.count

            var cy = listView.contentY + p.y
            var idx = listView.indexAt(p.x, p.y)

            if (idx === -1)
                return computeInsertionIndexLinear(p.y)

            // If hovering the dragged item itself, bias to neighbor so line moves
            if (idx === sourceIndex) {
                var itSelf = listView.itemAtIndex(idx)
                if (itSelf) {
                    var midSelf = itSelf.y + itSelf.height / 2
                    if (cy < midSelf)
                        idx = Math.max(0, idx - 1)
                    else
                        idx = Math.min(root.model.count - 1, idx + 1)
                }
            }

            var it = listView.itemAtIndex(idx)
            if (!it)
                return idx

            var midY = it.y + it.height / 2
            var after = (cy >= midY)
            return after ? (idx + 1) : idx
        }

        function updateWhileDragging() {
            if (!root.model)
                return
            var ins = computeInsertionIndex()
            targetInsert = clamp(ins, 0, root.model.count)
        }

        function drop() {
            if (!dragging || !root.model)
                return

            // Move only now (C++ level)
            root.model.moveRowQml(sourceIndex, targetInsert)
            root.moved(sourceIndex, targetInsert)

            dragging = false
            sourceIndex = -1
            targetInsert = -1
            draggedData = ({})
        }

        function cancel() {
            dragging = false
            sourceIndex = -1
            targetInsert = -1
            draggedData = ({})
        }
    }

    Item {
        id: frame
        anchors.fill: parent

        ListView {
            id: listView
            width: parent.width
            anchors.left: parent.left
            anchors.right: parent.right
            y: root.topPadding
            height: contentHeight

            clip: true
            spacing: root.spacing
            model: root.model
            interactive: false

            delegate: Item {
                id: row
                width: ListView.view.width
                height: root.rowHeight

                // Hide the *real* row that is being dragged (we show proxy instead)
                opacity: (dragCtl.dragging
                          && index === dragCtl.currentIndex) ? 0 : 1

                Loader {
                    id: contentLoader
                    anchors.fill: parent
                    sourceComponent: root.contentDelegate_

                    // pass data into loaded delegate
                    property var modelData: model
                    property int modelIndex: index

                    onStatusChanged: {
                        if (contentLoader.status === Loader.Ready
                                && contentLoader.item)
                            contentLoader.item.modelData_ = modelData
                    }
                    onModelDataChanged: {
                        if (modelData === undefined || modelData === null)
                            return
                        if (contentLoader.item)
                            contentLoader.item.modelData_ = modelData
                    }
                }

                MouseArea {
                    id: pressArea
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    propagateComposedEvents: true
                    enabled: draggingEnabled

                    property bool longPressed: false

                    Timer {
                        id: longPressTimer
                        interval: root.longPressMs
                        repeat: false
                        onTriggered: {
                            pressArea.longPressed = true

                            var p = row.mapToItem(root, pressArea.mouseX,
                                                  pressArea.mouseY)

                            // Only used for proxy visuals
                            var original = root.model.get(index)
                            var dataCopy = {}
                            for (var k in original)
                                dataCopy[k] = original[k]

                            dragCtl.beginDrag(index, dataCopy,
                                              Qt.point(p.x, p.y))
                        }
                    }

                    onPressed: {
                        longPressed = false
                        longPressTimer.restart()
                    }

                    onPositionChanged: {
                        if (!pressArea.longPressed || !dragCtl.dragging)
                            return
                        var p = row.mapToItem(root, mouseX, mouseY)
                        dragCtl.updateDrag(Qt.point(p.x, p.y))
                    }

                    onReleased: {
                        longPressTimer.stop()
                        if (pressArea.longPressed && dragCtl.dragging) {
                            pressArea.longPressed = false
                            dragCtl.drop()
                        }
                    }

                    onCanceled: {
                        longPressTimer.stop()
                        pressArea.longPressed = false
                        dragCtl.cancel()
                    }
                }
            }

            // Optional indicator line (shows insertion position)
            Rectangle {
                id: indicator
                width: parent.width
                height: 2
                radius: radius
                color: root.indicatorColor
                visible: dragCtl.dragging

                y: {
                    if (!dragCtl.dragging || !root.model)
                        return -9999

                    var ins = dragCtl.targetInsert
                    var yPos = 0

                    if (ins <= 0) {
                        yPos = 0
                    } else if (ins >= root.model.count) {
                        yPos = listView.contentHeight - listView.contentY - indicator.height
                    } else {
                        var it = listView.itemAtIndex(ins)
                        yPos = it ? (it.y - listView.contentY) : 0
                    }

                    // keep inside visible area (ListView is clipped)
                    return Math.max(
                                0, Math.min(yPos,
                                            listView.height - indicator.height))
                }
            }
        }
    }

    // ---- Floating drag proxy ----
    Item {
        id: proxy
        visible: dragCtl.dragging
        z: 999

        width: frame.width
        height: root.rowHeight

        x: dragCtl.dragPos.x - width / 2
        y: dragCtl.dragPos.y - height / 2

        Rectangle {
            anchors.fill: parent
            color: root.proxyColor
            border.color: root.proxyBorderColor
            border.width: 1
            radius: root.radius
        }

        Loader {
            id: proxyLoader
            anchors.fill: parent
            sourceComponent: root.proxyDelegate

            property var proxyData: dragCtl.draggedData

            onStatusChanged: {
                if (proxyLoader.status === Loader.Ready && proxyLoader.item)
                    proxyLoader.item.modelData_ = proxyData
            }
            onProxyDataChanged: {
                if (proxyData === undefined || proxyData === null)
                    return
                if (proxyLoader.item)
                    proxyLoader.item.modelData_ = proxyData
            }
        }
    }

    Keys.onEscapePressed: dragCtl.cancel()
}
