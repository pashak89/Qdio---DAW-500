#ifndef PLOTSHAPEITEM_H
#define PLOTSHAPEITEM_H

#include "core/global.h"

#include "trackshapeitem.h"

#include <QColor>
#include <QQuickItem>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGNode>
#include <QSGSimpleRectNode>

class PlotShapeItem : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea READ clipArea WRITE setClipArea NOTIFY areaInfoChanged)
    struct PlotLine {
        QPointF p1;
        QPointF p2;
        QColor color;
        QColor selected_color;
        QPointF selected_p1;
        QPointF selected_p2;
        QSGGeometryNode* selected_node = nullptr;
        QSGGeometryNode* node;

        QSGGeometry* selected_geom = nullptr;
        QSGGeometry* geom;

        QSGFlatColorMaterial* selected_mat = nullptr;
        QSGFlatColorMaterial* mat;
    };
    struct PlotRect {
        QRectF rect;
        QColor color;
        QColor selected_color;
        QRectF selected_rect;
        QSGSimpleRectNode* selected_node = nullptr;
        QSGGeometryNode* node;
        QSGFlatColorMaterial* mat;
        QSGGeometry* geom;
    };

public:
    PlotShapeItem(AreaInfo* areaInfo = Q_NULLPTR, QQuickItem* parent = nullptr);

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    ClipArea* clipArea() const;
    void setClipArea(ClipArea* newClipArea);

signals:

    void areaInfoChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;

private slots:
    void sltTimerWidthTimeout();

    void sltClipPropertiesChanged();

private:
    QTimer _timerWidth;
    double _lastDuration = 0;
    double _lastStartffset = 0;

    bool _move = false;
    QVector<RectSeg> m_rectsA;
    QVector<LineSeg> m_linesA;
    QVector<bool> _pixs;
    QVector<bool> _pixs_temp;
    double _pixR = 0;

    int _adaptiveGrid;
    int _fixedGrid;
    bool _offGrid = false;

    ClipArea* _clipArea;
    AreaInfo* _areaInfo;

    QSGGeometryNode* m_bgNode = nullptr;

    MultiColorLinesNode* m_lines_node = nullptr;
    MultiColorRectsNode* m_rects_node = nullptr;
    std::vector<TrackShapeItem::PlotRect> m_rectNodes;

    double m_stepColorSize = 0;
    double m_realStartX = 0;
    double m_final_size = 0;
    double m_offsetPix = 0;
    double m_lastWidth = 0;
    double m_clipDuration = 0;
    double m_startTime = 0;
    double m_endX = 0;
    double m_posX = 0;
};

#endif // PLOTSHAPEITEM_H
