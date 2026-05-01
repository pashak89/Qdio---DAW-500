#include "plotshapeitem.h"

#include <core/cliparea.h>

PlotShapeItem::PlotShapeItem(AreaInfo* areaInfo, QQuickItem* parent)
    : QQuickItem(parent)
    , _areaInfo(areaInfo)
{
    setFlag(ItemHasContents, true);

    _timerWidth.start();
    _timerWidth.setInterval(300);
    connect(&_timerWidth, &QTimer::timeout, this, &PlotShapeItem::sltTimerWidthTimeout);
}

AreaInfo* PlotShapeItem::areaInfo() const
{
    return _areaInfo;
}

void PlotShapeItem::sltTimerWidthTimeout()
{
    int _width = width();
    if (_areaInfo->lastWidth() != _width && _width > 0) {
        int min = ScreenInterface::instance()->freeResolution(20);
        int max = ScreenInterface::instance()->freeResolution(40);

        double barDuration = 2.0;
        double final_size = (max + min) / 2.0;

        _areaInfo->init(_width, final_size, barDuration);

        _areaInfo->setPlotRect(boundingRect().toRect());
        _clipArea->drawGrids();
        update();
    }
}

ClipArea* PlotShapeItem::clipArea() const
{
    return _clipArea;
}

void PlotShapeItem::setClipArea(ClipArea* newClipArea)
{
    _clipArea = newClipArea;

    connect(_clipArea, &ClipArea::sigUpdateUI, this, &PlotShapeItem::sltClipPropertiesChanged);
}

void PlotShapeItem::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;
    // connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &PlotShapeItem::sltClipPropertiesChanged);
    //  connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &PlotShapeItem::update);

    //  connect(_areaInfo, &AreaInfo::sigUpdateUI, this, &PlotShapeItem::update);
    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &PlotShapeItem::sltClipPropertiesChanged);
}

void PlotShapeItem::sltClipPropertiesChanged()
{

    m_linesA = _clipArea->trackSegLines(-1);
    m_rectsA = _clipArea->trackSegRects(-1);

    update();
}

QSGNode* PlotShapeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{

    QSGNode* rootNode = oldNode ? oldNode : new QSGNode;

    QRectF _rect = boundingRect();

    if (!m_bgNode) {
        m_bgNode = new QSGGeometryNode;
        QSGGeometry* geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 4);
        geom->setDrawingMode(GL_TRIANGLE_STRIP);
        m_bgNode->setGeometry(geom);
        m_bgNode->setFlag(QSGNode::OwnsGeometry);

        QSGFlatColorMaterial* mat = new QSGFlatColorMaterial();
        mat->setColor(Qt::black);
        m_bgNode->setMaterial(mat);
        m_bgNode->setFlag(QSGNode::OwnsMaterial);

        rootNode->appendChildNode(m_bgNode);
    }

    if (_areaInfo == Q_NULLPTR)
        return rootNode;

    auto _lastWidth = _areaInfo->lastWidth();

    if (_lastWidth == 0)
        return rootNode;

    QSGGeometry::Point2D* v = m_bgNode->geometry()->vertexDataAsPoint2D();
    v[0].set(_rect.left(), _rect.top());
    v[1].set(_rect.right(), _rect.top());
    v[2].set(_rect.left(), _rect.bottom());
    v[3].set(_rect.right(), _rect.bottom());
    m_bgNode->geometry()->markVertexDataDirty();
    m_bgNode->markDirty(QSGNode::DirtyGeometry);

    if (!m_rects_node) {
        m_rects_node = new MultiColorRectsNode;
        rootNode->appendChildNode(m_rects_node);
    }

    if (!m_lines_node) {
        m_lines_node = new MultiColorLinesNode;
        rootNode->appendChildNode(m_lines_node);
    }

    for (int i = 0; i < m_linesA.size(); i++) {
        m_linesA[i].p1.setY(_rect.top());
        m_linesA[i].p2.setY(_rect.bottom());
    }

    for (int i = 0; i < m_rectsA.size(); i++) {
        m_rectsA[i].rect.setTop(_rect.top());
        m_rectsA[i].rect.setBottom(_rect.bottom());
    }

    m_lines_node->setLines(m_linesA);
    m_rects_node->setLines(m_rectsA);

    return rootNode;
}
