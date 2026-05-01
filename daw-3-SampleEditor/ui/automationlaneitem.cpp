#include "automationlaneitem.h"

#include "core/cliparea.h"
#include "core/global.h"

#include <QQuickWindow>
#include <QSGSimpleRectNode>
#include <QtMath>

AutomationLaneItem::AutomationLaneItem(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    setAcceptHoverEvents(false);
    setZ(10e6); // ensure on top (adjust as you like)
}

ClipArea* AutomationLaneItem::clipArea() const
{
    return _clipArea;
}

void AutomationLaneItem::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;
}

AreaInfo* AutomationLaneItem::areaInfo() const
{
    return _areaInfo;
}

void AutomationLaneItem::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;

    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &AutomationLaneItem::update);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &AutomationLaneItem::update);

    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &AutomationLaneItem::update);

    connect(_areaInfo, &AreaInfo::sigUpdateUI, this, &AutomationLaneItem::update);
    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &AutomationLaneItem::update);

    connect(_clipArea->tracksModel(), &TracksModel::sigClipChanged, this, &AutomationLaneItem::update);

    connect(_areaInfo, &AreaInfo::playheadMarkerChanged, this, &AutomationLaneItem::update);
}

bool AutomationLaneItem::automationLaneEnabled()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->automationLaneEnabled();
}

QSharedPointer<AutomationLaneModel> AutomationLaneItem::automationLaneModel()
{

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(m_trackIndex);
    return trackItem->automationLaneModel();
}

int AutomationLaneItem::trackIndex() const
{
    return m_trackIndex;
}

void AutomationLaneItem::setTrackIndex(int index)
{
    m_trackIndex = index;
}

static inline void writeRect6(QSGGeometry::Point2D* v, const QRectF& r)
{
    const float x1 = r.left(), y1 = r.top();
    const float x2 = r.right(), y2 = r.bottom();
    // tri1
    v[0].set(x1, y1);
    v[1].set(x2, y1);
    v[2].set(x1, y2);
    // tri2
    v[3].set(x1, y2);
    v[4].set(x2, y1);
    v[5].set(x2, y2);
}

QSGNode* AutomationLaneItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{

    if (!automationLaneEnabled()) {
        delete oldNode;
        return nullptr;
    }
    QSGGeometryNode* node = static_cast<QSGGeometryNode*>(oldNode);
    if (!node) {
        node = new QSGGeometryNode();
        QSGGeometry* geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2);
        geom->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geom);
        node->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial* mat = new QSGFlatColorMaterial();
        node->setMaterial(mat);
        node->setFlag(QSGNode::OwnsMaterial);
    }

    // Update geometry
    QSGGeometry* g = node->geometry();
    g->setLineWidth(1);
    QSGGeometry::Point2D* v = g->vertexDataAsPoint2D();
    v[0].set(0, _areaInfo->titleHeight()); // top
    v[1].set(width(), _areaInfo->titleHeight()); // bottom
    g->markVertexDataDirty();

    static_cast<QSGFlatColorMaterial*>(node->material())->setColor(Qt::black);

    node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
    return node;
}
