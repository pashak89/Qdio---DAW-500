//#include "scrubareaitem.h"
//#include "audio/audioengine.h"
//#include "audio/engine.h"
//#include "audio/song.h"
//#include "core/sampleeditor.h"
//#include <QCursor>
//#include <core/cliparea.h>
//ScrubAreaItem::ScrubAreaItem(AreaInfo* areaInfo)
//    : IPainterItem()
//    , _areaInfo(areaInfo)
//{
//}

//int ScrubAreaItem::mousePressEvent(QMouseEvent* event)
//{

//    if (!_rect.contains(event->pos()))
//        return 0;

//    auto _currentDuration = _areaInfo->clipDuration();
//    auto _lastWidth = _areaInfo->lastWidth();
//    auto _startTime = _areaInfo->startTime();

//    double currentIndicator = event->pos().x() / (double)_lastWidth * _currentDuration + _startTime;

//    Q_EMIT sigCurrentIndicatorChanged(currentIndicator);

//    return 1;
//}

//int ScrubAreaItem::hoverMoveEvent(QHoverEvent* event)
//{

//    if (_rect.contains(event->pos())) {
//        _mouseEnter = true;
//        return 1;
//    } else {
//        if (_mouseEnter) {

//            return 0;
//        }
//    }

//    return 0;
//}

//void ScrubAreaItem::draw(IPainter* painter)
//{
//    painter->drawLine(X, Y, X + width, Y);
//    painter->drawLine(X, Y + height, X + width, Y + height);

//    painter->beginPath();
//    painter->setFillStyle("#A0808080");
//    painter->setStrokeStyle("#A0808080");
//    painter->fillRect(X, Y, width, height);
//}
