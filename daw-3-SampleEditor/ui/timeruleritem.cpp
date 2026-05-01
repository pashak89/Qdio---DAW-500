#include "timeruleritem.h"
#include "core/sampleeditor.h"
#include "core/screeninterface.h"
#include <QTime>
#include <QtMath>
#include <core/cliparea.h>
#include <core/measure.h>
TimeRulerItem::TimeRulerItem(Automation* automation, AreaInfo* areaInfo)
    : IPainterItem()
    , _automation(automation)
    , _areaInfo(areaInfo)
{
}

void TimeRulerItem::increaseZoomLevel()
{

    auto _zoomLevel = _areaInfo->zoomLevel();

    switch (_zoomLevel) {
    case 0:
        _zoomLevel = (ZoomLevel)1;
        break;
    case 1:
        _zoomLevel = (ZoomLevel)2;
        break;
    case 2:
        _zoomLevel = (ZoomLevel)5;
        break;
    case 5:
        _zoomLevel = (ZoomLevel)10;
        break;
    case 10:
        _zoomLevel = (ZoomLevel)20;
        break;
    case 20:
        _zoomLevel = (ZoomLevel)50;
        break;
    case 50:
        _zoomLevel = (ZoomLevel)100;
        break;
    case 100:
        _zoomLevel = (ZoomLevel)200;
        break;
    case 200:
        _zoomLevel = (ZoomLevel)500;
        break;
    case 500:
        _zoomLevel = (ZoomLevel)1000;
        break;
    case 1000:
        _zoomLevel = (ZoomLevel)2000;
        break;
    case 2000:
        _zoomLevel = (ZoomLevel)5000;
        break;
    case 5000:
        _zoomLevel = (ZoomLevel)10000;
        break;
    case 10000:
        _zoomLevel = (ZoomLevel)15000;
        break;
    case 15000:
        _zoomLevel = (ZoomLevel)20000;
        break;
    case 20000:
        _zoomLevel = (ZoomLevel)30000;
        break;
    case 30000:
        _zoomLevel = (ZoomLevel)60000;
        break;
    case 60000:
        _zoomLevel = (ZoomLevel)120000;
        break;
    case 120000:
        _zoomLevel = (ZoomLevel)300000;
        break;
    case 300000:
        _zoomLevel = (ZoomLevel)600000;
        break;
    }
}

double TimeRulerItem::find(double value)
{

    double result = 1;
    if (value < 1) {
        result = 1;
    } else if (value >= 1 && value < 2) {
        result = 2;
    } else if (value >= 2 && value < 5) {
        result = 5;
    } else if (value >= 5 && value < 10) {
        result = 10;
    } else if (value >= 10 && value < 20) {
        result = 20;
    } else if (value >= 20 && value < 50) {
        result = 50;
    } else if (value >= 50 && value < 100) {
        result = 100;
    } else if (value >= 100 && value < 200) {
        result = 200;
    } else if (value >= 200 && value < 500) {
        result = 500;
    } else if (value >= 500 && value < 1000) {
        result = 1000;
    } else if (value >= 1000 && value < 2000) {
        result = 2000;
    } else if (value >= 2000 && value < 5000) {
        result = 5000;
    } else if (value >= 5000 && value < 10000) {
        result = 10000;
    } else if (value >= 10000 && value < 15000) {
        result = 15000;
    } else if (value >= 15000 && value < 20000) {
        result = 20000;
    } else if (value >= 20000 && value < 30000) {
        result = 30000;
    } else if (value >= 30000 && value < 60000) {
        result = 60000;
    }

    return result;
}

bool TimeRulerItem::validNumber(int number)
{
    if (number == 0 || number == 1 || number == 2 || number == 5
        || number == 10 || number == 20 || number == 50 || number == 100 || number == 200 || number == 500
        || number == 1000 || number == 2000 || number == 5000 || number == 10000 || number == 15000
        || number == 20000 || number == 30000 || number == 60000
        || number == 120000 || number == 300000 || number == 600000) {
        return true;
    }
    return false;
}

int TimeRulerItem::validDiv(int number)
{
    int result = 0;
    switch (number) {
    case 1:
        result = 5;
        break;
    case 2:
        result = 5;
        break;
    case 5:
        result = 4;
        break;
    case 10:
        result = 5;
        break;
    case 20:
        result = 5;
        break;
    case 50:
        result = 4;
        break;
    case 100:
        result = 5;
        break;
    case 200:
        result = 5;
        break;
    case 500:
        result = 5;
        break;
    case 1000:
        result = 5;
        break;
    case 2000:
        result = 4;
        break;
    case 5000:
        result = 5;
        break;
    case 10000:
        result = 5;
        break;
    case 15000:
        result = 3;
        break;
    case 20000:
        result = 4;
        break;
    case 30000:
        result = 6;
        break;
    case 60000:
        result = 6;
        break;
    }
    return result;
}

void TimeRulerItem::draw(IPainter* painter)
{

    auto _currentDuration = _areaInfo->clipDuration();
    double _lastWidth = _areaInfo->lastWidth();
    auto _startTime = _areaInfo->startTime();
    auto _lenghtOfBeat = _areaInfo->lenghtOfBeat();
    auto _beatPerBar = _areaInfo->beatPerBar();
    auto _endTime = _areaInfo->endTime();

    auto _tempo = _areaInfo->tempo();

    auto _startMarker = 0;
    if (_currentDuration <= 0) {
        return;
    }

    int W = _rect.width();
    int B = _rect.bottom();
    int T = _rect.top();
    int H = _rect.height();
    int R = _rect.right();
    int L = _rect.left();
    int X = _rect.x();
    int Y = _rect.y();
    double final_size = 0;
    double offsetPix = 0;

    painter->setFillStyle("#2f3032");
    painter->setStrokeStyle("#2f3032");
    painter->fillRect(_rect);

    painter->setFillStyle("#FFFFFF");
    painter->setStrokeStyle("#FFFFFF");

    int min = ScreenInterface::instance()->freeResolution(20);
    int max = ScreenInterface::instance()->freeResolution(60);

    auto currentDuration = _currentDuration;
    auto lastWidth = _lastWidth;
    double currentStartMarkerPix = _startMarker / currentDuration * lastWidth;

    painter->beginPath();
    painter->setStrokeStyle("#eeeeee");
    painter->setLineWidth(1);

    double currentTime = 0;
    int stepDuration = 0;
    QMap<double, double> pixs;
    double sum = 0;
    double sum_sum = 0;
    QList<double> bars;

    double a = _areaInfo->time2Pixel(_endTime);
    double b = _areaInfo->time2Pixel(_startTime);

    double w = a - b;

    auto m1 = Measure::timeToMeasure(_startTime, _lenghtOfBeat, _beatPerBar, _tempo);
    auto m2 = Measure::timeToMeasure(_endTime, _lenghtOfBeat, _beatPerBar, _tempo);
    double count_sixteenth_last = (m2.bar - 1) * 4 * 4 + (m2.beat - 1) * 4 + (m2.sixteenth - 1);
    count_sixteenth_last += ((_endTime - ((count_sixteenth_last * m2.sixteenthDuration) * 1000.0)) / (m2.sixteenthDuration * 1000.0));
    double count_sixteenth_first = (m1.bar - 1) * 4 * 4 + (m1.beat - 1) * 4 + (m1.sixteenth - 1);
    count_sixteenth_first += ((_startTime - ((count_sixteenth_first * m1.sixteenthDuration) * 1000.0)) / (m1.sixteenthDuration * 1000.0));

    double count_sixteenth = count_sixteenth_last - count_sixteenth_first;

    double k2 = (m2.bar - 1) * 4 * 4 + (m2.beat - 1) * 4 + (m2.sixteenth - 1);
    k2 += ((_endTime - ((count_sixteenth_last * m2.sixteenthDuration) * 1000.0)) / (m2.sixteenthDuration * 1000.0));

    double k1 = (m1.bar - 1) * 4 * 4 + (m1.beat - 1) * 4 + (m1.sixteenth - 1);
    k1 += ((_startTime - ((count_sixteenth_last * m1.sixteenthDuration) * 1000.0)) / (m1.sixteenthDuration * 1000.0));

    double delta = 0; //(list[i].tempo_end - list[i].tempo_start) / (double)count;

    int pos_start = _areaInfo->time2Pixel(_startTime);
    int pos_end = _areaInfo->time2Pixel(_endTime);

    int posX = pos_start;
    bars.clear();
    double sub_six_per_pixel = (count_sixteenth / (double)w) * 4096.0;
    double S = 0;
    double bar_counter = 1;

    double value = count_sixteenth_first * 4096.0;
    double current = 0;
    double nT = 0.0;
    for (int p = 0; p < qRound(_lastWidth); p++) {

        nT = delta * (bar_counter) + _tempo;

        sum = ((60.0 / nT / 4.0) * 1000.0) * value / 4096.0;
        bars.append(sum);
        value += sub_six_per_pixel;

        pixs.insert((p + b), sum);
    }

    sum_sum = sum;

    if (pixs.size() > 0) {
        int min = width;
        QMapIterator<double, double> i(pixs);
        double time = 0;
        while (i.hasNext()) {
            i.next();
            if (qAbs(i.key() - currentStartMarkerPix) < min) {
                min = qAbs(i.key() - currentStartMarkerPix);
                time = i.value();
            }
        }
    }

    double m = 0;
    QList<double> values = pixs.values();
    QList<double> keys = pixs.keys();
    int f = 0;
    for (int i = 0; i < values.count() - min; i++) {
        QList<double> l2 = values.mid(i, min);

        if (l2.last() - l2.first() > m) {
            f = i;
            m = l2.last() - l2.first();
        }
    }

    int major = validDiv(find(m)) * find(m);
    int minor = find(m);

    int stepDuration_minor = qCeil(pixs.first() / (double)minor) * minor;
    int stepDuration_major = qCeil(pixs.first() / (double)major) * major;
    for (int i = 0; i < keys.count(); i++) {
        QString text;
        if (major >= 1000.0) {

            text = QTime::fromMSecsSinceStartOfDay(qRound(values[i])).toString("mm:ss");

        } else {
            text = QTime::fromMSecsSinceStartOfDay((qCeil(values[i] / (double)major) * major)).toString("mm:ss.zzz");
        }

        if (values[i] >= stepDuration_major) {
            stepDuration_major += major;
            stepDuration_minor += minor;
            if (keys[i] >= 0 && keys[i] < lastWidth) {

                auto font = QNanoFont(":/resources/font/SourceSansPro-Regular.ttf");
                font.setPixelSize(ScreenInterface::scaleSize2(28, _areaInfo->zoomFactor()));

                painter->setFont(font);

                auto textrect = painter->textBoundingBox(text, 0, 0);

                painter->setFillStyle("#CFCFCF");
                painter->setStrokeStyle("#CFCFCF");

                painter->drawText(keys[i] + 2, T + textrect.height(), text);
                painter->drawLine(keys[i], T, keys[i], T + rect().height() / 2);
            }

        } else {
            if (values[i] >= stepDuration_minor) {
                if (keys[i] >= 0 && keys[i] < lastWidth) {
                    stepDuration_minor += minor;
                    painter->setFillStyle("#CFCFCF");
                    painter->setStrokeStyle("#CFCFCF");
                    painter->drawLine(keys[i], T, keys[i], T + rect().height() / 4);
                }
            }
        }
    }

    painter->stroke();
}
