#ifndef TRACKAREA_H
#define TRACKAREA_H

#include "audio/audioengine.h"
#include "audio/engine.h"
#include "audio/sampletrack.h"
#include "core/tracksmodel.h"
#include "measure.h"
#include "sampleeditor.h"
#include "ui/automationitem.h"
#include "ui/backgrounditem.h"
#include "ui/beatbaritem.h"
#include "ui/clipitem.h"
#include "ui/envelopeitem.h"
#include "ui/hotspotitem.h"
#include "ui/indicatoritem.h"
#include "ui/loopitem2.h"
#include "ui/markerareaitem.h"
#include "ui/plotitem.h"
#include "ui/scrubareaitem.h"
#include "ui/timeruleritem.h"
#include "ui/trackitem.h"
#include "ui/warpareaitem.h"
#include "warp.h"
#include <QAbstractListModel>
#include <QJsonArray>
#include <QQuickItem>
#include <QTimer>
#include <ui/timeruleritem.h>

class ClipArea;
class TrackArea : public INanoItem {

    Q_OBJECT
    Q_PROPERTY(AreaInfo* areaInfo READ areaInfo WRITE setAreaInfo NOTIFY areaInfoChanged)
    Q_PROPERTY(ClipArea* clipArea WRITE setClipArea NOTIFY areaInfoChanged)
    Q_PROPERTY(int trackIndex READ trackIndex WRITE setTrackIndex NOTIFY trackIndexChanged)
    Q_PROPERTY(TrackItem* masterTrack READ masterTrack NOTIFY masterTrackChanged)
    Q_PROPERTY(bool masterTrackVisible READ masterTrackVisible WRITE setMasterTrackVisible NOTIFY masterTrackVisibleChanged)
public:
    explicit TrackArea(QQuickItem* parent = Q_NULLPTR);
    void initialize();

    AreaInfo* areaInfo() const;
    void setAreaInfo(AreaInfo* areaInfo);

    TrackItem* masterTrack() const;

    ClipArea* clipArea() const;

    void setClipArea(ClipArea* clipArea);
    int trackIndex() const;
    void setTrackIndex(int newTrackIndex);

    bool masterTrackVisible() const;
    void setMasterTrackVisible(bool masterTrackVisible);

    bool isMatserTrack() const;
    void setIsMatserTrack(bool newIsMatserTrack);

    Q_INVOKABLE void setMasterTrackHeight(int posY);

private:
    AreaInfo* _areaInfo;

    ClipArea* _clipArea = Q_NULLPTR;
    TrackItem* _masterTrack = Q_NULLPTR;
    QSharedPointer<ClipItem> _draggingItem;
    bool _masterTrackVisible = true;
    int _listViewPositionChanged = 0;

    QRect _boundingRect;
    QTimer _timerWidth;
    int _trackIndex = -1;

    QTimer _updateTimer;

protected:
    QNanoQuickItemPainter* createItemPainter() const;

signals:
    void areaInfoChanged();
    void trackIndexChanged();
    void masterTrackVisibleChanged();
    void masterTrackChanged();

private slots:

    void sltAddVisr(int trackIndex, bool enabled, bool reverbEnabled);
    void sltMultiEqEffect(int trackIndex);
    void sltAddEqEffect(int trackIndex, QString name, bool enabled);
    void sltMasterRowsExpandedChanged(int index);

protected:
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);
};

class TrackAreaPainter : public INanoPainterItem {

private:
    int _trackIndex;

public:
    TrackAreaPainter(int trackIndex, const TrackArea* trackArea);

    // INanoPainterItem interface
public:
    void draw(IPainter* painter);

    const TrackArea* _trackArea;
};

#endif // TRACKAREA_H
