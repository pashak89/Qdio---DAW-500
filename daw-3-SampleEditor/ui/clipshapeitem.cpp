#include "clipshapeitem.h"
#include "audio/sampleclip.h"
#include "core/beatbar.h"
#include "qquickwindow.h"

#include <QFileInfo>
#include <QMutex>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>
#include <QTextLayout>
#include <QTimer>
#include <QUuid>

#include <core/cliparea.h>

#include <audio/track.h>

ClipShapeItem::ClipShapeItem(QQuickItem* parent)
    : QQuickItem(parent)

{
    setFlag(ItemHasContents, true);
    setClip(true);

    setZ(1e6);
    //    setFlag(ItemAcceptsDrops, true);
    //    setAcceptHoverEvents(true);
    //    setAcceptedMouseButtons(Qt::AllButtons);
    //    setFlag(ItemAcceptsInputMethod, true);
}

ClipShapeItem::~ClipShapeItem()
{
}

AreaInfo* ClipShapeItem::areaInfo() const
{
    return _areaInfo;
}

void ClipShapeItem::setAreaInfo(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;

    connect(_areaInfo, &AreaInfo::sigLastWidthChanged, this, &ClipShapeItem::update);
    connect(_areaInfo, &AreaInfo::sigZoomLevelChanged, this, &ClipShapeItem::update);
    connect(_areaInfo, &AreaInfo::sigClipPropertiesChanged1, this, &ClipShapeItem::update);
    // connect(_areaInfo, &AreaInfo::sigUpdateUI, this, &ClipShapeItem::update);
    connect(_areaInfo, &AreaInfo::loopFocusActiveChanged, this, &ClipShapeItem::update);

    connect(_areaInfo, &AreaInfo::sigSelectionAreaRectChanged, this, &ClipShapeItem::update);
}

ClipArea* ClipShapeItem::clipArea() const
{
    return _clipArea;
}

void ClipShapeItem::setClipArea(ClipArea* clipArea)
{
    _clipArea = clipArea;

    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);

    connect(trackItem.data(), &TrackItem::sigDragClipAdded, this, &ClipShapeItem::sltDragClipAdded);
    connect(trackItem.data(), &TrackItem::sigDragClipRemoved, this, &ClipShapeItem::sltDragClipRemoved);
    connect(trackItem.data(), &TrackItem::sigClipAdded, this, &ClipShapeItem::sltClipAdded);
    connect(trackItem.data(), &TrackItem::sigDragClipMoved, this, &ClipShapeItem::sltUpdate);
    connect(trackItem.data(), &TrackItem::sigClipRemoved, this, &ClipShapeItem::sltDragClipRemoved);

    connect(_clipArea, &ClipArea::sigClipItemMoving, this, &ClipShapeItem::sltClipItemMoving);
    connect(_clipArea, &ClipArea::sigClipItemRemoved, this, &ClipShapeItem::sltClipItemRemoved);
    connect(_clipArea, &ClipArea::sigClipItemEdited, this, &ClipShapeItem::sltClipItemEdited);
}

void ClipShapeItem::sltClipItemRemoved()
{
    QMutexLocker lock(&m_mutex);
    for (int i = 0; i < m_movingItems.size(); i++) {
        m_pendingRemovals.push_back(m_movingItems[i]);
    }
    m_movingItems.clear();

    m_dirty.storeRelease(1);
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void ClipShapeItem::sltClipItemEdited()
{

    QMutexLocker lock(&m_mutex);
    auto clips = _clipArea->movingItems();

    for (int i = 0; i < clips.size(); i++) {

        if (clips[i].clipItem == nullptr) {
            continue;
        }

        if (!m_movingItems.contains(clips[i].clipItem->clipIndex()) && clips[i].trackIndex == _trackIndex) {

            // qDebug() << "clips[i].clipItem" << i << clips[i].clipItem->clipIndex() << m_movingItems.size() << _trackIndex;
            m_pendingUpserts.insert(clips[i].clipItem->clipIndex(), clips[i].clipItem);
            m_movingItems.append(clips[i].clipItem->clipIndex());
        } else if (m_movingItems.contains(clips[i].clipItem->clipIndex()) && clips[i].trackIndex != _trackIndex) {
            // qDebug() << "clips[i].clipItem2" << i << clips[i].clipItem->clipIndex() << m_movingItems.size() << _trackIndex;
            m_pendingRemovals.push_back(clips[i].clipItem->clipIndex());
            m_movingItems.removeAll(clips[i].clipItem->clipIndex());
        }
    }
    m_dirty.storeRelease(1);

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void ClipShapeItem::sltClipItemMoving()
{

    QMutexLocker lock(&m_mutex);
    auto clips = _clipArea->movingItems();

    for (int i = 0; i < clips.size(); i++) {

        if (clips[i].clipItem == nullptr) {
            continue;
        }
        if (!m_movingItems.contains(clips[i].clipItem->clipIndex()) && clips[i].trackIndex == _trackIndex) {

            // qDebug() << "clips[i].clipItem" << i << clips[i].clipItem->clipIndex() << m_movingItems.size() << _trackIndex;
            m_pendingUpserts.insert(clips[i].clipItem->clipIndex(), clips[i].clipItem);
            m_movingItems.append(clips[i].clipItem->clipIndex());
        } /*else if (m_movingItems.contains(clips[i].clipItem->clipIndex()) && clips[i].trackIndex != _trackIndex) {
            qDebug() << "clips[i].clipItem2" << i << clips[i].clipItem->clipIndex() << m_movingItems.size() << _trackIndex;
            m_pendingRemovals.push_back(clips[i].clipItem->clipIndex());
            m_movingItems.removeAll(clips[i].clipItem->clipIndex());
        }*/
    }
    m_dirty.storeRelease(1);
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void ClipShapeItem::sltUpdate()
{
    update();
}
void ClipShapeItem::applyPending()
{
    QMap<QString, QSharedPointer<ClipItem>> upserts;
    QVector<QString> removals;
    {
        QMutexLocker lock(&m_mutex);
        upserts.swap(m_pendingUpserts);
        removals.swap(m_pendingRemovals);
        m_dirty.storeRelease(0);
    }
    for (QString id : std::as_const(removals))
        m_items.remove(id);
    for (auto it = upserts.begin(); it != upserts.end(); ++it)
        m_items.insert(it.key(), it.value());
    for (QString id : std::as_const(removals))
        m_items.remove(id);
    // qDebug() << "applyPending";
}

void ClipShapeItem::sltDragClipAdded()
{
    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);
    auto dragClipItem = trackItem->dragClipItem();

    // qDebug() << "sltDragClipAdded" << dragClipItem->clipIndex();

    m_pendingUpserts.insert(dragClipItem->clipIndex(), dragClipItem);
    m_dirty.storeRelease(1);
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}
void ClipShapeItem::sltClipAdded(QString id)
{
    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);
    auto clipItem = trackItem->clipItems(id);

    qDebug() << "sltClipAdded" << id;

    m_pendingUpserts.insert(clipItem->clipIndex(), clipItem);
    m_dirty.storeRelease(1);
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void ClipShapeItem::sltDragClipRemoved(QString id)
{

    // qDebug() << "sltDragClipRemoved" << id;

    QMutexLocker lock(&m_mutex);
    m_pendingRemovals.push_back(id);
    m_dirty.storeRelease(1);
    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}
int ClipShapeItem::trackIndex() const
{
    return _trackIndex;
}

void ClipShapeItem::setTrackIndex(int index)
{
    _trackIndex = index;
}

QColor ClipShapeItem::getTitleColor()
{
    auto _tracksModel = _clipArea->tracksModel();
    auto trackItem = _tracksModel->trackItem(_trackIndex);
    return trackItem->getTitleColor();
}

QImage ClipShapeItem::renderToImage(qreal dpr, QString text)
{

    auto m_font = QFont(":/resources/font/SourceSansPro-Regular.ttf");
    QFontMetrics fm(m_font);

    // measure text bounding rect
    QRect rect = fm.boundingRect(text);
    QSize pxSize(rect.size()); // add padding

    QImage img(pxSize, QImage::Format_RGBA8888_Premultiplied);
    img.fill(Qt::transparent);

    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Background rect (fill)
    QRectF r(0, 0, width(), height()); // logical coords
    p.fillRect(r, getTitleColor());

    auto m_textColor = Qt::black;

    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(m_textColor);
    p.setFont(m_font);
    p.drawText(img.rect(), Qt::AlignLeft, text);
    p.end();

    return img;
}

ItemNode* ClipShapeItem::acquireItemNode(RootNode* root)
{
    ItemNode* n = nullptr;
    n = new ItemNode();
    return n;
}

void ClipShapeItem::releaseItemNode(RootNode* root, ItemNode* node)
{
    // Clean children (they will be rebuilt if reused)
    while (node->firstChild())
        node->removeChildNode(node->firstChild());

    for (int i = 0; i < node->nodes.size(); i++) {
        ClipNode n = node->nodes[i];
        delete n.minGeom;
        n.minGeom = nullptr;
        delete n.maxGeom;
        n.maxGeom = nullptr;
        delete n.jointGeom;
        n.jointGeom = nullptr;

        delete n.minMat;
        n.minMat = nullptr;
        delete n.maxMat;
        n.maxMat = nullptr;
        delete n.jointMat;
        n.jointMat = nullptr;

        n.minNode = n.maxNode = n.jointNode = nullptr;
    }
    delete node->textNode;
    delete node->titleNode;
    node->titleNode = nullptr;
    node->textNode = nullptr;
    node->id = "";
}

void ClipShapeItem::ensurePolylineGeom(QSGGeometryNode* node,
    QSGGeometry*& geom,
    int vertexCount,
    qreal lineWidth,
    bool aa)
{
    if (vertexCount < 1) {
        if (geom) {
            delete geom;
            geom = nullptr;
            node->setGeometry(nullptr);
        }
        return;
    }
    const bool needNew = !geom || geom->vertexCount() != vertexCount || geom->drawingMode() != QSGGeometry::DrawLineStrip;
    if (needNew) {

        delete geom;
        geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount);
        geom->setDrawingMode(QSGGeometry::DrawLineStrip);
        node->setGeometry(geom);
        node->setFlag(QSGNode::OwnsGeometry, false);
    }
    geom->setLineWidth(lineWidth);
    // geom->setLineJoinStyle(QSGGeometry::MiterJoin);
    geom->setDrawingMode(QSGGeometry::DrawLineStrip);
    geom->setVertexDataPattern(QSGGeometry::StaticPattern);
    node->setFlag(QSGNode::OwnsGeometry, false);
    // node->setAntialiasing(aa);
}

void ClipShapeItem::updatePolyline(QSGGeometry* geom, const QVector<QPointF>& pts)
{

    if (!geom || pts.isEmpty())
        return;

    auto* v = reinterpret_cast<QSGGeometry::Point2D*>(geom->vertexData());
    for (int i = 0; i < pts.size(); ++i) {
        v[i].set(pts[i].x(), pts[i].y());
    }
}
void ClipShapeItem::updateLinePairs(QSGGeometry* geom, const QVector<QPointF>& ptsPairs)
{
    if (!geom)
        return;
    const int pairs = (ptsPairs.size() / 2);
    auto* v = reinterpret_cast<QSGGeometry::Point2D*>(geom->vertexData());
    for (int i = 0; i < pairs; i++) {
        const QPointF& a = ptsPairs[2 * i];
        const QPointF& b = ptsPairs[2 * i + 1];
        v[2 * i].set(a.x(), a.y());
        v[2 * i + 1].set(b.x(), b.y());
    }
}

void ClipShapeItem::ensureLinesGeom(QSGGeometryNode* node,
    QSGGeometry*& geom,
    int vertexCount,
    qreal lineWidth,
    bool aa)
{
    if (vertexCount < 2) {
        if (geom) {
            delete geom;
            geom = nullptr;
            node->setGeometry(nullptr);
        }
        return;
    }
    const bool needNew = !geom || geom->vertexCount() != vertexCount || geom->drawingMode() != QSGGeometry::DrawLines;
    if (needNew) {
        delete geom;
        geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertexCount);
        geom->setDrawingMode(QSGGeometry::DrawLines);
        node->setGeometry(geom);
        node->setFlag(QSGNode::OwnsGeometry, false);
    }
    geom->setLineWidth(lineWidth);
    // geom->setLineJoinStyle(QSGGeometry::MiterJoin);
    geom->setDrawingMode(QSGGeometry::DrawLines);
    geom->setVertexDataPattern(QSGGeometry::StaticPattern);
    // node->setAntialiasing(aa);
}

static int layoutHeightPx(const QString& text, const QFont& font, int widthPx)
{
    if (text.isEmpty() || widthPx <= 0)
        return 0;

    QTextOption opt;
    opt.setWrapMode(QTextOption::WordWrap);

    QTextLayout layout(text, font);
    layout.setTextOption(opt);

    qreal y = 0.0;
    layout.beginLayout();
    while (true) {
        QTextLine line = layout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(qMax(1, widthPx));
        line.setPosition(QPointF(0, y));
        y += line.height();
    }
    layout.endLayout();
    return qCeil(y);
}

static int bestPixelSizeToFitHeight(
    const QString& text,
    QFont baseFont,
    int widthPx,
    int heightPx,
    int minPx = 6,
    int maxPx = 128) // you can expose these as properties if you like
{
    if (heightPx <= 0 || widthPx <= 0)
        return minPx;

    // If the user already set a pixelSize, respect it as the upper bound.
    if (baseFont.pixelSize() > 0) {
        maxPx = qMax(minPx, baseFont.pixelSize());
    }

    int lo = minPx, hi = qMax(minPx, maxPx), best = minPx;
    while (lo <= hi) {
        const int mid = (lo + hi) / 2;
        baseFont.setPixelSize(mid);
        const int h = layoutHeightPx(text, baseFont, widthPx);
        if (h <= heightPx) {
            best = mid;
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return best;
}
static QString fitTextToWidth(const QString& text,
    int maxWidthPx,
    const QFont& font)
{
    if (maxWidthPx <= 0 || text.isEmpty())
        return QString();

    // Normalize whitespace to single spaces, trim ends.
    const QString normalized = QString(text).simplified();
    if (normalized.isEmpty())
        return QString();

    QStringList words = text.split("", Qt::SkipEmptyParts);

    QFontMetrics fm(font);
    QString result;
    result.reserve(normalized.size()); // minor perf hint

    for (int i = 0; i < words.size(); ++i) {
        const bool first = result.isEmpty();
        const QString candidate = first ? words[i] : result + words[i];

        // Measure proposed text width in pixels.
        const int width = fm.horizontalAdvance(candidate);
        if (width <= maxWidthPx) {
            result = candidate; // accept and continue
        } else {
            break; // adding this word would overflow
        }
    }

    return result; // may be empty if even the first word didn't fit
}

QString ClipShapeItem::loadSourceSansPro()
{
    static QString cached;
    if (!cached.isEmpty())
        return cached;

    const int id = QFontDatabase::addApplicationFont(":/resources/font/SourceSansPro-Regular.ttf");
    if (id < 0) {
        qDebug() << "Failed to load SourceSansPro-Regular.ttf from resources";
        cached = QLatin1String("Sans Serif"); // fallback
        return cached;
    }
    const QStringList fams = QFontDatabase::applicationFontFamilies(id);
    if (fams.isEmpty()) {
        qDebug() << "Font loaded but no families exposed";
        cached = QLatin1String("Sans Serif");
        return cached;
    }
    cached = fams.first(); // e.g. "Source Sans Pro"
    return cached;
}

QSGNode* ClipShapeItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{

    auto* root = static_cast<RootNode*>(oldNode);
    if (!root)
        root = new RootNode;

    if (m_dirty.loadAcquire())
        applyPending();

    QVector<QString> removeList;
    QMapIterator<QString, ItemNode*> i(root->idToNode);
    while (i.hasNext()) {
        i.next();

        if (!m_items.contains(i.key())) {
            ItemNode* node = i.value();

            root->removeChildNode(node); // detach from scene graph
            releaseItemNode(root, node);
            removeList.append(i.key());
        }
    }
    for (int i = 0; i < removeList.size(); i++) {
        root->idToNode.remove(removeList[i]);
    }

    for (auto it = m_items.constBegin(); it != m_items.constEnd(); ++it) {
        QString id = it.key();
        auto data = it.value();

        ItemNode* item = root->idToNode.value(id, nullptr);
        if (!item) {

            item = acquireItemNode(root);
            item->id = id;
            root->idToNode.insert(id, item);
            root->appendChildNode(item);
        }

        QVector<QVector<QPointF>> minPoints;
        QVector<QVector<QPointF>> maxPoints;
        QVector<QVector<QPointF>> jointPoints;

        data->draw(minPoints, maxPoints, jointPoints);

        if (item->nodes.size() != minPoints.size()) {
            item->nodes.resize(minPoints.size());
        }

        auto _rect = boundingRect();

        const double startTime = _areaInfo->startTime();
        const double clipDur = _areaInfo->clipDuration();
        const double lastWidth = _areaInfo->lastWidth();

        const double pxPerMs = lastWidth / clipDur;

        double pix1d = (data->sampleClip()->startTime() - startTime) * pxPerMs;
        double pix2d = (data->sampleClip()->endTime() - startTime) * pxPerMs;

        if (!item->titleNode) {
            item->titleNode = new QSGSimpleRectNode(QRectF(pix1d, _rect.y(), pix2d - pix1d, _areaInfo->titleHeight()), data->baseColor());
            item->titleNode->setFlag(QSGNode::OwnedByParent);
            item->appendChildNode(item->titleNode);
        }
        if (!item->textNode) {
            item->textNode = new QSGSimpleTextureNode();
            item->appendChildNode(item->textNode);
        }

        if (!item->titleBorderNode) {
            item->titleBorderNode = new QSGSimpleRectNode(QRectF(pix1d, _rect.y() + _areaInfo->titleHeight(), pix2d - pix1d, 1), Qt::black);
            item->titleBorderNode->setFlag(QSGNode::OwnedByParent);
            item->appendChildNode(item->titleBorderNode);

            item->topSelectedArea = new QSGSimpleRectNode(QRectF(0, 0, 0, 0), "#96D4E8");
            item->topSelectedArea->setFlag(QSGNode::OwnedByParent);
            item->appendChildNode(item->topSelectedArea);
        }

        auto m_text = QFileInfo(data->sampleClip()->sampleBuffer()->audioFile()).baseName();

        if (pix1d < 0) {
            pix1d = 0;
        }

        auto w = pix2d - pix1d;

        auto start_pix_text = pix1d + ScreenInterface::scaleSize2(5, _areaInfo->zoomFactor());

        if (start_pix_text < 0)
            start_pix_text = 0;
        // auto m_font = QFont(":/Resource/font/SourceSansPro-Regular.ttf");
        QFont m_font(loadSourceSansPro());
        auto m_color = Qt::black;

        m_font.setPixelSize(ScreenInterface::scaleSize2(28, _areaInfo->zoomFactor()));
        m_font.setKerning(false);

        QFontMetrics fm(m_font);
        QRect rect = fm.boundingRect(m_text);
        QSize pxSize(rect.size()); // add padding

        // Render into an image that is exactly the rect width/height in device px
        QImage img(QSize(pxSize.width(), fm.height()), QImage::Format_ARGB32_Premultiplied);
        img.setDevicePixelRatio(window()->effectiveDevicePixelRatio());
        img.fill(Qt::transparent);

        auto textRect = QRect(start_pix_text, _areaInfo->titleHeight() / 2 - fm.height() / 2, img.width(), img.height());

        {

            QPainter p(&img);
            p.setRenderHint(QPainter::Antialiasing, true);
            p.setRenderHint(QPainter::TextAntialiasing, true);
            p.setFont(m_font);

            p.setPen(m_color);

            p.setClipRect(0, 0, w - ScreenInterface::scaleSize2(5, _areaInfo->zoomFactor()), _areaInfo->titleHeight());

            QString fitted = fitTextToWidth(m_text, w - ScreenInterface::scaleSize2(5, _areaInfo->zoomFactor()), m_font);

            p.drawText(QRect(0, 0, img.width(), fm.height()), Qt::AlignLeft | Qt::AlignVCenter, fitted);
        }

        item->tex = window()->createTextureFromImage(img);
        item->tex->setFiltering(QSGTexture::Linear);
        item->textNode->setTexture(item->tex);

        item->textNode->setRect(textRect);

        auto title_rect = QRectF(pix1d, _rect.y(), w, _areaInfo->titleHeight());

        item->titleNode->setRect(title_rect);
        item->titleNode->setColor(data->baseColor());

        auto titleBorderRect = QRectF(pix1d, _rect.y() + _areaInfo->titleHeight(), pix2d - pix1d, ScreenInterface::scaleSize2(5, _areaInfo->zoomFactor()));
        item->titleBorderNode->setRect(titleBorderRect);

        auto _tracksModel = _clipArea->tracksModel();
        auto trackItem = _tracksModel->trackItem(_trackIndex);

        {
            auto selectedAreaRect = _areaInfo->selectionAreaRect();

            double titleLeft = titleBorderRect.left();
            double viewLeft = pix1d;
            double viewRight = pix2d;
            double outY = _rect.y();
            double outH = ScreenInterface::scaleSize2(4, _areaInfo->zoomFactor());

            double selLeftRel = selectedAreaRect.left() - titleLeft;
            double selRightRel = selectedAreaRect.right() - titleLeft;

            // Compute viewport width in the same relative coordinate system:
            // treat the viewport left as 0 .. (viewRight - viewLeft)
            double viewportWidth = viewRight - viewLeft;

            // Clip selection to [0, viewportWidth] in title-relative coordinates
            double visibleLeft = std::max(0.0, std::min(selLeftRel, viewportWidth));
            double visibleRight = std::max(0.0, std::min(selRightRel, viewportWidth));

            // width can't be negative
            double w = visibleRight > visibleLeft ? (visibleRight - visibleLeft) : 0.0;

            // Convert visibleLeft back to global/viewport X coordinate.
            // visibleLeft is an offset inside the viewport (0..viewportWidth),
            // so globalX = viewLeft + visibleLeft
            double globalX = viewLeft + visibleLeft;

            auto selectedTopRect = QRectF(globalX, outY, w, outH);

            // qDebug() << "outH" << outH << outY << w << selectedAreaRect << viewLeft << viewRight;

            if (w > 0 && _trackIndex >= _areaInfo->selectionArea()->firstTrackIndex() && _trackIndex <= _areaInfo->selectionArea()->lastTrackIndex()) {
                item->topSelectedArea->setRect(selectedTopRect);
            } else {
                item->topSelectedArea->setRect(QRectF(0, 0, 0, 0));
            }
        }

        for (int i = 0; i < item->nodes.size(); i++) {

            if (!item->nodes[i].minMat) {

                item->nodes[i].minMat = new QSGFlatColorMaterial();
                item->nodes[i].minMat->setColor(Qt::black);
            }
            if (!item->nodes[i].maxMat) {

                item->nodes[i].maxMat = new QSGFlatColorMaterial();
                item->nodes[i].maxMat->setColor(Qt::black);
            }
            if (!item->nodes[i].jointMat) {

                item->nodes[i].jointMat = new QSGFlatColorMaterial();
                item->nodes[i].jointMat->setColor(Qt::black);
            }

            // --- MIN (LineStrip) ---

            if (!item->nodes[i].minNode) {
                item->nodes[i].minNode = new QSGGeometryNode();
            }

            if (!item->nodes[i].minGeom) {
                item->nodes[i].minGeom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
                item->nodes[i].minGeom->setDrawingMode(QSGGeometry::DrawLineStrip);
                item->nodes[i].minNode->setGeometry(item->nodes[i].minGeom);
                item->nodes[i].minMat->setColor(Qt::black);
                item->nodes[i].minNode->setMaterial(item->nodes[i].minMat);
                item->nodes[i].minNode->setFlag(QSGNode::OwnsMaterial);
            }

            if (item->nodes[i].minNode) {
                QSGGeometry* g = item->nodes[i].minNode->geometry();
                const int pairCount = minPoints[i].size();
                g->allocate(pairCount); // 2 verts per segment
                g->setDrawingMode(QSGGeometry::DrawLineStrip);
                updatePolyline(item->nodes[i].minGeom, minPoints[i]);
                item->nodes[i].minNode->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
            }

            if (!item->nodes[i].minNode->parent()) {
                item->appendChildNode(item->nodes[i].minNode);
            }

            // --- MAX (LineStrip) ---

            if (!item->nodes[i].maxNode) {
                item->nodes[i].maxNode = new QSGGeometryNode();
            }

            if (!item->nodes[i].maxGeom) {
                item->nodes[i].maxGeom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
                item->nodes[i].maxGeom->setDrawingMode(QSGGeometry::DrawLineStrip);
                item->nodes[i].maxNode->setGeometry(item->nodes[i].maxGeom);
                item->nodes[i].maxMat->setColor(Qt::black);
                item->nodes[i].maxNode->setMaterial(item->nodes[i].maxMat);
                item->nodes[i].maxNode->setFlag(QSGNode::OwnsMaterial);
            }

            if (item->nodes[i].maxNode) {
                QSGGeometry* g = item->nodes[i].maxNode->geometry();
                const int pairCount = maxPoints[i].size();
                g->allocate(pairCount); // 2 verts per segment
                g->setDrawingMode(QSGGeometry::DrawLineStrip);
                updatePolyline(item->nodes[i].maxGeom, maxPoints[i]);
                item->nodes[i].maxNode->markDirty(QSGNode::DirtyGeometry);
            }

            if (!item->nodes[i].maxNode->parent()) {
                item->appendChildNode(item->nodes[i].maxNode);
            }

            // --- JOINT (independent pairs) ---

            if (!item->nodes[i].jointNode) {
                item->nodes[i].jointNode = new QSGGeometryNode();
            }

            if (!item->nodes[i].jointGeom) {
                item->nodes[i].jointGeom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
                item->nodes[i].jointGeom->setDrawingMode(QSGGeometry::DrawLines);
                item->nodes[i].jointNode->setGeometry(item->nodes[i].jointGeom);
                item->nodes[i].jointMat->setColor(Qt::black);
                item->nodes[i].jointNode->setMaterial(item->nodes[i].jointMat);
                item->nodes[i].jointNode->setFlag(QSGNode::OwnsMaterial);
            }

            if (item->nodes[i].jointNode) {
                QSGGeometry* g = item->nodes[i].jointNode->geometry();
                const int pairs = (jointPoints[i].size() / 2);
                g->allocate(pairs * 2); // 2 verts per segment
                g->setDrawingMode(QSGGeometry::DrawLineStrip);

                updateLinePairs(item->nodes[i].jointGeom, jointPoints[i]);

                item->nodes[i].jointNode->markDirty(QSGNode::DirtyGeometry);
            }

            if (!item->nodes[i].jointNode->parent()) {
                item->appendChildNode(item->nodes[i].jointNode);
            }
        }
    }

    return root;
}
