#include "bottomarea.h"

#include "audio/song.h"
#include "audio/track.h"
#include <QDir>
#include <QDrag>
#include <QJsonArray>
#include <QMimeData>
#include <QThread>
#include <audio/engine.h>
#include <core/screeninterface.h>
#include <qdatetime.h>

BottomArea::BottomArea(QQuickItem* parent)
    : INanoItem(parent)
{

    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setMouseEventsEnabled(true);
    setMouseHoverEventsEnabled(true);
    setFlag(QQuickItem::ItemIsFocusScope, true);
    setFlag(QQuickItem::ItemAcceptsDrops, true);
}

QSharedPointer<TimeRulerItem> BottomArea::timeRulerItem() const
{
    return _timeRulerItem;
}

QSharedPointer<BackgroundItem> BottomArea::backgroundItem() const
{
    return _backgroundItem;
}

QSharedPointer<PlotItem> BottomArea::plotItem() const
{
    return _plotItem;
}

AreaInfo* BottomArea::areaInfo() const
{
    return _areaInfo;
}

void BottomArea::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;
    initlaize();
}

int BottomArea::timeRulerHeight() const
{
    return _timeRulerHeight;
}

void BottomArea::initlaize()
{
    if (AudioManager::getSong() == Q_NULLPTR) {
        return;
    }

    connect(_areaInfo, &AreaInfo::sigPlayBackUpdateTimeout, this, &BottomArea::sltPlayBackUpdateTimeout);

    _timeRulerItem = QSharedPointer<TimeRulerItem>(new TimeRulerItem(nullptr, _areaInfo));

    _backgroundItem = QSharedPointer<BackgroundItem>(new BackgroundItem());

    _plotItem = QSharedPointer<PlotItem>(new PlotItem(_areaInfo));

    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &BottomArea::sltUpdate);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &BottomArea::sltUpdate);

    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &BottomArea::update);
}

void BottomArea::sltUpdate()
{
    update();
}

void BottomArea::sltPlayBackUpdateTimeout()
{
    if (AudioManager::getSong()->isPause() == false) {

        sltUpdate();
    }
}

void BottomArea::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{

    QRect rect = newGeometry.toRect();
    _boundingRect = newGeometry.toRect();

    _boundingRect.setX(0);
    rect.setX(0);

    backgroundItem()->setRect(_boundingRect);

    rect = _boundingRect;
    rect.setY(_boundingRect.height() - ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor()));
    rect.setHeight(ScreenInterface::scaleSize2(40, _areaInfo->zoomFactor()));
    timeRulerItem()->setRect(rect);

    _timeRulerHeight = timeRulerItem()->rect().height();
    Q_EMIT timeRulerHeightChanged();

    //    setImplicitHeight(timeRulerItem()->rect().height() + masterTrack()->getTotalRect().height());
    //    setHeight(timeRulerItem()->rect().height() + masterTrack()->getTotalRect().height());
    sltUpdate();
}

QNanoQuickItemPainter* BottomArea::createItemPainter() const
{
    return new BottomAreaPainter(this);
}

BottomAreaPainter::BottomAreaPainter(const BottomArea* bottomArea)
    : INanoPainterItem()
    , _bottomArea(bottomArea)
{
}

void BottomAreaPainter::draw(IPainter* painter)
{
    _bottomArea->timeRulerItem()->draw(painter);
}
