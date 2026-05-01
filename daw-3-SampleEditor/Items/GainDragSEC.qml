import QtQuick 2.9
import QtQuick.Window 2.3

TextInput{
    id: numberBox
    width: resoless(59)
    height: resoless(19)

    Component.onCompleted: {
        numberBox._value = _min.toFixed(_lengthDecimalSEC)
        //        numberBox._saveTXT = _min.toFixed(_lengthDecimalSEC)
        numberBox.text = "-inf dB"
    }

    property color _tempo_color: "transparent"

    property bool _shift_is_clicked: false

    on_Shift_is_clickedChanged: console.log("_shift_is_clicked= " + _shift_is_clicked)

    property int _fontSize_txt: resoless(13)
    property string _family_txt: topFontFamily //"Swis721 Cn BT"
    property color _color_txt: theme.tColor
    property real _max: 100
    property real _min: 10
    property int _stepSizeMouse: 3

    //    property real _saveTXT: 0
    property real _value: -70.00
    property real _valueLog:  5

    property string _text_2: "-inf dB"
    property bool _doubleClicked_isEnable: false

//    Rectangle {
//        anchors.fill: parent

//    }

    function logFunc(value){

        var log = Math.log(value + 70)

        var max = Math.log(_max + 70)

        var log_normal = (log/max)

        var res = (log_normal * (_max-_min))-70

        if(log === 0 || res < _min){
            res = _min
        }

        return res
    }

    on_ValueChanged:
    {
        //            numberBox.text = (_value === -70) ? "-inf dB" : (_value.toFixed(_lengthDecimalSEC) +" dB")

        // -- برای انتقال مرکز اعداد به پایینتر از روش لگاریتمی استفاده کردیم --//
        //  -------------------------------------------------  //
        _valueLog = _value //logFunc(_value)

        // --  برای کنترل نمایش اعداد اعشاری کمتر عدد را با استرینگتبدیل کردیم --//
        // -------------------------------------------------------- //
        if(_valueLog === -70)
        {
            numberBox.text = "-inf dB"
        }
        else
        {
            // -- عدد را با 3 رقم اعشار به استرینگ تبدیل کردیم تا بتوان صفرهای اعشار را تشخیص داد --//
            _text_2 = _valueLog.toFixed(_lengthDecimalSEC).toString()

            if((_valueLog <10) && (_valueLog > -10))
            {
                numberBox.text = _text_2.substring(0, (_text_2.indexOf(".")+3)) + " dB"
            }
            else{
                numberBox.text = _text_2.substring(0, (_text_2.indexOf(".")+2)) + " dB"
            }
        }
        // -------------------------------------------------------- //


        //-----------------------------------------------------

        /* if(_value === -70)
        {
            numberBox.text = "-inf dB"
        }
        else
        {
            // -- عدد را با 3 رقم اعشار به استرینگ تبدیل کردیم تا بتوان صفرهای اعشار را تشخیص داد --//
            _text_2 = _value.toFixed(_lengthDecimalSEC).toString()

            if((_value <10) && (_value > -10))
            {
                numberBox.text = _text_2.substring(0, (_text_2.indexOf(".")+3)) + " dB"
            }
            else{
                numberBox.text = _text_2.substring(0, (_text_2.indexOf(".")+2)) + " dB"
            }
        }
*/
    }

    property int _bigWidth: resoless(200)
    property int _bigHeight: resoless(200)
    //    property int _borderWidth: borderRec.border.width

    property int _lengthDecimalSEC: 3// ((dragREC_NumberBox._value >= -9.99) && (dragREC_NumberBox._value <= 9.99)) ? 2 : 1

    //-- رو داره اما بدون رنگ هستش selectionColor این دستور نقش دستور  --//
    property bool _numberIsSaved: true

    verticalAlignment: Qt.AlignVCenter
    horizontalAlignment: Qt.AlignHCenter

    property int pressed_globalX: 0
    property int pressed_globalY: 0
    property int pressed_itemY: 0
    property int pressed_itemX: 0

    signal releaseMouse_sig

    property bool _justDragActived: false


    text: "10" //.toFixed(_lengthDecimalSEC)
    //    onTextChanged: numberBox.text = (_value === -70) ? "-inf dB" : (_value.toFixed(_lengthDecimalSEC) +" dB")
    color: _color_txt

    // -- ورودی را برای اعداد اعشاری تنظیم میکنیم --//
    validator: DoubleValidator {bottom: numberBox._min; top: numberBox._max; decimals: _lengthDecimalSEC-1}

    renderType: Text.QtRendering

    font{
        family: numberBox._family_txt
        pixelSize: numberBox._fontSize_txt
        weight: Font.DemiBold
    }

    Behavior on font{NumberAnimation {duration: 200} }

    cursorVisible: false

    // -------------------------------------------------------------------------------------------------------------------- //
    // -- با پرس بر روی مربع، فکوس تکس اینپوپ فعال میشه و فکوس موس عریا غیر فعال هستش -- //
    // -- با ریلیز بر روی مربع، فکوس  تکس اینپوپ غیرفعال میشه و فکوس موس عریا فعال میشه -- //
    // -- پس در هر دوحالت فکوس آنها مخالف هم هستن اما اگر در بیرون مربع کلیک بشه فکوس هر دوی آنها فالس میشه -- //
    // -- در حالتی که در بیرون مربع کلیک کنیم فکوس تکس اینپوت غیر فعال هستس و فکوس موس عریا فعال هستش -- //
    // -- بنابرین برای تشخیص اینکه در بیرون از مربع کلیک کردیم دستورات زیرو توی تکساینپوت دوباره میزنیم تا با تغییر فکوس آن کلیک در بیرون از مربع رو تشخیص دهیم -- //
    // -------------------------------------------------------------------------------------------------------------------- //
    //تا انتخاب شد کرسر رو پاک میکنیم
    onActiveFocusChanged: {

        if(numberBox.focus){
            cursorVisible = true
        }
        else{
            cursorVisible = false
        }
    }

    Keys.onReleased: {
        if(event.key === Qt.Key_Shift)
            numberBox._shift_is_clicked = false
    }

    Keys.onPressed: {
        console.log("keys onPressed")

        //--  اگر دکمه شیفت زده شد --//
        //------------------------------------------------
        if(event.key === Qt.Key_Shift)
            numberBox._shift_is_clicked = true

        if(!_justDragActived){

            //--  اگر  اعداد بصورت دستی وارد شد و دکمه اینتر زده شد --//
            //------------------------------------------------
            if((event.key === 16777220) || (event.key === Qt.Key_Enter)){

                _numberIsSaved = true
                cursorVisible = false

                //-- زمانی که کاربر کاراکتری وارد میکند مقدار تکس تغییر میکند نه ولیو برای همین مقدار تکس را بررسی میکنیم --//
                if((numberBox.text > _min) && (numberBox.text <= _max))
                {
                    _value = parseFloat(numberBox.text)
//                    _value = logFunc(parseFloat(numberBox.text))

                    // -- اگر بیش از یک بار عددی تکراری وارد کنیمم ولیو آپدیت نمیشه برای همین خودمون بصورت دستی تغییرش میدیم --//
                    if((_value <10) && (_value > -10))
                        _value = _value.toFixed(2)
                    else
                        _value = _value.toFixed(1)
                }
                else
                    if((numberBox.text <= _min)){
                        numberBox._value = _min.toFixed(_lengthDecimalSEC)

                        // -- اگر بیش از یک بار عددی بزرگتر از مینیمم وارد کنیمم ولیو آپدیت نمیشه برای همین خودمون بصورت دستی تغییرش میدیم --//
                        text = "-inf dB"
                    }
                    else{
                        numberBox._value = _max.toFixed(_lengthDecimalSEC)
                        // -- اگر بیش از یک بار عددی بزرگتر از ماکسیمم وارد کنیمم ولیو آپدیت نمیشه برای همین خودمون بصورت دستی تغییرش میدیم --//
                        text = _value.toFixed(1)
                    }

            }


            //--  اگر دکمه بالا و پایین زده شد --//
            //------------------------------------------------

            if ((event.key === Qt.Key_Up) && (numberBox._value<(numberBox._max)) && (!cursorVisible)) {

                if((_value > -10.1) && (_value < 10))
                {
                    numberBox._value = (_value + .01).toFixed(_lengthDecimalSEC)
                }
                else{
                    // --  رقم صدگان که بویسه درگ کردن تغییر کرده رو صفر میکنیم و بعدش عدد را تغییر میدهیم --//
                    numberBox._value = (_value).toFixed(1)
                    numberBox._value = (_value + .1).toFixed(_lengthDecimalSEC)
                }
            }
            else
                if ((event.key === Qt.Key_Down) && (_value>(numberBox._min)) && (!cursorVisible)) {

                    if((_value > -10) && (_value < 10.1))
                    {
                        numberBox._value = (_value - .01).toFixed(_lengthDecimalSEC)
                    }
                    else{
                        // --  رقم صدگان که بویسه درگ کردن تغییر کرده رو صفر میکنیم و بعدش عدد را تغییر میدهیم --//
                        numberBox._value = (_value).toFixed(1)
                        numberBox._value = (_value - .1).toFixed(_lengthDecimalSEC)
                    }
                }

            //--  اگر یک عدد زده شد زده شد --//
            //------------------------------------------------
            if ( ((_min)<0 && event.key === 45) || (event.key === Qt.Key_0)|| (event.key === Qt.Key_1)|| (event.key === Qt.Key_2)|| (event.key === Qt.Key_3)|| (event.key === Qt.Key_4)|| (event.key === Qt.Key_5)|| (event.key === Qt.Key_6)||(event.key === Qt.Key_7)||(event.key === Qt.Key_8)||(event.key === Qt.Key_9))
            {
                //-- اگر متن انتخاب شده بود این دستور رو اجرا میکنیم --//
                if(_numberIsSaved){

                    clear()
                    cursorVisible = true
                    _numberIsSaved = false

                }

            }

        }
    }

    //بخش درگینگ تام سیگنچر صورت
    MouseArea{
        id: numberBox_ma
        anchors.fill: parent

        anchors.centerIn: parent

        property int _PrevY: -50000
        acceptedButtons: Qt.AllButtons
        //        propagateComposedEvents : true

        // -------------------------------------------------------------------------------------------------------------------- //
        // -- با پرس بر روی مربع، فکوس تکس اینپوپ فعال میشه و فکوس موس عریا غیر فعال هستش -- //
        // -- با ریلیز بر روی مربع، فکوس  تکس اینپوپ غیرفعال میشه و فکوس موس عریا فعال میشه -- //
        // -- پس در هر دوحالت فکوس آنها مخالف هم هستن اما اگر در بیرون مربع کلیک بشه فکوس هر دوی آنها فالس میشه -- //
        // -- در حالتی که در بیرون مربع کلیک کنیم فکوس تکس اینپوت غیر فعال هستس و فکوس موس عریا فعال هستش -- //
        // -- بنابرین برای تشخیص اینکه در بیرون از مربع کلیک کردیم دستورات زیرو توی تکساینپوت دوباره میزنیم تا با تغییر فکوس آن کلیک در بیرون از مربع رو تشخیص دهیم -- //
        // -------------------------------------------------------------------------------------------------------------------- //
        onFocusChanged: {

            // --اگر درون از مربع کلیک کردییم --//
            //------------------------------------------------
            if(numberBox_ma.focus || numberBox.focus){
                //                borderRec.border.width = 2
            }
            // --اگر بیرون از مربع کلیک کردییم --//
            //------------------------------------------------
            else{
                cursorVisible = false

                if((numberBox._value >= _min) && (numberBox._value <= _max)){
                    _value = parseFloat(numberBox.text).toFixed(_lengthDecimalSEC)
                }
                else
                    if((numberBox._value < _min)){
                        _value = _min.toFixed(_lengthDecimalSEC)
                    }
                    else{
                        _value =  _max.toFixed(_lengthDecimalSEC)

                    }

            }

        }

        onPressed: {

            _numberIsSaved = false

            pressed_itemY = mouse.y
            pressed_itemX = mouse.x

            //-- مختصات موس در تمام صفحه تور متغییر زیر ذخیره میشه --//
            var globalPosition= mapToGlobal(mouse.x, mouse.y)

            pressed_globalX = globalPosition.x
            pressed_globalY = globalPosition.y

            //            console.log("_X2= " + pressed_globalX + "_Y2= " + pressed_globalY)

            numberBox_ma._PrevY = globalPosition.y

            mainRoot_id.cursorPosition.hideCursor()

            //------------------------//
            numberBox.focus = true
            //------------------------//
            //            console.log("x= " + numberBox.x)
            //            console.log("y= " + numberBox.y)

        }

        onReleased: {

            releaseMouse_sig()

            //            mainRoot_id.cursorPosition_pro.moveCursor(pressed_globalX,pressed_globalY)
            //            console.log("pressed_globalY= " + pressed_globalY)


            _numberIsSaved = true

            //            mainRoot_id.cursorPosition_pro.moveCursor((pressed_globalX) ,(pressed_globalY))

            //            mainRoot_id.cursorPosition_pro.moveCursor((dragX_pro) ,(dragY_pro))
            //            console.log("--------------")
            //            console.log("_X= " + dragX_pro + "_Y= " + dragY_pro)

            //-- مختصات موس در تمام صفحه تور متغییر زیر ذخیره میشه --//
            //            var globalPosition= mapToGlobal(mouse.x, mouse.y)

            //            mainRoot_id.cursorPosition_pro.moveCursor(numberBox.x , numberBox.y)
            //            console.log("numberBox.Y= " + numberBox.y)

            //            var globalCoordinare = numberBox.mapToItem(numberBox.parent, 0, 0)
            //            console.log("X: " + globalCoordinare.x + " y: " + globalCoordinare.y)

            mainRoot_id.cursorPosition.showCursor()

            //------------------------//
            numberBox_ma.focus = true
            //------------------------//

        }

        onDoubleClicked: {
            if(_doubleClicked_isEnable)
            _value = 0
        }

        property bool _isEdge: false
        property real _step: .01 //((_value >= _min && _value <= -10) || (_value >= 10 && _value <= _max)) ? .1 : .01
        property bool _isTen: false
        onMouseYChanged: {
            //-- مختصات موس در تمام صفحه تور متغییر زیر ذخیره میشه --//
            var globalPosition= mapToGlobal(mouse.x, mouse.y)



            //            pressed_globalY = globalPosition.y
            //            console.log("_X= " + pressed_globalX)
            //                        console.log("_Y= " + pressed_globalY)

            numberBox.cursorVisible = false

            // -- چون با رسیدن به لبه صفحه کرسر رو جابجا میکنیم و به وسط صفحه میکشیم مقدار وای نسبت به وای قبلی تغییر زیادی  --//
            // --پیدا میکنه، بنابراین مقدار وای قبلی رو به محض رسدن به لبه صفحه به روز رسانی میکنیم --//
            // ----------------------------------------------------------------- //
            if(_isEdge){
                _isEdge = false
                numberBox_ma._PrevY = globalPosition.y
                return
            }

            // -- متغیر سایز برای تشخیص تغییرات یهویی استفاده میشه --//
            // ----------------------------------------------------------------- //
            var size = Math.abs(Math.round(globalPosition.y) - Math.round(_PrevY))

            // -- کنترل کردن گام ها در حالتی که کاربر تغییرات ربز یا زیادی رو داره --//
            // -------------------------------------------------------//
            if(!numberBox._shift_is_clicked){

                if(size === 1){
                    _step = .1
                }
                else{
                    _step = Math.round(size)
                }

            }
            else
                if(numberBox._shift_is_clicked){

                    if(size === 1){
                        _step = (_value > -10.1 && _value < 10.1) ? .003 : .03
                    }
                    else{
                        _step = Math.round(size)*.4
                    }

                }

            // -- اعمال تغییرات موس برروی دکمه با توجه به گام تعریف شده در مرحله قبلی --//
            // ------------------------------------------------------------- //
            if(!numberBox._shift_is_clicked){

                //----------------------------------------------------------------------
                //در این بخش متناسب با مختصات موس مقدار تکس اینپوت مرتبط را تغییر میدهیم
                //----------------------------------------------------------------------
                if(((Math.round(globalPosition.y)) < (numberBox_ma._PrevY)) && (numberBox._value<(numberBox._max)) /*&& (Math.round(globalPosition.y%(numberBox._stepSizeMouse)) === 0)*/){
                    numberBox._value = ((numberBox._value) + _step) <= (numberBox._max) ? ((numberBox._value) + _step).toFixed(_lengthDecimalSEC) : _max.toFixed(_lengthDecimalSEC)
                    //عدد وارد شده با اینتر را ذخیره کن
                }
                else
                    if(((Math.round(globalPosition.y)) > (numberBox_ma._PrevY)) && (numberBox._value>(numberBox._min)) /*&& (Math.round(globalPosition.y%(numberBox._stepSizeMouse)) === 0)*/){

                        numberBox._value = ((numberBox._value) - _step) >= (numberBox._min) ? ((numberBox._value) - _step).toFixed(_lengthDecimalSEC) : _min.toFixed(_lengthDecimalSEC)
                    }
            }
            else{
                //----------------------------------------------------------------------
                //در این بخش متناسب با مختصات موس مقدار تکس اینپوت مرتبط را تغییر میدهیم
                //----------------------------------------------------------------------
                if((Math.round(globalPosition.y) < numberBox_ma._PrevY) && (numberBox._value<(numberBox._max)) /*&& (Math.round(globalPosition.y%(numberBox._stepSizeMouse)) === 0)*/ /*&& (_step === 1)*/){

                    numberBox._value = ((numberBox._value) + _step) <= (numberBox._max) ? ((numberBox._value) + _step   ).toFixed(_lengthDecimalSEC) : _max.toFixed(_lengthDecimalSEC)
                }
                else{
                    if((Math.round(globalPosition.y) > numberBox_ma._PrevY) && (numberBox._value>(numberBox._min)) /*&& (Math.round(globalPosition.y%(numberBox._stepSizeMouse)) === 0)*/ /*&& (_step === 1)*/){

                        numberBox._value = ((numberBox._value) - _step) >= (numberBox._min) ? ((numberBox._value) - _step).toFixed(_lengthDecimalSEC) : _min.toFixed(_lengthDecimalSEC)

                    }
                }

            }

            //عدد وارد شده با اینتر را ذخیره کن
            numberBox_ma._PrevY = globalPosition.y

            //--  کنترل کردن کرسر برای اینکه از ویندوز بیرون نره --//
            // -- باید در نظرداشت چونکه تشخیص لبه صفحه با خطا همراه از هر طرف بصورت پیش فرض 50 پیکسل فاصله میگیریم --//
            //---------------------------------------------//
            if((globalPosition.y <= 50 ) && ((numberBox._value) < numberBox._max) ||
                    (globalPosition.y >= (Screen.height-50)) && ((numberBox.text) > numberBox._min)){
                mainRoot_id.cursorPosition.moveCursor((Screen.width-50)/2 ,((Screen.height-50)/2))
                _isEdge = true

            }

        }

    }

}




