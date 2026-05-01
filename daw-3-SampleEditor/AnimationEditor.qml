import QtQuick 2.2
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQml 2.12
import QtQuick.Controls.Material 2.12
import "qrc:/CommonEngine/Forms/"
import "qrc:/Common/Forms/views/CommonItems/"

import Tutorial2 1.0

Window {
    id:animationWindow
    minimumWidth: 800
    minimumHeight: 800
    property ThemeManager themeObject

    Material.theme: themeObject.getCurrentQMLTheme()
    Material.accent: themeObject.getCurrentColor(
                         ThemeManager.ColorType.AccentColor)
    Material.primary: themeObject.getCurrentColor(
                          ThemeManager.ColorType.PrimaryColor)
    Material.background: themeObject.getCurrentColor(
                             ThemeManager.ColorType.WindowBG)
    Material.foreground: themeObject.getCurrentColor(
                             ThemeManager.ColorType.TextsColor)

    color: themeObject.getCurrentColor(ThemeManager.ColorType.WindowBG)

    property AnimationEditor animationEditor: AnimationEditor

}
