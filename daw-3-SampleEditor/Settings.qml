import AudioEngine 1.0
import AudioDevice 1.0

import Tools 1.0
import BottomArea 1.0
import TopArea 1.0

import FilterVisualizer 1.0
import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import QtQml 2.12
import QtQuick.Controls 2.15
ColumnLayout {

    Button{
        text: "VST2 Path"
    }
    Label{
        text: vst3.vst2Path
    }
    Button{
        text: "VST3 Path"
    }
    Label{
       text: vst3.vst3Path
    }
}
