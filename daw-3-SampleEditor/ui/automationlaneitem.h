#ifndef AUTOMATIONLANEITEM_H
#define AUTOMATIONLANEITEM_H

#include <QColor>
#include <QQuickItem>
#include <QRectF>
class AreaInfo;
class ClipArea;
class SampleClip;
class ClipShapeItem;
class AutomationLaneModel;
class AutomationLaneItem : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea READ clipArea WRITE setClipArea NOTIFY areaInfoChanged)
    Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)
public:
    explicit AutomationLaneItem(QQuickItem* parent = nullptr);

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

private:
    int m_trackIndex = -1;
    AreaInfo* _areaInfo = nullptr;
    ClipArea* _clipArea = nullptr;
};
#endif // AUTOMATIONLANEITEM_H
