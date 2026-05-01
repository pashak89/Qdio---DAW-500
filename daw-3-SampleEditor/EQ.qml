import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQml 2.2
import QtQuick.Dialogs 1.2
import ScreenInterface 1.0
import SampleEditor 1.0
import AudioManager 1.0
import AudioEngine 1.0
import AudioDevice 1.0

import Tools 1.0
import BottomArea 1.0
import TopArea 1.0

import FilterVisualizer 1.0
//import MultiEQ 1.0
import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import MultiEQ 1.0
import QtQml 2.12
import TracksModel 1.0
import QtQml.Models 2.15
import EffectChain 1.0
import "Eq8Plugin"

ColumnLayout {
    id: root
    width: eq8.scaleSize(480)
    height: eq8.scaleSize(975)

    //property alias afilterVisualizer: filterVisualizer
    property var currentIndex

    property EffectChain chainModel
    property int trackIndex: -1

    property string name
    property string type

    PanelEq8Plugin {
        id: eq8
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.margins: eq8.scaleSize(15)
    }
}
