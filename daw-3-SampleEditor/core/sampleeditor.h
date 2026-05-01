#ifndef SAMPLEEDITOR_H
#define SAMPLEEDITOR_H

#include "measure.h"
#include "sampleeditor.h"
#include "ui/automationitem.h"
#include "ui/backgrounditem.h"
#include "ui/beatbaritem.h"
#include "ui/clipitem.h"
#include "ui/envelopeitem.h"
#include "ui/indicatoritem.h"
#include "ui/scrubareaitem.h"
#include "ui/markerareaitem.h"
#include "ui/plotitem.h"
#include "ui/loopitem2.h"
#include "ui/timeruleritem.h"
#include "ui/warpareaitem.h"
#include "ui/waveitem.h"
#include "ui/trackitem.h"
#include "warp.h"
#include <QQuickItem>

class SampleEditor;

class SampleEditor : public INanoItem {
    Q_OBJECT

    Q_PROPERTY(SampleClip* sampleClip READ sampleClip WRITE setSampleClip NOTIFY sampleClipChanged)
    Q_PROPERTY(bool automationEnabled READ automationEnabled WRITE setAutomationEnabled NOTIFY automationEnabledChanged)
public:
    explicit SampleEditor(QQuickItem* parent = Q_NULLPTR);

private:
    QSharedPointer<BackgroundItem> _backgroundItem;
    QSharedPointer<WarpAreaItem> _warpItem;
    QSharedPointer<BeatBarItem> _beatBarItem;
    QSharedPointer<PlotItem> _plotItem;
    QSharedPointer<WaveItem> _waveItem;

    QSharedPointer<ScrubAreaItem> _scrubAreaItem;
    QSharedPointer<Automation> _automation;
    QSharedPointer<VolumeAutomationItem> _automationItem;
    QSharedPointer<MarkerAreaItem> _markerItem;
    QTimer _timerWidth;
    AreaInfo* _areaInfo;
    QRect _boundingRect;
    ZoomLevel _minZoomLevel = ZoomLevel_Init;
    ZoomLevel _zoomLevel = ZoomLevel_Init;

    SampleClip* _sampleClip = Q_NULLPTR;

    double _startMarker;
    double _endMarker;

    bool _automationEnabled;

    QTimer _playbackUpdateTimer;

public:
    void paint(QPainter* painter);

    QSharedPointer<AutomationItem> automationItem() const;

    QSharedPointer<BackgroundItem> backgroundItem() const;

    QSharedPointer<BeatBarItem> beatBarItem() const;

    QSharedPointer<PlotItem> plotItem() const;

    SampleClip* sampleClip() const;
    void setSampleClip(SampleClip* sampleClip);

    QSharedPointer<WaveItem> waveItem() const;

    AreaInfo* areaInfo() const;

    QString startMarkerTime() const;
    void setStartMarkerTime(const QString& startMarkerTime);

    QString endMarkerTime() const;
    void setEndMarkerTime(const QString& endMarkerTime);

    double startMarker() const;
    void setStartMarker(double startMarker);

    double endMarker() const;
    void setEndMarker(double endMarker);

    QSharedPointer<WarpAreaItem> warpItem() const;


    QSharedPointer<ScrubAreaItem> scrubAreaItem() const;

    bool automationEnabled() const;
    void setAutomationEnabled(bool automationEnabled);

    QSharedPointer<MarkerAreaItem> markerItem() const;

protected:
    QNanoQuickItemPainter* createItemPainter() const;

private slots:
    void sltTimerWidthTimeout();
    void sltScrubCurrentIndicatorChanged(double time);
    void sltPlatBackUpdateTimeout();
signals:
    void lastWidthChanged();
    void sampleClipChanged();
    void automationEnabledChanged();

    // QQuickItem interface
protected:
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry);

    // QQuickItem interface
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void hoverMoveEvent(QHoverEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
};

class SampleEditorPainter : public INanoPainterItem {

public:
    SampleEditorPainter(const SampleEditor* sampleEditor);
    // INanoPainterItem interface
public:
    void draw(IPainter* painter);

private:
    const SampleEditor* _sampleEditor = Q_NULLPTR;
};

#endif // SAMPLEEDITOR_H
