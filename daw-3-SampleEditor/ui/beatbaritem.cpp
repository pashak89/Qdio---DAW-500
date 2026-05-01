#include "beatbaritem.h"
#include "core/global.h"
#include "core/screeninterface.h"
#include <QCursor>
#include <QScreen>
#include <core/cliparea.h>
#include <core/measure.h>

BeatBarItem::BeatBarItem(ClipArea* clipArea, AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
    , _clipArea(clipArea)
{
}

void BeatBarItem::draw(IPainter* painter)
{

    auto _currentDuration = _areaInfo->clipDuration();
    auto _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    auto _lenghtOfBeat = _areaInfo->lenghtOfBeat();
    auto _beatPerBar = _areaInfo->beatPerBar();
    auto _tempo = _areaInfo->tempo();

    if (_lastWidth <= 0)
        return;

    if (_currentDuration <= 0)
        return;

    painter->beginPath();
    painter->setFillStyle("#FFFFFF");
    painter->setStrokeStyle("#FFFFFF");
    painter->setLineWidth(1);
    auto font = QNanoFont(":/resources/font/SourceSansPro-Regular.ttf");
    font.setPixelSize(ScreenInterface::scaleSize2(28, _areaInfo->zoomFactor()));

    painter->setFont(font);
    painter->setLineCap(IPainter::CAP_ROUND);
    painter->setLineJoin(IPainter::JOIN_ROUND);
    double final_size = 0;

    double offsetPix = 0;

    int H = _rect.bottom();
    int B = _rect.bottom();
    int T = _rect.top();
    int min = ScreenInterface::scaleSize2(60, _areaInfo->zoomFactor());
    int max = ScreenInterface::scaleSize2(120, _areaInfo->zoomFactor());

    double currentDuration = _currentDuration;
    double lastWidth = _lastWidth;
    double startTime = _startTime;

    int lob = _lenghtOfBeat;
    int bpb = _beatPerBar;

    auto m = Measure::timeToMeasure(currentDuration, lob, bpb, _tempo);
    double beatDuration = m.getBeatDuration();
    double barDuration = m.getBarDuration();
    double sixDuration = m.getSixteenthDuration();

    int coeff_six = 1;
    int coeff_beat = 1;
    int coeff_bar = 1;
    double stepDuration = barDuration;
    final_size = (barDuration * 1000.0) / currentDuration * lastWidth;

    _zoomLevel = ZoomLevel_Bar;
    if (final_size < min) {
        while (final_size < min) {

            coeff_bar *= 2;
            _zoomLevel = ZoomLevel(_zoomLevel - 1);
            stepDuration = (barDuration * coeff_bar);
            final_size = (stepDuration * 1000.0) / currentDuration * lastWidth;
        }
    }
    if (final_size > max) {
        _zoomLevel = ZoomLevel_Beat;
        coeff_beat = 2;
        while (final_size > max && coeff_beat <= bpb) {

            if (bpb % coeff_beat == 0) {
                stepDuration = (barDuration / (coeff_beat));
                final_size = (stepDuration * 1000.0) / currentDuration * lastWidth;
            }
            coeff_beat++;
        }

        if (final_size > max) {

            coeff_six = 2;

            _zoomLevel = ZoomLevel_Beat2;
            while (final_size > max) {

                stepDuration = (beatDuration / coeff_six);
                _zoomLevel = ZoomLevel(_zoomLevel + 1);
                final_size = (stepDuration * 1000.0) / currentDuration * lastWidth;
                coeff_six *= 2;
            }
        }
        if (final_size < min) {
            coeff_bar = 1;

            _zoomLevel = ZoomLevel_Bar;
            stepDuration = (barDuration * coeff_bar);
            final_size = (stepDuration * 1000.0) / currentDuration * lastWidth;
        }
    }

    if (_zoomLevel >= ZoomLevel_Beat4096) {

        _zoomLevel = ZoomLevel_Beat4096;

        stepDuration = sixDuration / (2048.0 / (16 / lob));
        final_size = (stepDuration * 1000.0) / currentDuration * lastWidth;

        coeff_six = 2048 * 2;

        Q_EMIT sigZoomEnded(true);
    }

    double posX = 0;
    double offsetTime = 0;
    offsetTime = (qCeil((startTime) / (stepDuration * 1000.0))) * (stepDuration * 1000.0);
    offsetPix = (offsetTime - (startTime)) / currentDuration * lastWidth;

    posX = offsetPix;

    double currentTime = offsetTime;

    Q_EMIT sigZoomLevelChanged(_zoomLevel);

    painter->setFillStyle("#CFCFCF");
    painter->setStrokeStyle("#CFCFCF");
    while (posX < lastWidth) {

        if (qAbs(currentTime) < 0.00001)
            currentTime = 0;

        Measure m = Measure::timeToMeasure2(currentTime, lob, bpb, _tempo);
        QString time;

        if (_zoomLevel < ZoomLevel_Beat) {
            time = QString::number(m.bar);
            if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0 && (m.bar >= 0 ? (qAbs(m.bar) % (coeff_bar * 2) == 1) : (qAbs(m.bar) % (coeff_bar * 2) == 0))) {

                auto rect = painter->textBoundingBox(time, 0, 0);

                painter->drawText(posX + 2, H - rect.height() / 4, time);
                painter->drawLine(posX, B, posX, B - rect.height() * 2 / 3.0);

            } else {
                painter->drawLine(posX, B, posX, B - 2);
            }
        } else if (_zoomLevel == ZoomLevel_Beat) {
            time = QString::number(m.bar);
            if (m.beat == 1 && m.sixteenth == 1 && m.subSixteenth == 0) {
                auto rect = painter->textBoundingBox(time, 0, 0);

                painter->drawText(posX + 2, H - rect.height() / 4, time);
                painter->drawLine(posX, B, posX, B - rect.height() * 2 / 3.0);
            } else {
                painter->drawLine(posX, B, posX, B - 2);
            }
        } else if (_zoomLevel > ZoomLevel_Beat && _zoomLevel <= ZoomLevel_Beat4) {
            time = QString::number(m.bar) + "." + QString::number(m.beat);
            if (m.sixteenth == 1 && m.subSixteenth == 0) {
                auto rect = painter->textBoundingBox(time, 0, 0);
                painter->drawText(posX + 2, H - rect.height() / 4, time);
                painter->drawLine(posX, B, posX, B - rect.height() * 2 / 3.0);
            } else {

                painter->drawLine(posX, B, posX, B - 2);
            }
        } else {

            int diff = qPow(2, _zoomLevel - 19);
            int k = (4 * (16 / lob)) / diff;
            if (k >= 1) {
                int p = (16 / lob) / (diff / 4);
                if (p > 1) {

                    time = QString::number(m.bar) + "." + QString::number(m.beat)
                        + "." + QString::number(m.sixteenth);
                    auto rect = painter->textBoundingBox(time, 0, 0);
                    if (m.sixteenth % p == 1 && m.subSixteenth == 0) {

                        painter->drawText(posX + 2, H - rect.height() / 4, time);
                        painter->drawLine(posX, B, posX, B - rect.height() * 2 / 3.0);
                    } else {

                        painter->drawLine(posX, B, posX, B - 2);
                    }

                } else {
                    time = QString::number(m.bar) + "." + QString::number(m.beat) + "." + QString::number(m.sixteenth);
                    int k = (coeff_six / (16 / lob * 2 * 4));
                    int p = 2048 / k;
                    auto rect = painter->textBoundingBox(time, 0, 0);
                    if (m.subSixteenth == 0) {

                        painter->drawText(posX + 2, H - rect.height() / 4, time);
                        painter->drawLine(posX, B, posX, B - rect.height() * 2 / 3.0);
                    } else if (qAbs((qRound(m.subSixteenth / (double)p) * p) - m.subSixteenth) < 10) {
                        painter->drawLine(posX, B, posX, B - rect.height() * 2 / 3.0);
                    } else {

                        painter->drawLine(posX, B, posX, B - 2);
                    }
                }
            } else {
                time = QString::number(m.bar) + "." + QString::number(m.beat) + "." + QString::number(m.sixteenth);
                int k = ((coeff_six / 2) / (16 / lob)); // EACH SIXTEENTH
                double p = 2048.0 / (k / 4);

                // SIX = 8 parts
                //  SIX / 4 = 2
                //   p = 1024 / 2
                auto rect = painter->textBoundingBox(time, 0, 0);
                if (m.subSixteenth == 0) {

                    painter->drawText(posX + 2, H - rect.height() / 4, time);
                    painter->drawLine(posX, B, posX, B - rect.height() * 2 / 3.0);
                } else if (qAbs(m.subSixteenth / (double)p - qRound(m.subSixteenth / (double)p)) < 0.1) {
                    painter->drawLine(posX, B, posX, B - rect.height() * 2 / 3.0);
                } else {

                    painter->drawLine(posX, B, posX, B - 2);
                }
            }
        }

        posX += (final_size);
        double W = stepDuration * 1000.0;
        currentTime += (W);
    }

    painter->stroke();
}

double BeatBarItem::lastCursorPosY() const
{
    return _lastCursorPosY;
}

int BeatBarItem::hoverMoveEvent(QHoverEvent* event)
{
    if (_rect.contains(event->pos())) {
        return 1;
    }
    return 0;
}

double BeatBarItem::lastCursorPosX() const
{
    return _lastCursorPosX;
}

int BeatBarItem::mousePressEvent(QMouseEvent* event)
{

    _containMouse = false;

    if (!_rect.contains(event->pos()))
        return 0;

    _containMouse = true;

    auto _minStartTime = _areaInfo->minStartTime();
    auto _endTime = _areaInfo->endTime();
    auto _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    Q_EMIT sigPlayMoveModeChanged(PlayMovingMode_Indicator);

    int posX = event->pos().x();

    double t0 = _startTime + qAbs(_minStartTime);
    double t1 = _endTime + qAbs(_minStartTime);

    double posTime = posX / (double)_lastWidth * (t1 - t0) + t0;

    Q_EMIT sigLastBeatBarPressX(posTime);

    _currentPressPosTime = posTime;

    return 1;
}

int BeatBarItem::mouseMoveEvent(QMouseEvent* event)
{

    if (_containMouse == false)
        return 0;

    auto _currentDuration = _areaInfo->clipDuration();
    double _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    auto _lenghtOfBeat = _areaInfo->lenghtOfBeat();
    auto _beatPerBar = _areaInfo->beatPerBar();
    auto _endTime = _startTime + _currentDuration;
    auto _tempo = _areaInfo->tempo();
    auto _maxEndTime = _areaInfo->maxEndTime();
    auto _minStartTime = _areaInfo->minStartTime();

    if (_mouseMove == false) {
        _mouseMove = true;

        _lastCursorPosX = event->pos().x();
        _lastCursorPosY = event->pos().y();

        _startPositionX = event->pos().x();
        _startPositionY = event->pos().y();

        _firstPositionX = event->pos().x();

        _startPositionTime = _startPositionX * (_currentDuration / _lastWidth) + _startTime;

        _zoomCounter = 1;
        _mouseReleased = false;
        QGuiApplication::setOverrideCursor(Qt::BlankCursor);
    } else {
        _zoomCounter++;
    }

    int lob = _lenghtOfBeat;
    int bpb = _beatPerBar;
    double currentEnd = _endTime;
    double currentStart = _startTime;
    double clipDuration = _currentDuration;
    //    double startPixel = _startPixel;
    //    double endPixel = _endPixel;

    auto m = Measure::timeToMeasure(clipDuration, lob, bpb, _tempo);

    _currentPosistionY = (event->pos().y());
    _currentPosistionX = (event->pos().x());

    int deltaY = _lastCursorPosY - _currentPosistionY;
    int deltaX = _lastCursorPosX - _currentPosistionX;

    // qDebug() << "_lastCursorPosX" << _lastCursorPosX << _lastCursorPosY;
    Q_EMIT sigCursorPosition(QPointF(_lastCursorPosX, _lastCursorPosY).toPoint());

    double timePerPixel = _startPositionX / _lastWidth;

    double minDuration = 10;
    int maxSize = ScreenInterface::instance()->freeResolution(40);

    double stepDuration = m.getSixteenthDuration() / 1024.0;

    minDuration = (stepDuration * 1000.0) / (maxSize / _lastWidth);

    double zoomValueUp = 0;
    double zoomValueDown = 0;

    zoomValueUp = -deltaY / _lastWidth * clipDuration * 10.0;
    zoomValueDown = (-deltaY / _lastWidth * clipDuration) * 10.0;

    bool isZoom = false;
    bool isPan = false;

    if (qAbs(deltaX) > qAbs(deltaY)) {
        isPan = true;
        _startPositionX -= deltaX;
        if (_startPositionX < 0)
            _startPositionX = 0;

        if (_startPositionX > _lastWidth)
            _startPositionX = _lastWidth;

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
            if (_zoomEnded) {

                _zoomEnded = false;
                Q_EMIT sigZoomEnded(false);
            }

            if (currentEnd + value * (1 - timePerPixel) > _maxEndTime) {
                currentStart -= value;
                currentEnd = _maxEndTime;
            } else if (currentStart - value * timePerPixel < _minStartTime) {
                currentStart = _minStartTime;
                currentEnd += value;
            } else {
                currentStart -= value * timePerPixel;
                currentEnd += value * (1 - timePerPixel);
            }

            if (currentEnd > _maxEndTime) {
                currentEnd = _maxEndTime;
            }
            if (currentStart < _minStartTime) {
                currentStart = _minStartTime;
            }

            clipDuration = currentEnd - currentStart;
        }

        //_sampleEditor->setSamplePerPixel(clipDuration / (double)_lastWidth);
        Q_EMIT sigSamplePerPixelChanged(clipDuration / (double)_lastWidth);
        auto _samplePerPixel = clipDuration / (double)_lastWidth;
    }

    if (qAbs(deltaX) >= 1 && isPan) {

        double panValue = 0.001 * clipDuration * qAbs(deltaX);

        if (deltaX > 0) {

            if (currentEnd + panValue <= _maxEndTime) {
                currentStart = currentStart + panValue;
                currentEnd = currentEnd + panValue;
            } else {
                currentEnd = _maxEndTime;
                currentStart = currentEnd - clipDuration;
            }

        } else if (deltaX < 0) {

            if (currentStart - panValue >= _minStartTime) {
                currentStart = currentStart - panValue;
                currentEnd = currentEnd - panValue;
            } else {
                currentStart = _minStartTime;
                currentEnd = currentStart + clipDuration;
            }
        }

        Q_EMIT sigGreenWindowZoomStop(false);
    }

    if ((qAbs(deltaX) > 0 || qAbs(deltaY) > 0) && (isPan == true || isZoom == true)) {

        _startTime = currentStart;
        _endTime = currentEnd;
        _currentDuration = clipDuration;

        _areaInfo->setClipProperties(currentStart, currentEnd, clipDuration);

        //_sampleEditor->setLastWidth(width);

        double t0 = _startTime + qAbs(_minStartTime);
        double t1 = (_endTime) + qAbs(_minStartTime);

        double posTime = _lastCursorPosX / _lastWidth * (t1 - t0) + t0;

        Q_EMIT sigLastBeatBarPressX(posTime);
    }

    return 1;
}

int BeatBarItem::mouseReleaseEvent(QMouseEvent* event)
{

    _containMouse = false;
    auto _currentDuration = _areaInfo->clipDuration();
    auto _startTime = _areaInfo->startTime();

    Q_UNUSED(event)
    if (_mouseMove) {
        auto s_pixel = (_startPositionTime - _startTime) * width / _currentDuration;

        if (s_pixel < 0)
            s_pixel = 0;

        if (s_pixel > width)
            s_pixel = width;

        Q_EMIT sigCursorPosition(QPointF(_firstPositionX, _startPositionY).toPoint());
    }
    _mouseReleased = true;
    _mouseMove = false;
    QGuiApplication::restoreOverrideCursor();

    return 1;
}
