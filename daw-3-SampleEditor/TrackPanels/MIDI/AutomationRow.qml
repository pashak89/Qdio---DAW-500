import QtQuick 2.12
import QtQuick.Layouts 1.12
import '.'
import 'qrc:/'
import "qrc:/Items"
import 'Automation/'

Automation {
    id:      automationCondition

    visible: automationTurn

    height: scaleSize2(37)
    width:  scaleSize2(106)*4 + 3* scaleSize2(15)
}
