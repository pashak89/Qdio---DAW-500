#ifndef AUTOMATIONSHAPEITEM_H
#define AUTOMATIONSHAPEITEM_H

#include "automationitem.h"

#include <QColor>
#include <QFontDatabase>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRectF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>
#include <QSGVertexColorMaterial>
class AreaInfo;
class ClipArea;
class SampleClip;
class ClipShapeItem;
class AutomationLaneModel;
struct ColoredLine {
    QLineF line;
    QColor color;

    double curve_m { 1.0 }; // straight if m==1 && n==1
    double curve_n { 1.0 };

    double startX;
    double endX;

    bool p1_circle = false;
    bool p2_circle = false;
    bool p1_filled = false;
    bool p2_filled = false;
};

struct ColoredPoint {
    QPointF line;
    QColor color;
    bool fill = false;
    // 0 = circle (Bezier / default), 1 = diamond (Linear), 2 = square (Hold).
    // Numbers match KeyFramesType in ui/keyframesitem.h.
    int shape = 0;
};

class CircleMarkersNode : public QSGGeometryNode {
public:
    CircleMarkersNode()
    {
        setFlag(QSGNode::OwnsGeometry, true);
        setFlag(QSGNode::OwnsMaterial, true);

        m_geom = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 0, 0);
        m_geom->setDrawingMode(QSGGeometry::DrawLines);
        setGeometry(m_geom);

        m_mat = new QSGVertexColorMaterial();
        m_mat->setFlag(QSGMaterial::Blending, true);
        setMaterial(m_mat);
    }
    struct CircleMarker {
        QPointF p;
        QColor color;
        bool fill = false;
        int shape = 0;
    };
    void setPoints(const QVector<ColoredPoint>& points)
    {
        m_points = points;
    }
    void rebuild();

    bool show() const;
    void setShow(bool newShow);

private:
    bool m_show = false;
    QSGVertexColorMaterial* m_mat { nullptr };
    QVector<ColoredPoint> m_points;
    QSGGeometry* m_geom { nullptr };
};

class ColoredLinesNode : public QSGGeometryNode {
public:
    ColoredLinesNode();
    enum class JointMarker { None,
        Filled,
        Hollow };
    void setLines(const QVector<ColoredLine>& lines);
    void setThickness(float t);
    void rebuild();

    void setJointMarker(JointMarker jm) { m_jointMarker = jm; }
    void setJointRadius(float r) { m_jointRadius = qMax(0.f, r); }
    void setJointRingWidth(float w) { m_jointRingWidth = qMax(0.5f, w); }
    void setJointColor(const QColor& c)
    {
        m_jointColor = c;
        m_useLineColorForJoint = false;
    }
    void useLineColorForJoint(bool on) { m_useLineColorForJoint = on; }

    void setJointBackgroundColor(const QColor& c) { m_jointBgColor = c; }
    void setUseJointBgEraser(bool on) { m_useJointBgEraser = on; }

    bool show() const;
    void setShow(bool newShow);

    void setPixelSnap(bool on) { m_pixelSnap = on; }
    void setSuperellipseSteps(int s) { m_superSteps = qBound(8, s, 512); }

    void setBoundingRect(QRectF rect)
    {
        _boundingRect = rect;
    }

private:
    QRectF _boundingRect;
    bool _show = false;
    QVector<ColoredLine> m_lines;

    // In private members:
    QColor m_jointBgColor { "#4F4F4F" }; // set this to your scene/bg color
    bool m_useJointBgEraser { true }; // enable the eraser disk

    // curves
    int m_superSteps { 64 }; // increase to 96/128 if very long or curvy

    float m_thickness { 2.f };
    float m_feather { 1.f }; // kept for compatibility
    bool m_pixelSnap { true };

    // joint marker config (defaults: filled, radius=4, width=1.5)
    JointMarker m_jointMarker { JointMarker::Filled };
    float m_jointRadius { 4.f };
    float m_jointRingWidth { 1.5f };
    QColor m_jointColor { Qt::white };
    bool m_useLineColorForJoint { true };

    QSGVertexColorMaterial* m_mat { nullptr };

    QSGGeometry* m_geom { nullptr };
};

class CircleNode : public QSGGeometryNode {
public:
    CircleNode();
    void setCenter(const QPointF& c);
    void setRadius(float r);
    void setColor(const QColor& c);
    void updateGeometryIfNeeded();

    float radius() const;

private:
    void rebuild();
    bool n_show = false;

    QPointF m_center { 0, 0 };
    float m_radius { 4.f };
    QColor m_color { "#eeeeee" };
    bool m_dirty { true };
    QSGFlatColorMaterial* m_mat { nullptr };
};

class DashedLineNode : public QSGGeometryNode {
public:
    DashedLineNode(AreaInfo* areaInfo, QSharedPointer<AutomationItem> automationItem);

    void rebuild(QRectF _boundingRect);

    bool show() const;
    void setShow(bool newShow);

    QColor color() const;
    void setColor(const QColor& newColor);

    QLineF line() const;
    void setLine(const QLineF& newLine);

    bool hasGap() const;
    void setHasGap(bool newHasGap);

private:
    bool _show = false;
    QLineF m_line;
    bool m_hasGap = true;
    QColor m_color = "#FFA500";
    AreaInfo* _areaInfo;
    QVector<ColoredLine> m_lines;
    QSharedPointer<AutomationItem> _automationItem;
    QSGFlatColorMaterial* m_mat { nullptr };
    QSGGeometry* geom { nullptr };
};

class LabelBoxNode : public QSGNode {
public:
    LabelBoxNode(QQuickWindow* win, AreaInfo* areaInfo, ClipArea* clipArea);

    // API
    void setPosition(QPointF pos)
    {
        m_pos = pos;
        m_dirty = true;
    }
    void setLabel(const QString& s)
    {
        m_label = s;
        m_dirty = true;
    }
    void setPadding(float pad)
    {
        m_padding = qMax(0.f, pad);
        m_dirty = true;
    }
    void setBackgroundColor(const QColor& c)
    {
        m_bgColor = c;
        m_dirty = true;
    }
    void setTextColor(const QColor& c)
    {
        m_textColor = c;
        m_dirty = true;
    }
    void setFont(const QFont& f)
    {
        m_font = f;
        m_dirty = true;
    }
    void setVisible(bool v)
    {
        m_visible = v;
        m_dirty = true;
    }

    QString loadSourceSansPro();

    QImage renderToImage(QString m_text);
    QSize renderToSize(QString m_text);
    // Call from your item's updatePaintNode
    void rebuild();

    bool show() const;
    void setShow(bool newShow);

    void ensureDummyTextureBound()
    {
        if (!m_window)
            return;

        if (!m_texture) {
            QImage dummy(1, 1, QImage::Format_RGBA8888_Premultiplied);
            dummy.fill(Qt::transparent);
            QSGTexture* tex = m_window->createTextureFromImage(dummy);
            tex->setFiltering(QSGTexture::Nearest);
            m_texture.reset(tex);
        }
        if (!m_hasTextureBound) {
            m_textNode->setTexture(m_texture.data()); // bind once
            m_textNode->setOwnsTexture(false);
            m_hasTextureBound = true;
        }
    }

private:
    bool m_hasTextureBound { false };
    QQuickWindow* m_window { nullptr };
    QSGSimpleRectNode* m_bg { nullptr };
    QSGSimpleTextureNode* m_textNode { nullptr };
    QSharedPointer<QSGTexture> m_texture; // keep texture alive

    ClipArea* _clipArea;
    AreaInfo* _areaInfo;

    // state
    QPointF m_pos { 0, 0 }; // top-left position where text starts (like your x,y)
    QString m_label { "Label" };
    float m_padding { 2 }; // like your +/-5 in the rect painter
    QColor m_bgColor { Qt::black };
    QColor m_textColor { Qt::white };
    QFont m_font;
    bool m_visible { false };
    bool m_dirty { true };
};
class AutomationNode : public QSGGeometryNode {
public:
    AutomationNode(QString name, QQuickWindow* window, AreaInfo* areaInfo, ClipArea* clipArea, QSharedPointer<TrackItem> trackItem, QSharedPointer<AutomationItem> automationItem);
    void updateGeometryIfNeeded(QRectF boundingRect);

    bool show() const;
    void setShow(bool newShow);

    void applyPending(QVector<ColoredLine> m_pendingUpserts, QVector<ColoredPoint> m_pendingPoints, QVector<ColoredLine> m_pendingSelectedLines);

    QSharedPointer<AutomationItem> automationItem() const;

private:
    QSharedPointer<TrackItem> _trackItem;
    QSharedPointer<AutomationItem> _automationItem;
    void rebuild();
    bool m_dirty { true };
    QRectF _boundingRect;
    DashedLineNode* _dashedNode = nullptr;
    DashedLineNode* _redDashedNode = nullptr;
    LabelBoxNode* _labelBoxNode = nullptr;
    CircleNode* _hoverCircleNode = nullptr;
    ClipArea* _clipArea;
    AreaInfo* _areaInfo;
    bool _show = false;
    QQuickWindow* _window;

    QString _name;

    QMutex m_mutex;
    bool m_dirty2 = false;
    bool doReplace = false;
    bool m_replaceAllPending = false;
    QVector<ColoredLine> m_replaceAllBuffer;
    QVector<ColoredLine> m_replaceAllSelectedLines;
    QVector<ColoredPoint> m_replaceAllPoints;
    QVector<ColoredLine> m_lines;
    QVector<ColoredLine> m_selectedLines;
    QVector<ColoredPoint> m_points;
    bool m_geomDirty = false;

    ColoredLinesNode* _linesNode = Q_NULLPTR;
    ColoredLinesNode* _selectedLinesNode = Q_NULLPTR;
    CircleMarkersNode* _circleNode = Q_NULLPTR;
    QSGFlatColorMaterial* m_mat { nullptr };

    QPointF drawLevel(QSize imgSize, QPointF point);
};

class AutomationShapeItem : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea READ clipArea WRITE setClipArea NOTIFY areaInfoChanged)
    Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)
public:
    explicit AutomationShapeItem(QQuickItem* parent = nullptr);

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    ClipArea* clipArea() const;
    void setClipArea(ClipArea* clipArea);

    bool automationLaneEnabled();
    QSharedPointer<AutomationLaneModel> automationLaneModel();

    int trackIndex() const;
    void setTrackIndex(int index);

signals:
    void areaInfoChanged();
    void trackIndexChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;

    QSGNode* rootNode = nullptr;
    int m_trackIndex = -1;
    AreaInfo* _areaInfo = nullptr;
    ClipArea* _clipArea = nullptr;

    AutomationNode* _volumeAutomationNode = nullptr;
    AutomationNode* _panAutomationNode = nullptr;
    AutomationNode* _speakerAutomationNode = nullptr;
    AutomationNode* _keyFramesAutomationNode = nullptr;

    QMap<QString, AutomationNode*> _automations;

    QVector<ColoredPoint> m_pendingPoints;
    QVector<ColoredLine> m_pendingUpserts;
    QVector<ColoredLine> m_pendingSelectedLines;
    QAtomicInt m_dirty { 0 };
    QMutex m_mutex;

private slots:

    void sltUpdateAutomation(AutomationItem* item);

    void sltUpdate();
    void sltModelUpdate();
};
#endif // AUTOMATIONSHAPEITEM_H
