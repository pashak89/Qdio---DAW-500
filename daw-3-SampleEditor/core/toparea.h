#ifndef TOPAREA_H
#define TOPAREA_H

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

class TopArea : public INanoItem {
    Q_OBJECT
    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea WRITE setClipArea NOTIFY areaInfoChanged)

public:
    explicit TopArea(QQuickItem* parent = Q_NULLPTR);
    Q_INVOKABLE void initlaize();

    BeatBarItem* beatBarItem() const;

    ScrubAreaItem* scrubAreaItem() const;
    HotspotItem* hotspotItem() const;
    PlotItem* plotItem() const;

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);
    void setClipArea(ClipArea* clipArea);

    BackgroundItem* backgroundItem() const;

private:
    int _hotspotResizing = -1;
    ClipArea* _clipArea;
    AreaInfo* _areaInfo = Q_NULLPTR;
    BeatBarItem* _beatBarItem = Q_NULLPTR;
    PlotItem* _plotItem = Q_NULLPTR;
    BackgroundItem* _backgroundItem = Q_NULLPTR;

    ScrubAreaItem* _scrubAreaItem = Q_NULLPTR;
    HotspotItem* _hotspotItem = Q_NULLPTR;
    QRect _boundingRect;
    QTimer _timerWidth;
signals:
    void areaInfoChanged();

private slots:
    void sltUpdate();

    // QQuickItem interface
    void sltScrubCurrentIndicatorChanged(double time);

protected:
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

    // QNanoQuickItem interface
protected:
    QNanoQuickItemPainter* createItemPainter() const;

    // QQuickItem interface
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void hoverMoveEvent(QHoverEvent* event);

    void hoverLeaveEvent(QHoverEvent* event);
};

class TopAreaPainter : public INanoPainterItem {

public:
    TopAreaPainter(const TopArea* topArea);

    // INanoPainterItem interface
public:
    void draw(IPainter* painter);

    const TopArea* _topArea;
};

#endif // TOPAREA_H
