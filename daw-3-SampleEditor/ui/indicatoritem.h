#ifndef POSITIONERPLAYERITEM_H
#define POSITIONERPLAYERITEM_H
#include "trackitem.h"

#include <QColor>
#include <QQuickItem>
#include <QRectF>
#include <QTimer>
class AreaInfo;
class ClipArea;
class SampleClip;
class ClipShapeItem;
class AutomationLaneModel;
class IndicatorShapeItem : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea READ clipArea WRITE setClipArea NOTIFY areaInfoChanged)
    Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)
public:
    explicit IndicatorShapeItem(QQuickItem* parent = nullptr);

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    ClipArea* clipArea() const;
    void setClipArea(ClipArea* clipArea);

    int trackIndex() const;
    void setTrackIndex(int index);

    int lastRowExpanded();
    int rowsExpandedCount();
    double currentIndicator() const;

    TrackItem::IndicatorType indicatorEnabled() const;

    bool automationLaneEnabled();
    QSharedPointer<AutomationLaneModel> automationLaneModel();

signals:

    void areaInfoChanged();
    void trackIndexChanged();

    void indicatorEnabledChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;

private:
    AreaInfo* _areaInfo = nullptr;
    ClipArea* _clipArea = nullptr;
    QTimer* m_blinkTimer = nullptr;
    int m_trackIndex = -1;

    bool m_blinkState = false;
};

#endif // POSITIONERPLAYERITEM_H
