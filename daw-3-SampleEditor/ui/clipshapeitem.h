#ifndef CLIPSHAPEITEM_H
#define CLIPSHAPEITEM_H

#include "audio/sampleclip.h"
#include "core/beatbar.h"
#include "core/global.h"

#include <QColor>
#include <QQuickItem>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGNode>
#include <QTimer>
#include <QUuid>

class Track;
class ClipArea;
class ClipItem;
class AreaInfo;
class AutomationLaneModel;
class QSGSimpleRectNode;
class QSGSimpleTextureNode;

struct ClipNode {
    QSGGeometryNode* minNode = nullptr;
    QSGGeometryNode* maxNode = nullptr;
    QSGGeometryNode* jointNode = nullptr;

    QSGGeometry* minGeom = nullptr;
    QSGGeometry* maxGeom = nullptr;
    QSGGeometry* jointGeom = nullptr;

    class QSGFlatColorMaterial* minMat = nullptr;
    class QSGFlatColorMaterial* maxMat = nullptr;
    class QSGFlatColorMaterial* jointMat = nullptr;
};

struct ItemNode : public QSGNode {
    QVector<ClipNode> nodes;

    QSGSimpleRectNode* titleBorderNode = nullptr;
    QSGSimpleRectNode* titleNode = nullptr;
    QSGSimpleTextureNode* textNode = nullptr;
    QSGSimpleRectNode* topSelectedArea = nullptr;
    QSGTexture* tex = nullptr;
    QString id = "";
};

struct RootNode : public QSGNode {
    // live nodes
    QMap<QString, ItemNode*> idToNode;

    ~RootNode()
    {
        // delete children automatically (OwnedByParent), but also clear maps
        idToNode.clear();
    }
};

class ClipShapeItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea READ clipArea WRITE setClipArea NOTIFY areaInfoChanged)
    Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)

public:
    ClipShapeItem(QQuickItem* parent = nullptr);
    ~ClipShapeItem();

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    ClipArea* clipArea() const;
    void setClipArea(ClipArea* clipArea);

    int trackIndex() const;
    void setTrackIndex(int index);

    QColor getTitleColor();

private:
    AreaInfo* _areaInfo;
    ClipArea* _clipArea;
    int _trackIndex;

    QMap<QString, QSharedPointer<ClipItem>> m_items;
    QMap<QString, QSharedPointer<ClipItem>> m_pendingUpserts;

    QAtomicInt m_dirty { 0 };
    QMutex m_mutex;
    QVector<QString> m_pendingRemovals;

    QVector<QString> m_movingItems;

private:
    QImage renderToImage(qreal dpr, QString text);

    void applyPending();
    void releaseItemNode(RootNode* root, ItemNode* node);
    void ensurePolylineGeom(QSGGeometryNode* node, QSGGeometry*& geom, int vertexCount, qreal lineWidth, bool aa);
    void updatePolyline(QSGGeometry* geom, const QVector<QPointF>& pts);
    void updateLinePairs(QSGGeometry* geom, const QVector<QPointF>& ptsPairs);
    void ensureLinesGeom(QSGGeometryNode* node, QSGGeometry*& geom, int vertexCount, qreal lineWidth, bool aa);
    QString loadSourceSansPro();

signals:
    void trackIndexChanged();
    void areaInfoChanged();

protected:
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);
private slots:
    void sltDragClipAdded();
    ItemNode* acquireItemNode(RootNode* root);
    void sltDragClipRemoved(QString id);
    void sltClipAdded(QString id);
    void sltUpdate();
    void sltClipItemMoving();
    void sltClipItemRemoved();
    void sltClipItemEdited();
};

#endif // CLIPSHAPEITEM_H
