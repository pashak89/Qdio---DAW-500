#ifndef TRACKSHAPEITEM_H
#define TRACKSHAPEITEM_H

#include "core/cliparea.h"
#include "trackitem.h"
#include <QColor>
#include <QQuickItem>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGNode>
#include <QSGSimpleRectNode>
#include <QSGVertexColorMaterial>
#include <QTimer>
#include <QUrl>

class AreaInfo;
class ClipArea;
class SampleClip;
class ClipShapeItem;
class ClipItem;

class MultiColorRectsNode : public QSGGeometryNode {
public:
    MultiColorRectsNode()
    {
        // Use vertex format with per-vertex color:
        QSGGeometry* geom = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0);
        geom->setDrawingMode(QSGGeometry::DrawTriangles); // Qt 6-safe (no GL_*)

        setGeometry(geom);
        setFlag(QSGNode::OwnsGeometry, true);

        // Material that uses vertex colors:
        auto* mat = new QSGVertexColorMaterial();
        setMaterial(mat);
        mat->setFlag(QSGMaterial::Blending, false);
        setFlag(QSGNode::OwnsMaterial, true);
    }
    // Rebuild all lines at once (call whenever your external data changes)
    void setLines(const QVector<RectSeg>& segs)
    {
        // 2 triangles per rect => 6 vertices
        const int vCount = segs.size() * 6;
        QSGGeometry* g = geometry();
        g->allocate(vCount);

        auto* v = g->vertexDataAsColoredPoint2D();
        int i = 0;

        for (const RectSeg& s : segs) {
            const quint8 r = qBound(0, int(s.color.redF() * 255.0f + 0.5f), 255);
            const quint8 g8 = qBound(0, int(s.color.greenF() * 255.0f + 0.5f), 255);
            const quint8 b = qBound(0, int(s.color.blueF() * 255.0f + 0.5f), 255);
            const quint8 a = qBound(0, int(s.color.alphaF() * 255.0f + 0.5f), 255);

            const float l = s.rect.left();
            const float rgt = s.rect.right();
            const float t = s.rect.top();
            const float btm = s.rect.bottom();

            v[i++].set(l, t, r, g8, b, a);
            v[i++].set(rgt, t, r, g8, b, a);
            v[i++].set(l, btm, r, g8, b, a);

            v[i++].set(rgt, t, r, g8, b, a);
            v[i++].set(rgt, btm, r, g8, b, a);
            v[i++].set(l, btm, r, g8, b, a);
        }
        g->markVertexDataDirty();
        markDirty(QSGNode::DirtyGeometry); // material is static
    }
};

class MultiColorLinesNode : public QSGGeometryNode {
public:
    MultiColorLinesNode()
    {
        // Per-vertex color format
        QSGGeometry* geom = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0);
        geom->setDrawingMode(QSGGeometry::DrawLines); // Qt 6-safe
        setGeometry(geom);
        setFlag(QSGNode::OwnsGeometry, true);

        // Material uses vertex colors
        auto* mat = new QSGVertexColorMaterial();
        mat->setFlag(QSGMaterial::Blending, false);
        setMaterial(mat);

        setFlag(QSGNode::OwnsMaterial, true);
    }

    void setLines(const QVector<LineSeg>& segs)
    {
        QSGGeometry* g = geometry();
        const int vCount = segs.size() * 2; // two vertices per line segment
        g->allocate(vCount);

        auto* v = g->vertexDataAsColoredPoint2D();
        int i = 0;

        for (const LineSeg& s : segs) {
            const quint8 r = qBound(0, int(s.color.redF() * 255.0f + 0.5f), 255);
            const quint8 g8 = qBound(0, int(s.color.greenF() * 255.0f + 0.5f), 255);
            const quint8 b = qBound(0, int(s.color.blueF() * 255.0f + 0.5f), 255);
            quint8 a = qBound(0, int(s.color.alphaF() * 255.0f + 0.5f), 255);

            v[i++].set(s.p1.x(), s.p1.y(), r, g8, b, a);
            v[i++].set(s.p2.x(), s.p2.y(), r, g8, b, a);
        }

        g->markVertexDataDirty();
        markDirty(QSGNode::DirtyGeometry); // material doesn’t change
    }
};

class TrackShapeItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea READ clipArea WRITE setClipArea NOTIFY areaInfoChanged)
    Q_PROPERTY(TrackItem* masterTrack READ masterTrack NOTIFY masterTrackChanged)
    Q_PROPERTY(bool masterTrackVisible READ masterTrackVisible WRITE setMasterTrackVisible NOTIFY masterTrackVisibleChanged)
    Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)

    Q_PROPERTY(int outputSampleRate READ outputSampleRate)

    struct ClipPlot {
        QRectF rect;
        QColor color;
        QSGSimpleRectNode* rectNode = nullptr;
    };

    struct PlotLine {
        QPointF p1;
        QPointF p2;
        QColor color;

        QSGGeometryNode* node;

        QSGGeometry* geom;

        QSGFlatColorMaterial* mat;
    };

public:
    struct PlotRect {
        QRectF rect;
        QColor color;

        QSGGeometryNode* node;
        QSGFlatColorMaterial* mat;
        QSGGeometry* geom;
    };

    explicit TrackShapeItem(QQuickItem* parent = nullptr);

    TrackItem* masterTrack() const;

    Q_INVOKABLE void setMasterTrackHeight(int posY);

    bool masterTrackVisible() const;
    void setMasterTrackVisible(bool masterTrackVisible);

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    ClipArea* clipArea() const;
    void setClipArea(ClipArea* clipArea);

    int trackIndex() const;
    void setTrackIndex(int index);

    int outputSampleRate();

    Q_INVOKABLE void addClip(ClipShapeItem* clip);

    Q_INVOKABLE void clearClips();

signals:
    void dragEntered(int trackIndex, QString clipIndex, int xPos, int width, QString fileName, QString color);
    void dragMoved(int trackIndex, QString clipIndex, int xPos, double startTime, double endTime);
    void dragLeft();
    void dropped(int xPos);
    void areaInfoChanged();
    void trackIndexChanged();
    void hoverMoved(int xPos, int yPos);

    void clipDragStarted(int trackIndex, QString clipIndex, QString fileName, int xPos, double startTime, double endTime);
    void clipDragUpdated(int xPos);
    void clipDragFinished();
    void clipsChanged();
    void masterTrackVisibleChanged();
    void masterTrackChanged();

protected:
    // QQuickItem DnD handlers
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    AreaInfo* _areaInfo = Q_NULLPTR;
    int _trackIndex;
    ClipArea* _clipArea = Q_NULLPTR;
    QSharedPointer<TrackItem> _masterTrack = Q_NULLPTR;

    QString _draggingClipIndex = "";
    bool _masterTrackVisible = true;
    // std::vector<PlotLine> m_lineNodes;
    MultiColorLinesNode* m_lines_node = nullptr;
    MultiColorRectsNode* m_rects_node = nullptr;
    MultiColorRectsNode* m_selected_rects_node = nullptr;
    std::vector<PlotRect> m_rectNodes;

    QMap<QString, ClipPlot> m_clipNodes;

    QSGGeometryNode* m_bgNode = nullptr;

    double m_stepColorSize = 0;
    double m_realStartX = 0;
    double m_final_size = 0;
    double m_offsetPix = 0;
    double m_lastWidth = 0;
    double m_clipDuration = 0;
    double m_startTime = 0;
    double m_endX = 0;
    double m_posX = 0;

    int _adaptiveGrid;
    int _fixedGrid;
    bool _offGrid = false;

    QTimer _timerWidth;

    bool _dragEntered = false;
    static void findSnap(QVector<QPair<double, double>> _snapPoints, double posX, double posY, double& final_xpos, double& final_ypos);
    bool acceptMime(const QMimeData* m) const; // customize what you accept

    QVector<RectSeg> m_rectsA;
    QVector<LineSeg> m_linesA;
    QVector<RectSeg> m_selected_rects;

    // QQuickItem interface
protected:
    void hoverMoveEvent(QHoverEvent* event);

    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*);

private slots:
    void sltTimerWidthTimeout();

    // QQuickItem interface
    void sltAddEqEffect(int trackIndex, QString name, bool enabled);

    void sltAddVisr(int trackIndex, bool enabled, bool reverbEnabled);

    void sltClipPropertiesChanged();

protected:
    void hoverLeaveEvent(QHoverEvent* event);
};
#endif // TRACKSHAPEITEM_H
