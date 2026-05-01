#ifndef LOOPITEM_H
#define LOOPITEM_H

#include <QCursor>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QTimer>

#include "core/beatbar.h"
#include "ui/ipainteritem.h"
#include "ui/plotitem.h"
class ScrubAreaItem : public IPainterItem {

    Q_OBJECT
public:
    explicit ScrubAreaItem(AreaInfo* areaInfo);

    // QNanoQuickItem interface
protected:
    AreaInfo* _areaInfo;
    bool _startMarkerMove = false;
    bool _endMarkerMove = false;
    bool _midMarkerMove = false;
    float _lastMousePressX;
    float _startMoveTime = 0;
    float _braceDuration;

    bool _autoScroll = false;
    QTimer _timerCursorPosition;
    QPoint _lastPoint;

    QColor _loopBgColor = Qt::gray;

    // IPainterItem interface
public:
    void draw(IPainter* painter);

    // IPainterItem interface
public:
    int mousePressEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int mouseReleaseEvent(QMouseEvent* event);
    int hoverMoveEvent(QHoverEvent* event);

    void sltTimerCheckPosition();

public:
    void setLoopBgColor(const QColor& loopBgColor);

signals:
    void sigLoopBgColorChanged(const QColor& loopBgColor);

    void sigTimeDurationChanged(double duration);
    void sigCurrentIndicatorChanged(double);
};

#endif // LOOPITEM_H
