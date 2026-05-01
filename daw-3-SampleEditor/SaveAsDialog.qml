import QtQuick 2.3
import Qt.labs.platform 1.1

FileDialog {
    id: saveDialog
    title: "Save Dialog"
    nameFilters: ["Audio files (*.wav)"]

    currentFile: "file:///"+"untitled.wav"//The name of the item that you want to save
    fileMode: FileDialog.SaveFile
}
