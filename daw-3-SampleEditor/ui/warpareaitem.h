#ifndef WARPAREAITEM_H
#define WARPAREAITEM_H

#include <QPainter>
#include <QPainterPath>
#include <QQuickPaintedItem>
#include <QTimer>

#include "core/beatbar.h"
#include "ui/ipainteritem.h"
#include "ui/plotitem.h"
class Warp;
class SampleEditor;
class AreaInfo;
class WarpAreaItem : public IPainterItem {

public:
    WarpAreaItem(SampleEditor* sampleEditor,AreaInfo* areaInfo);

    int _warpMoveIndex0 = -1;
    int _warpMoveIndex1 = -1;
    QTimer _timerCursorPosition;
    QPoint _lastPoint;
    bool _autoScroll = false;
    SampleEditor* _sampleEditor;

    // IPainterItem interface
public:
    void draw(IPainter* painter);

    // IPainterItem interface
public:
    int mousePressEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int mouseReleaseEvent(QMouseEvent* event);
    int hoverMoveEvent(QHoverEvent* event);
    int mouseDoubleClickEvent(QMouseEvent* event);

    void sltTimerCheckPosition();

private:
    AreaInfo* _areaInfo;
};

#endif // WARPAREAITEM_H
