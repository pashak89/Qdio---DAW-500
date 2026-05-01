#ifndef ZOOMITEM_H
#define ZOOMITEM_H

#include <QGuiApplication>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QtMath>

#include <core/global.h>
#include <core/warp.h>

#include "core/beatbar.h"
#include "ui/backgrounditem.h"
#include "ui/ipainteritem.h"

class AreaInfo;
class ClipArea;
class BeatBarItem : public IPainterItem {
    Q_OBJECT
    struct Data {

        bool valid = false;
        double time = 0;
    };

public:
    BeatBarItem(ClipArea* clipArea, AreaInfo* areaInfo);
    // IPainterItem interface
public:
    void draw(IPainter* painter);

private:
    int pixelsHigh = 15;
    double _startPositionX = 0;
    double _startPositionY = 0;
    double _lastCursorPosX = 0;
    double _lastCursorPosY = 0;
    bool _mouseReleased = false;
    bool _mouseMove = false;
    bool _panMove = true;
    double _currentPosistionY = 0;
    double _currentPosistionX = 0;
    double _lastDuration = 0;

    double _currentPressPosTime = 0;

    bool _move = false;
    double _lastStartffset = 0;
    QVector<Data> _pixs;
    QVector<Data> _pixs_temp;

    int _zoomCounter = 0;

    double _startPositionTime;
    double _firstPositionX;

    ZoomLevel _zoomLevel;

    int _playMovingMode;

    bool _isAutomationEnabled;

    bool _zoomEnded = false;
    AreaInfo* _areaInfo;
    ClipArea* _clipArea;

public:
    int mousePressEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int mouseReleaseEvent(QMouseEvent* event);

public:
    double lastCursorPosX() const;

    double lastCursorPosY() const;

signals:
    void sigZoomLevelChanged(ZoomLevel zoomLevel);
    void sigZoomEnded(bool end);
    void sigPlayMoveModeChanged(int mode);
    void sigLastBeatBarPressX(double time);
    void sigSamplePerPixelChanged(double spp);
    void sigGreenWindowZoomStop(bool stop);

    void sigStartPixelChanged(double pixel);
    void sigEndPixelChanged(double pixel);

    // IPainterItem interface
public:
    int hoverMoveEvent(QHoverEvent* event);
};

#endif // ZOOMITEM_H
