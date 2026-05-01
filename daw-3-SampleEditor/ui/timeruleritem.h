#ifndef TIMERULER_H
#define TIMERULER_H

#include <QGuiApplication>
#include <QPainter>
#include <QQuickPaintedItem>

#include "core/automation.h"
#include "core/beatbar.h"
#include "ui/ipainteritem.h"

class ClipArea;
class TimeRulerItem : public IPainterItem {
    Q_OBJECT
    // INanoPainterItem interface
public:
    TimeRulerItem(Automation* automation,  AreaInfo* areaInfo);

public:
    void draw(IPainter* painter);


private:
    AreaInfo* _areaInfo;
    Automation* _automation;

    bool validNumber(int number);
    void increaseZoomLevel();
    int validDiv(int number);

    double find(double value);
Q_SIGNALS:
    void sigInsertMarker();
};

#endif // TIMERULER_H
