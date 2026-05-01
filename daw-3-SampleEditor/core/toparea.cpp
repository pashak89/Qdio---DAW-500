#include "toparea.h"

#include "audio/audioengine.h"
#include "audio/song.h"
#include "audio/track.h"
#include "cliparea.h"
#include <QDir>
#include <QDrag>
#include <QJsonArray>
#include <QMimeData>
#include <QThread>
#include <audio/engine.h>
#include <core/screeninterface.h>
#include <qdatetime.h>
TopArea::TopArea(QQuickItem* parent)
    : INanoItem(parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setMouseEventsEnabled(true);
    setMouseHoverEventsEnabled(true);
    setFlag(QQuickItem::ItemIsFocusScope, true);
    setFlag(QQuickItem::ItemAcceptsDrops, true);
}

void TopArea::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{

    if (_areaInfo == Q_NULLPTR)
        return;

    QRect rect = newGeometry.toRect();
    _boundingRect = newGeometry.toRect();

    _boundingRect.setX(0);

    backgroundItem()->setRect(_boundingRect);

    rect = _boundingRect;
    rect.setY(0);
    rect.setHeight(ScreenInterface::scaleSize2(75, _areaInfo->zoomFactor()));
    hotspotItem()->setRect(rect);

    rect = _boundingRect;
    rect.setY(hotspotItem()->rect().y() + hotspotItem()->rect().height());
    rect.setHeight(ScreenInterface::scaleSize2(75, _areaInfo->zoomFactor()));

    beatBarItem()->setRect(rect);

    rect = _boundingRect;
    rect.setY(hotspotItem()->rect().y() + hotspotItem()->rect().height() + beatBarItem()->rect().height());
    rect.setHeight(_boundingRect.height() - beatBarItem()->rect().bottom());
    plotItem()->setRect(rect);

    rect = _boundingRect;
    rect.setY(beatBarItem()->rect().y() + beatBarItem()->rect().height());
    rect.setHeight(ScreenInterface::scaleSize2(75, _areaInfo->zoomFactor()));
    scrubAreaItem()->setRect(rect);

    _areaInfo->setScrubAreaTop(scrubAreaItem()->rect().top());

    setImplicitHeight(hotspotItem()->rect().height() + beatBarItem()->rect().height() + scrubAreaItem()->rect().height());
    setHeight(hotspotItem()->rect().height() + beatBarItem()->rect().height() + scrubAreaItem()->rect().height());
    sltUpdate();
}

void TopArea::initlaize()
{

    _backgroundItem = new BackgroundItem();
    _hotspotItem = new HotspotItem(_clipArea, _areaInfo);
    _beatBarItem = new BeatBarItem(_clipArea, _areaInfo);
    _plotItem = new PlotItem(_areaInfo);
    _scrubAreaItem = new ScrubAreaItem(_areaInfo);

    _backgroundItem->setBrush("#2f3032");

    connect(_scrubAreaItem, &ScrubAreaItem::sigSetCursor, [this](const QCursor& cursor) {
        QGuiApplication::setOverrideCursor(cursor);
    });

    connect(_scrubAreaItem, &ScrubAreaItem::sigCurrentIndicatorChanged, this, &TopArea::sltScrubCurrentIndicatorChanged);

    connect(_beatBarItem, &BeatBarItem::sigZoomLevelChanged, [this](ZoomLevel level) {
        _areaInfo->setZoomLevel(level);
    });

    connect(_beatBarItem, &BeatBarItem::sigCursorPosition, [this](QPointF pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(_hotspotItem, &HotspotItem::sigCursorPosition, [this](QPointF pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(_hotspotItem, &HotspotItem::sigCursorPosition, [this](QPoint pos) {
        QCursor::setPos(mapToGlobal(pos).toPoint());
    });

    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &TopArea::sltUpdate);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &TopArea::sltUpdate);

    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &TopArea::sltUpdate);

    connect(_areaInfo, &AreaInfo::sigUpdateUI, this, &TopArea::sltUpdate);
    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &TopArea::sltUpdate);

    connect(_areaInfo, &AreaInfo::loopActiveChanged, this, &TopArea::sltUpdate);

    connect(_clipArea->tracksModel(), &TracksModel::sigClipChanged, this, &TopArea::sltUpdate);

    update();
}

void TopArea::sltScrubCurrentIndicatorChanged(double time)
{

    AudioManager::getSong()->setPause();
    _areaInfo->sigPlayChanged(false);

    _areaInfo->setPlayheadMarker(time);
    _areaInfo->setPlayheadMarkerVisible(true);

    AudioManager::getSong()->readyToPlay();
}

ScrubAreaItem* TopArea::scrubAreaItem() const
{
    return _scrubAreaItem;
}

HotspotItem* TopArea::hotspotItem() const
{
    return _hotspotItem;
}

PlotItem* TopArea::plotItem() const
{
    return _plotItem;
}

BeatBarItem* TopArea::beatBarItem() const
{
    return _beatBarItem;
}

AreaInfo* TopArea::areaInfo() const
{
    return _areaInfo;
}

void TopArea::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;
    initlaize();
}

void TopArea::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;
}

BackgroundItem* TopArea::backgroundItem() const
{
    return _backgroundItem;
}

void TopArea::sltUpdate()
{
    update();
}

QNanoQuickItemPainter* TopArea::createItemPainter() const
{
    return new TopAreaPainter(this);
}

void TopArea::mouseMoveEvent(QMouseEvent* event)
{

    if (_hotspotResizing == 1) {

        auto rect2 = _hotspotItem->rect();
        rect2.setBottom(event->pos().y());

        if (rect2.height() < 100 && rect2.height() > 20) {
            _hotspotItem->setRect(rect2);
        }
        if (rect2.height() < 0) {
            rect2.setBottom(20);
            _hotspotItem->setRect(rect2);
        }

        QRect rect = _boundingRect;

        rect.setY(hotspotItem()->rect().bottom());
        rect.setHeight(20);
        beatBarItem()->setRect(rect);

        rect = _boundingRect;
        rect.setY(beatBarItem()->rect().bottom());
        rect.setHeight(40);
        scrubAreaItem()->setRect(rect);

    } else {
        if (scrubAreaItem()->mouseMoveEvent(event)) {

            Q_EMIT _areaInfo->sigUpdateUI();
        }
        if (beatBarItem()->mouseMoveEvent(event)) {
            QCursor::setPos(mapToGlobal(QPointF(beatBarItem()->lastCursorPosX(), beatBarItem()->lastCursorPosY())).toPoint());
        }

        if (hotspotItem()->mouseMoveEvent(event)) {
            QCursor::setPos(mapToGlobal(QPointF(hotspotItem()->lastCursorPosX(), hotspotItem()->lastCursorPosY())).toPoint());
        }
    }
}

void TopArea::mouseReleaseEvent(QMouseEvent* event)
{
    _hotspotResizing = -1;

    beatBarItem()->mouseReleaseEvent(event);
    hotspotItem()->mouseReleaseEvent(event);
    scrubAreaItem()->mouseReleaseEvent(event);
}

void TopArea::hoverMoveEvent(QHoverEvent* event)
{
    _hotspotResizing = -1;

    if (qAbs(_hotspotItem->rect().bottom() - event->pos().y()) < 3) {
        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
        _hotspotResizing = 0;
    } else {
        auto result1 = scrubAreaItem()->hoverMoveEvent(event);
        auto result2 = hotspotItem()->hoverMoveEvent(event);
        auto result3 = beatBarItem()->hoverMoveEvent(event);

        if (result1) {
            QGuiApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
        } else if (result2) {
            QGuiApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
        } else if (result3) {
            QGuiApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
        } else {
            QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void TopArea::hoverLeaveEvent(QHoverEvent* event)
{
    QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void TopArea::mousePressEvent(QMouseEvent* event)
{
    beatBarItem()->mousePressEvent(event);

    scrubAreaItem()->mousePressEvent(event);
    hotspotItem()->mousePressEvent(event);

    update();
    Q_EMIT _areaInfo->sigUpdateUI();
}
TopAreaPainter::TopAreaPainter(const TopArea* topArea)
    : INanoPainterItem()
    , _topArea(topArea)
{
}

void TopAreaPainter::draw(IPainter* painter)
{
    // _topArea->backgroundItem()->draw(painter);
    //_topArea->plotItem()->draw(painter);
    _topArea->hotspotItem()->draw(painter);
    _topArea->beatBarItem()->draw(painter);
    _topArea->scrubAreaItem()->draw(painter);
}
