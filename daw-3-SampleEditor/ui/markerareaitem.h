#ifndef MARKERAREAITEM_H
#define MARKERAREAITEM_H

#include "ipainteritem.h"
#include "plotitem.h"
#include <QPainter>
#include <QQuickPaintedItem>
class ClipArea;
class MarkerAreaItem : public IPainterItem {
    Q_OBJECT

public:
    explicit MarkerAreaItem(AreaInfo* areaInfo);

private:
    AreaInfo* _areaInfo;
    int _playMovingMode;

    bool _showIndicator = false;

public:
    void draw(IPainter* painter);
    void setPlayMovingMode(int playMovingMode);

    // IPainterItem interface
public:
    int mousePressEvent(QMouseEvent* event);

    bool showIndicator() const;
    void setShowIndicator(bool showIndicator);

signals:
    void sigPlaybackMarkerChanged(double time, bool toPlay);
};

#endif // MARKERAREAITEM_H
