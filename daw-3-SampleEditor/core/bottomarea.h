#ifndef BOTTOMAREA_H
#define BOTTOMAREA_H

#include "audio/audioengine.h"
#include "audio/engine.h"
#include "audio/sampletrack.h"
#include "core/tracksmodel.h"
#include "measure.h"
#include "sampleeditor.h"
#include "ui/automationitem.h"
#include "ui/backgrounditem.h"
#include "ui/beatbaritem.h"
#include "ui/clipitem.h"
#include "ui/envelopeitem.h"
#include "ui/hotspotitem.h"
#include "ui/indicatoritem.h"
#include "ui/loopitem2.h"
#include "ui/markerareaitem.h"
#include "ui/plotitem.h"
#include "ui/scrubareaitem.h"
#include "ui/timeruleritem.h"
#include "ui/trackitem.h"
#include "ui/warpareaitem.h"
#include "warp.h"
#include <QAbstractListModel>
#include <QJsonArray>
#include <QQuickItem>
#include <QTimer>
#include <ui/timeruleritem.h>

class BottomArea : public INanoItem {
    Q_OBJECT

    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)

    Q_PROPERTY(int timeRulerHeight READ timeRulerHeight NOTIFY timeRulerHeightChanged)
public:
    explicit BottomArea(QQuickItem* parent = Q_NULLPTR);
    Q_INVOKABLE void initlaize();

    QSharedPointer<TimeRulerItem> timeRulerItem() const;
    QSharedPointer<BackgroundItem> backgroundItem() const;
    QSharedPointer<PlotItem> plotItem() const;

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    int timeRulerHeight() const;

private:
    AreaInfo* _areaInfo;

    QSharedPointer<BackgroundItem> _backgroundItem = Q_NULLPTR;
    QSharedPointer<TimeRulerItem> _timeRulerItem = Q_NULLPTR;
    QSharedPointer<PlotItem> _plotItem = Q_NULLPTR;

    int _timeRulerHeight;
    QRect _boundingRect;

signals:

    void areaInfoChanged();

    void timeRulerHeightChanged();

private slots:

    void sltUpdate();
    void sltPlayBackUpdateTimeout();

    // QQuickItem interface
protected:
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

    // QNanoQuickItem interface
protected:
    QNanoQuickItemPainter* createItemPainter() const;
};

class BottomAreaPainter : public INanoPainterItem {

public:
    BottomAreaPainter(const BottomArea* bottomArea);

    // INanoPainterItem interface
public:
    void draw(IPainter* painter);

    const BottomArea* _bottomArea;
};

#endif // BOTTOMAREA_H
