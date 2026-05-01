#include "waveitem.h"
#include "core/sampleeditor.h"
#include "core/warp.h"
#include <QTime>
#include <QtMath>
#include <audio/audioengine.h>
#include <audio/engine.h>
#include <audio/samplebuffer.h>
#include <audio/sampleclip.h>
#include <audio/song.h>
WaveItem::WaveItem(SampleEditor* sampleEditor, AreaInfo* areaInfo)
    : IPainterItem()
    , _areaInfo(areaInfo)
    , _sampleEditor(sampleEditor)
{
}

void WaveItem::draw(IPainter* painter)
{

    auto _sampleClip = _sampleEditor->sampleClip();
    if (_sampleClip == Q_NULLPTR) {
        return;
    }
    double startTime = _areaInfo->startTime();
    double endTime = _areaInfo->endTime();
    double startBaseTime = _sampleClip->baseStartTime();
    double endBaseTime = _sampleClip->baseEndTime();
    double lastWidth = _areaInfo->lastWidth();
    double clipDuration = _areaInfo->clipDuration();

    int channels = _sampleClip->sampleBuffer()->channels();

    if (_sampleClip->currentFileDuration() > 0 && channels > 0) {

        painter->beginPath();
        painter->setLineWidth(1.5);
        painter->setStrokeStyle(QNanoColor("#000000"));
        painter->setLineCap(IPainter::CAP_ROUND);
        painter->setLineJoin(IPainter::JOIN_ROUND);

        int H = _rect.height() / channels;

        if (qAbs(_lastDuration - clipDuration) <= 0 && qAbs(_lastStartffset - startTime) > 0) {
            _move = true;
        } else {
            _move = false;
        }

        double diff = _lastStartffset - startTime;

        double pix = static_cast<double>(diff / clipDuration * lastWidth);

        if (pix >= 1 || pix <= -1) {
            _lastStartffset = startTime;
        }

        _lastDuration = clipDuration;

        for (int i = 0; i < 2; i++) {
            int Y = i * H + _rect.y();

            QList<Warp*> list = _sampleClip->warpMarkers();

            for (int j = 0; j < list.size(); j++) {

                double start, end;
                double pix1 = ((list[j]->t0() - startTime) / clipDuration * lastWidth);
                double pix2 = ((list[j]->t1() - startTime) / clipDuration * lastWidth);

                if (startTime >= list[j]->base_t0())
                    start = startTime;
                else
                    start = list[j]->base_t0();

                if (endTime <= list[j]->bsase_t1())
                    end = endTime;
                else
                    end = list[j]->bsase_t1();

                if (pix1 < 0 && pix2 < 0)
                    continue;

                if (pix1 > lastWidth && pix2 > lastWidth)
                    continue;

                double p0, p1;

                if (pix2 > lastWidth)
                    p1 = lastWidth;
                else
                    p1 = pix2;

                p0 = pix1;

                if (p0 < 0)
                    p0 = 0;

                if (list[j]->base_t0() > endBaseTime)
                    continue;

                if (list[j]->bsase_t1() < startBaseTime)
                    continue;

                QRectF rect(p0, Y, p1 - p0, H);

                QVector<float> arr_min;
                QVector<float> arr_max;

                arr_min.resize(p1 - p0);
                arr_max.resize(p1 - p0);

                if (arr_min.size() <= 0)
                    continue;

                _sampleClip->sampleBuffer()->get(rect, lastWidth, i,
                    start, end, arr_max.data(), arr_min.data());

                _sampleClip->sampleBuffer()->visualize(
                    *painter, rect.toRect(),i,
                    start, end,
                    arr_max.data(), arr_min.data(), arr_min.size());
            }
        }
        painter->stroke();
    }

    _dirty = false;
}
