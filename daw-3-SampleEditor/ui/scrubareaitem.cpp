#include "scrubareaitem.h"
#include "core/screeninterface.h"
#include <core/measure.h>
#include <core/sampleeditor.h>
ScrubAreaItem::ScrubAreaItem(AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
{
    _timerCursorPosition.setInterval(40);
    connect(&_timerCursorPosition, &QTimer::timeout, this, &ScrubAreaItem::sltTimerCheckPosition);
}

void ScrubAreaItem::setLoopBgColor(const QColor& loopBgColor)
{
    _loopBgColor = loopBgColor;
}

void ScrubAreaItem::draw(IPainter* painter)
{
    int W = _rect.width();
    int H = _rect.height();
    int B = _rect.bottom();
    int X = _rect.x();
    int Y = _rect.y();
    int T = _rect.top();

    float startX = _areaInfo->time2Pixel(_areaInfo->startBraceMarker());

    float endX = _areaInfo->time2Pixel(_areaInfo->endBraceMarker());

    painter->beginPath();

    _loopBgColor = "#656565";
    QColor _lineColor = "#1E1E1E";

    if (_areaInfo->themeType() == AreaInfo::ThemeType_Theme1) {
        _loopBgColor = "#656565";
    }
    if (_areaInfo->themeType() == AreaInfo::ThemeType_Theme2) {
        _loopBgColor = "#949494";
    }

    if (_areaInfo->loopFocusActive()) {
        _loopBgColor = "#96D4E8";
    } else {
        if (_areaInfo->loopActive()) {
            _loopBgColor = "#FFA647";
        }
    }

    painter->fillRect(startX, Y, endX - startX, (B - T) / 2.0, _loopBgColor, _lineColor);

    if (_areaInfo->startBraceMarker() >= _areaInfo->startTime()) {

        painter->drawTriangleBraceLeft(painter, startX, Y, H / 2, _loopBgColor, _lineColor);
    }
    if ((_areaInfo->endBraceMarker() <= _areaInfo->endTime())) {

        painter->drawTriangleBraceRight(painter, endX, Y, H / 2, _loopBgColor, _lineColor);
    }
}

int ScrubAreaItem::mousePressEvent(QMouseEvent* event)
{

    _startMarkerMove = false;
    _endMarkerMove = false;
    _midMarkerMove = false;
    if (!_rect.contains(event->pos()))
        return 0;

    _timerCursorPosition.start();

    int H = _rect.height();
    float startX = _areaInfo->time2Pixel(_areaInfo->startBraceMarker());

    float endX = _areaInfo->time2Pixel(_areaInfo->endBraceMarker());

    float W = (SQLRT3_4 * _rect.height());

    QRect rect_start;
    QRect rect_end;
    QRect rect_mid;

    int pix_4 = ScreenInterface::instance()->freeResolution(6);
    int pix_2 = ScreenInterface::instance()->freeResolution(6);
    if (qAbs(startX - endX) <= 2 || startX >= endX) {
        rect_end = QRect(startX + pix_4, _rect.y(), (startX + W) - (startX + pix_4), H);
        rect_start = QRect(endX - W, _rect.y(), (endX - pix_4) - (endX - W), H);
        rect_mid = QRect(endX - pix_4, _rect.y(), (startX + W) - (endX - pix_4), H);

    } else {

        if (startX + W > endX - W && startX < endX && startX > endX - W && endX < startX + W) {
            rect_start = QRect(endX, _rect.y(), startX + W - endX, H);
            rect_end = QRect(endX - W, _rect.y(), startX - (endX - W), H);
            rect_mid = QRect(startX, _rect.y(), endX - startX, H);

            if (rect_start.width() == 0 || rect_end.width() == 0) {
                rect_start = QRect(startX, _rect.y(), 1, H);
                rect_end = QRect(endX - 1, _rect.y(), 2 * 1, H);
                rect_mid = QRect(startX + 1, _rect.y(), (endX - 1) - (startX + 1), H);
            }

        } else if (startX + W >= endX - W && startX <= endX && startX <= endX - W && startX + W <= endX) {

            rect_start = QRect(startX, _rect.y(), pix_2, H);
            rect_end = QRect(endX - pix_2, _rect.y(), 2 * pix_2, H);
            rect_mid = QRect(startX + pix_2, _rect.y(), (endX - pix_2) - (startX + pix_2), H);

        } else {
            rect_start = QRect(startX, _rect.y(), W, H);
            rect_end = QRect(endX - W, _rect.y(), W, H);
            rect_mid = QRect(rect_start.x() + rect_start.width(), _rect.y(), (endX - W) - (startX + W), H);
        }
    }

    if (rect_start.contains(event->pos())) {
        _startMarkerMove = true;
        _loopBgColor = Qt::gray;

    } else if (rect_end.contains(event->pos())) {
        _endMarkerMove = true;
        _loopBgColor = Qt::gray;

    } else if (rect_mid.contains(event->pos())) {
        _midMarkerMove = true;
        _lastMousePressX = event->pos().x();
        _loopBgColor = Qt::blue;
        _areaInfo->setLoopFocusActive(true);
        _braceDuration = _areaInfo->endBraceMarker() - _areaInfo->startBraceMarker();
        _startMoveTime = _areaInfo->startBraceMarker();
    } else {
        auto _currentDuration = _areaInfo->clipDuration();
        auto _lastWidth = _areaInfo->lastWidth();
        auto _startTime = _areaInfo->startTime();

        double currentIndicator = event->pos().x() / (double)_lastWidth * _currentDuration + _startTime;

        Q_EMIT sigCurrentIndicatorChanged(currentIndicator);
    }

    _areaInfo->setStartMarker(_areaInfo->startBraceMarker());
    _areaInfo->setEndMarker(_areaInfo->endBraceMarker());

    return 1;
}

int ScrubAreaItem::mouseMoveEvent(QMouseEvent* event)
{

    _lastPoint = event->pos();

    if ((_startMarkerMove == false && _endMarkerMove == false && _midMarkerMove == false) || _autoScroll) {
        return 0;
    }

    int currentPix;
    float startMarker;
    float endMarker;

    double clipDuration = _areaInfo->clipDuration();
    double fileDuration = _areaInfo->fileDuration();
    double step = _areaInfo->stepPlotDuration();

    int lob = _areaInfo->lenghtOfBeat();
    int bpb = _areaInfo->beatPerBar();

    double _tempo = _areaInfo->tempo();

    auto m = Measure::timeToMeasure(clipDuration, lob, bpb, _tempo);

    currentPix = event->pos().x();

    if (_startMarkerMove) {
        startMarker = currentPix / (double)_areaInfo->lastWidth() * _areaInfo->clipDuration() + _areaInfo->startTime();
        startMarker = qRound(startMarker / (double)(step)) * (step);

        if (startMarker < _areaInfo->minStartTime()) {
            startMarker = _areaInfo->minStartTime();
        }

        float minDuration = m.getBeatDuration() * 1000;
        if (startMarker + minDuration > _areaInfo->endBraceMarker()) {
            startMarker = _areaInfo->endBraceMarker() - minDuration;
        }

        _areaInfo->setStartBraceMarker(startMarker);
        _areaInfo->setStartMarker(_areaInfo->startBraceMarker());
    }

    if (_endMarkerMove) {
        endMarker = currentPix / (double)_areaInfo->lastWidth() * _areaInfo->clipDuration() + _areaInfo->startTime();
        endMarker = qRound(endMarker / (double)(step)) * (step);

        float minDuration = m.getBeatDuration() * 1000;
        if (endMarker - minDuration < _areaInfo->startBraceMarker()) {
            endMarker = _areaInfo->startBraceMarker() + minDuration;
        }

        _areaInfo->setEndBraceMarker(endMarker);
        _areaInfo->setEndMarker(_areaInfo->endBraceMarker());

        if (endMarker <= _areaInfo->fileDuration()) {
            _areaInfo->setTimeDuration(fileDuration);
        } else {
            _areaInfo->setTimeDuration(endMarker);
        }
    }

    if (_midMarkerMove) {
        float startMarker = _areaInfo->startBraceMarker();
        float endMarker = _areaInfo->endBraceMarker();

        float diffX = event->pos().x() - _lastMousePressX;
        float diffPos = qAbs(diffX) / (double)_areaInfo->lastWidth() * _areaInfo->clipDuration();
        diffPos = qRound(diffPos / (step)) * (step);

        if (diffX < 0) {

            if (_startMoveTime - diffPos >= _areaInfo->minStartTime()) {
                startMarker = _startMoveTime - diffPos;
                endMarker = startMarker + _braceDuration;
            } else {
                startMarker = 0;
                endMarker = startMarker + _braceDuration;
            }

            _areaInfo->setStartBraceMarker(startMarker);
            _areaInfo->setStartMarker(_areaInfo->startBraceMarker());

            _areaInfo->setEndBraceMarker(endMarker);
            _areaInfo->setEndMarker(_areaInfo->endBraceMarker());

        } else {

            if (_startMoveTime + diffPos < _areaInfo->maxEndTime()) {
                startMarker = _startMoveTime + diffPos;
                endMarker = startMarker + _braceDuration;
            }

            _areaInfo->setStartBraceMarker(startMarker);
            _areaInfo->setStartMarker(_areaInfo->startBraceMarker());

            _areaInfo->setEndBraceMarker(endMarker);
            _areaInfo->setEndMarker(_areaInfo->endBraceMarker());
        }
    }

    return 1;
}

int ScrubAreaItem::mouseReleaseEvent(QMouseEvent* event)
{

    if (_startMarkerMove) {
        _startMarkerMove = false;
    }
    if (_midMarkerMove) {
        _midMarkerMove = false;
    }
    if (_endMarkerMove) {
        _endMarkerMove = false;
    }
    sigSetCursor(Qt::ArrowCursor);

    _timerCursorPosition.stop();
    return 1;
}

int ScrubAreaItem::hoverMoveEvent(QHoverEvent* event)
{

    int startX = 0;
    if (_areaInfo->startBraceMarker() >= _areaInfo->startTime()) {
        startX = (_areaInfo->startBraceMarker() - _areaInfo->startTime()) / _areaInfo->clipDuration() * _areaInfo->lastWidth();
    }

    int endX = _rect.width();
    if ((_areaInfo->endBraceMarker() <= _areaInfo->endTime())) {
        endX = (_areaInfo->endBraceMarker() - _areaInfo->startTime()) / _areaInfo->clipDuration() * _areaInfo->lastWidth();
    }

    int pix_4 = ScreenInterface::instance()->freeResolution(6);
    int pix_2 = ScreenInterface::instance()->freeResolution(6);

    QRect rect_start;
    QRect rect_end;
    QRect rect_mid;
    float W = (SQLRT3_4 * _rect.height());
    int H = _rect.height();

    if (qAbs(startX - endX) <= 2 || startX >= endX) {
        rect_end = QRect(startX + pix_4, _rect.y(), (startX + W) - (startX + pix_4), H);
        rect_start = QRect(endX - W, _rect.y(), (endX - pix_4) - (endX - W), H);
        rect_mid = QRect(endX - pix_4, _rect.y(), (startX + W) - (endX - pix_4), H);

    } else {

        if (startX + W > endX - W && startX < endX && startX > endX - W && endX < startX + W) {
            rect_start = QRect(endX, _rect.y(), startX + W - endX, H);
            rect_end = QRect(endX - W, _rect.y(), startX - (endX - W), H);
            rect_mid = QRect(startX, _rect.y(), endX - startX, H);

            if (rect_start.width() == 0 || rect_end.width() == 0) {
                rect_start = QRect(startX, _rect.y(), 1, H);
                rect_end = QRect(endX - 1, _rect.y(), 2 * 1, H);
                rect_mid = QRect(startX + 1, _rect.y(), (endX - 1) - (startX + 1), H);
            }

        } else if (startX + W >= endX - W && startX <= endX && startX <= endX - W && startX + W <= endX) {

            rect_start = QRect(startX, _rect.y(), pix_2, H);
            rect_end = QRect(endX - pix_2, _rect.y(), 2 * pix_2, H);
            rect_mid = QRect(startX + pix_2, _rect.y(), (endX - pix_2) - (startX + pix_2), H);

        } else {
            rect_start = QRect(startX, _rect.y(), W, H);
            rect_end = QRect(endX - W, _rect.y(), W, H);
            rect_mid = QRect(rect_start.x() + rect_start.width(), _rect.y(), (endX - W) - (startX + W), H);
        }
    }

    if (rect_start.contains(event->pos()) || rect_end.contains(event->pos())) {
        return 1;
    } else if (rect_mid.contains(event->pos())) {
        return 2;
    } else {
        if (_rect.contains(event->pos())) {
            return 3;
        }
        return 0;
    }
}

void ScrubAreaItem::sltTimerCheckPosition()
{

    //    _autoScroll = false;
    //    if (_lastPoint.x() > rect().width()) {

    //        _autoScroll = true;
    //        float timeDuration = _sampleEditor->timeDuration();
    //        float startTime = _sampleEditor->sampleClip()->startTime();
    //        float endTime = _sampleEditor->sampleClip()->endTime();
    //        float rate = 2 * (_sampleEditor->sampleClip()->currentClipDuration() / _sampleEditor->lastWidth());

    //        _sampleEditor->setTimeDuration(rate + timeDuration);

    //        _sampleEditor->sampleClip()->setStartTime(startTime + rate);
    //        _sampleEditor->sampleClip()->setEndTime(endTime + rate);

    //        _sampleEditor->set_areaInfo->endBraceMarker()(endTime + rate);
    //        _sampleEditor->setEndMarker(endTime + rate);

    //        _sampleEditor->setMaxEndTime(endTime + rate);

    //        Q_EMIT sigUpdate();
    //    }
    //    if (_lastPoint.x() < rect().width()) {
    //    }
}
