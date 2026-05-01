#include "backgroundarea.h"

#include "cliparea.h"
#include <QDir>
#include <QDrag>
#include <QJsonArray>
#include <QMimeData>
#include <QThread>
#include <audio/engine.h>
#include <core/screeninterface.h>
#include <qdatetime.h>
BackgroundArea::BackgroundArea(QQuickItem* parent)
    : INanoItem(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setMouseEventsEnabled(true);
    setMouseHoverEventsEnabled(true);
    setFlag(QQuickItem::ItemIsFocusScope, true);
    setFlag(QQuickItem::ItemAcceptsDrops, true);
}

void BackgroundArea::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{

    if (_areaInfo == Q_NULLPTR)
        return;

    auto _boundingRect = newGeometry.toRect();

    plotItem()->setRect(_boundingRect);

    auto rect = _boundingRect;

    rect.setY(0);
    rect.setHeight(_boundingRect.height());
    markerItem()->setRect(rect);
}

void BackgroundArea::initlaize()
{

    _plotItem = new PlotItem(_areaInfo);
    _markerItem = new MarkerAreaItem(_areaInfo);
    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &BackgroundArea::sltUpdate);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &BackgroundArea::sltUpdate);

    connect(_clipArea, &ClipArea::sigUpdateUI, this, &BackgroundArea::update);

    connect(_areaInfo, &AreaInfo::sigUpdateUI, this, &BackgroundArea::sltUpdate);
    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &BackgroundArea::sltUpdate);

    connect(_clipArea->tracksModel(), &TracksModel::sigClipChanged, this, &BackgroundArea::sltUpdate);

    update();
}

MarkerAreaItem* BackgroundArea::markerItem() const
{
    return _markerItem;
}

PlotItem* BackgroundArea::plotItem() const
{
    return _plotItem;
}

AreaInfo* BackgroundArea::areaInfo() const
{
    return _areaInfo;
}

void BackgroundArea::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;
    initlaize();
}

void BackgroundArea::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;
}

void BackgroundArea::sltUpdate()
{
    update();
}

void BackgroundArea::sltCurrentIndicatorChanged(bool value)
{
    _markerItem->setShowIndicator(value);
}

QNanoQuickItemPainter* BackgroundArea::createItemPainter() const
{
    return new BackgroundAreaPainter(this);
}

BackgroundAreaPainter::BackgroundAreaPainter(const BackgroundArea* topArea)
    : INanoPainterItem()
    , _topArea(topArea)
{
}

void BackgroundAreaPainter::draw(IPainter* painter)
{
    _topArea->plotItem()->draw(painter);
    _topArea->markerItem()->draw(painter);
}
