#ifndef WAVEITEM_H
#define WAVEITEM_H

#include <QMutex>
#include <QPainter>
#include <QQuickPaintedItem>

#include "audio/samplebuffer.h"
#include "audio/sampleclip.h"
#include "core/beatbar.h"
#include "ui/ipainteritem.h"
#include <core/global.h>

class SampleEditor;
class WaveItem : public IPainterItem {

public:
    WaveItem(SampleEditor* sampleEditor,AreaInfo* areaInfo);

    // IPainterItem interface
public:
    void draw(IPainter* painter);

private:
    double _lastDuration = 0;
    double _lastStartffset = 0;
    bool _move = false;
    AreaInfo* _areaInfo;
    SampleEditor* _sampleEditor = Q_NULLPTR;

    QMap<int, float*> min;
    QMap<int, float*> max;

    QMap<int, float*> min_;
    QMap<int, float*> max_;

    double _pixR = 0;
};

#endif // CLIPITEM_H
