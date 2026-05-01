#ifndef CLIPITEM_H
#define CLIPITEM_H

#include <QMutex>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QTimer>

#include "audio/sampleclip.h"
#include "core/beatbar.h"
#include "ui/ipainteritem.h"
#include <QUuid>

class SampleEditor;
class Track;
class ClipArea;
class AreaInfo;
class AutomationLaneModel;
class ClipItem : public IPainterItem {
    Q_OBJECT
public:
    enum MouseAction {
        MouseAction_None,
        MouseAction_ResizeRight,
        MouseAction_ResizeLeft,
        MouseAction_Move,
    };

    ClipItem(int trackIndex, QColor titleColor, QString clipIndex, AreaInfo* areaInfo, SampleClip* sampleClip = Q_NULLPTR);
    ClipItem(ClipItem& item, Track* track, bool isFake = false);
    QSharedPointer<ClipItem> clone(Track* track, bool isFake = false)
    {
        return QSharedPointer<ClipItem>(new ClipItem(*this, track, isFake));
    }

    void setParameters(double startTime, double endTime, double startOffset, double endOffset);
    virtual ~ClipItem();

    void setBaseColor(QColor titleColor);

    void setAutomationLaneModel(QSharedPointer<AutomationLaneModel> automationLaneModel);
    void setRowsExpandedCount(int count);
    void setAutomationLaneEnabled(bool status);
    // IPainterItem interface
public:
    void draw(IPainter* painter);
    void draw(QVector<QVector<QPointF>>& minPoints,
        QVector<QVector<QPointF>>& maxPoints,
        QVector<QVector<QPointF>>& points);

private:
    QSharedPointer<AutomationLaneModel> _automationLaneModel = Q_NULLPTR;
    AreaInfo* _areaInfo;
    std::mutex m_mutex;
    QString m_baseFileName;
    int _rowsExpandedCount = 0;
    int _titleHeight;
    int _seperatorHeight;

    double _lastDuration = 0;
    double _lastStartffset = 0;
    bool _automationLaneEnabled = false;

    double _tempo;

    int _trackIndex = 0;
    QString _clipIndex = 0;

    MouseAction _mouseHoverAction = MouseAction_None;
    MouseAction _pressAndHoldAction = MouseAction_None;

    QMap<int, float*> min;
    QMap<int, float*> max;

    QMap<int, float*> min_;
    QMap<int, float*> max_;

    SampleClip* _sampleClip;
    QColor _baseColor;

    QPointF _lastMouseMovingPoint;

public:
    int mousePressEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int hoverMoveEvent(QHoverEvent* event);
    int mouseReleaseEvent(QMouseEvent* event);

    bool containsPixel(double pixel);
    bool containsTime(double time);

    SampleClip* sampleClip() const;

    void setTrackIndex(int trackIndex);

    void setClipIndex(QString clipIndex);

    int trackIndex() const;

    QString clipIndex() const;

    double mousePressY() const;

    double startTime() const;

    MouseAction mouseAction() const;

    MouseAction pressAndHold() const;

    void setPressAndHold(const MouseAction& pressAndHold);

    void setMouseAction(const MouseAction& mouseAction);

    ClipArea* clipArea() const;

    QVector<QPair<double, double>> snapPoints() const;

    AreaInfo* areaInfo() const;

    QColor baseColor() const;

    QString baseFileName() const;

    int rowsExpandedCount() const;
};

#endif // CLIPITEM_H
