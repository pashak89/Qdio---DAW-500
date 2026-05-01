//#include "clipviewselector.h"
//#include "core/sampleeditor.h"
//#include "core/screeninterface.h"
//#include <QCursor>

//ClipViewSelectorItem::ClipViewSelectorItem(QQuickItem* parent)
//    : INanoItem(parent)
//{
//    setMouseHoverEventsEnabled(true);
//    setAcceptedMouseButtons(Qt::AllButtons);
//    setAcceptHoverEvents(true);
//    setMouseEventsEnabled(true);
//    setMouseHoverEventsEnabled(true);

//    _sampleEditor = SampleEditor::inst();
//    connect(_sampleEditor->sampleClip(), &SampleClip::sampleChanged, this, &ClipViewSelectorItem::update);

//    //    _timerWidth.setInterval(40);
//    //    _timerWidth.start();
//    //    connect(&_timerWidth, &QTimer::timeout, [this]() {
//    //        if (_lastWidth != width()) {
//    //            Q_EMIT sigWidthChanged();
//    //        }
//    //        _lastWidth = width();
//    //    });

//    //    connect(this, &ClipViewSelectorItem::sigWidthChanged, this, [this]() {
//    //        _beatBar->setUpdate(true);
//    //        _beatBar->setLastWidth(width());
//    //        _beatBar->update(width());
//    //        update();
//    //    });
//}

//void ClipViewSelectorItem::update()
//{
//    INanoItem::update();
//}

//QNanoQuickItemPainter* ClipViewSelectorItem::createItemPainter() const
//{
//    return new ClipViewSelectorPainterItem(_sampleEditor);
//}

//void ClipViewSelectorItem::mousePressEvent(QMouseEvent* event)
//{

//    double currentClipDuration = _sampleEditor->sampleClip()->currentClipDuration();
//    double totalDuration = _sampleEditor->maxEndTime() - _sampleEditor->minStartTime();

//    int posX = event->pos().x();
//    double posTime = posX / (double)width() * totalDuration + _sampleEditor->minStartTime();

//    double startPix = _sampleEditor->lastHostSpotTimeStart();
//    double endPix = _sampleEditor->lastHotSpotTimeEnd();

//    double t0 = _sampleEditor->sampleClip()->startTime() + qAbs(_sampleEditor->minStartTime());
//    double t1 = (_sampleEditor->sampleClip()->endTime()) + qAbs(_sampleEditor->minStartTime());
//    double posTime2 = (posX - startPix) / (endPix - startPix) * (t1 - t0) + t0;

//    int maxSize = ScreenInterface::instance()->freeResolution(40);

//    double _durationWidth = endPix - startPix;

//    if (qAbs(startPix - posX) < 4) {
//        _resizeStart = true;
//    } else if (qAbs(endPix - posX) < 4) {
//        _resizeEnd = true;
//    }

//    if (_resizeStart) {
//        QGuiApplication::setOverrideCursor((QCursor(Qt::SplitHCursor)));
//    } else if (_resizeEnd) {
//        QGuiApplication::setOverrideCursor((QCursor(Qt::SplitHCursor)));
//    } else {

//        _mousePressRatio = (posX - startPix) / (endPix - startPix);

//        if (posX > startPix && posX < endPix) {
//            if (endPix - startPix > maxSize) {
//                _sampleEditor->setLastHotSpotPressTime(_mousePressRatio);
//            }
//            _sampleEditor->setLastBeatBarPressX(posTime2);

//        } else {
//            _sampleEditor->setGreenWindowZoomStop(false);
//            _sampleEditor->setLastBeatBarPressX(posTime);
//            _sampleEditor->setLastHotSpotPressTime(0.5);
//            _mousePressRatio = 0.5;
//        }

//        QGuiApplication::setOverrideCursor(Qt::BlankCursor);

//        if (posX > startPix && posX < endPix) {
//        } else {
//            if (posX - _durationWidth / 2 < 0) {

//                _sampleEditor->sampleClip()->setStartTime(_sampleEditor->minStartTime());
//                _sampleEditor->sampleClip()->setEndTime(_sampleEditor->minStartTime() + currentClipDuration);

//                Q_EMIT sigUpdate();
//            } else if (posX + _durationWidth / 2 > width()) {

//                _sampleEditor->sampleClip()->setStartTime(_sampleEditor->maxEndTime() - currentClipDuration);
//                _sampleEditor->sampleClip()->setEndTime(_sampleEditor->maxEndTime());

//                Q_EMIT sigUpdate();
//            } else if (posX < startPix || posX > endPix) {

//                _sampleEditor->sampleClip()->setStartTime(posTime - currentClipDuration / 2);
//                _sampleEditor->sampleClip()->setEndTime(posTime + currentClipDuration / 2);

//                Q_EMIT sigUpdate();
//            }
//        }
//    }
//}

//void ClipViewSelectorItem::mouseMoveEvent(QMouseEvent* event)
//{

//    if (_mouseMove == false) {
//        _mouseMove = true;

//        _lastCursorPosX = event->pos().x();
//        _lastCursorPosY = event->pos().y();

//        _startPositionX = event->pos().x();
//        _startPositionY = event->pos().y();

//        _mouseReleased = false;
//    }

//    double currentEnd = _sampleEditor->sampleClip()->endTime();
//    double currentStart = _sampleEditor->sampleClip()->startTime();
//    double timeDuration = _sampleEditor->timeDuration();
//    double lastWidth = _sampleEditor->lastWidth();
//    double fileDuration = _sampleEditor->sampleClip()->currentFileDuration();
//    double startPixel = _sampleEditor->startPixel();
//    double endPixel = _sampleEditor->endPixel();
//    double minStartTime = _sampleEditor->minStartTime();
//    double maxEndTime = _sampleEditor->maxEndTime();

//    int lob = _sampleEditor->getLenghtOfBeat();
//    int bpb = _sampleEditor->getBeatPerBar();

//    auto m = Measure::timeToMeasure(timeDuration, lob, bpb, _sampleEditor->tempo());

//    _currentPosistionY = (event->pos().y());
//    _currentPosistionX = (event->pos().x());

//    int deltaY = _lastCursorPosY - _currentPosistionY;
//    int deltaX = _lastCursorPosX - _currentPosistionX;

//    if (_resizeStart == false && _resizeEnd == false) {
//        QCursor::setPos(mapToGlobal(QPointF(_lastCursorPosX, _lastCursorPosY).toPoint()).toPoint());
//    }

//    double timePerPixel = _sampleEditor->lastHotSpotPressTime();

//    double minDuration = 10;
//    int maxSize = ScreenInterface::instance()->freeResolution(40);

//    double stepDuration = m.getSixteenthDuration() / 1024.0;

//    minDuration = (stepDuration * 1000.0) / (maxSize / width());

//    double currentClipDuration = _sampleEditor->sampleClip()->currentClipDuration();

//    double zoomValueUp = 0;
//    double zoomValueDown = 0;

//    zoomValueUp = -deltaY / width() * _sampleEditor->sampleClip()->currentClipDuration() * 1.0;
//    zoomValueDown = -deltaY / width() * _sampleEditor->sampleClip()->currentClipDuration() * 1.0;

//    bool isZoom = false;
//    bool isPan = false;

//    if (qAbs(deltaX) > qAbs(deltaY)) {
//        isPan = true;

//    } else if (qAbs(deltaY) > qAbs(deltaX)) {
//        isZoom = true;
//    }

//    if (qAbs(deltaY) >= 1 && isZoom) {

//        if (deltaY < 0) {
//            double value = qAbs(zoomValueDown);

//            if ((currentEnd - (value * (1 - timePerPixel))) - (currentStart + (value * timePerPixel)) >= minDuration) {

//                currentStart += value * timePerPixel;
//                currentEnd -= value * (1 - timePerPixel);
//                currentClipDuration = currentEnd - currentStart;

//            } else {

//                currentStart += (currentClipDuration - minDuration) * timePerPixel;
//                currentClipDuration = minDuration;
//                currentEnd = currentStart + currentClipDuration;
//            }

//        } else if (deltaY > 0) {
//            double value = qAbs(zoomValueUp);
//            if (_sampleEditor->getZoomEndded()) {
//                _sampleEditor->setZoomEndded(false);
//            }

//            if (currentEnd + value * (1 - timePerPixel) > maxEndTime) {
//                currentStart -= value;
//                currentEnd = maxEndTime;
//            } else if (currentStart - value * timePerPixel < minStartTime) {
//                currentStart = minStartTime;
//                currentEnd += value;
//            } else {
//                currentStart -= value * timePerPixel;
//                currentEnd += value * (1 - timePerPixel);
//            }

//            if (currentEnd > maxEndTime) {
//                currentEnd = maxEndTime;
//            }
//            if (currentStart < minStartTime) {
//                currentStart = minStartTime;
//            }

//            currentClipDuration = currentEnd - currentStart;
//        }

//        _sampleEditor->setSamplePerPixel(currentClipDuration / lastWidth);

//        startPixel = -currentStart / _sampleEditor->samplePerPixel();
//        endPixel = lastWidth + (timeDuration - currentEnd) / _sampleEditor->samplePerPixel();

//        if (startPixel > 0)
//            startPixel = 0;

//        if (endPixel < width())
//            endPixel = width();
//    }
//    if (_resizeStart) {
//        if (qAbs(deltaX) > 2 && isPan == true) {

//            if (_currentPosistionX < 0) {
//                _currentPosistionX = 0;
//            }
//            if (_currentPosistionX > width()) {
//                _currentPosistionX = width();
//            }

//            double duration = _sampleEditor->maxEndTime() - _sampleEditor->minStartTime();
//            double t1 = (_sampleEditor->sampleClip()->endTime()) + qAbs(_sampleEditor->minStartTime());
//            double startPix = _currentPosistionX;
//            double endPix = (t1) / (double)duration * width();
//            double t0 = startPix / width() * duration + _sampleEditor->minStartTime();
//            // _beatBar->setLastHotSpotPressTime(-1);

//            if (endPix - startPix > maxSize) {
//                _sampleEditor->sampleClip()->setStartTime(t0);
//                _sampleEditor->sampleClip()->setCurrentClipDuration(_sampleEditor->sampleClip()->endTime() - _sampleEditor->sampleClip()->startTime());
//            } else {
//                startPix = endPix - maxSize;
//                t0 = startPix / width() * duration + _sampleEditor->minStartTime();
//                _sampleEditor->sampleClip()->setStartTime(t0);
//                _sampleEditor->sampleClip()->setCurrentClipDuration(_sampleEditor->sampleClip()->endTime() - _sampleEditor->sampleClip()->startTime());
//            }

//            currentStart = _sampleEditor->sampleClip()->startTime();
//            currentClipDuration = _sampleEditor->sampleClip()->currentClipDuration();
//            currentEnd = _sampleEditor->sampleClip()->endTime();
//            lastWidth = width();

//            QList<Warp*> list = _sampleEditor->warpMarkers();
//            for (int j = 0; j < list.size(); j++) {

//                double p0 = ((list[j]->t0() - currentStart) / currentClipDuration * lastWidth);
//                double p1 = ((list[j]->t1() - currentStart) / currentClipDuration * lastWidth);
//                list[j]->setWidth(p1 - p0);
//                list[j]->setPps((list[j]->bsase_t1() - list[j]->base_t0()) / (p1 - p0));
//            }

//            QList<int> L = _sampleEditor->findLowerWarp();
//            QList<int> H = _sampleEditor->findHigherWarp();

//            int f = _sampleEditor->findFirstWarp();
//            int l = _sampleEditor->findLastWarp();
//            if (f >= 0 && l >= 0) {
//                double start = _sampleEditor->sampleClip()->baseStartTime();
//                double end = _sampleEditor->sampleClip()->baseEndTime();

//                double size = 0;
//                double du = 0;
//                for (int i = 0; i < L.size(); i++) {
//                    int k = L[i];
//                    du += (list[k]->bsase_t1() - list[k]->base_t0());
//                }

//                double p0 = ((list[f]->t0() - currentStart) / currentClipDuration * lastWidth);

//                start = du + qAbs(p0) * list[f]->pps();

//                size = 0;
//                du = 0;
//                for (int i = 0; i < H.size(); i++) {
//                    int k = H[i];
//                    du += (list[k]->bsase_t1() - list[k]->base_t0());
//                }

//                double p1 = (list[l]->t1() - currentEnd) / currentClipDuration * lastWidth;

//                end = fileDuration - du - qAbs(p1) * list[l]->pps();

//                _sampleEditor->sampleClip()->setBaseStartTime(start);
//                _sampleEditor->sampleClip()->setBaseEndTime(end);

//                if (list[f]->t0() >= currentStart)
//                    _sampleEditor->sampleClip()->setBaseStartTime(0);
//                else {
//                    _sampleEditor->sampleClip()->setBaseStartTime(start);
//                }

//                if (list[l]->t1() <= currentEnd)
//                    _sampleEditor->sampleClip()->setBaseEndTime(fileDuration);
//                else {
//                    _sampleEditor->sampleClip()->setBaseEndTime(end);
//                }
//            }
//            Q_EMIT sigUpdate();
//        }
//    } else if (_resizeEnd) {
//        if (qAbs(deltaX) > 2 && isPan == true) {

//            if (_currentPosistionX < 0) {
//                _currentPosistionX = 0;
//            }
//            if (_currentPosistionX > width()) {
//                _currentPosistionX = width();
//            }

//            double duration = _sampleEditor->maxEndTime() - _sampleEditor->minStartTime();
//            double t0 = _sampleEditor->sampleClip()->startTime() + qAbs(_sampleEditor->minStartTime());
//            double startPix = (t0) / (double)duration * width();
//            double endPix = _currentPosistionX;
//            double t1 = endPix / width() * duration + _sampleEditor->minStartTime();

//            //_beatBar->setLastHotSpotPressTime(-1);

//            if (endPix - startPix > maxSize) {
//                _sampleEditor->sampleClip()->setEndTime(t1);
//                _sampleEditor->sampleClip()->setCurrentClipDuration(_sampleEditor->maxEndTime() - _sampleEditor->sampleClip()->startTime());
//            } else {

//                endPix = startPix + maxSize;
//                t1 = endPix / width() * duration + _sampleEditor->minStartTime();
//                _sampleEditor->sampleClip()->setEndTime(t1);
//                _sampleEditor->sampleClip()->setCurrentClipDuration(_sampleEditor->sampleClip()->endTime() - _sampleEditor->sampleClip()->startTime());
//            }

//            currentStart = _sampleEditor->sampleClip()->startTime();
//            currentClipDuration = _sampleEditor->sampleClip()->currentClipDuration();
//            currentEnd = _sampleEditor->sampleClip()->endTime();
//            lastWidth = width();

//            QList<Warp*> list = _sampleEditor->warpMarkers();
//            for (int j = 0; j < list.size(); j++) {

//                double p0 = ((list[j]->t0() - currentStart) / currentClipDuration * lastWidth);
//                double p1 = ((list[j]->t1() - currentStart) / currentClipDuration * lastWidth);
//                list[j]->setWidth(p1 - p0);
//                list[j]->setPps((list[j]->bsase_t1() - list[j]->base_t0()) / (p1 - p0));
//            }

//            QList<int> L = _sampleEditor->findLowerWarp();
//            QList<int> H = _sampleEditor->findHigherWarp();

//            int f = _sampleEditor->findFirstWarp();
//            int l = _sampleEditor->findLastWarp();
//            if (f >= 0 && l >= 0) {
//                double start = _sampleEditor->sampleClip()->baseStartTime();
//                double end = _sampleEditor->sampleClip()->baseEndTime();

//                double size = 0;
//                double du = 0;
//                for (int i = 0; i < L.size(); i++) {
//                    int k = L[i];
//                    du += (list[k]->bsase_t1() - list[k]->base_t0());
//                }

//                double p0 = ((list[f]->t0() - currentStart) / currentClipDuration * lastWidth);

//                start = du + qAbs(p0) * list[f]->pps();

//                size = 0;
//                du = 0;
//                for (int i = 0; i < H.size(); i++) {
//                    int k = H[i];
//                    du += (list[k]->bsase_t1() - list[k]->base_t0());
//                }

//                double p1 = (list[l]->t1() - currentEnd) / currentClipDuration * lastWidth;

//                end = fileDuration - du - qAbs(p1) * list[l]->pps();

//                _sampleEditor->sampleClip()->setBaseStartTime(start);
//                _sampleEditor->sampleClip()->setBaseEndTime(end);

//                if (list[f]->t0() >= currentStart)
//                    _sampleEditor->sampleClip()->setBaseStartTime(0);
//                else {
//                    _sampleEditor->sampleClip()->setBaseStartTime(start);
//                }

//                if (list[l]->t1() <= currentEnd)
//                    _sampleEditor->sampleClip()->setBaseEndTime(fileDuration);
//                else {
//                    _sampleEditor->sampleClip()->setBaseEndTime(end);
//                }
//            }

//            Q_EMIT sigUpdate();
//        }
//    } else {
//        if (qAbs(deltaX) >= 1 && isPan) {

//            double panValue = 0.001 * currentClipDuration * qAbs(deltaX);

//            if (deltaX < 0) {

//                if (currentEnd + panValue <= maxEndTime) {
//                    currentStart = currentStart + panValue;
//                    currentEnd = currentEnd + panValue;
//                } else {
//                    currentEnd = maxEndTime;
//                    currentStart = currentEnd - currentClipDuration;
//                }

//            } else if (deltaX > 0) {

//                if (currentStart - panValue >= minStartTime) {
//                    currentStart = currentStart - panValue;
//                    currentEnd = currentEnd - panValue;
//                } else {
//                    currentStart = minStartTime;
//                    currentEnd = currentStart + currentClipDuration;
//                }
//            }

//            _sampleEditor->setGreenWindowZoomStop(false);
//        }

//        if ((qAbs(deltaX) > 0 || qAbs(deltaY) > 0) && (isPan == true || isZoom == true)) {

//            _sampleEditor->sampleClip()->setStartTime(currentStart);
//            _sampleEditor->sampleClip()->setEndTime(currentEnd);
//            _sampleEditor->sampleClip()->setCurrentClipDuration(currentClipDuration);

//            double r = _sampleEditor->lastHotSpotPressTime();
//            double posTime = (currentEnd - currentStart) * r + currentStart;
//            _sampleEditor->setLastBeatBarPressX(posTime);

//            _sampleEditor->setStartPixel(startPixel);
//            _sampleEditor->setEndPixel(endPixel);

//            currentStart = _sampleEditor->sampleClip()->startTime();
//            currentClipDuration = _sampleEditor->sampleClip()->currentClipDuration();
//            currentEnd = _sampleEditor->sampleClip()->endTime();
//            lastWidth = width();

//            QList<Warp*> list = _sampleEditor->warpMarkers();
//            for (int j = 0; j < list.size(); j++) {

//                double p0 = ((list[j]->t0() - currentStart) / currentClipDuration * lastWidth);
//                double p1 = ((list[j]->t1() - currentStart) / currentClipDuration * lastWidth);
//                list[j]->setWidth(p1 - p0);
//                list[j]->setPps((list[j]->bsase_t1() - list[j]->base_t0()) / (p1 - p0));
//            }

//            QList<int> L = _sampleEditor->findLowerWarp();
//            QList<int> H = _sampleEditor->findHigherWarp();

//            int f = _sampleEditor->findFirstWarp();
//            int l = _sampleEditor->findLastWarp();
//            if (f >= 0 && l >= 0) {
//                double start = _sampleEditor->sampleClip()->baseStartTime();
//                double end = _sampleEditor->sampleClip()->baseEndTime();

//                double size = 0;
//                double du = 0;
//                for (int i = 0; i < L.size(); i++) {
//                    int k = L[i];
//                    du += (list[k]->bsase_t1() - list[k]->base_t0());
//                }

//                double p0 = ((list[f]->t0() - currentStart) / currentClipDuration * lastWidth);

//                start = du + qAbs(p0) * list[f]->pps();

//                size = 0;
//                du = 0;
//                for (int i = 0; i < H.size(); i++) {
//                    int k = H[i];
//                    du += (list[k]->bsase_t1() - list[k]->base_t0());
//                }

//                double p1 = (list[l]->t1() - currentEnd) / currentClipDuration * lastWidth;

//                end = fileDuration - du - qAbs(p1) * list[l]->pps();

//                _sampleEditor->sampleClip()->setBaseStartTime(start);
//                _sampleEditor->sampleClip()->setBaseEndTime(end);

//                if (list[f]->t0() >= currentStart)
//                    _sampleEditor->sampleClip()->setBaseStartTime(0);
//                else {
//                    _sampleEditor->sampleClip()->setBaseStartTime(start);
//                }

//                if (list[l]->t1() <= currentEnd)
//                    _sampleEditor->sampleClip()->setBaseEndTime(fileDuration);
//                else {
//                    _sampleEditor->sampleClip()->setBaseEndTime(end);
//                }
//            }
//        }
//    }

//    Q_EMIT sigUpdate();
//}
//void ClipViewSelectorItem::mouseReleaseEvent(QMouseEvent* event)
//{
//    Q_UNUSED(event)
//    if (_mouseMove && (_resizeEnd == false && _resizeStart == false)) {

//        double _startPix = _sampleEditor->lastHostSpotTimeStart();
//        double _endPix = _sampleEditor->lastHotSpotTimeEnd();

//        double posX = _mousePressRatio * (_endPix - _startPix) + _startPix;

//        QCursor::setPos(mapToGlobal(QPointF(posX, _startPositionY)).toPoint());
//    }

//    //_beatBar->setLastHotSpotPressTime(0);
//    _mouseReleased = true;
//    _mouseMove = false;
//    _resizeEnd = false;
//    _resizeStart = false;
//    QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
//}

//void ClipViewSelectorItem::hoverMoveEvent(QHoverEvent* event)
//{

//    double posX = event->posF().x();

//    double _startPix = _sampleEditor->lastHostSpotTimeStart();
//    double _endPix = _sampleEditor->lastHotSpotTimeEnd();

//    if (qAbs(_startPix - posX) < 4) {
//        QGuiApplication::setOverrideCursor(QCursor(Qt::SplitHCursor));
//    } else if (qAbs(_endPix - posX) < 4) {
//        QGuiApplication::setOverrideCursor(QCursor(Qt::SplitHCursor));
//    } else {
//        QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
//    }
//}

//void ClipViewSelectorItem::sltSampleChanged()
//{
//    update();
//}

//void ClipViewSelectorItem::sltTimerUpdated()
//{
//}

//ClipViewSelectorPainterItem::ClipViewSelectorPainterItem(SampleEditor* sampleEditor)
//    : INanoPainterItem()
//    , _sampleEditor(sampleEditor)
//{
//    int channelCount = _sampleEditor->sampleClip()->sampleBuffer()->channels();
//}

//void ClipViewSelectorPainterItem::draw(IPainter* painter)
//{
//    painter->drawRect(0, 0, width() - 2, height() - 2);
//    painter->stroke();
//    painter->beginPath();
//    if (_sampleEditor) {

//        int channelCount = _sampleEditor->sampleClip()->sampleBuffer()->channels();
//        double lastWidth = width();
//        double duration = _sampleEditor->maxEndTime() - _sampleEditor->minStartTime();

//        if (_sampleEditor->timeDuration() > 0 && channelCount > 0) {
//            painter->setStrokeStyle(QNanoColor("#000000"));
//            qint64 startFrame = 0;
//            qint64 endFrame = _sampleEditor->sampleClip()->sampleBuffer()->frames();

//            int Y = 0;
//            int H = height() / channelCount;

//            if (startFrame < 0 || endFrame < 0)
//                return;

//            painter->setLineWidth(1.5);
//            painter->setGlobalAlpha(1);
//            painter->setLineCap(IPainter::CAP_ROUND);
//            painter->setLineJoin(IPainter::JOIN_ROUND);

//            for (int i = 0; i < 2; i++) {
//                Y = i * H;
//                QList<Warp*> list = _sampleEditor->warpMarkers();

//                for (int j = 0; j < list.size(); j++) {

//                    double start, end;
//                    double pix1, pix2;
//                    double t0 = (list[j]->t0()) + qAbs(_sampleEditor->minStartTime());
//                    double t1 = (list[j]->t1()) + qAbs(_sampleEditor->minStartTime());

//                    pix1 = (t0) / duration * lastWidth;
//                    pix2 = (t1) / duration * lastWidth;

//                    start = list[j]->base_t0();
//                    end = list[j]->bsase_t1();

//                    if (pix1 < 0) {
//                        pix1 = 0;
//                    }

//                    if (pix2 > lastWidth) {
//                        pix2 = lastWidth;
//                    }

//                    QRectF rect(pix1, Y, pix2 - pix1, H);

//                    QVector<float> arr_min;
//                    QVector<float> arr_max;

//                    arr_min.resize(pix2 - pix1);
//                    arr_max.resize(pix2 - pix1);

//                    if (arr_max.size() == 0) {
//                        continue;
//                    }

//                    _sampleEditor->sampleClip()->sampleBuffer()->get(rect, lastWidth, i,
//                        start, end, arr_max.data(), arr_min.data());

//                    _sampleEditor->sampleClip()->sampleBuffer()->visualize(
//                        *painter, rect.toRect(),
//                        start, end,
//                        arr_max.data(), arr_min.data(), arr_min.size());
//                }
//            }

//            painter->stroke();
//        }

//        int maxSize = ScreenInterface::instance()->freeResolution(40);

//        double t0 = _sampleEditor->sampleClip()->startTime() + qAbs(_sampleEditor->minStartTime());
//        double t1 = (_sampleEditor->sampleClip()->endTime()) + qAbs(_sampleEditor->minStartTime());
//        double startPix = (t0) / (double)duration * width();
//        double endPix = (t1) / (double)duration * width();

//        if (endPix - startPix < maxSize) {

//            if (_sampleEditor->greenWindowZoomStop() == false) {
//                _sampleEditor->setGreenWindowZoomStop(true);
//                double center = _sampleEditor->lastBeatBarPressX() + qAbs(_sampleEditor->minStartTime());
//                double x = center / duration * width();
//                double rr = _sampleEditor->lastHotSpotPressTime();
//                if (x - maxSize * rr > 0 && x + maxSize * (1 - rr) < width()) {
//                    _startPix = x - maxSize * rr;
//                    _endPix = x + maxSize * (1 - rr);
//                }

//                if (x - maxSize * rr < 0) {
//                    _startPix = 0;
//                    _endPix = _startPix + maxSize;
//                }

//                if (x + maxSize * (1 - rr) > width()) {
//                    _endPix = width();
//                    _startPix = _endPix - maxSize;
//                }
//            }

//            if (startPix < _startPix) {
//                _startPix = startPix;
//                _endPix = startPix + maxSize;
//            }
//            if (endPix > _endPix) {
//                _endPix = endPix;
//                _startPix = endPix - maxSize;
//            }

//        } else {
//            _sampleEditor->setGreenWindowZoomStop(false);
//            _endPix = endPix;
//            _startPix = startPix;
//        }

//        _sampleEditor->setLastHotSpotTimeEnd(_endPix);
//        _sampleEditor->setLastHostSpotTimeStart(_startPix);

//        painter->beginPath();
//        painter->setLineWidth(3);
//        painter->setStrokeStyle(QNanoColor("#008000"));
//        painter->drawLine(_startPix, 2, _startPix, height() - 2);
//        painter->drawLine(_endPix, 2, _endPix, height() - 2);
//        painter->stroke();
//    }

//    painter->beginPath();
//    painter->setStrokeStyle(QNanoColor("#EAEAEA"));

//    double endOffset = _sampleEditor->sampleClip()->startTime() + _sampleEditor->sampleClip()->currentClipDuration();
//    double totalDuration = _sampleEditor->timeDuration();
//    double startOffset = _sampleEditor->sampleClip()->startTime();

//    if (_sampleEditor->playMovingMode() == SampleEditor::PlayMovingMode_Indicator) {
//        double currentIndicator = _sampleEditor->playbackMarker();
//        int currentPix = (currentIndicator) / (double)totalDuration * width();
//        if (currentIndicator <= endOffset && currentIndicator >= startOffset) {
//            painter->setFillStyle("#eaeaea");
//            painter->setStrokeStyle("#eaeaea");
//            painter->moveTo(currentPix, 0);
//            painter->lineTo(currentPix, height());
//        }
//    }
//    if (_sampleEditor->playMovingMode() == SampleEditor::PlayMovingMode_Background) {
//        double currentIndicator = _sampleEditor->sampleClip()->currentClipDuration() / 2 + _sampleEditor->sampleClip()->startTime();
//        double currentPix = (currentIndicator) / (double)totalDuration * width();
//        if (currentIndicator <= endOffset && currentIndicator >= startOffset) {
//            painter->setFillStyle("#eaeaea");
//            painter->setStrokeStyle("#eaeaea");
//            painter->moveTo(currentPix, 0);
//            painter->lineTo(currentPix, height());
//        }
//    }
//    painter->stroke();
//}
