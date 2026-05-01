#include "warpareaitem.h"
#include "core/warp.h"
#include <QCursor>
#include <core/sampleeditor.h>
WarpAreaItem::WarpAreaItem(SampleEditor* sampleEditor, AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
    , _sampleEditor(sampleEditor)
{
    _timerCursorPosition.setInterval(40);
    connect(&_timerCursorPosition, &QTimer::timeout, this, &WarpAreaItem::sltTimerCheckPosition);
}

void WarpAreaItem::draw(IPainter* painter)
{

    if (_sampleEditor->sampleClip() == Q_NULLPTR)
        return;

    int W = _rect.width();
    int B = _rect.bottom();
    int H = _rect.height();
    int X = _rect.x();
    int Y = _rect.y();
    int T = _rect.top();

    painter->beginPath();
    painter->setFillStyle("#808080");
    painter->setStrokeStyle("#808080");
    painter->setLineWidth(1);

    double startTime = _sampleEditor->areaInfo()->startTime();
    double clipDuration = _sampleEditor->areaInfo()->clipDuration();
    double lastWidth = _sampleEditor->areaInfo()->lastWidth();

    painter->beginPath();
    painter->setFillStyle("#202020");
    painter->setStrokeStyle("#202020");

    painter->fillRect(X, Y, W, H);

    QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();
    for (int i = 0; i < list.size(); i++) {

        int markerPix1 = qRound((list[i]->t0() - startTime) / clipDuration * lastWidth);

        if (markerPix1 < 0)
            continue;

        painter->beginPath();
        painter->setFillStyle("#808080");
        painter->setStrokeStyle("#808080");
        painter->drawTriangle(qCeil(markerPix1), B, 10);
        painter->fill();

        painter->beginPath();
        painter->setFillStyle("#FF0000");
        painter->setStrokeStyle("#FF0000");
        painter->drawRectTop(qCeil(markerPix1), B, Y, 10);
        painter->fill();
    }

    if (list.size() > 0) {
        int markerPix1 = (list[list.size() - 1]->t1() - startTime) / clipDuration * lastWidth;

        if (markerPix1 > 0) {

            painter->beginPath();
            painter->setFillStyle("#808080");
            painter->setStrokeStyle("#808080");
            painter->drawTriangle(qCeil(markerPix1), B, 10);
            painter->fill();

            painter->beginPath();
            painter->setFillStyle("#FF0000");
            painter->setStrokeStyle("#FF0000");
            painter->drawRectTop(qCeil(markerPix1), B, Y, 10);
            painter->fill();
        }
    }

    float startBraceMarker = (_areaInfo->startBraceMarker() - _areaInfo->startTime())
        / _areaInfo->clipDuration() * _areaInfo->lastWidth();

    float endBraceMarker = (_areaInfo->endBraceMarker() - _areaInfo->startTime())
        / _areaInfo->clipDuration() * _areaInfo->lastWidth();

    painter->beginPath();
    painter->setLineWidth(1);
    painter->setStrokeStyle("#EAEAEA");
    painter->drawLine(startBraceMarker, T, startBraceMarker, B + 1);
    painter->drawLine(endBraceMarker, T, endBraceMarker, B + 1);
    painter->stroke();
}

int WarpAreaItem::mousePressEvent(QMouseEvent* event)
{

    if (_sampleEditor->sampleClip() == Q_NULLPTR)
        return 0;

    if (!_rect.contains(event->pos()))
        return 0;

    _containMouse = true;
    _lastPoint = event->pos();

    double lastWidth = _sampleEditor->areaInfo()->lastWidth();
    double clipDuration = _sampleEditor->areaInfo()->clipDuration();
    double startTime = _sampleEditor->areaInfo()->startTime();

    QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();
    for (int i = list.size() - 1; i >= 0; i--) {

        QPainterPath path0;

        int x0 = (list[i]->t0() - startTime) / clipDuration * lastWidth;

        double y0 = _rect.bottom();
        double height_ = 10;

        path0.moveTo(x0, y0);
        path0.lineTo(x0 - (SQLRT4_3 * height_) / 2, y0 - height_);
        path0.lineTo(x0 - (SQLRT4_3 * height_) / 2, 0);
        path0.lineTo(x0 + (SQLRT4_3 * height_) / 2, 0);
        path0.lineTo(x0 + (SQLRT4_3 * height_) / 2, y0 - height_);
        path0.lineTo(x0, y0);

        if (path0.contains(event->pos())) {
            _warpMoveIndex1 = i;
            _warpMoveIndex0 = i - 1;
            break;
        }
    }

    if (list.size() > 0) {
        QPainterPath path;

        int x0 = (list[list.size() - 1]->t1() - startTime) / clipDuration * lastWidth;

        double y0 = _rect.bottom();
        double height_ = 10;

        path.moveTo(x0, y0);
        path.lineTo(x0 - (SQLRT4_3 * height_) / 2, y0 - height_);
        path.lineTo(x0 - (SQLRT4_3 * height_) / 2, 0);
        path.lineTo(x0 + (SQLRT4_3 * height_) / 2, 0);
        path.lineTo(x0 + (SQLRT4_3 * height_) / 2, y0 - height_);
        path.lineTo(x0, y0);
        if (path.contains(event->pos())) {
            _warpMoveIndex0 = list.size() - 1;
            _warpMoveIndex1 = -1;
        }
    }

    _timerCursorPosition.start();

    Q_EMIT sigLockUpdate(0);
    return 1;
}

int WarpAreaItem::mouseMoveEvent(QMouseEvent* event)
{

    _lastPoint = event->pos();

    if (_sampleEditor->sampleClip() == Q_NULLPTR)
        return 0;

    if (_containMouse == false)
        return 0;

    double currentPix = event->pos().x();

    QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();
    double w = _sampleEditor->width();

    double currentStart = _sampleEditor->areaInfo()->startTime();
    double clipDuration = _sampleEditor->areaInfo()->clipDuration();
    double lastWidth = _sampleEditor->areaInfo()->lastWidth();
    double startMarker = currentPix / (double)w * clipDuration;
    double step = _sampleEditor->areaInfo()->stepPlotDuration();

    if (_lastPoint.x() < 0 || _lastPoint.x() > lastWidth) {
        return 0;
    }

    if (step > 0) {
        startMarker = qRound((startMarker + currentStart) / (step)) * (step);
        currentPix = (startMarker - currentStart) / clipDuration * w;
    } else {
        startMarker = startMarker + currentStart;
        currentPix = (startMarker - currentStart) / clipDuration * w;
    }

    if (_warpMoveIndex0 >= 0 && _warpMoveIndex1 < 0) {
        double p0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
        _sampleEditor->setStartMarker(0);
        _sampleEditor->setEndMarker(0);
        if (qAbs(p0 - currentPix) < 0.01)
            return 0;
    } else if (_warpMoveIndex0 < 0 && _warpMoveIndex1 >= 0) {
        double p0 = ((list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth);
        _sampleEditor->setStartMarker(0);
        _sampleEditor->setEndMarker(0);
        if (qAbs(p0 - currentPix) < 0.01)
            return 0;
    } else if (_warpMoveIndex0 >= 0 && _warpMoveIndex1 >= 0) {
        double p0 = ((list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth);
        _sampleEditor->setStartMarker(0);
        _sampleEditor->setEndMarker(0);
        if (qAbs(p0 - currentPix) < 0.01)
            return 0;
    } else {

        return 0;
    }

    if (currentPix < 0 || currentPix > width) {
        return 0;
    }

    if (_warpMoveIndex1 >= list.size() || _warpMoveIndex0 >= list.size()) {
        return 0;
    }

    if (_warpMoveIndex0 < 0 && _warpMoveIndex1 >= 0) {

        list[_warpMoveIndex1]->setT0(startMarker);

        double p0 = (list[_warpMoveIndex1]->t0() - currentStart) / clipDuration * lastWidth;
        double p1 = (list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth;

        double _width = p1 - p0;

        double _temp = (_width / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo();

        list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

        if (_temp < 5.0) {

            double p = 5.0 / list[_warpMoveIndex1]->tempo() * list[_warpMoveIndex1]->width();
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex1]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex1]->setTempo(5);

            double p0 = (list[_warpMoveIndex1]->t0() - currentStart) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth;
            _width = p1 - p0;
            list[_warpMoveIndex1]->setWidth(_width);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

        } else if (_temp >= 5 && _temp <= 999.0) {
            list[_warpMoveIndex1]->setWidth(_width);
            list[_warpMoveIndex1]->setTempo(_temp);
        } else if (_temp > 999.0) {
            double p = 999.0 / list[_warpMoveIndex1]->tempo() * list[_warpMoveIndex1]->width();
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex1]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex1]->setTempo(999.0);

            double p0 = (list[_warpMoveIndex1]->t0() - currentStart) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth;
            _width = p1 - p0;
            list[_warpMoveIndex1]->setWidth(_width);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());
        }
    }
    if (_warpMoveIndex0 >= 0 && _warpMoveIndex1 < 0) {

        double _pre_index0_t0 = list[_warpMoveIndex0]->t0();
        double _pre_index0_t1 = list[_warpMoveIndex0]->t1();
        double _pre_temp0 = list[_warpMoveIndex0]->tempo();
        double _pre_width0 = list[_warpMoveIndex0]->width();
        double _pre_pps0 = list[_warpMoveIndex0]->pps();

        list[_warpMoveIndex0]->setT1(startMarker);

        double p0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
        double p1 = (list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth;

        double _width = p1 - p0;

        double _temp = (_width / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo();

        list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

        if (_temp < 5.0) {
            double p = 5.0 / list[_warpMoveIndex0]->tempo() * list[_warpMoveIndex0]->width();
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex0]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex0]->setTempo(5);

            double p0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth;
            double _width = p1 - p0;
            list[_warpMoveIndex0]->setWidth(_width);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

        } else if (_temp >= 5 && _temp <= 999) {

            list[_warpMoveIndex0]->setWidth(_width);
            list[_warpMoveIndex0]->setTempo(_temp);

        } else if (_temp > 999) {

            double p = 999.0 / list[_warpMoveIndex0]->tempo() * list[_warpMoveIndex0]->width();
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex0]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex0]->setTempo(999.0);

            double p0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth;
            double _width = p1 - p0;
            list[_warpMoveIndex0]->setWidth(_width);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());
        }
    }
    if (_warpMoveIndex0 >= 0 && _warpMoveIndex1 >= 0) {

        double _pre_index0_t0 = list[_warpMoveIndex0]->t0();
        double _pre_index0_t1 = list[_warpMoveIndex0]->t1();
        double _pre_index1_t0 = list[_warpMoveIndex1]->t0();
        double _pre_index1_t1 = list[_warpMoveIndex1]->t1();

        double _pre_temp0 = list[_warpMoveIndex0]->tempo();
        double _pre_temp1 = list[_warpMoveIndex1]->tempo();

        double _pre_width0 = list[_warpMoveIndex0]->width();
        double _pre_width1 = list[_warpMoveIndex1]->width();

        double _pre_pps0 = list[_warpMoveIndex0]->pps();
        double _pre_pps1 = list[_warpMoveIndex1]->pps();

        list[_warpMoveIndex0]->setT1(startMarker);
        list[_warpMoveIndex1]->setT0(startMarker);

        double p0_0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
        double p1_0 = (list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth;

        double p0_1 = (list[_warpMoveIndex1]->t0() - currentStart) / clipDuration * lastWidth;
        double p1_1 = (list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth;

        double width0 = p1_0 - p0_0;
        double width1 = p1_1 - p0_1;

        double _temp0 = (width0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo();
        list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

        double _temp1 = (width1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo();
        list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

        if (_temp0 < 5.0) {
            double p = 5.0 / list[_warpMoveIndex0]->tempo() * list[_warpMoveIndex0]->width();
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex0]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex0]->t1());
            list[_warpMoveIndex0]->setTempo(5);

            double p0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth;
            double width = p1 - p0;

            list[_warpMoveIndex0]->setWidth(width);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            double p0_1 = (list[_warpMoveIndex1]->t0() - currentStart) / clipDuration * lastWidth;
            double p1_1 = (list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth;
            double width_1 = p1_1 - p0_1;

            list[_warpMoveIndex1]->setTempo((width_1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo());

            list[_warpMoveIndex1]->setWidth(width_1);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            if (list[_warpMoveIndex1]->tempo() > 999) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }

        } else if (_temp1 >= 5 && _temp0 >= 5 && _temp0 <= 999 && _temp1 <= 999) {
            list[_warpMoveIndex0]->setWidth(width0);
            list[_warpMoveIndex0]->setTempo(_temp0);
        } else if (_temp0 > 999) {

            double p = 999.0 / list[_warpMoveIndex0]->tempo() * list[_warpMoveIndex0]->width();
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex0]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex0]->t1());
            ;
            list[_warpMoveIndex0]->setTempo(999.0);

            double p0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth;
            double width = p1 - p0;

            list[_warpMoveIndex0]->setWidth(width);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());
            ;

            double p0_1 = (list[_warpMoveIndex1]->t0() - currentStart) / clipDuration * lastWidth;
            double p1_1 = (list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth;
            double width_1 = p1_1 - p0_1;

            list[_warpMoveIndex1]->setTempo((width_1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo());

            list[_warpMoveIndex1]->setWidth(width_1);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            if (list[_warpMoveIndex1]->tempo() < 5) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }
        }

        if (_temp1 < 5.0) {

            double p = 5.0 / list[_warpMoveIndex1]->tempo() * list[_warpMoveIndex1]->width();
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex1]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex1]->t0());
            list[_warpMoveIndex1]->setTempo(5.0);

            double p0 = (list[_warpMoveIndex1]->t0() - currentStart) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth;
            double width = p1 - p0;

            list[_warpMoveIndex1]->setWidth(width);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            double p0_0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
            double p1_0 = (list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth;
            double width_0 = p1_0 - p0_0;

            list[_warpMoveIndex0]->setTempo((width_0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo());

            list[_warpMoveIndex0]->setWidth(width_0);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            if (list[_warpMoveIndex0]->tempo() > 999) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }

        } else if (_temp1 >= 5 && _temp0 >= 5 && _temp0 <= 999 && _temp1 <= 999) {
            list[_warpMoveIndex1]->setWidth(width1);
            list[_warpMoveIndex1]->setTempo(_temp1);
        } else if (_temp1 > 999) {
            double p = 999.0 / list[_warpMoveIndex1]->tempo() * list[_warpMoveIndex1]->width();
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex1]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex1]->t0());
            list[_warpMoveIndex1]->setTempo(999.0);

            double p0 = (list[_warpMoveIndex1]->t0() - currentStart) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex1]->t1() - currentStart) / clipDuration * lastWidth;
            double width = p1 - p0;

            list[_warpMoveIndex1]->setWidth(width);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            double p0_0 = (list[_warpMoveIndex0]->t0() - currentStart) / clipDuration * lastWidth;
            double p1_0 = (list[_warpMoveIndex0]->t1() - currentStart) / clipDuration * lastWidth;
            double width_0 = p1_0 - p0_0;

            list[_warpMoveIndex0]->setTempo((width_0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo());
            ;
            list[_warpMoveIndex0]->setWidth(width_0);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());
            ;

            if (list[_warpMoveIndex0]->tempo() < 5) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }
        }
    }

    return 1;
}

int WarpAreaItem::mouseReleaseEvent(QMouseEvent* event)
{
    _warpMoveIndex0 = -1;
    _warpMoveIndex1 = -1;
    _containMouse = false;
    _timerCursorPosition.stop();
    Q_EMIT sigSetCursor(QCursor(Qt::ArrowCursor));
    Q_EMIT sigFreeUpdate(0);
    return 1;
}

int WarpAreaItem::hoverMoveEvent(QHoverEvent* event)
{

    if (_sampleEditor->sampleClip() == Q_NULLPTR)
        return 0;

    if (!_rect.contains(event->pos()))
        return 0;

    double startTime = _sampleEditor->areaInfo()->startTime();
    double clipDuration = _sampleEditor->areaInfo()->clipDuration();
    double lastWidth = _sampleEditor->areaInfo()->lastWidth();

    qint64 index0 = -1;
    qint64 index1 = -1;
    QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();
    for (int i = 0; i < list.size(); i++) {
        QPainterPath path;

        int x0 = (list[i]->t0() - startTime) / clipDuration * lastWidth;

        double y0 = _rect.bottom();
        double height_ = 10;

        path.moveTo(x0, y0);
        path.lineTo(x0 - (SQLRT4_3 * height_) / 2, y0 - height_);
        path.lineTo(x0 - (SQLRT4_3 * height_) / 2, 0);
        path.lineTo(x0 + (SQLRT4_3 * height_) / 2, 0);
        path.lineTo(x0 + (SQLRT4_3 * height_) / 2, y0 - height_);
        path.lineTo(x0, y0);
        if (path.contains(event->pos())) {
            index0 = i;
        }
    }

    if (list.size() > 0) {
        QPainterPath path;

        double x0 = (list[list.size() - 1]->t1() - startTime) / clipDuration * lastWidth;

        double y0 = _rect.bottom();
        double height_ = 10;

        path.moveTo(x0, y0);
        path.lineTo(x0 - (SQLRT4_3 * height_) / 2, y0 - height_);
        path.lineTo(x0 - (SQLRT4_3 * height_) / 2, 0);
        path.lineTo(x0 + (SQLRT4_3 * height_) / 2, 0);
        path.lineTo(x0 + (SQLRT4_3 * height_) / 2, y0 - height_);
        path.lineTo(x0, y0);
        if (path.contains(event->pos())) {
            index0 = list.size() - 1;
        }
    }

    if (index0 >= 0 || index1 >= 0) {
        return 1;
    } else {
        return 0;
    }
}

int WarpAreaItem::mouseDoubleClickEvent(QMouseEvent* event)
{

    if (_sampleEditor->sampleClip() == Q_NULLPTR)
        return 0;

    if (!_rect.contains(event->pos()))
        return 0;

    int currentPix = event->x();

    double lastWidth = _sampleEditor->areaInfo()->lastWidth();
    double clipDuration = _sampleEditor->areaInfo()->clipDuration();
    double startTime = _sampleEditor->areaInfo()->startTime();

    QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();

    if (_warpMoveIndex0 >= 0 && _warpMoveIndex1 >= 0) {
        Warp* w0 = list[_warpMoveIndex0];
        Warp* w1 = list[_warpMoveIndex1];

        double p0 = (w0->t0() - startTime) / clipDuration * lastWidth;
        double p1 = (w1->t1() - startTime) / clipDuration * lastWidth;
        double t0 = w0->t0();
        double t1 = w1->t1();
        double bt0 = w0->base_t0();
        double bt1 = w1->bsase_t1();
        double width0 = w0->width();
        double width1 = w1->width();
        double tempo0 = w0->tempo();
        double tempo1 = w1->tempo();

        _sampleEditor->sampleClip()->removeWarpMarker(_warpMoveIndex0);
        _sampleEditor->sampleClip()->removeWarpMarker(_warpMoveIndex0);

        double k = (p1 - p0) / ((tempo0 / tempo1) * width1 + width0);

        _sampleEditor->sampleClip()->addWarpMarker(_warpMoveIndex0, p0, p1, t0, bt0, t1, bt1, k * tempo0);

        _warpMoveIndex0 = -1;
        _warpMoveIndex1 = -1;

    } else if (_warpMoveIndex0 < 0 && _warpMoveIndex1 < 0) {

        double startMarker = currentPix / (double)lastWidth * clipDuration + startTime;

        int i = _sampleEditor->sampleClip()->findWarpByTime(startMarker);

        double bt0 = list[i]->base_t0();
        double bt1 = list[i]->bsase_t1();

        double t0 = list[i]->t0();
        double t1 = list[i]->t1();

        double p0 = (list[i]->t0() - startTime) / clipDuration * lastWidth;
        double p1 = (list[i]->t1() - startTime) / clipDuration * lastWidth;

        double time_base = ((currentPix)-p0) / (double)(p1 - p0) * (bt1 - bt0) + bt0;
        double time_base_1 = ((currentPix)-p0) / (double)(p1 - p0) * (bt1 - bt0) + bt0;
        double time = ((currentPix)-p0) / (double)(p1 - p0) * (t1 - t0) + t0;
        double time_1 = ((currentPix)-p0) / (double)(p1 - p0) * (t1 - t0) + t0;

        double tempo = list[i]->tempo();

        _sampleEditor->sampleClip()->removeWarpMarker(i);

        _sampleEditor->sampleClip()->addWarpMarker(i, p0, currentPix, t0, bt0, time, time_base, tempo);
        _sampleEditor->sampleClip()->addWarpMarker(i + 1, currentPix, p1, time_1, time_base_1, t1, bt1, tempo);

        _warpMoveIndex0 = -1;
        _warpMoveIndex1 = -1;
    }

    return 1;
}

void WarpAreaItem::sltTimerCheckPosition()
{

    if (_sampleEditor->sampleClip() == Q_NULLPTR)
        return;

    _autoScroll = false;
    double _sutoScrollSpeedRight = qAbs(rect().width() - _lastPoint.x());
    double _sutoScrollSpeedLeft = qAbs(0 - _lastPoint.x());
    if (_lastPoint.x() > rect().width() && _warpMoveIndex0 >= 0 && _warpMoveIndex1 < 0) {

        QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();
        int index = list.size() - 1;

        _autoScroll = true;

        double clipDuration = _sampleEditor->areaInfo()->clipDuration();
        double lastWidth = _sampleEditor->areaInfo()->lastWidth();
        double startTime = _sampleEditor->areaInfo()->startTime();
        double endTime = _sampleEditor->areaInfo()->endTime();
        double maxEndTime = _sampleEditor->areaInfo()->maxEndTime();
        double rate = _sutoScrollSpeedRight / lastWidth * clipDuration;

        if (index >= 0) {

            double _pre_index0_1 = list[index]->t1();
            double _pre_temp0 = list[index]->tempo();
            double _pre_width0 = list[index]->width();

            list[index]->setT1(endTime + rate);

            int p0 = (list[index]->t0() - (startTime + rate)) / clipDuration * lastWidth;
            int p1 = (list[index]->t1() - (startTime + rate)) / clipDuration * lastWidth;

            double width = p1 - p0;

            list[index]->setTempo((width / list[index]->width()) * list[index]->tempo());
            list[index]->setWidth(width);
            list[index]->setPps((list[index]->bsase_t1() - list[index]->base_t0()) / list[index]->width());

            if (list[index]->tempo() > 999.0) {
                double p = (999.0 / _pre_temp0) * _pre_width0;
                list[index]->setT1(list[index]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
                list[index]->setTempo(999.0);

                double p0 = (list[index]->t0() - (startTime + rate)) / clipDuration * lastWidth;
                double p1 = (list[index]->t1() - (startTime + rate)) / clipDuration * lastWidth;
                double width = p1 - p0;
                list[index]->setWidth(width);
                list[index]->setPps((list[index]->bsase_t1() - list[index]->base_t0()) / list[index]->width());

                rate = (list[index]->t1() - _pre_index0_1);

                if (maxEndTime > list[index]->t1()) {

                } else {
                    _sampleEditor->sampleClip()->setBaseEndTime(endTime + rate);
                    _sampleEditor->sampleClip()->setBaseStartTime(startTime + rate);
                    _sampleEditor->areaInfo()->setMaxEndTime(endTime + rate);

                    _areaInfo->setClipProperties(startTime + rate, endTime + rate, endTime - startTime);
                }

            } else {

                _sampleEditor->sampleClip()->setBaseEndTime(endTime + rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime + rate);
                _sampleEditor->areaInfo()->setMaxEndTime(endTime + rate);

                _areaInfo->setClipProperties(startTime + rate, endTime + rate, endTime - startTime);
            }
        }

        Q_EMIT sigUpdate();
    }

    if (_lastPoint.x() > rect().width() && _warpMoveIndex0 < 0 && _warpMoveIndex1 >= 0) {

        QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();
        int index = 0;

        _autoScroll = true;

        double clipDuration = _sampleEditor->areaInfo()->clipDuration();
        double lastWidth = _sampleEditor->areaInfo()->lastWidth();
        double startTime = _sampleEditor->areaInfo()->startTime();

        double endTime = _sampleEditor->areaInfo()->endTime();
        double rate = _sutoScrollSpeedRight / lastWidth * clipDuration;

        if (list.count() > 0) {

            double _pre_index0_1 = list[index]->t0();
            double _pre_temp0 = list[index]->tempo();
            double _pre_width0 = list[index]->width();

            list[index]->setT0(endTime + rate);

            int p0 = (list[index]->t0() - (startTime + rate)) / clipDuration * lastWidth;
            int p1 = (list[index]->t1() - (startTime + rate)) / clipDuration * lastWidth;

            double width = p1 - p0;

            list[index]->setTempo((width / list[index]->width()) * list[index]->tempo());
            list[index]->setWidth(width);
            list[index]->setPps((list[index]->bsase_t1() - list[index]->base_t0()) / list[index]->width());

            if (list[index]->tempo() < 5) {
                double p = (5.0 / _pre_temp0) * _pre_width0;
                list[index]->setT0(list[index]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
                list[index]->setTempo(5.0);

                double p0 = (list[index]->t0() - (startTime + rate)) / clipDuration * lastWidth;
                double p1 = (list[index]->t1() - (startTime + rate)) / clipDuration * lastWidth;
                double width = p1 - p0;
                list[index]->setWidth(width);
                list[index]->setPps((list[index]->bsase_t1() - list[index]->base_t0()) / list[index]->width());

                rate = (list[index]->t1() - endTime);

                _sampleEditor->sampleClip()->setBaseEndTime(endTime + rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime + rate);

                _areaInfo->setClipProperties(startTime + rate, endTime + rate, endTime - startTime);

            } else {

                _sampleEditor->sampleClip()->setBaseEndTime(endTime + rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime + rate);

                _areaInfo->setClipProperties(startTime + rate, endTime + rate, endTime - startTime);
            }
        }

        Q_EMIT sigUpdate();
    }

    if (_lastPoint.x() > rect().width() && _warpMoveIndex0 >= 0 && _warpMoveIndex1 >= 0) {

        _autoScroll = true;

        double clipDuration = _sampleEditor->areaInfo()->clipDuration();
        double lastWidth = _sampleEditor->areaInfo()->lastWidth();
        double startTime = _sampleEditor->areaInfo()->startTime();
        double endTime = _sampleEditor->areaInfo()->endTime();
        double rate = _sutoScrollSpeedRight / lastWidth * clipDuration;

        double lastWarper = _sampleEditor->sampleClip()->warpMarkers()[_sampleEditor->sampleClip()->warpMarkers().size() - 1]->t1();
        double maxEndTime = _sampleEditor->areaInfo()->maxEndTime();
        double t1 = endTime + rate;

        if (lastWarper >= endTime) {
            if (t1 > lastWarper) {

                _sampleEditor->sampleClip()->setBaseEndTime(endTime + (lastWarper - endTime));
                _sampleEditor->sampleClip()->setBaseStartTime(startTime + (lastWarper - endTime));

                _areaInfo->setClipProperties(startTime + (lastWarper - endTime), endTime + (lastWarper - endTime), endTime - startTime);

            } else {

                _sampleEditor->sampleClip()->setBaseEndTime(endTime + rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime + rate);

                _areaInfo->setClipProperties(startTime + rate, endTime + rate, endTime - startTime);
            }
        } else {
            if (t1 > maxEndTime) {

                _sampleEditor->sampleClip()->setBaseEndTime(endTime + (maxEndTime - endTime));
                _sampleEditor->sampleClip()->setBaseStartTime(startTime + (maxEndTime - endTime));

                _areaInfo->setClipProperties(startTime + (maxEndTime - endTime), endTime + (maxEndTime - endTime), endTime - startTime);

            } else {

                _sampleEditor->sampleClip()->setBaseEndTime(endTime + rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime + rate);

                _areaInfo->setClipProperties(startTime + rate, endTime + rate, endTime - startTime);
            }
        }

        startTime = _sampleEditor->areaInfo()->startTime();
        endTime = _sampleEditor->areaInfo()->endTime();

        QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();

        double _pre_index0_t0 = list[_warpMoveIndex0]->t0();
        double _pre_index0_t1 = list[_warpMoveIndex0]->t1();
        double _pre_index1_t0 = list[_warpMoveIndex1]->t0();
        double _pre_index1_t1 = list[_warpMoveIndex1]->t1();

        double _pre_temp0 = list[_warpMoveIndex0]->tempo();
        double _pre_temp1 = list[_warpMoveIndex1]->tempo();

        double _pre_width0 = list[_warpMoveIndex0]->width();
        double _pre_width1 = list[_warpMoveIndex1]->width();

        double _pre_pps0 = list[_warpMoveIndex0]->pps();
        double _pre_pps1 = list[_warpMoveIndex1]->pps();

        list[_warpMoveIndex0]->setT1(_sampleEditor->areaInfo()->endTime());
        list[_warpMoveIndex1]->setT0(_sampleEditor->areaInfo()->endTime());

        int p0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
        int p1 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;

        double _width0 = p1 - p0;

        double _temp0 = (_width0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo();
        list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

        p0 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
        p1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;

        double _width1 = p1 - p0;

        double _temp1 = (_width1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo();
        list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

        if (_temp0 < 5.0) {
            double p = 5.0 / list[_warpMoveIndex0]->tempo() * list[_warpMoveIndex0]->width();

            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex0]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex0]->t1());
            list[_warpMoveIndex0]->setTempo(5);

            double p0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;
            double width = p1 - p0;
            list[_warpMoveIndex0]->setWidth(width);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            double p0_1 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
            double p1_1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;
            double width_1 = p1_1 - p0_1;

            list[_warpMoveIndex1]->setTempo((width_1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo());
            list[_warpMoveIndex1]->setWidth(width_1);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            if (list[_warpMoveIndex1]->tempo() > 999) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }

        } else if (_temp0 >= 5 && _temp0 >= 5 && _temp0 <= 999 && _temp1 <= 999) {
            list[_warpMoveIndex0]->setWidth(_width0);
            list[_warpMoveIndex0]->setTempo(_temp0);
        } else if (_temp0 > 999) {
            double p = 999.0 / list[_warpMoveIndex0]->tempo() * list[_warpMoveIndex0]->width();

            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex0]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex0]->t1());
            ;
            list[_warpMoveIndex0]->setTempo(999.0);

            double p0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;
            double width = p1 - p0;
            list[_warpMoveIndex0]->setWidth(width);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            double p0_1 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
            double p1_1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;
            double width_1 = p1_1 - p0_1;

            list[_warpMoveIndex1]->setTempo((width_1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo());
            list[_warpMoveIndex1]->setWidth(width_1);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            if (list[_warpMoveIndex1]->tempo() < 5) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }
        }

        if (_temp1 < 5.0) {
            double p = 5.0 / list[_warpMoveIndex1]->tempo() * list[_warpMoveIndex1]->width();

            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex1]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex1]->t0());
            list[_warpMoveIndex1]->setTempo(5);

            double p0 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;
            double width = p1 - p0;
            list[_warpMoveIndex1]->setWidth(width);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            double p0_0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
            double p1_0 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;
            double width_0 = p1_0 - p0_0;

            list[_warpMoveIndex0]->setTempo((width_0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo());

            list[_warpMoveIndex0]->setWidth(width_0);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            if (list[_warpMoveIndex0]->tempo() > 999) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }

        } else if (_temp1 >= 5 && _temp0 >= 5 && _temp0 <= 999 && _temp1 <= 999) {
            list[_warpMoveIndex1]->setWidth(_width1);
            list[_warpMoveIndex1]->setTempo(_temp1);
        } else if (_temp1 > 999) {
            double p = 999.0 / list[_warpMoveIndex1]->tempo() * list[_warpMoveIndex1]->width();

            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex1]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex1]->t0());
            list[_warpMoveIndex1]->setTempo(999.0);

            double p0 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;
            double width = p1 - p0;
            list[_warpMoveIndex1]->setWidth(width);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            double p0_0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
            double p1_0 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;
            double width_0 = p1_0 - p0_0;

            list[_warpMoveIndex0]->setTempo((width_0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo());
            list[_warpMoveIndex0]->setWidth(width_0);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            if (list[_warpMoveIndex0]->tempo() < 5) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }
        }

        Q_EMIT sigUpdate();
    }

    if (_lastPoint.x() < 0 && _warpMoveIndex0 < 0 && _warpMoveIndex1 >= 0) {

        _autoScroll = true;

        double clipDuration = _sampleEditor->areaInfo()->clipDuration();
        double lastWidth = _sampleEditor->areaInfo()->lastWidth();
        double startTime = _sampleEditor->areaInfo()->startTime();
        double endTime = _sampleEditor->areaInfo()->endTime();
        double minStartTime = _sampleEditor->areaInfo()->minStartTime();
        double rate = _sutoScrollSpeedLeft / lastWidth * clipDuration;

        QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();

        int index = 0;
        if (list.count() > 0) {

            double _pre_index0_1 = list[index]->t0();
            double _pre_temp0 = list[index]->tempo();
            double _pre_width0 = list[index]->width();
            double _pre_pps0 = list[index]->pps();

            list[index]->setT0(startTime - rate);

            int p0 = (list[index]->t0() - (startTime - rate)) / clipDuration * lastWidth;
            int p1 = (list[index]->t1() - (startTime - rate)) / clipDuration * lastWidth;

            double width = p1 - p0;

            list[index]->setTempo((width / list[index]->width()) * list[index]->tempo());
            list[index]->setWidth(width);
            list[index]->setPps((list[index]->bsase_t1() - list[index]->base_t0()) / list[index]->width());

            if (list[index]->tempo() > 999.0) {

                double p = (999.0 / _pre_temp0) * _pre_width0;
                list[index]->setT0(list[index]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
                list[index]->setTempo(999.0);

                double p0 = (list[index]->t0() - (startTime - rate)) / clipDuration * lastWidth;
                double p1 = (list[index]->t1() - (startTime - rate)) / clipDuration * lastWidth;
                double width = p1 - p0;
                list[index]->setWidth(width);
                list[index]->setPps((list[index]->bsase_t1() - list[index]->base_t0()) / list[index]->width());

                rate = (_pre_index0_1 - list[index]->t0());

                if (minStartTime < list[index]->t0()) {
                } else {

                    _sampleEditor->areaInfo()->setMinStartTime(startTime - rate);
                    _sampleEditor->sampleClip()->setBaseEndTime(startTime - rate);
                    _sampleEditor->sampleClip()->setBaseStartTime(endTime - rate);

                    _areaInfo->setClipProperties(startTime - rate, endTime - rate, endTime - startTime);
                }

            } else {

                _sampleEditor->areaInfo()->setMinStartTime(startTime - rate);
                _sampleEditor->sampleClip()->setBaseEndTime(startTime - rate);
                _sampleEditor->sampleClip()->setBaseStartTime(endTime - rate);

                _areaInfo->setClipProperties(startTime - rate, endTime - rate, endTime - startTime);
            }
        }

        Q_EMIT sigUpdate();
    }

    if (_lastPoint.x() < 0 && _warpMoveIndex0 >= 0 && _warpMoveIndex1 < 0) {

        QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();
        int index = 0;

        _autoScroll = true;

        double clipDuration = _sampleEditor->areaInfo()->clipDuration();
        double lastWidth = _sampleEditor->areaInfo()->lastWidth();
        double startTime = _sampleEditor->areaInfo()->startTime();

        double endTime = _sampleEditor->areaInfo()->endTime();
        double rate = _sutoScrollSpeedLeft / lastWidth * clipDuration;

        if (list.count() > 0) {

            double _pre_index0_1 = list[index]->t1();
            double _pre_temp0 = list[index]->tempo();
            double _pre_width0 = list[index]->width();

            list[index]->setT1(startTime - rate);

            int p0 = (list[index]->t0() - (startTime - rate)) / clipDuration * lastWidth;
            int p1 = (list[index]->t1() - (startTime - rate)) / clipDuration * lastWidth;

            double width = p1 - p0;

            list[index]->setTempo((width / list[index]->width()) * list[index]->tempo());
            list[index]->setWidth(width);
            list[index]->setPps((list[index]->bsase_t1() - list[index]->base_t0()) / list[index]->width());

            if (list[index]->tempo() < 5) {
                double p = (5.0 / _pre_temp0) * _pre_width0;
                list[index]->setT1(list[index]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
                list[index]->setTempo(5.0);

                double p0 = (list[index]->t0() - (startTime - rate)) / clipDuration * lastWidth;
                double p1 = (list[index]->t1() - (startTime - rate)) / clipDuration * lastWidth;
                double width = p1 - p0;
                list[index]->setWidth(width);
                list[index]->setPps((list[index]->bsase_t1() - list[index]->base_t0()) / list[index]->width());

                rate = (startTime - list[index]->t0());

                _areaInfo->setClipProperties(startTime - rate, endTime - rate, endTime - startTime);

                _sampleEditor->sampleClip()->setBaseEndTime(endTime - rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime - rate);

            } else {

                _areaInfo->setClipProperties(startTime - rate, endTime - rate, endTime - startTime);

                _sampleEditor->sampleClip()->setBaseEndTime(endTime - rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime - rate);
            }
        }

        Q_EMIT sigUpdate();
    }

    if (_lastPoint.x() < 0 && _warpMoveIndex0 >= 0 && _warpMoveIndex1 >= 0) {

        _autoScroll = true;

        double lastWidth = _sampleEditor->areaInfo()->lastWidth();
        double startTime = _sampleEditor->areaInfo()->startTime();
        double endTime = _sampleEditor->areaInfo()->endTime();
        double clipDuration = _sampleEditor->areaInfo()->clipDuration();
        double rate = _sutoScrollSpeedLeft / lastWidth * clipDuration;

        double firstWarper = _sampleEditor->sampleClip()->warpMarkers()[0]->t0();
        double minStartTime = _sampleEditor->areaInfo()->minStartTime();
        double t1 = startTime - rate;

        if (firstWarper <= startTime) {

            if (t1 < firstWarper) {

                _sampleEditor->sampleClip()->setBaseEndTime(endTime - (startTime - firstWarper));
                _sampleEditor->sampleClip()->setBaseStartTime(startTime - (startTime - firstWarper));

                _areaInfo->setClipProperties(startTime - (startTime - firstWarper), endTime - (startTime - firstWarper), endTime - startTime);

            } else {
                _areaInfo->setClipProperties(startTime - rate, endTime - rate, endTime - startTime);

                _sampleEditor->sampleClip()->setBaseEndTime(endTime - rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime - rate);
            }
        } else {
            if (t1 < minStartTime) {

                _sampleEditor->sampleClip()->setBaseEndTime(endTime - (startTime - minStartTime));
                _sampleEditor->sampleClip()->setBaseStartTime(startTime - (startTime - minStartTime));

                _areaInfo->setClipProperties(startTime - (startTime - minStartTime), endTime - (startTime - minStartTime), endTime - startTime);

            } else {
                _areaInfo->setClipProperties(startTime - rate, endTime - rate, endTime - startTime);

                _sampleEditor->sampleClip()->setBaseEndTime(endTime - rate);
                _sampleEditor->sampleClip()->setBaseStartTime(startTime - rate);
            }
        }

        startTime = _sampleEditor->areaInfo()->startTime();
        endTime = _sampleEditor->areaInfo()->endTime();

        QList<Warp*> list = _sampleEditor->sampleClip()->warpMarkers();

        double _pre_index0_t0 = list[_warpMoveIndex0]->t0();
        double _pre_index0_t1 = list[_warpMoveIndex0]->t1();
        double _pre_index1_t0 = list[_warpMoveIndex1]->t0();
        double _pre_index1_t1 = list[_warpMoveIndex1]->t1();

        double _pre_temp0 = list[_warpMoveIndex0]->tempo();
        double _pre_temp1 = list[_warpMoveIndex1]->tempo();

        double _pre_width0 = list[_warpMoveIndex0]->width();
        double _pre_width1 = list[_warpMoveIndex1]->width();

        double _pre_pps0 = list[_warpMoveIndex0]->pps();
        double _pre_pps1 = list[_warpMoveIndex1]->pps();

        list[_warpMoveIndex0]->setT1(_sampleEditor->areaInfo()->startTime());
        list[_warpMoveIndex1]->setT0(_sampleEditor->areaInfo()->startTime());

        int p0_0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
        int p1_0 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;

        double _width0 = p1_0 - p0_0;

        double _temp0 = (_width0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo();
        list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

        int p0_1 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
        int p1_1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;

        double _width1 = p1_1 - p0_1;

        double _temp1 = (_width1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo();
        list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

        if (_temp0 < 5.0) {
            double p = 5.0 / list[_warpMoveIndex0]->tempo() * list[_warpMoveIndex0]->width();

            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex0]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex0]->t1());

            list[_warpMoveIndex0]->setTempo(5);

            double p0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;
            double width = p1 - p0;

            list[_warpMoveIndex0]->setWidth(width);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            double p0_1 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
            double p1_1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;
            double width_1 = p1_1 - p0_1;

            list[_warpMoveIndex1]->setTempo((width_1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo());
            list[_warpMoveIndex1]->setWidth(width_1);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            if (list[_warpMoveIndex1]->tempo() > 999) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);

                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }

        } else if (_temp0 >= 5 && _temp0 >= 5 && _temp0 <= 999 && _temp1 <= 999) {
            list[_warpMoveIndex0]->setWidth(_width0);
            list[_warpMoveIndex0]->setTempo(_temp0);
        } else if (_temp0 > 999) {
            double p = 999.0 / list[_warpMoveIndex0]->tempo() * list[_warpMoveIndex0]->width();

            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex0]->t0() + (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex0]->t1());
            ;
            list[_warpMoveIndex0]->setTempo(999.0);

            double p0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;
            double width = p1 - p0;

            list[_warpMoveIndex0]->setWidth(width);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            double p0_1 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
            double p1_1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;
            double width_1 = p1_1 - p0_1;

            list[_warpMoveIndex1]->setTempo((width_1 / list[_warpMoveIndex1]->width()) * list[_warpMoveIndex1]->tempo());
            list[_warpMoveIndex1]->setWidth(width_1);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            if (list[_warpMoveIndex1]->tempo() < 5) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }
        }

        if (_temp1 < 5.0) {
            double p = 5.0 / list[_warpMoveIndex1]->tempo() * list[_warpMoveIndex1]->width();

            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex1]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex1]->t0());
            list[_warpMoveIndex1]->setTempo(5);

            double p0 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;
            double width = p1 - p0;
            list[_warpMoveIndex1]->setWidth(width);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            double p0_0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
            double p1_0 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;
            double width_0 = p1_0 - p0_0;
            list[_warpMoveIndex0]->setTempo((width_0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo());
            ;
            list[_warpMoveIndex0]->setWidth(width_0);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());
            ;

            if (list[_warpMoveIndex0]->tempo() > 999) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }

        } else if (_temp1 >= 5 && _temp0 >= 5 && _temp0 <= 999 && _temp1 <= 999) {

            list[_warpMoveIndex1]->setWidth(_width1);
            list[_warpMoveIndex1]->setTempo(_temp1);
        } else if (_temp1 > 999) {
            double p = 999.0 / list[_warpMoveIndex1]->tempo() * list[_warpMoveIndex1]->width();

            list[_warpMoveIndex1]->setT0(list[_warpMoveIndex1]->t1() - (p / _sampleEditor->areaInfo()->lastWidth() * _sampleEditor->areaInfo()->clipDuration()));
            list[_warpMoveIndex0]->setT1(list[_warpMoveIndex1]->t0());
            list[_warpMoveIndex1]->setTempo(999.0);

            double p0 = (list[_warpMoveIndex1]->t0() - startTime) / clipDuration * lastWidth;
            double p1 = (list[_warpMoveIndex1]->t1() - startTime) / clipDuration * lastWidth;
            double width = p1 - p0;
            list[_warpMoveIndex1]->setWidth(width);
            list[_warpMoveIndex1]->setPps((list[_warpMoveIndex1]->bsase_t1() - list[_warpMoveIndex1]->base_t0()) / list[_warpMoveIndex1]->width());

            double p0_0 = (list[_warpMoveIndex0]->t0() - startTime) / clipDuration * lastWidth;
            double p1_0 = (list[_warpMoveIndex0]->t1() - startTime) / clipDuration * lastWidth;
            double width_0 = p1_0 - p0_0;
            list[_warpMoveIndex0]->setTempo((width_0 / list[_warpMoveIndex0]->width()) * list[_warpMoveIndex0]->tempo());

            list[_warpMoveIndex0]->setWidth(width_0);
            list[_warpMoveIndex0]->setPps((list[_warpMoveIndex0]->bsase_t1() - list[_warpMoveIndex0]->base_t0()) / list[_warpMoveIndex0]->width());

            if (list[_warpMoveIndex0]->tempo() < 5) {
                list[_warpMoveIndex0]->setT0(_pre_index0_t0);
                ;
                list[_warpMoveIndex0]->setT1(_pre_index0_t1);
                list[_warpMoveIndex1]->setT0(_pre_index1_t0);
                list[_warpMoveIndex1]->setT1(_pre_index1_t1);
                list[_warpMoveIndex0]->setTempo(_pre_temp0);
                list[_warpMoveIndex1]->setTempo(_pre_temp1);

                list[_warpMoveIndex0]->setWidth(_pre_width0);
                list[_warpMoveIndex1]->setWidth(_pre_width1);

                list[_warpMoveIndex0]->setPps(_pre_pps0);
                list[_warpMoveIndex1]->setPps(_pre_pps1);
            }
        }
        Q_EMIT sigUpdate();
    }
}
