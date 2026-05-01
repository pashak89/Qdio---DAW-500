#ifndef RUBBERBANDITEM_H
#define RUBBERBANDITEM_H

#include <QColor>
#include <QQuickItem>
#include <QRectF>
class AreaInfo;
class ClipArea;
class SampleClip;
class ClipShapeItem;
class AutomationLaneModel;
class RubberBandItem : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
    Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(qreal borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)

    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea READ clipArea WRITE setClipArea NOTIFY areaInfoChanged)

    Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)

public:
    explicit RubberBandItem(QQuickItem* parent = nullptr);

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    ClipArea* clipArea() const;
    void setClipArea(ClipArea* clipArea);

    int trackIndex() const;
    void setTrackIndex(int index);

    QColor fillColor() const { return m_fill; }
    void setFillColor(const QColor& c);

    QColor borderColor() const { return m_border; }
    void setBorderColor(const QColor& c);

    qreal borderWidth() const { return m_borderWidth; }
    void setBorderWidth(qreal w);

    bool isActive() const { return m_active; }
    void setActive(bool on);

    int lastRowExpanded();
    int rowsExpandedCount();

    bool automationLaneEnabled();
    QSharedPointer<AutomationLaneModel> automationLaneModel();

signals:
    void selectionRectChanged();
    void fillColorChanged();
    void borderColorChanged();
    void borderWidthChanged();
    void activeChanged();
    void selectionFinished(QRectF rect); // emitted on mouse release (normalized to item coords)

    void areaInfoChanged();
    void trackIndexChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;

private:
    QRectF normalizedRect(const QPointF& a, const QPointF& b) const;

    bool _automationLaneEnabled;
    QColor m_fill = QColor(0, 120, 215, 60); // default: light translucent
    QColor m_border = QColor(0, 120, 215, 255);
    qreal m_borderWidth = 1.0;
    bool m_active = false;

    bool m_dragging = false;
    QPointF m_pressPos;

    AreaInfo* _areaInfo = nullptr;
    int _trackIndex;
    ClipArea* _clipArea = nullptr;
    QRect drawSelectedArea();
};

#endif // RUBBERBANDITEM_H
