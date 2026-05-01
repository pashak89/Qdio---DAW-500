#ifndef MARKERSHAPEITEM_H
#define MARKERSHAPEITEM_H

#include <QColor>
#include <QQuickItem>
#include <QRectF>
class AreaInfo;
class ClipArea;
class SampleClip;
class ClipShapeItem;
class AutomationLaneModel;
class MarkerShapeItem : public QQuickItem {
    Q_OBJECT

    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea READ clipArea WRITE setClipArea NOTIFY areaInfoChanged)

public:
    explicit MarkerShapeItem(QQuickItem* parent = nullptr);

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    ClipArea* clipArea() const;
    void setClipArea(ClipArea* clipArea);

signals:

    void areaInfoChanged();
    void trackIndexChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;

private:
    AreaInfo* _areaInfo = nullptr;
    ClipArea* _clipArea = nullptr;
};

#endif // MARKERSHAPEITEM_H
