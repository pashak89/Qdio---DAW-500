#ifndef PLOTITEM_H
#define PLOTITEM_H

#include <QPainter>
#include <QQuickPaintedItem>

#include "audio/samplebuffer.h"
#include "audio/sampleclip.h"
#include "core/beatbar.h"
#include "core/global.h"
#include "ui/backgrounditem.h"
#include "ui/ipainteritem.h"
class AreaInfo;
class PlotItem : public IPainterItem {
    Q_OBJECT
public:
    PlotItem(AreaInfo* areaInfo);

    // IPainterItem interface
public:
    void draw(IPainter* painter);

private:
    double _lastDuration = 0;
    double _lastStartffset = 0;

    bool _move = false;
    QVector<bool> _pixs;
    QVector<bool> _pixs_temp;
    double _pixR = 0;

    int _adaptiveGrid;
    int _fixedGrid;
    bool _offGrid = false;

    int _trackIndex = -1;

    AreaInfo* _areaInfo;

public:
    int mouseReleaseEvent(QMouseEvent* event);
    int trackIndex() const;
    void setTrackIndex(int newTrackIndex);
};

#endif // PLOTITEM_H
