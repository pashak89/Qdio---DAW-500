import QtQuick.Controls 2.15
import QtQuick 2.3
import "Items"
import "TrackPanels/Audio"
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15
import QtQuick 2.15

Item {
    id: root

    // --- Layout base width for content ---
    property int _width: scaleSize2(300)
    property int minWidth: scaleSize2(300)

    // Root size is driven by its content
    implicitWidth: _width + 2 * scaleSize2(20) // left + right margins

    property real currentItemHeight: 0

    implicitHeight: root.currentItemHeight + 2 * scaleSize2(
                        20) // top + bottom margins

    // Make root actually use its implicit size by default
    width: implicitWidth
    height: implicitHeight

    // --- Inputs ---
    property string title: "27L"
    property real gainDb: 6.00
    property real pan: 22.3
    property real fader: 0.55 // 0..1
    property real meterL: 0.88 // 0..1
    property real meterR: 0.95 // 0..1
    property real peakHold: 0.92 // 0..1
    property bool clip: false
    property real meterSplit: 0.99 // 0..1

    property bool holdShift: false
    property bool holdCtrl: _areaInfo.ctrlkeyPressed

    property var cursorPosition: cursorPositionClass

    property string topFontFamily: myFont2.name
    property int topfontPixelSize: scaleSize2(28)
    property real topRadius: radiusCal(3)
    property int topWidth: resolution[0] >= 3840 & resolution[1]
                           >= 2170 ? ((refSize < .83) ? 1 : (refSize < 1.12) ? 2 : 2) : ((refSize < .83) ? 1 : (refSize < 1.12) ? 1 : 2)

    property var resolution: cursorPosition.getResolution()

    property real refSize: _areaInfo.zoomFactor
    property real _scale: cursorPosition.getScale()

    // === math: invert your DB(Y) curve ===
    // DB = -42.3529 * Y^2 - 33.6471 * Y + 6
    // given real dB (negative) -> Y in [0..1]
    function decibel2yNorm(db) {
        var a = -42.3529
        var b = -33.6471
        var c = 6 - db

        var discriminant = b * b - 4 * a * c
        if (discriminant < 0)
            return NaN

        var sqrtD = Math.sqrt(discriminant)

        // Y >= 0 root
        return (-b - sqrtD) / (2 * a)
    }

    // Convert *display* dB (0,3,6,10,…) to pixel Y
    // top = 0, bottom = height
    function dbDisplayToYPixel(dbDisplay, h) {
        var realDb = -dbDisplay
        // 0 -> 0 dB, 60 -> -60 dB
        var yNorm = decibel2yNorm(realDb)
        return yNorm * h
    }

    Theme1 {
        id: theme1
    }
    Theme2 {
        id: theme2
    }
    property Item theme: _areaInfo.themeType === 0 ? theme1 : theme2

    FontLoader {
        id: myFont
        source: "qrc:/Resource/font/Open_Sans/OpenSans-VariableFont.ttf"
    }
    FontLoader {
        id: myFont2
        source: "qrc:/Resource/font/SourceSansPro-Regular.ttf"
    }

    function radiusCal(value) {
        value = value * _scale * refSize
        return Math.round(value)
    }
    function scaleSize2(value) {
        value = Math.round(value * _scale * refSize / 2)
        return value
    }
    function oddSize(value) {
        value = Math.round(value)
        if (value % 2 !== 0)
            return value
        else
            return value - 1
    }
    function evenSize(value) {
        value = Math.round(value)
        if (value % 2 === 0)
            return value
        else
            return value - 1
    }

    Rectangle {
        anchors.fill: parent
        color: theme.bg2
        border.color: "black"
        border.width: 1
    }

    Component {
        id: fxBlankDelegate
        Rectangle {
            height: scaleSize2(57)
            width: parent.width
            color: theme.mixerDD100_DeActive_FIL
            radius: scaleSize2(5)
        }
    }
    Component {
        id: fxHoverDelegate
        Item {
            id: delegateRoot
            Rectangle {
                id: borderRec
                z: -1
                anchors {
                    fill: parent
                    topMargin: -1
                    bottomMargin: -1
                }

                color: theme.mixerDD11_Select1_FIL
                border.color: theme.mixerDD11_Select1_BRD
                border.width: topWidth
                radius: topRadius
            }

            Item {
                id: dropdownButton
                anchors.right: parent.right
                anchors.rightMargin: scaleSize2(20)
                anchors.verticalCenter: parent.verticalCenter

                width: scaleSize2(25)
                height: scaleSize2(25)

                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                }

                Image {
                    id: icon
                    anchors.fill: parent

                    source: "qrc:/resources/svg/track_list/Triangle.svg"

                    smooth: true

                    ColorOverlay {
                        anchors.fill: icon
                        source: icon
                        color: theme.button_unclicked_color
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onPressed: {

                        if (popup.visible) {
                            popup.close()
                            return
                        }

                        var overlayItem = popup.parent
                        var p = delegateRoot.mapToItem(overlayItem, 0,
                                                       delegateRoot.height)

                        popup.x = p.x
                        popup.y = p.y + scaleSize2(2)
                        popup.visible = true
                    }
                }
            }
        }
    }

    Component {
        id: sendsLabelDelegate
        SteppedValueScrubber {
            id: steppedValueScrubber
            anchors.fill: parent

            property var modelData_

            property bool isActive: modelData_.enabled

            text: modelData_.name

            topWidth: topWidth
            radius: topRadius

            borderColor: steppedValueScrubber.isActive ? theme.mixerDD21_Select2_BRD : theme.mixerDD11_Select1_BRD
            flColor: steppedValueScrubber.isActive ? theme.mixerDD21_Select2_FIL : theme.mixerDD11_Select1_FIL
            barColor: steppedValueScrubber.isActive ? theme.mixerDD21_Select2_BAR : theme.mixerDD11_Select1_BAR

            minimumValue: 0
            maximumValue: 500
            stepSize: 1
            value: 40

            Rectangle {
                id: circleButton
                width: parent.height - scaleSize2(19)
                height: width
                visible: steppedValueScrubber.isHovered
                         || steppedValueScrubber.dragging

                anchors.leftMargin: scaleSize2(19)
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                radius: width / 2
                color: steppedValueScrubber.isActive ? theme.b31_Enable_FIL : theme.b300_Disable2_FIL
                border.color: theme.bg5
                border.width: 1

                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    onClicked: {

                        steppedValueScrubber.isActive = !steppedValueScrubber.isActive
                    }
                }
            }
        }
    }

    Component {
        id: fxLabelDelegate
        CustomLabel {

            property var modelData_
            height: scaleSize2(57)
            color: modelData_.enabled ? theme.mixerDD10_DeSelect_FIL : theme.mixerDD100_DeActive_FIL
            fontSize: topfontPixelSize
            fontFamilty: topFontFamily
            fontColor: modelData_.enabled ? theme.f1_ : theme.f5_
            radius: scaleSize2(5)
            width: parent.width
            text: modelData_.name
            leftPaddingTxt: scaleSize2(15)
            horizontalTextAlignment: Text.AlignLeft
        }
    }

    Component {
        id: fxEditDelegate

        Item {

            height: scaleSize2(57)
            property var modelData_
            onEnabledChanged: {
                dropdownButton.requestPaint()
            }

            Rectangle {
                id: borderRec
                z: -1
                anchors {
                    fill: parent
                }
                color: modelData_.enabled ? theme.mixerDD21_Select2_FIL : theme.mixerDD11_Select1_FIL
                border.color: theme.mixerDD11_Select1_BRD
                border.width: topWidth
                radius: topRadius
            }

            RowLayout {
                anchors.fill: parent

                // --- left circular button ------------------------------------------
                Rectangle {
                    id: circleButton
                    Layout.preferredWidth: parent.height - scaleSize2(19)
                    Layout.preferredHeight: Layout.preferredWidth
                    Layout.leftMargin: scaleSize2(19)
                    radius: width / 2
                    color: modelData_.enabled ? theme.b31_Enable_FIL : theme.b300_Disable2_FIL
                    border.color: theme.bg5
                    border.width: 1

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {

                            modelData_.enabled = !modelData_.enabled
                        }
                    }
                }

                // --- middle square "open" button -----------------------------------
                Item {
                    id: openButton
                    Layout.preferredWidth: scaleSize2(47)
                    Layout.preferredHeight: scaleSize2(47)
                    Layout.leftMargin: scaleSize2(3)

                    Rectangle {
                        anchors.fill: parent
                        color: modelData_.controlVisible ? theme.b31_Enable_FIL : theme.b300_Disable2_FIL
                        border.color: theme.b300_Disable2_BRD
                        border.width: 1
                        radius: scaleSize2(5)
                    }

                    Image {
                        id: icon
                        anchors.fill: parent
                        anchors.margins: scaleSize2(7)
                        source: "qrc:/Resource/ShowPlugin_Icon.svg" // or "qrc:/images/external_link.svg"
                        scale: 0.9
                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: theme.bg5
                        }
                    }
                    MouseArea {
                        anchors.fill: parent

                        onClicked: {

                            if (modelData_.controlVisible === false) {
                                _clipArea.tracksModel.effectChain(
                                            _areaInfo.selectedTrackItem).getEffect(
                                            modelData_.name).showControl()
                            } else {

                                _clipArea.tracksModel.effectChain(
                                            _areaInfo.selectedTrackItem).getEffect(
                                            modelData_.name).hideControl()
                            }
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }

                // --- right dropdown arrow ------------------------------------------
                Item {
                    id: dropdownButton

                    Layout.preferredWidth: scaleSize2(25)
                    Layout.preferredHeight: scaleSize2(25)
                    Layout.rightMargin: scaleSize2(20)
                    Layout.alignment: Qt.AlignHCenter

                    Rectangle {
                        anchors.fill: parent
                        color: "transparent"
                    }

                    Image {
                        id: icon2
                        anchors.fill: parent

                        source: "qrc:/resources/svg/track_list/Triangle.svg"

                        smooth: true

                        ColorOverlay {
                            anchors.fill: icon2
                            source: icon2
                            color: theme.button_unclicked_color
                        }
                    }
                }
            }
        }
    }

    property int currentIndex: _areaInfo.selectedTrackItem

    Repeater {
        model: _clipArea.tracksModel
        id: repeaterMainColumn
        delegate: Column {
            id: mainColumn
            visible: index === parent.currentIndex

            onHeightChanged: {
                if (visible)
                    root.currentItemHeight = mainColumn.implicitHeight
            }
            Component.onCompleted: {
                if (visible)
                    root.currentItemHeight = mainColumn.implicitHeight
            }

            onVisibleChanged: {
                if (visible)
                    root.currentItemHeight = mainColumn.implicitHeight
            }

            // position inside root, margins handled here
            x: scaleSize2(20)
            y: scaleSize2(20)
            width: _width
            spacing: scaleSize2(10)

            Rectangle {

                color: theme.bg5
                radius: topRadius
                height: fxcontentCol.implicitHeight + 2 * scaleSize2(11)
                width: parent.width

                Column {
                    id: fxcontentCol

                    width: parent.width - scaleSize2(21)

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    spacing: scaleSize2(5)

                    CustomLabel {
                        fontSize: topfontPixelSize
                        fontFamilty: topFontFamily
                        fontColor: theme.f9_
                        height: scaleSize2(38)
                        width: parent.width
                        radius: topRadius
                        text: "FX"
                    }

                    ReorderableListView {
                        id: reorderView
                        z: repeaterMainColumn.z + 1

                        width: parent.width

                        radius: topRadius

                        topPadding: scaleSize2(5)
                        bottomPadding: scaleSize2(5)
                        model: _clipArea.tracksModel.effectChain(
                                   _areaInfo.selectedTrackItem)
                        rowHeight: scaleSize2(57)

                        draggingEnabled: holdCtrl
                        spacing: scaleSize2(5)
                        longPressMs: 350
                        proxyTextRole: "name"

                        contentDelegate_: Item {
                            id: row
                            anchors.fill: parent

                            property var myModel

                            Loader {
                                id: loader
                                anchors.fill: parent
                                property var modelData_: modelData
                                sourceComponent: fxLabelDelegate
                                onLoaded: {
                                    loader.item.modelData_ = modelData_
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true

                                propagateComposedEvents: true

                                onEntered: {

                                    loader.sourceComponent = fxEditDelegate
                                }

                                onExited: {

                                    loader.sourceComponent = fxLabelDelegate
                                }
                            }
                        }

                        proxyDelegate: Component {

                            CustomLabel {

                                height: scaleSize2(57)
                                color: proxyData.enabled ? theme.mixerDD10_DeSelect_FIL : theme.mixerDD100_DeActive_FIL
                                fontSize: topfontPixelSize
                                fontFamilty: topFontFamily
                                fontColor: proxyData.enabled ? theme.f1_ : theme.f5_
                                radius: scaleSize2(5)
                                width: parent.width
                                text: proxyData.name
                                leftPaddingTxt: scaleSize2(15)
                                horizontalTextAlignment: Text.AlignLeft
                            }
                        }

                        onMoved: (from, to) => {
                                     console.log("Moved from", from, "to", to)
                                 }
                    }
                    Item {

                        width: parent.width
                        height: scaleSize2(57)

                        Loader {
                            id: blankLoader
                            anchors.fill: parent

                            sourceComponent: fxBlankDelegate
                        }

                        HoverHandler {

                            acceptedDevices: PointerDevice.Mouse

                            onHoveredChanged: {

                                if (hovered) {
                                    blankLoader.sourceComponent = fxHoverDelegate
                                } else {
                                    blankLoader.sourceComponent = fxBlankDelegate
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {

                color: theme.bg5
                radius: topRadius
                height: lSendsContentCol.implicitHeight + 2 * scaleSize2(11)
                width: parent.width

                Column {
                    id: lSendsContentCol

                    width: parent.width - scaleSize2(21)

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    spacing: scaleSize2(5)

                    CustomLabel {
                        fontSize: topfontPixelSize
                        fontFamilty: topFontFamily
                        fontColor: theme.f9_
                        height: scaleSize2(38)
                        width: parent.width
                        radius: topRadius
                        text: "L Sends"
                    }

                    ReorderableListView {

                        width: parent.width

                        radius: topRadius

                        topPadding: scaleSize2(5)
                        bottomPadding: scaleSize2(5)
                        model: _clipArea.tracksModel.effectChain(
                                   _areaInfo.selectedTrackItem)
                        rowHeight: scaleSize2(57)

                        draggingEnabled: holdCtrl
                        spacing: scaleSize2(5)
                        longPressMs: 350
                        proxyTextRole: "name"

                        contentDelegate_: Item {

                            anchors.fill: parent

                            property var myModel

                            Loader {
                                id: lSendsLoader
                                anchors.fill: parent
                                property var modelData_: modelData
                                sourceComponent: sendsLabelDelegate
                                onLoaded: {
                                    lSendsLoader.item.modelData_ = modelData_
                                }
                            }

                            HoverHandler {
                                acceptedDevices: PointerDevice.Mouse

                                onHoveredChanged: {
                                    if (hovered) {
                                        gSendsLoader.sourceComponent = sendsLabelDelegate
                                    }
                                }
                            }
                        }

                        proxyDelegate: Component {

                            CustomLabel {

                                height: scaleSize2(57)
                                color: proxyData.enabled ? theme.mixerDD10_DeSelect_FIL : theme.mixerDD100_DeActive_FIL
                                fontSize: topfontPixelSize
                                fontFamilty: topFontFamily
                                fontColor: proxyData.enabled ? theme.f1_ : theme.f5_
                                radius: scaleSize2(5)
                                width: parent.width
                                text: proxyData.name
                                leftPaddingTxt: scaleSize2(15)
                                horizontalTextAlignment: Text.AlignLeft
                            }
                        }

                        onMoved: (from, to) => {
                                     console.log("Moved from", from, "to", to)
                                 }
                    }

                    Item {

                        width: parent.width
                        height: scaleSize2(57)

                        Loader {
                            id: lSendsBlankLoader
                            anchors.fill: parent

                            sourceComponent: fxBlankDelegate
                        }

                        HoverHandler {
                            acceptedDevices: PointerDevice.Mouse

                            onHoveredChanged: {
                                if (hovered) {
                                    lSendsBlankLoader.sourceComponent = fxHoverDelegate
                                } else {
                                    lSendsBlankLoader.sourceComponent = fxBlankDelegate
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {

                color: theme.bg5
                radius: topRadius
                height: gSendsContentCol.implicitHeight + 2 * scaleSize2(11)
                width: parent.width

                Column {
                    id: gSendsContentCol

                    width: parent.width - scaleSize2(21)

                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter

                    spacing: scaleSize2(5)

                    CustomLabel {
                        fontSize: topfontPixelSize
                        fontFamilty: topFontFamily
                        fontColor: theme.f9_
                        height: scaleSize2(38)
                        width: parent.width
                        radius: topRadius
                        text: "G Sends"
                    }

                    ReorderableListView {

                        width: parent.width

                        radius: topRadius

                        topPadding: scaleSize2(5)
                        bottomPadding: scaleSize2(5)
                        model: _clipArea.tracksModel.effectChain(
                                   _areaInfo.selectedTrackItem)
                        rowHeight: scaleSize2(57)

                        draggingEnabled: holdCtrl
                        spacing: scaleSize2(5)
                        longPressMs: 350
                        proxyTextRole: "name"

                        contentDelegate_: Item {

                            anchors.fill: parent

                            property var myModel

                            Loader {
                                id: gSendsLoader
                                anchors.fill: parent
                                property var modelData_: modelData
                                sourceComponent: sendsLabelDelegate
                                onLoaded: {
                                    gSendsLoader.item.modelData_ = modelData_
                                }
                            }

                            HoverHandler {
                                acceptedDevices: PointerDevice.Mouse

                                onHoveredChanged: {
                                    if (hovered) {
                                        gSendsLoader.sourceComponent = sendsLabelDelegate
                                    }
                                }
                            }
                        }

                        proxyDelegate: Component {

                            CustomLabel {

                                height: scaleSize2(57)
                                color: proxyData.enabled ? theme.mixerDD10_DeSelect_FIL : theme.mixerDD100_DeActive_FIL
                                fontSize: topfontPixelSize
                                fontFamilty: topFontFamily
                                fontColor: proxyData.enabled ? theme.f1_ : theme.f5_
                                radius: scaleSize2(5)
                                width: parent.width
                                text: proxyData.name
                                leftPaddingTxt: scaleSize2(15)
                                horizontalTextAlignment: Text.AlignLeft
                            }
                        }

                        onMoved: (from, to) => {
                                     console.log("Moved from", from, "to", to)
                                 }
                    }

                    Item {

                        width: parent.width
                        height: scaleSize2(57)

                        Loader {
                            id: gSendsBlankLoader
                            anchors.fill: parent

                            sourceComponent: fxBlankDelegate
                        }

                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true

                            propagateComposedEvents: true

                            onEntered: {
                                gSendsBlankLoader.sourceComponent = null
                                gSendsBlankLoader.sourceComponent = fxHoverDelegate
                            }

                            onExited: {

                                gSendsBlankLoader.sourceComponent = null
                                gSendsBlankLoader.sourceComponent = fxBlankDelegate
                            }
                        }
                    }
                }
            }

            // --- Top single text field (pan) ---
            Rectangle {
                width: parent.width
                height: scaleSize2(86)
                anchors.horizontalCenter: parent.horizontalCenter

                color: "transparent"
                radius: topRadius

                TrackTextField2 {
                    id: tf2
                    height: scaleSize2(37)
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - scaleSize2(36)
                    anchors.horizontalCenter: parent.horizontalCenter
                    bigHeight: true

                    name: "pan"
                    isPan: true
                    isVolume: false

                    max: 100
                    min: 0
                    decimal: 0
                    has2Slider: true
                    hasSlider: false

                    enableRightClick: false
                    shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
                    moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

                    flColor: theme.s11_Enable_FIL
                    brColor: theme.s11_Enable_BRD

                    onSigPressed: {
                        _clipArea.tracksModel.saveUndoNoramized(model.index,
                                                                "Mixer", "Pan")
                    }

                    onDidShadowChanged: {
                        if (didShadow) {

                            model.automationMenuTitle = "Mixer,Pan"
                            _clipArea.tracksModel.panAutomationSelected(
                                        model.index)
                        }
                    }

                    property real automatedValue: model.automatedPanValue
                    property real automatedNormalized: model.automatedPanNormalized
                    property real automatedActive: model.automatedPanActive
                    property real automatedEnabled: model.automatedPanEnabled
                    property real normalized: model.panNormalized

                    defaultValue: model.panDefaultNormalized * 100
                    value: model.panNormalized * 100
                    text: model.panValue.toFixed(decimal)

                    onAutomatedEnabledChanged: {

                        if (!model.automatedPanEnabled) {
                            tf2.shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                            tf2.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                        } else {
                            tf2.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                            tf2.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                        }
                    }
                    onAutomatedValueChanged: {

                        if (model.automatedPanEnabled) {
                            value = model.automatedPanNormalized * 100
                            text = model.automatedPanValue.toFixed(decimal)
                        } else {

                        }
                    }
                    onNormalizedChanged: {

                        value = model.panNormalized * 100
                        text = model.panValue.toFixed(decimal)
                    }

                    onSigValueChanged: {

                        if (model.automatedPanActive) {
                            model.automatedPanEnabled = false
                            shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                            moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                        }

                        model.panNormalized = value / 100.0

                        tf2.value = model.panNormalized * 100
                        tf2.text = model.panValue.toFixed(decimal)
                    }

                    onAction: {

                        if (text == "Re-Enable Automation") {
                            model.automatedPanEnabled = true
                        }

                        if (text == "Delete Automation") {
                            model.clear = true
                        }

                        if (text == "Show Automation") {
                            model.automationLaneEnabled = true
                        }
                        if (text == "Disable Automation") {
                            model.automatedPanEnabled = false
                            moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']

                            model.volumeNormalized = value / 100.0
                            tf2.value = model.volumeNormalized * 100
                            tf2.text = model.volumeValue.toFixed(decimal)
                        }
                    }
                }
            }

            // ===== Two value boxes =====
            Row {
                spacing: scaleSize2(8)
                width: parent.width - scaleSize2(36)
                height: scaleSize2(56)
                anchors.horizontalCenter: parent.horizontalCenter

                Rectangle {

                    width: (parent.width - parent.spacing) / 2

                    color: "transparent"
                    radius: topRadius
                    height: scaleSize2(37)

                    TrackTextField2 {
                        id: tf1
                        anchors.fill: parent
                        bigHeight: true

                        isVolume: true

                        max: 100
                        min: 0
                        decimal: 1
                        hasSlider: true

                        enableRightClick: false
                        shortcutKeyTxt: ['', '', '', 'Del', 'Ctrl + Del']
                        moduls: ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']

                        //            shortcutKeyTxt:     ['', '', '']
                        //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation']
                        //            shortcutKeyTxt:     ['', '', '', '', 'Ctrl + Del']
                        //            moduls:             ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Active Automation', 'Delete Automation']
                        flColor: theme.s11_Enable_FIL
                        brColor: theme.s11_Enable_BRD

                        onSigPressed: {
                            _clipArea.tracksModel.saveUndoNoramized(
                                        model.index, "Mixer", "Volume")
                        }

                        onDidShadowChanged: {
                            if (didShadow) {
                                model.automationMenuTitle = "Mixer,Volume"
                                _clipArea.tracksModel.volumeAutomationSelected(
                                            model.index)
                            }
                        }

                        property real automatedValue: model.automatedVolumeValue
                        property real automatedNormalized: model.automatedVolumeNormalized
                        property real automatedActive: model.automatedVolumeActive
                        property real automatedEnabled: model.automatedVolumeEnabled
                        property real normalized: model.volumeNormalized

                        value: model.volumeNormalized * 100
                        text: model.volumeValue.toFixed(decimal)
                        defaultValue: model.volumeDefaultNormalized * 100
                        onAutomatedEnabledChanged: {

                            if (!model.automatedVolumeEnabled) {
                                tf1.shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                                tf1.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                            } else {
                                tf1.shortcutKeyTxt = ['', '', '', 'Del', 'Ctrl + Del']
                                tf1.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                            }
                        }
                        onAutomatedValueChanged: {

                            if (model.automatedVolumeEnabled) {
                                tf1.value = model.automatedVolumeNormalized * 100
                                tf1.text = model.automatedVolumeValue.toFixed(
                                            decimal)
                            } else {

                            }
                        }
                        onNormalizedChanged: {

                            tf1.value = model.volumeNormalized * 100
                            tf1.text = model.volumeValue.toFixed(decimal)
                        }

                        onSigValueChanged: {
                            if (model.automatedVolumeActive) {
                                model.automatedVolumeEnabled = false
                                shortcutKeyTxt = ['', '', '', '', 'Del', 'Ctrl + Del']
                                moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                            }

                            model.volumeNormalized = value / 100.0
                            tf1.value = model.volumeNormalized * 100
                            tf1.text = model.volumeValue.toFixed(decimal)
                        }

                        onAction: {

                            if (text == "Re-Enable Automation") {
                                model.automatedVolumeEnabled = true
                            }

                            if (text == "Delete Automation") {
                                model.clear = true
                            }
                            if (text == "Show Automation") {
                                model.automationLaneEnabled = true
                            }

                            if (text == "Disable Automation") {
                                model.automatedVolumeEnabled = false
                                moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']

                                model.volumeNormalized = value / 100.0
                                tf1.value = model.volumeNormalized * 100
                                tf1.text = model.volumeValue.toFixed(decimal)
                            }
                        }
                    }
                }

                Rectangle {
                    width: (parent.width - parent.spacing) / 2
                    height: scaleSize2(37)
                    color: "transparent"
                    radius: topRadius

                    TrackTextField2 {
                        anchors.fill: parent

                        bigHeight: true

                        isVolume: true

                        max: 100
                        min: 0
                        decimal: 1

                        enableRightClick: false
                        has2Slider: false
                        hasSlider: false
                        value: 0
                        text: "0.0"
                        defaultValue: 0

                        flColor: theme.s10_Disable_FIL
                        brColor: theme.s11_Enable_BRD
                    }
                }
            }

            // ===== Main strip area =====
            Item {
                id: mainStrip
                width: parent.width
                height: scaleSize2(1200)

                Row {
                    id: mainStripRow
                    anchors.left: parent.left
                    anchors.right: parent.right

                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    anchors.topMargin: scaleSize2(65)
                    anchors.bottomMargin: scaleSize2(65)

                    spacing: scaleSize2(18)
                    anchors.horizontalCenter: parent.horizontalCenter

                    // ---- Fader ----
                    Item {
                        id: faderArea
                        width: mainStrip.width / 2
                        height: parent.height

                        // Track
                        Rectangle {
                            id: faderTrack
                            width: scaleSize2(12)
                            radius: scaleSize2(3)
                            color: theme.bg5
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            height: parent.height
                        }

                        // Knob
                        Rectangle {
                            id: knob
                            width: scaleSize2(76)
                            height: scaleSize2(137)
                            radius: scaleSize2(8)
                            color: "#bdbdbd"
                            border.color: "#0f1011"
                            border.width: 1
                            x: (parent.width - width) / 2

                            property real minimumValue: 0
                            property real maximumValue: 100

                            property var clipArea: _clipArea
                            property bool dragging: false
                            property real stepSize: 0.1

                            property real value: model.volumeNormalized * 100

                            property real minY: faderTrack.y
                            property real maxY: faderTrack.y + faderTrack.height - height
                            y: (model.volumeNormalized * faderArea.height) - height / 2

                            Repeater {
                                model: 2
                                Rectangle {
                                    width: parent.width / 3.0
                                    height: 2
                                    radius: 1
                                    color: "#2b2b2b"
                                    y: parent.height / 2 - height / 2
                                    x: index === 0 ? 0 : 2 * parent.width / 3.0
                                }
                            }

                            MouseArea {
                                id: mouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                acceptedButtons: Qt.LeftButton
                                preventStealing: true

                                // global press point (fixed visual cursor position)
                                property point pressGlobalPos
                                // last total dy from press point (for incremental diff)
                                property real lastDyFromPress: 0

                                onPressed: {
                                    if (knob.clipArea
                                            && knob.clipArea.globalPos) {
                                        pressGlobalPos = knob.clipArea.globalPos()
                                        lastDyFromPress = 0
                                    }
                                    knob.dragging = true

                                    // hide cursor
                                    if (knob.clipArea
                                            && knob.clipArea.setOverrideCursor)
                                        knob.clipArea.setOverrideCursor(
                                                    Qt.BlankCursor)
                                }

                                onPositionChanged: {
                                    if (!knob.dragging
                                            || !(mouse.buttons & Qt.LeftButton))
                                        return
                                    if (!knob.clipArea
                                            || !knob.clipArea.globalPos)
                                        return

                                    var p = knob.clipArea.globalPos()

                                    var stepsDelta = p.y - pressGlobalPos.y

                                    var raw = knob.value + stepsDelta * knob.stepSize

                                    var stepIndex = Math.round(
                                                (raw - knob.minimumValue) / knob.stepSize)
                                    var stepped = knob.minimumValue + stepIndex * knob.stepSize
                                    stepped = Math.max(
                                                knob.minimumValue,
                                                Math.min(knob.maximumValue,
                                                         stepped))

                                    model.volumeNormalized = stepped / 100.0

                                    // keep mouse visually at the press point
                                    if (knob.clipArea.setCursorPosition)
                                        knob.clipArea.setCursorPosition(
                                                    pressGlobalPos.x,
                                                    pressGlobalPos.y)
                                }

                                function finishDrag() {
                                    if (!knob.dragging)
                                        return
                                    knob.dragging = false

                                    // restore cursor shape
                                    if (knob.clipArea
                                            && knob.clipArea.setOverrideCursor)
                                        knob.clipArea.setOverrideCursor(
                                                    Qt.ArrowCursor)

                                    // keep cursor at press point when releasing (as requested)
                                    if (knob.clipArea
                                            && knob.clipArea.setCursorPosition
                                            && pressGlobalPos) {

                                        var gp = knob.mapToGlobal(
                                                    knob.width / 2,
                                                    knob.height / 2)
                                        knob.clipArea.setCursorPosition(gp.x,
                                                                        gp.y)
                                    }
                                }

                                onReleased: finishDrag()
                                onCanceled: finishDrag()
                            }
                        }
                    }

                    // ---- Meter ----
                    Item {
                        id: meterBoxItem
                        width: mainStrip.width / 2
                        height: parent.height

                        Item {
                            id: meterScale
                            width: scaleSize2(76)
                            height: parent.height
                            anchors.right: meterBox.left
                            anchors.rightMargin: scaleSize2(15)

                            property var displayValues: [0, 3, 6, 10, 20, 30, 40, 45, 50, 60]

                            property int lineLength: scaleSize2(25)

                            property real fontSize: topfontPixelSize
                            property string fontFamilty: topFontFamily
                            property color fontColor: theme.f1_

                            // === Draw all ticks & numbers ===
                            Repeater {
                                model: meterScale.displayValues.length

                                delegate: Item {
                                    width: meterScale.width
                                    height: labelTemplate.implicitHeight

                                    property real value: meterScale.displayValues[index]

                                    // place vertically according to the curve
                                    y: root.dbDisplayToYPixel(
                                           value,
                                           meterScale.height) - height / 2

                                    // tick line
                                    Rectangle {
                                        id: line
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.right: parent.right
                                        width: meterScale.lineLength
                                        height: 2
                                        color: meterScale.fontColor
                                    }

                                    // label text
                                    Text {
                                        id: labelTemplate
                                        anchors.verticalCenter: parent.verticalCenter
                                        anchors.right: line.left
                                        anchors.rightMargin: scaleSize2(10)

                                        text: value // “0”, “3”, “6”, …
                                        color: meterScale.fontColor
                                        font.family: meterScale.fontFamilty
                                        font.pixelSize: meterScale.fontSize
                                    }
                                }
                            }
                        }

                        Rectangle {
                            id: meterBox
                            width: meterRow.implicitWidth + 1 * scaleSize2(
                                       3) + (2 - 1) * scaleSize2(3)
                            height: parent.height

                            radius: scaleSize2(5)
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: theme.bg5

                            Rectangle {
                                width: parent.width
                                height: 2
                                radius: 0
                                color: theme.bg5
                                y: root.dbDisplayToYPixel(
                                       0, meterBoxItem.height) - height / 2
                                z: meterRow.z + 1
                            }

                            Row {
                                id: meterRow
                                anchors.fill: parent
                                anchors.margins: scaleSize2(3)
                                spacing: scaleSize2(3)

                                Repeater {
                                    id: meterRepeater
                                    model: 2 // 0 = left, 1 = right

                                    delegate: Item {
                                        width: scaleSize2(24)
                                        height: parent.height

                                        // level per bar
                                        property real level: (index
                                                              === 0) ? root.meterL : root.meterR

                                        // fill with gradient, clipped by "level"
                                        Rectangle {
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.bottom: parent.bottom
                                            radius: 2

                                            height: parent.height * Math.max(
                                                        0, Math.min(1, level))

                                            gradient: Gradient {
                                                GradientStop {
                                                    position: 0.0
                                                    color: "#15D0E9"
                                                }
                                                GradientStop {
                                                    position: 1.0
                                                    color: "#26E9CC"
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // ===== Two value boxes =====

            // ===== Two value boxes =====
            Row {
                spacing: scaleSize2(8)
                width: parent.width - scaleSize2(36)
                height: scaleSize2(47)
                anchors.horizontalCenter: parent.horizontalCenter

                Select {
                    id: selectBtn
                    name: index + 1
                    width: (parent.width - parent.spacing) / 2
                    height: scaleSize2(37)
                    moduls: !model.automatedSpeakerOnEnabled
                            && model.automatedSpeakerOnActive ? ['Show Automation', 'Show Automation in New Lane', 'Re-Enable Automation', 'Delete Automation'] : ['Show Automation', 'Show Automation in New Lane', 'Delete Automation']

                    activated: model.trackActivated
                    mute: model.speakerOnValue === 0

                    property real automatedValue: model.automatedSpeakerOnValue
                    property real automatedNormalized: model.automatedSpeakerOnNormalized
                    property real automatedActive: model.automatedSpeakerOnActive
                    property real automatedEnabled: model.automatedSpeakerOnEnabled
                    property real normalized: model.speakerOnNormalized

                    onNormalizedChanged: {
                        selectBtn.mute = model.speakerOnValue === 0
                    }

                    onAutomatedEnabledChanged: {

                        if (!model.automatedSpeakerOnEnabled) {
                            selectBtn.mute = model.speakerOnValue === 0
                            selectBtn.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Re-Enable Automation', 'Disable Automation', 'Delete Automation']
                        } else {
                            selectBtn.mute = model.speakerOnValue === 0
                            selectBtn.moduls = ['Show Automation', 'Show Automation in New Lane', 'Show Modulation', 'Disable Automation', 'Delete Automation']
                        }
                    }
                    onAutomatedValueChanged: {
                        if (model.automatedSpeakerOnEnabled) {

                            selectBtn.mute = model.speakerOnValue === 0
                        } else {

                        }
                    }

                    Rectangle {
                        id: automatedLight
                        z: 1000
                        visible: model.automatedSpeakerOnActive
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.leftMargin: 1
                        anchors.topMargin: 0
                        color: model.automatedSpeakerOnEnabled ? "#EA2027" : "#808080"
                        width: scaleSize2(11)
                        height: scaleSize2(11)
                        radius: scaleSize2(11)
                    }

                    onAction: {

                        if (text == "Re-Enable Automation") {
                            model.automatedEnabled = true
                            selectBtn.moduls = ['Show Automation', 'Show Automation in New Lane', 'Delete Automation']
                        }

                        if (text == "Delete Automation") {
                            model.clear = true
                        }
                    }

                    anchors.verticalCenter: parent.verticalCenter
                }

                Solo {
                    id: soloBtn
                    width: (parent.width - parent.spacing) / 2
                    height: scaleSize2(37)
                }
            }
            Rectangle {
                width: parent.width
                height: scaleSize2(37)
                anchors.horizontalCenter: parent.horizontalCenter

                color: "transparent"
                radius: topRadius

                TrackTextField2 {
                    height: scaleSize2(37)
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - scaleSize2(36)
                    anchors.horizontalCenter: parent.horizontalCenter
                    bigHeight: true

                    name: "pan"
                    isPan: true
                    isVolume: false

                    max: 100
                    min: 0
                    decimal: 0
                    has2Slider: true
                    hasSlider: false

                    enableRightClick: false

                    fontColor: theme.f1_
                    flColor: theme.b10_Disable_FIL
                    brColor: theme.s11_Enable_BRD
                }
            }
        }
    }
    Popup {
        id: popup

        visible: false

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        width: scaleSize2(650)
        height: mainCol.implicitHeight

        background: Rectangle {
            radius: scaleSize2(5)
            color: theme.w3_FILL
            border.color: theme.bg5_new
            border.width: scaleSize2(3)
        }

        contentItem: Column {
            id: mainCol

            anchors.fill: parent
            spacing: 0

            Item {
                id: topBar
                width: parent.width
                height: scaleSize2(56) + scaleSize2(20)

                // tweak these if you want
                readonly property int pad: scaleSize2(15)
                readonly property int gap: scaleSize2(8)
                readonly property int iconSize: scaleSize2(18)

                Rectangle {
                    id: searchBox
                    anchors.left: parent.left
                    anchors.right: icon.left
                    anchors.leftMargin: scaleSize2(15)
                    anchors.rightMargin: scaleSize2(8)
                    anchors.verticalCenter: parent.verticalCenter
                    height: scaleSize2(56)

                    radius: scaleSize2(5)
                    color: theme.bg5_new

                    TextArea {
                        id: searchInput
                        anchors.fill: parent
                        anchors.leftMargin: scaleSize2(8)
                        anchors.rightMargin: scaleSize2(8)
                        verticalAlignment: Text.AlignVCenter
                        color: "#c7c7c7c7"
                        font.pixelSize: topfontPixelSize
                        font.family: topFontFamily
                        clip: true
                        cursorVisible: true
                        text: ""
                        placeholderText: "Search"
                        topPadding: 0
                        bottomPadding: 0
                        leftPadding: scaleSize2(8)
                        rightPadding: scaleSize2(8)
                    }
                }

                Image {
                    id: icon
                    source: "qrc:/Resource/search_icon.svg"
                    smooth: true
                    fillMode: Image.PreserveAspectFit

                    width: scaleSize2(56)
                    height: scaleSize2(56)

                    anchors.right: parent.right
                    anchors.rightMargin: scaleSize2(15)

                    anchors.verticalCenter: parent.verticalCenter

                    ColorOverlay {
                        anchors.fill: parent
                        source: parent
                        color: theme.button_unclicked_color
                    }
                }
            }
            Rectangle {
                height: 1
                color: theme.s7
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2
            }

            Item {
                id: colorBar
                width: parent.width
                height: scaleSize2(56) + 2 * scaleSize2(10)

                // Tweakables
                property int padding: scaleSize2(5)
                property int dotSize: scaleSize2(56)
                property int dotSpacing: scaleSize2(20)
                property color barColor: theme.w3_FILL
                property color borderColor: "#00000000"
                property real radius: scaleSize2(5)

                // Selection + colors
                property int selectedIndex: 0 // ✅ first selected by default
                property var colors: ["#EE3743", "#FFA138", "#FFEE00", "#1CE637", "#3DB5FF", "#D55CED"]
                signal colorSelected(int index, color c)

                Rectangle {
                    id: bar

                    height: parent.height
                    implicitWidth: colorBar.padding * 2
                                   + (colorBar.colors.length * colorBar.dotSize)
                                   + ((colorBar.colors.length - 1) * colorBar.dotSpacing)

                    color: colorBar.barColor
                    radius: colorBar.radius
                    antialiasing: true

                    anchors.left: parent.left
                    anchors.leftMargin: scaleSize2(5)

                    Row {
                        id: dotRow

                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: colorBar.dotSpacing

                        Repeater {
                            model: colorBar.colors.length

                            Item {
                                id: cell
                                width: colorBar.dotSize
                                height: colorBar.dotSize

                                readonly property bool isSelected: index === colorBar.selectedIndex

                                // selection highlight (rounded square)
                                Rectangle {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter

                                    width: cell.isSelected ? (colorBar.dotSize) : 0
                                    height: cell.isSelected ? (colorBar.dotSize) : 0
                                    radius: colorBar.radius
                                    color: theme.bg6

                                    antialiasing: true
                                    opacity: cell.isSelected ? 1 : 0

                                    Behavior on width {
                                        NumberAnimation {
                                            duration: 120
                                        }
                                    }
                                    Behavior on height {
                                        NumberAnimation {
                                            duration: 120
                                        }
                                    }
                                    Behavior on opacity {
                                        NumberAnimation {
                                            duration: 120
                                        }
                                    }
                                }

                                // dot itself
                                Rectangle {
                                    anchors.centerIn: parent
                                    width: scaleSize2(25)
                                    height: scaleSize2(25)
                                    radius: width / 2
                                    color: colorBar.colors[index]
                                    antialiasing: true
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        colorBar.selectedIndex = index
                                        colorBar.colorSelected(
                                                    index,
                                                    colorBar.colors[index])
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                height: 1
                color: theme.s7
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2
            }
            Item {
                id: vstTabs

                width: parent.width
                height: scaleSize2(56) + 2 * scaleSize2(10)

                property int selectedIndex: 0 // 0 = VST2, 1 = VST3
                property var tabs: ["VST2", "VST3"]
                signal tabSelected(int index)

                property real topPaddingTxt: scaleSize2(5)
                property real bottomPaddingTxt: scaleSize2(5)
                property real leftPaddingTxt: scaleSize2(5)
                property real rightPaddingTxt: scaleSize2(5)

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: scaleSize2(45)
                    anchors.left: parent.left
                    anchors.leftMargin: scaleSize2(15)

                    Repeater {
                        model: vstTabs.tabs.length

                        Item {
                            id: tabItem

                            width: tabText.implicitWidth + scaleSize2(25)
                            height: tabText.implicitHeight + scaleSize2(10)

                            Rectangle {
                                anchors.fill: parent
                                radius: scaleSize2(5)

                                color: vstTabs.selectedIndex === index ? theme.bg6 : "transparent"
                            }

                            Text {
                                id: tabText
                                renderType: Text.QtRendering
                                text: vstTabs.tabs[index]
                                anchors.centerIn: parent
                                antialiasing: true

                                anchors.leftMargin: vstTabs.leftPaddingTxt
                                anchors.rightMargin: vstTabs.rightPaddingTxt
                                anchors.topMargin: vstTabs.topPaddingTxt
                                anchors.bottomMargin: vstTabs.bottomPaddingTxt

                                color: vstTabs.selectedIndex === index ? theme.f10_ : theme.f1_

                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter

                                font {
                                    pixelSize: topfontPixelSize
                                    family: topFontFamily
                                    letterSpacing: 0.5
                                    weight: Font.DemiBold
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    vstTabs.selectedIndex = index
                                    vstTabs.tabSelected(index)
                                    stack.currentIndex = index
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                height: 1
                color: theme.s7

                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2
            }

            Item {
                id: noPluginTab

                width: parent.width
                height: scaleSize2(56) + 2 * scaleSize2(10)

                property real topPaddingTxt: scaleSize2(5)
                property real bottomPaddingTxt: scaleSize2(5)
                property real leftPaddingTxt: scaleSize2(5)
                property real rightPaddingTxt: scaleSize2(5)

                Item {

                    anchors.left: parent.left
                    anchors.leftMargin: scaleSize2(5)
                    anchors.verticalCenter: parent.verticalCenter

                    width: noPluginText.implicitWidth + scaleSize2(25)
                    height: noPluginText.implicitHeight + scaleSize2(10)

                    Text {
                        id: noPluginText
                        renderType: Text.QtRendering
                        text: "No Plugin"
                        anchors.centerIn: parent
                        antialiasing: true
                        color: theme.f1_

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        font {
                            pixelSize: topfontPixelSize
                            family: topFontFamily
                            letterSpacing: 0.5
                            weight: Font.DemiBold
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }
            Rectangle {
                height: 1
                color: theme.s7
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 2
            }

            StackLayout {
                id: stack
                width: parent.width
                implicitHeight: scaleSize2(700)
                TreeView {

                    width: parent.width
                    implicitHeight: scaleSize2(700)
                    model: vst3.vst2modules
                    selectionEnabled: true
                    hoverEnabled: true

                    highlight: Rectangle {
                        color: "transparent"
                    }

                    hoverColor: theme.bg6

                    selectedItemColor: theme.s7
                    handleColor: theme.s7

                    onChildSelectedIndexChanged: {

                        var i = vst3.vst2modules.getTreeItemIndex(
                                    childSelectedIndex)
                        var path = vst3.vst2modules.getTreeItemPath(
                                    childSelectedIndex)
                        var name = vst3.vst3modules.getTreeItemName(
                                    childSelectedIndex)

                        _clipArea.addVst3Effect(_areaInfo.selectedTrackItem,
                                                path, name, i)
                    }

                    contentItem: Row {
                        spacing: 10

                        Text {
                            verticalAlignment: Text.AlignVCenter

                            color: currentRow.isHoveredIndex ? theme.f10_ : theme.f1_
                            text: currentRow.currentData

                            font {
                                pixelSize: topfontPixelSize
                                family: topFontFamily
                                letterSpacing: 0.5
                                weight: Font.DemiBold
                            }
                        }
                    }

                    scrollItem: ScrollBar {
                        id: threeViewVs2Control
                        orientation: Qt.Vertical
                        active: true

                        // normalized values (0..1)
                        size: 0.3
                        position: 0.2

                        contentItem: Rectangle {
                            implicitWidth: scaleSize2(15)

                            implicitHeight: Math.max(
                                                scaleSize2(20),
                                                threeViewVs2Control.size
                                                * threeViewVs2Control.availableHeight)

                            radius: scaleSize2(5)
                            color: theme.bg7

                            // correct vertical position
                            y: threeViewVs2Control.position
                               * (threeViewVs2Control.availableHeight - height)
                        }
                    }
                }
                TreeView {

                    width: parent.width
                    implicitHeight: scaleSize2(700)
                    model: vst3.vst3modules
                    selectionEnabled: true
                    hoverEnabled: true

                    property var menuIndex: null
                    property string menuName: ""
                    property int menuItemId: -1

                    highlight: Rectangle {
                        color: "transparent"
                    }

                    hoverColor: theme.bg6

                    selectedItemColor: theme.s7
                    handleColor: theme.s7

                    onChildSelectedIndexChanged: {

                        var i = vst3.vst3modules.getTreeItemIndex(
                                    childSelectedIndex)
                        var path = vst3.vst3modules.getTreeItemPath(
                                    childSelectedIndex)
                        var name = vst3.vst3modules.getTreeItemName(
                                    childSelectedIndex)

                        _clipArea.addVst3Effect(_areaInfo.selectedTrackItem,
                                                path, name, i)
                    }
                    contentItem: Item {
                        id: item
                        Text {
                            verticalAlignment: Text.AlignVCenter
                            anchors.fill: parent

                            color: currentRow.isHoveredIndex ? theme.f10_ : theme.f1_
                            text: currentRow.currentData

                            font {
                                pixelSize: topfontPixelSize
                                family: topFontFamily
                                letterSpacing: 0.5
                                weight: Font.DemiBold
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.LeftButton | Qt.RightButton
                            hoverEnabled: true

                            onClicked: {
                                if (mouse.button === Qt.RightButton) {
                                    var p = mapToItem(null, mouse.x, mouse.y)

                                    console.log(p)
                                    rowMenu.popup(item, mouse.x, mouse.y)
                                }
                            }
                        }
                    }

                    scrollItem: ScrollBar {
                        id: threeViewVs3Control
                        orientation: Qt.Vertical
                        active: true

                        // normalized values (0..1)
                        size: 0.3
                        position: 0.2

                        contentItem: Rectangle {
                            implicitWidth: scaleSize2(15)

                            implicitHeight: Math.max(
                                                scaleSize2(20),
                                                threeViewVs3Control.size
                                                * threeViewVs3Control.availableHeight)

                            radius: scaleSize2(5)
                            color: theme.bg7

                            // correct vertical position
                            y: threeViewVs3Control.position
                               * (threeViewVs3Control.availableHeight - height)
                        }
                    }
                }
            }
        }
    }

    Component {
        id: colorItemDelegate

        MenuItem {
            id: mi
            property color dotColor: "transparent"

            implicitHeight: scaleSize2(500)
            implicitWidth: scaleSize2(500)

            background: Rectangle {
                color: mi.highlighted ? theme.bg6 : "transparent"
            }

            contentItem: Row {
                spacing: scaleSize2(10)
                anchors.verticalCenter: parent.verticalCenter

                Rectangle {
                    width: scaleSize2(10)
                    height: scaleSize2(10)
                    radius: width / 2
                    color: mi.dotColor
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    text: mi.text
                    color: theme.f1_
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: topfontPixelSize
                    font.family: topFontFamily
                }
            }
        }
    }

    Menu {
        id: rowMenu

        // Optional: make the menu look closer to your screenshot
        background: Rectangle {
            implicitWidth: scaleSize2(340)
            color: theme.w3_FILL
            radius: scaleSize2(5)
            border.color: theme.w3_BRD
            border.width: scaleSize2(3)
        }

        // Exclusive selection (radio behavior)
        ButtonGroup {
            id: colorGroup
            exclusive: true
        }

        component ColorItem: MenuItem {
            id: menuItem
            property string name
            property color dotColor

            checkable: true
            ButtonGroup.group: colorGroup

            implicitWidth: scaleSize2(220)
            implicitHeight: scaleSize2(56)
            background: Rectangle {
                anchors.fill: parent
                implicitWidth: scaleSize2(220)
                implicitHeight: scaleSize2(56)
                color: menuItem.highlighted ? theme.bg6 : "transparent"
            }
            // Custom content to show a colored dot + label
            contentItem: Item {
                id: rectMenuItem

                anchors.fill: parent

                Row {
                    id: rowContent
                    spacing: scaleSize2(20)
                    anchors.left: parent.left
                    anchors.leftMargin: scaleSize2(5)
                    anchors.fill: parent // keep this if you want it to take the full row size

                    Rectangle {
                        width: scaleSize2(20)
                        height: scaleSize2(20)
                        radius: width / 2
                        color: dotColor
                        border.width: dotColor === "transparent" ? 1 : 0
                        border.color: theme.bg5_new

                        // vertical centering without anchors
                        y: (rowContent.height - height) / 2
                    }

                    Text {
                        text: name
                        color: menuItem.highlighted ? theme.f10_ : theme.f1_
                        font.family: topFontFamily
                        font.pixelSize: topfontPixelSize
                        elide: Text.ElideRight

                        // IMPORTANT: give Text a real height so centering is stable
                        height: rowContent.height

                        // Now this works (text centered within its own height)
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        ColorItem {
            name: "No Color"
            dotColor: "transparent"
        }
        ColorItem {
            name: "Red"
            dotColor: "#EE3743"
        }
        ColorItem {
            name: "Orange"
            dotColor: "#FFA138"
        }
        ColorItem {
            name: "Yellow"
            dotColor: "#FFEE00"
        }
        ColorItem {
            name: "Green"
            dotColor: "#1CE637"
        }
        ColorItem {
            name: "Blue"
            dotColor: "#3DB5FF"
        }
        ColorItem {
            name: "Pink"
            dotColor: "#D55CED"
        }
    }
}
