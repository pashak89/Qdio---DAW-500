#ifndef BACKGROUNDAREA_H
#define BACKGROUNDAREA_H

#include "ui/clipitem.h"
#include "ui/envelopeitem.h"
#include "ui/hotspotitem.h"
#include "ui/indicatoritem.h"
#include "ui/loopitem2.h"
#include "ui/markerareaitem.h"
#include "ui/plotitem.h"
#include "ui/timeruleritem.h"
#include "ui/trackitem.h"

#include <QAbstractListModel>
#include <QJsonArray>
#include <QQuickItem>
#include <QTimer>
#include <ui/timeruleritem.h>

class BackgroundArea : public INanoItem {
    Q_OBJECT
    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea WRITE setClipArea NOTIFY areaInfoChanged)

public:
    explicit BackgroundArea(QQuickItem* parent = Q_NULLPTR);
    Q_INVOKABLE void initlaize();

    MarkerAreaItem* markerItem() const;
    PlotItem* plotItem() const;

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);
    void setClipArea(ClipArea* clipArea);

private:
    ClipArea* _clipArea;
    MarkerAreaItem* _markerItem = Q_NULLPTR;
    AreaInfo* _areaInfo = Q_NULLPTR;
    PlotItem* _plotItem = Q_NULLPTR;
    QTimer _updateTimer;

signals:
    void areaInfoChanged();

private slots:
    void sltUpdate();
    void sltCurrentIndicatorChanged(bool value);

protected:
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

    // QNanoQuickItem interface
protected:
    QNanoQuickItemPainter* createItemPainter() const;
};

class BackgroundAreaPainter : public INanoPainterItem {

public:
    BackgroundAreaPainter(const BackgroundArea* topArea);

    // INanoPainterItem interface
public:
    void draw(IPainter* painter);

    const BackgroundArea* _topArea;
};

#endif // BACKGROUNDAREA_H
