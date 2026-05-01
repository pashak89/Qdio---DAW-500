import QtQuick 2.12
import QtQuick.Layouts 1.12

Rectangle {///----------------------------------------------------------------------------------------------------
    id: backgroundDrag_id
anchors.fill: parent

    radius: resoless(3)
    color: "#0D171F"

    // -- چون عرض را فیل کردیم و در رزولوشنهای مختلف تغییر سایز میده --//
    // -- ازطرفی عرض کلی روی مربع بکگراند روشن تاثر میذاره به روزرسانیش میکنیم --//
//                                    onHeightChanged: {
//                                        backLight_rec.height =
//                                                (((dragREC_NumberBox._value) - (dragREC_NumberBox._min)) /
//                                                 (((dragREC_NumberBox._max - dragREC_NumberBox._min)/(backgroundDrag_id.height))))
//                                    }
    //-- با کیلیک بر روی بکگراند دایره را جابجا میکنیم --//
    GainDragSEC{
        id: background_NumberBox
        width: dragREC_id.width
        height: parent.height

        anchors.centerIn: parent

        _max: 140.00
        _min: -70.00
        _justDragActived: true
        color: "transparent" //"yellow"
        y: parent.y
        x: parent.x
        property int _log_lengthDecimalSEC: 2

        property real save_value: 0
        onPressed_itemYChanged:  {
            //اندازه مربع هایلایت رو به اندازه مکانی که کلیک کردیم میکنیم
            backLight_rec.height = (backgroundDrag_id.height - pressed_itemY)

            //فرمول زیر برعکس فرمول بذست آوردن بلندی مربع هایلایت در هنگام درگ کردن هستش
            save_value = ((backLight_rec.height) * ((parseFloat(gain_NumberBox.max) -
                                                     (parseFloat(gain_NumberBox.min)))/backgroundDrag_id.height)) + (parseFloat(gain_NumberBox.min))
            //---------------------------------------------------------
            gain_NumberBox.value = save_value
            dragREC_NumberBox._value = save_value

        }
        on_ValueChanged: {
            gain_NumberBox.value = _value
            dragREC_NumberBox._value = _value

            if(_value < 0)
            {
                gain_NumberBox.text = (_value*(70/Math.abs(_min))).toFixed(2)
            }
            else if(_value > 0)
            {
                gain_NumberBox.text = (_value*(24/Math.abs(_max))).toFixed(2)
            }
            else
            {
                gain_NumberBox.text = (_value)
            }

            // - عرض های لایک بکگراند را تغییر میدهیمکه دکمه درگ به تاپ آن انتصاب داده شده --//
            backLight_rec.height = (((background_NumberBox._value) - (background_NumberBox._min)) / (((background_NumberBox._max - dragREC_NumberBox._min)/(backgroundDrag_id.height))))

        }
        onReleaseMouse_sig: {
            var globalDragableRecY = (dragREC_NumberBox.pressed_globalY + (dragREC_id.height/2))
            mainRoot_id.cursorPosition.moveCursor(pressed_globalX, globalDragableRecY)
        }

    }

    // -- backLight -- //
    Rectangle{
        id: backLight_rec

        width: backgroundDrag_id.width
        height: height

        onHeightChanged: {
            dragREC_id.y = Math.round(y - /*resoless*/(dragREC_id.height/2 + parent.border.width))
        }

        color: "#14cee6"
        radius: parent.radius

        anchors{
            bottom: parent.bottom
        }

    }

    // -- dragable REC -- //
    Rectangle{// KNOBE
        id: dragREC_id
        width: resoless(13)
        height: width
//        x: parent.width - ((width/2) + resoless(parent.border.width))
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height - ((height/2) + resoless(parent.border.width))

        radius: width/2

        color: "#839099"
        border{
            width: 1
            color: "#0b0c0d"
        }

        GainDragSEC{
            id: dragREC_NumberBox
            anchors.fill: parent
            _max: 140
            _min: -70
            _justDragActived: true
            color: "transparent"//"yellow"
            y: parent.y
            x: parent.x
            _doubleClicked_isEnable: true

            onYChanged: {
                //-- مختصات موس در تمام صفحه تور متغییر زیر ذخیره میشه --//
                var globalPosition= mapToGlobal(dragREC_NumberBox.x, dragREC_NumberBox.y)
                pressed_globalY = globalPosition.y + pressed_itemY
            }
            onReleaseMouse_sig: {
                mainRoot_id.cursorPosition.moveCursor(pressed_globalX,pressed_globalY)
            }

            property string _text_2: "-inf dB"
            on_ValueChanged: {
                gain_NumberBox.value = _value
                background_NumberBox._value = _value

                // - عرض های لایک بکگراند را تغییر میدهیمکه دکمه درگ به تاپ آن انتصاب داده شده --//
                backLight_rec.height = (((dragREC_NumberBox._value) -
                                         (dragREC_NumberBox._min)) / (((dragREC_NumberBox._max - dragREC_NumberBox._min)/(backgroundDrag_id.height))))
                //--------------------------------------//
                if(_value < 0)
                {
                    gain_NumberBox.text = (_value*(70/Math.abs(_min))).toFixed(2)
                }
                else if(_value > 0)
                {
                    gain_NumberBox.text = (_value*(24/Math.abs(_max))).toFixed(2)
                }
                else
                {
                    gain_NumberBox.text = _value
                }
                //--------------------------------------//


            }

        }


    }

}//-----------------------------------------------
