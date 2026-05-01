#ifndef HOTSPOTITEM_H
#define HOTSPOTITEM_H

#include "ipainteritem.h"

class AreaInfo;
class ClipArea;
class HotspotItem : public IPainterItem {
    Q_OBJECT
public:
    HotspotItem(ClipArea* clipArea, AreaInfo* areaInfo);

private:
    AreaInfo* _areaInfo;
    ClipArea* _clipArea;

    double _startPix;
    double _endPix;
    double _lastBeatBarPressX;
    double _lastHotSpotPressTime;
    double _lastHostSpotTimeStart;
    double _lastHotSpotTimeEnd;
    bool _zoomEnabled = false;
    bool _resizeEnd = false;
    bool _resizeStart = false;
    double _mousePressRatio;
    bool _zoomStop = false;
    bool _mouseMove = false;
    bool _mouseReleased = false;
    double _lastCursorPosX = 0;
    double _lastCursorPosY = 0;
    double _startPositionX = 0;
    double _startPositionY = 0;
    double _currentPosistionY = 0;
    double _currentPosistionX = 0;
    // IPainterItem interface
public:
    void draw(IPainter* painter);

    // IPainterItem interface
public:
    int mousePressEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int mouseReleaseEvent(QMouseEvent* event);
    int hoverMoveEvent(QHoverEvent* event);

public:
    double lastCursorPosX() const;
    double lastCursorPosY() const;
};

#endif // HOTSPOTITEM_H
