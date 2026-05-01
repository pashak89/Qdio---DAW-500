#include "markerareaitem.h"
#include "audio/audioengine.h"
#include "audio/engine.h"
#include "audio/song.h"

#include <core/cliparea.h>
MarkerAreaItem::MarkerAreaItem(AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
{
}

bool MarkerAreaItem::showIndicator() const
{
    return _showIndicator;
}

void MarkerAreaItem::setShowIndicator(bool showIndicator)
{
    _showIndicator = showIndicator;
}

int MarkerAreaItem::mousePressEvent(QMouseEvent* event)
{

    if (!_rect.contains(event->pos()))
        return 0;

    return 1;
}

void MarkerAreaItem::setPlayMovingMode(int playMovingMode)
{
    _playMovingMode = playMovingMode;
}

void MarkerAreaItem::draw(IPainter* painter)
{

    //    int Y = _rect.y();
    //    int B = _rect.bottom();
    //    int H = _rect.height();
    //    int T = _rect.top();

    //    auto _currentDuration = _areaInfo->clipDuration();
    //    auto _lastWidth = _areaInfo->lastWidth();
    //    auto _startTime = _areaInfo->startTime();

    //    painter->setStrokeStyle(QNanoColor("#EAEAEA"));
    //    float currentIndicator = _areaInfo->playbackMarker();
    //    float endOffset = _startTime + _currentDuration;

    //    painter->beginPath();

    //    int currentPix = ((currentIndicator - _startTime) / (double)_currentDuration) * _lastWidth;

    //    if (currentIndicator <= endOffset && currentIndicator >= _startTime) {
    //    }

    //    if (_showIndicator) {
    //        painter->setFillStyle("#eaeaea");
    //        painter->setStrokeStyle("#eaeaea");
    //        painter->moveTo(currentPix, Y);
    //        painter->lineTo(currentPix, B);
    //        painter->stroke();
    //    }
}
