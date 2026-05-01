#ifndef OBJECTPOSITIONITEM_H
#define OBJECTPOSITIONITEM_H

#include <QCursor>
#include <QPainter>
#include <QPainterPath>
#include <QQuickPaintedItem>
#include <QTimer>

#include "core/automation.h"
#include "core/beatbar.h"
#include "core/mathhelper.h"
#include "ui/ipainteritem.h"
#include "ui/plotitem.h"
class TrackItem;
class ObjectPositionItem : public IPainterItem {
    Q_OBJECT

public:
    explicit ObjectPositionItem(int trackIndex, AreaInfo* areaInfo);
    bool enabled() const;
    void setEnabled(bool value);

    double time2Pixel(double time);
    double pixel2Time(double pixel);

    void update(QRect windowRect);

private:
    QRect _windowRect;
    double _minValue = 0;
    double _maxValue = 1;
    double _value = 0.5;
    AreaInfo* _areaInfo;
    int _trackIndex;
    bool _enabled = true;

    int _mouseHoverX;
    int _mouseHoverY;

    int _mousePressedX;
    int _mousePressedY;

    int _currentPointIndex = -1;

    bool _mousePressed = false;
    bool _mouseMoved = false;

    QList<QPointF> _points;

    QTimer _checkPressAndHold;

    // IPainterItem interface
public:
    void draw(IPainter* painter);
    int hoverMoveEvent(QHoverEvent* event);
    int mousePressEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int mouseReleaseEvent(QMouseEvent* event);

    double value() const;

protected:
    QPointF mapPointToLine(int index, QPointF pixelPoint);

    QVector<QPolygonF> lineAreas();

    QVector<QLineF> getLines();

private:
    void drawLine(IPainter* painter);
    void drawKeyPoints(IPainter* painter);
    void drawKeyHoverPoint(IPainter* painter);
    int findPoint(QPointF point);
    int findLine(double time);

private Q_SLOTS:
    void checkPressAndHoldTimeout();

signals:
    void sigValueChanged(int trackIndex);
};

#endif // OBJECTPOSITIONITEM_H
