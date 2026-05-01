#include "hotspotitem.h"
#include "core/measure.h"
#include "core/sampleeditor.h"
#include "core/screeninterface.h"
#include <QCursor>
#include <QQuickItem>
#include <QtMath>
#include <core/cliparea.h>
HotspotItem::HotspotItem(ClipArea* clipArea, AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
    , _clipArea(clipArea)
{
}

double HotspotItem::lastCursorPosY() const
{
    return _lastCursorPosY;
}

double HotspotItem::lastCursorPosX() const
{
    return _lastCursorPosX;
}

void HotspotItem::draw(IPainter* painter)
{

    double endTime = _areaInfo->endTime();
    double totalDuration = _areaInfo->maxEndTime() - _areaInfo->minStartTime();
    double startTime = _areaInfo->startTime();
    double lastWidth = _areaInfo->lastWidth();

    double T = _rect.top();
    double B = _rect.bottom();
    double H = _rect.height();

    int maxSize = ScreenInterface::instance()->freeResolution(40);

    double t0 = startTime + qAbs(_areaInfo->minStartTime());
    double t1 = endTime + qAbs(_areaInfo->minStartTime());
    double startPix = (t0) / (double)totalDuration * lastWidth;
    double endPix = (t1) / (double)totalDuration * lastWidth;

    double trackHeight = H / _clipArea->tracksModel()->size();
    for (int i = 0; i < _clipArea->tracksModel()->size(); i++) {

        auto clipItems = _clipArea->tracksModel()->trackItem(i)->getClipItems();
        QMapIterator<QString, QSharedPointer<ClipItem>> j(clipItems);
        while (j.hasNext()) {
            j.next();

            double t0 = j.value()->sampleClip()->startTime() + qAbs(_areaInfo->minStartTime());
            double t1 = j.value()->sampleClip()->endTime() + qAbs(_areaInfo->minStartTime());
            double startPix = (t0) / (double)totalDuration * lastWidth;
            double endPix = (t1) / (double)totalDuration * lastWidth;

            painter->beginPath();
            painter->setLineWidth(1);
            painter->setFillStyle(QNanoColor::fromQColor(j.value()->baseColor()));
            painter->setStrokeStyle(QNanoColor::fromQColor(j.value()->baseColor()));
            painter->drawRect(startPix, i * trackHeight, endPix, i * trackHeight + trackHeight);
            painter->fill();
            painter->restore();
        }
    }

    if (endPix - startPix < maxSize) {

        if (_zoomStop == false) {
            _zoomStop = true;

            double center = _lastBeatBarPressX + qAbs(_areaInfo->minStartTime());
            double x = center / totalDuration * lastWidth;
            double rr = _lastHotSpotPressTime;
            if (x - maxSize * rr > 0 && x + maxSize * (1 - rr) < lastWidth) {
                _startPix = x - maxSize * rr;
                _endPix = x + maxSize * (1 - rr);
            }

            if (x - maxSize * rr < 0) {
                _startPix = 0;
                _endPix = _startPix + maxSize;
            }

            if (x + maxSize * (1 - rr) > lastWidth) {
                _endPix = lastWidth;
                _startPix = _endPix - maxSize;
            }
            if (startPix < _startPix) {
                _startPix = startPix;
                _endPix = startPix + maxSize;
            }
            if (endPix > _endPix) {
                _endPix = endPix;
                _startPix = endPix - maxSize;
            }
        }

    } else {
        _zoomStop = false;
        _endPix = endPix;
        _startPix = startPix;
    }

    painter->beginPath();
    painter->setLineWidth(ScreenInterface::instance()->freeResolution(4));
    painter->setStrokeStyle("#1E1E1E");
    painter->drawRect(2, 2, _rect.width() - 4, _rect.height() - 4);
    painter->stroke();

    painter->beginPath();
    painter->setLineWidth(ScreenInterface::instance()->freeResolution(4));
    painter->setStrokeStyle(QNanoColor("#969696"));

    painter->roundedRect(_startPix + 2, 2, _endPix - _startPix - 4, _rect.height() - 6, ScreenInterface::instance()->freeResolution(4));

    painter->stroke();
    painter->restore();
}

int HotspotItem::mousePressEvent(QMouseEvent* event)
{

    _resizeStart = false;
    _resizeEnd = false;
    _zoomEnabled = false;
    if (!_rect.contains(event->pos()))
        return 0;

    double endTime = _areaInfo->startTime() + _areaInfo->clipDuration();
    double clipDuration = _areaInfo->clipDuration();
    double startTime = _areaInfo->startTime();
    double lastWidth = _areaInfo->lastWidth();
    double totalDuration = _areaInfo->maxEndTime() - _areaInfo->minStartTime();

    int posX = event->pos().x();
    double posTime = posX / (double)lastWidth * totalDuration + _areaInfo->minStartTime();

    double startPix = _startPix;
    double endPix = _endPix;

    double t0 = startTime + qAbs(_areaInfo->minStartTime());
    double t1 = (endTime) + qAbs(_areaInfo->minStartTime());
    double posTime2 = (posX - startPix) / (endPix - startPix) * (t1 - t0) + t0;

    int maxSize = ScreenInterface::instance()->freeResolution(40);

    double _durationWidth = endPix - startPix;

    if (qAbs(startPix - posX) < 4) {
        _resizeStart = true;
    } else if (qAbs(endPix - posX) < 4) {
        _resizeEnd = true;

    } else {
        _zoomEnabled = true;
    }

    if (_resizeStart) {
        QGuiApplication::setOverrideCursor((QCursor(Qt::SplitHCursor)));
    } else if (_resizeEnd) {
        QGuiApplication::setOverrideCursor((QCursor(Qt::SplitHCursor)));
    } else {

        _mousePressRatio = (posX - startPix) / (endPix - startPix);

        if (posX > startPix && posX < endPix) {
            if (endPix - startPix > maxSize) {
                _lastHotSpotPressTime = _mousePressRatio;
            }
            _lastBeatBarPressX = posTime2;

        } else {

            _zoomStop = false;
            _lastBeatBarPressX = (posTime);
            _lastHotSpotPressTime = (0.5);
            _mousePressRatio = 0.5;
        }

        QGuiApplication::setOverrideCursor(Qt::BlankCursor);

        if (posX > startPix && posX < endPix) {
        } else {
            if (posX - _durationWidth / 2 < 0) {

                _areaInfo->setClipProperties(_areaInfo->minStartTime(), _areaInfo->minStartTime() + clipDuration, clipDuration);

                return 1;

            } else if (posX + _durationWidth / 2 > lastWidth) {

                _areaInfo->setClipProperties(_areaInfo->maxEndTime() - clipDuration, _areaInfo->maxEndTime(), clipDuration);

                return 1;
            } else if (posX < startPix || posX > endPix) {

                _areaInfo->setClipProperties(posTime - clipDuration / 2.0, posTime + clipDuration / 2.0, clipDuration);

                return 1;
            }
        }
    }

    return 0;
}

int HotspotItem::mouseMoveEvent(QMouseEvent* event)
{

    if (_zoomEnabled == false && _resizeEnd == false && _resizeStart == false) {
        return false;
    }

    if (_mouseMove == false) {
        _mouseMove = true;

        _lastCursorPosX = event->pos().x();
        _lastCursorPosY = event->pos().y();

        _startPositionX = event->pos().x();
        _startPositionY = event->pos().y();

        _mouseReleased = false;
    }

    bool status = 0;

    double currentEnd = _areaInfo->endTime();
    double currentStart = _areaInfo->startTime();

    double lastWidth = _areaInfo->lastWidth();
    double fileDuration = _areaInfo->clipDuration();
    double clipDuration = _areaInfo->clipDuration();

    double minStartTime = _areaInfo->minStartTime();
    double maxEndTime = _areaInfo->maxEndTime();

    int lob = _areaInfo->lenghtOfBeat();
    int bpb = _areaInfo->beatPerBar();

    auto m = Measure::timeToMeasure(fileDuration, lob, bpb, _areaInfo->tempo());

    _currentPosistionY = (event->pos().y());
    _currentPosistionX = (event->pos().x());

    int deltaY = _lastCursorPosY - _currentPosistionY;
    int deltaX = _lastCursorPosX - _currentPosistionX;

    if (_resizeStart == false && _resizeEnd == false) {

        status = 1;
    } else {
    }

    double timePerPixel = _lastHotSpotPressTime;

    double minDuration = 10;
    int maxSize = ScreenInterface::instance()->freeResolution(40);

    double stepDuration = m.getSixteenthDuration() / 1024.0;

    minDuration = (stepDuration * 1000.0) / (maxSize / (double)lastWidth);

    double zoomValueUp = 0;
    double zoomValueDown = 0;

    zoomValueUp = -deltaY / (double)lastWidth * _areaInfo->clipDuration() * 1.0;
    zoomValueDown = -deltaY / (double)lastWidth * _areaInfo->clipDuration() * 1.0;

    bool isZoom = false;
    bool isPan = false;

    if (qAbs(deltaX) > qAbs(deltaY)) {
        isPan = true;

    } else if (qAbs(deltaY) > qAbs(deltaX)) {
        isZoom = true;
    }

    if (qAbs(deltaY) >= 1 && isZoom) {

        if (deltaY < 0) {
            double value = qAbs(zoomValueDown);

            if ((currentEnd - (value * (1 - timePerPixel))) - (currentStart + (value * timePerPixel)) >= minDuration) {

                currentStart += value * timePerPixel;
                currentEnd -= value * (1 - timePerPixel);
                clipDuration = currentEnd - currentStart;

            } else {

                currentStart += (clipDuration - minDuration) * timePerPixel;
                clipDuration = minDuration;
                currentEnd = currentStart + clipDuration;
            }

        } else if (deltaY > 0) {
            double value = qAbs(zoomValueUp);
            if (_areaInfo->zoomEndded()) {
                _areaInfo->setZoomEndded(false);
            }

            if (currentEnd + value * (1 - timePerPixel) > maxEndTime) {
                currentStart -= value;
                currentEnd = maxEndTime;
            } else if (currentStart - value * timePerPixel < minStartTime) {
                currentStart = minStartTime;
                currentEnd += value;
            } else {
                currentStart -= value * timePerPixel;
                currentEnd += value * (1 - timePerPixel);
            }

            if (currentEnd > maxEndTime) {
                currentEnd = maxEndTime;
            }
            if (currentStart < minStartTime) {
                currentStart = minStartTime;
            }

            clipDuration = currentEnd - currentStart;
        }
    }
    if (_resizeStart) {
        if (qAbs(deltaX) > 2 && isPan == true) {

            if (_currentPosistionX < 0) {
                _currentPosistionX = 0;
            }
            if (_currentPosistionX > lastWidth) {
                _currentPosistionX = lastWidth;
            }

            double duration = _areaInfo->maxEndTime() - _areaInfo->minStartTime();
            double t1 = (_areaInfo->endTime()) + qAbs(_areaInfo->minStartTime());
            double startPix = _currentPosistionX;
            double endPix = (t1) / (double)duration * lastWidth;
            double t0 = startPix / (double)lastWidth * duration + _areaInfo->minStartTime();
            // _beatBar->setLastHotSpotPressTime(-1);

            if (endPix - startPix > maxSize) {

                _areaInfo->setClipProperties(t0, _areaInfo->endTime(), _areaInfo->endTime() - _areaInfo->startTime());

            } else {
                startPix = endPix - maxSize;
                t0 = startPix / (double)lastWidth * duration + _areaInfo->minStartTime();

                _areaInfo->setClipProperties(t0, _areaInfo->endTime(), _areaInfo->endTime() - _areaInfo->startTime());
            }

            currentStart = _areaInfo->startTime();
            clipDuration = _areaInfo->clipDuration();
            currentEnd = _areaInfo->endTime();
        }
    } else if (_resizeEnd) {
        if (qAbs(deltaX) > 2 && isPan == true) {

            if (_currentPosistionX < 0) {
                _currentPosistionX = 0;
            }
            if (_currentPosistionX > lastWidth) {
                _currentPosistionX = lastWidth;
            }

            double duration = _areaInfo->maxEndTime() - _areaInfo->minStartTime();
            double t0 = _areaInfo->startTime() + qAbs(_areaInfo->minStartTime());
            double startPix = (t0) / (double)duration * lastWidth;
            double endPix = _currentPosistionX;
            double t1 = endPix / (double)lastWidth * duration + _areaInfo->minStartTime();

            //_beatBar->setLastHotSpotPressTime(-1);

            if (endPix - startPix > maxSize) {

                _areaInfo->setClipProperties(_areaInfo->startTime(), t1, _areaInfo->endTime() - _areaInfo->startTime());

            } else {

                endPix = startPix + maxSize;
                t1 = endPix / (double)lastWidth * duration + _areaInfo->minStartTime();

                _areaInfo->setClipProperties(_areaInfo->startTime(), t1, _areaInfo->endTime() - _areaInfo->startTime());
            }

            currentStart = _areaInfo->startTime();
            clipDuration = _areaInfo->clipDuration();
            currentEnd = _areaInfo->endTime();

            Q_EMIT sigUpdate();
        }
    } else {
        if (qAbs(deltaX) >= 1 && isPan) {

            double panValue = 0.001 * clipDuration * qAbs(deltaX);

            if (deltaX < 0) {

                if (currentEnd + panValue <= maxEndTime) {
                    currentStart = currentStart + panValue;
                    currentEnd = currentEnd + panValue;
                } else {
                    currentEnd = maxEndTime;
                    currentStart = currentEnd - clipDuration;
                }

            } else if (deltaX > 0) {

                if (currentStart - panValue >= minStartTime) {
                    currentStart = currentStart - panValue;
                    currentEnd = currentEnd - panValue;
                } else {
                    currentStart = minStartTime;
                    currentEnd = currentStart + clipDuration;
                }
            }

            _zoomStop = false;
        }

        if ((qAbs(deltaX) > 0 || qAbs(deltaY) > 0) && (isPan == true || isZoom == true)) {

            _areaInfo->setClipProperties(currentStart, currentEnd, clipDuration);

            double r = _lastHotSpotPressTime;
            double posTime = (currentEnd - currentStart) * r + currentStart;
            _lastBeatBarPressX = (posTime);

            currentStart = _areaInfo->startTime();
            clipDuration = _areaInfo->clipDuration();
            currentEnd = _areaInfo->endTime();
        }
    }

    return status;
}
int HotspotItem::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event)
    if (!_rect.contains(event->pos()))
        return 0;

    if (_mouseMove && (_resizeEnd == false && _resizeStart == false)) {

        double posX = (_startPix + _endPix) / 2.0;

        Q_EMIT sigCursorPosition(QPointF(posX, _startPositionY).toPoint());
    }

    _lastHotSpotPressTime = 0;
    _mouseReleased = true;
    _mouseMove = false;
    _resizeEnd = false;
    _resizeStart = false;
    _zoomEnabled = false;
    QGuiApplication::setOverrideCursor(Qt::ArrowCursor);

    return 1;
}

int HotspotItem::hoverMoveEvent(QHoverEvent* event)
{

    double posX = event->posF().x();

    //    qDebug() << "_startPix" << _startPix;
    if (qAbs(_startPix - posX) < 4) {
        return 1;
    } else if (qAbs(_endPix - posX) < 4) {
        return 2;
    } else {
        if (_rect.contains(event->pos())) {
            return 3;
        }
    }

    return 0;
}
