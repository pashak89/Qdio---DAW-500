#include "indicatoritem.h"
#include "core/cliparea.h"
#include "core/global.h"

#include <QQuickWindow>
#include <QSGSimpleRectNode>
#include <QTimer>
#include <QtMath>

IndicatorShapeItem::IndicatorShapeItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    setAcceptHoverEvents(false);
    setZ(2e6); // ensure on top (adjust as you like)

    m_blinkTimer = new QTimer(this);
    m_blinkTimer->setInterval(200);
    connect(m_blinkTimer, &QTimer::timeout, this, [this]() {
        m_blinkState = !m_blinkState;
        update();
    });
    m_blinkTimer->start();
}

ClipArea* IndicatorShapeItem::clipArea() const
{
    return _clipArea;
}

void IndicatorShapeItem::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;
}

AreaInfo* IndicatorShapeItem::areaInfo() const
{
    return _areaInfo;
}

void IndicatorShapeItem::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;

    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &IndicatorShapeItem::update);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &IndicatorShapeItem::update);

    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &IndicatorShapeItem::update);

    connect(_areaInfo, &AreaInfo::sigUpdateUI, this, &IndicatorShapeItem::update);
    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &IndicatorShapeItem::update);

    connect(_clipArea->tracksModel(), &TracksModel::sigClipChanged, this, &IndicatorShapeItem::update);

    connect(_areaInfo, &AreaInfo::playheadMarkerChanged, this, &IndicatorShapeItem::update);
}

int IndicatorShapeItem::lastRowExpanded()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->getLastRowExpanded();
}

int IndicatorShapeItem::rowsExpandedCount()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->rowsExpandedCount();
}

QSGNode* IndicatorShapeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{

    QSGGeometryNode* node = static_cast<QSGGeometryNode*>(oldNode);
    QSGGeometry* geom = nullptr;
    QSGFlatColorMaterial* mat = nullptr;

    if (!node) {
        node = new QSGGeometryNode();

        geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2);
        geom->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geom);
        node->setFlag(QSGNode::OwnsGeometry);

        mat = new QSGFlatColorMaterial();
        mat->setColor(Qt::white);
        node->setMaterial(mat);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        geom = node->geometry();
        mat = static_cast<QSGFlatColorMaterial*>(node->material());
    }

    auto m_duration = _areaInfo->clipDuration();
    auto _lastWidth = _areaInfo->lastWidth();

    int Y = boundingRect().top();
    auto top = boundingRect().top() + rowsExpandedCount() * _areaInfo->titleHeight() + _areaInfo->seperatorHeight();

    int B = boundingRect().bottom();

    double x = 0;

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);

    if (currentIndicator() < 0 || !trackItem->isSelected() || trackItem->indicatorEnabled() == TrackItem::IndicatorType_Disable) {
        geom->allocate(0);
        node->markDirty(QSGNode::DirtyGeometry);
        return node;
    }

    if (indicatorEnabled() == TrackItem::IndicatorType_Track) {

        x = ((currentIndicator() - _areaInfo->startTime()) / (double)_areaInfo->clipDuration()) * _areaInfo->lastWidth();

        B = Y + trackItem->rect().height() - _areaInfo->seperatorHeight();
    }
    if (indicatorEnabled() == TrackItem::IndicatorType_Lane) {

        int index = automationLaneModel()->currentSelected();

        if (index >= 0) {
            x = ((currentIndicator() - _areaInfo->startTime()) / (double)_areaInfo->clipDuration()) * _areaInfo->lastWidth();
            auto H = automationLaneModel()->totalRowCountBefore2(index) * _areaInfo->titleHeight() + index * _areaInfo->seperatorHeight();

            Y = top + H;
            B = Y + automationLaneModel()->getItem(index)->automationLaneRowCount * _areaInfo->titleHeight();
        }
    }

    // Guard against invalid params
    if (m_duration <= 0.0 || _lastWidth <= 0.0) {
        // Hide: allocate zero vertices
        geom->allocate(0);
        node->markDirty(QSGNode::DirtyGeometry);
        return node;
    }

    // Hide when outside [0, width()]
    if (x < 0.0 || x > width()) {
        geom->allocate(0);
        node->markDirty(QSGNode::DirtyGeometry);
        return node;
    }

    // Make sure we have 2 vertices
    if (geom->vertexCount() != 2)
        geom->allocate(2);

    // Optional: snap to device pixels for crisp 1px line
    const qreal dpr = window() ? window()->effectiveDevicePixelRatio() : 1.0;
    auto snap = [dpr](qreal v) {
        return (dpr > 0.0) ? qRound(v * dpr) / dpr : v;
    };

    const float xPix = float(snap(x));

    geom->setLineWidth(1); // logical px; actual AA depends on platform
    QSGGeometry::Point2D* v = geom->vertexDataAsPoint2D();
    v[0].set(xPix, Y);
    v[1].set(xPix, B);
    node->markDirty(QSGNode::DirtyGeometry);

    if (m_blinkState)
        mat->setColor("#A1A1FF");
    else
        mat->setColor(Qt::transparent);

    node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return node;
}

bool IndicatorShapeItem::automationLaneEnabled()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->automationLaneEnabled();
}

QSharedPointer<AutomationLaneModel> IndicatorShapeItem::automationLaneModel()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->automationLaneModel();
}

double IndicatorShapeItem::currentIndicator() const
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->currentIndicator();
}

TrackItem::IndicatorType IndicatorShapeItem::indicatorEnabled() const
{
    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->indicatorEnabled();
}

int IndicatorShapeItem::trackIndex() const
{
    return m_trackIndex;
}

void IndicatorShapeItem::setTrackIndex(int index)
{
    m_trackIndex = index;
}
