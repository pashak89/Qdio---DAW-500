import Qt.labs.platform 1.1

FolderDialog {
    id: folderDialog
    property string defaultfolder: ""
    currentFolder: defaultfolder
    options: FolderDialog.ShowDirsOnly
    folder: defaultfolder


}
