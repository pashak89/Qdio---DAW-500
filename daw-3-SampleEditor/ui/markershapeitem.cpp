#include "markershapeitem.h"
#include "core/cliparea.h"
#include "core/global.h"

#include <QQuickWindow>
#include <QSGSimpleRectNode>
#include <QtMath>

MarkerShapeItem::MarkerShapeItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    setAcceptHoverEvents(false);
    setZ(2e6); // ensure on top (adjust as you like)
}

ClipArea* MarkerShapeItem::clipArea() const
{
    return _clipArea;
}

void MarkerShapeItem::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;
}

AreaInfo* MarkerShapeItem::areaInfo() const
{
    return _areaInfo;
}

void MarkerShapeItem::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;

    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &MarkerShapeItem::update);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &MarkerShapeItem::update);

    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &MarkerShapeItem::update);

    connect(_areaInfo, &AreaInfo::sigUpdateUI, this, &MarkerShapeItem::update);
    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &MarkerShapeItem::update);

    connect(_clipArea->tracksModel(), &TracksModel::sigClipChanged, this, &MarkerShapeItem::update);

    connect(_areaInfo, &AreaInfo::playheadMarkerChanged, this, &MarkerShapeItem::update);
}

QSGNode* MarkerShapeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
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

    double m_playIndicator = _areaInfo->playheadMarker();

    double m_showIndicator = _areaInfo->playheadMarkerVisible();

    double m_editPointIndicator = _areaInfo->editPointIndicator();

    bool m_showEdit = m_editPointIndicator > 0.0;

    // Guard against invalid params
    if (m_duration <= 0.0 || _lastWidth <= 0.0 || (m_showIndicator == false && m_showEdit == false)) {
        // Hide: allocate zero vertices
        geom->allocate(0);
        node->markDirty(QSGNode::DirtyGeometry);
        return node;
    }

    double m_indicator = m_playIndicator;
    if (m_showEdit) {
        m_indicator = m_editPointIndicator;
    }

    // Convert time -> pixel
    const double x = _areaInfo->time2Pixel(m_indicator);

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
    const float h = float(height());

    geom->setLineWidth(1); // logical px; actual AA depends on platform
    QSGGeometry::Point2D* v = geom->vertexDataAsPoint2D();
    v[0].set(xPix, 0.0f);
    v[1].set(xPix, h);
    node->markDirty(QSGNode::DirtyGeometry);

    // Update color if needed
    if (mat->color() != Qt::white) {
        mat->setColor(Qt::white);
        node->markDirty(QSGNode::DirtyMaterial);
    }

    return node;
}
