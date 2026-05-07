#ifndef TACKITEM_H
#define TACKITEM_H

#include <QAbstractListModel>
#include <QMutex>
#include <QPainter>
#include <QQmlEngine>
#include <QQuickPaintedItem>
#include <QTimer>

#include "core/beatbar.h"
#include <audio/effectcontrols.h>

#include "ui/automationitem.h"
#include "ui/clipitem.h"
#include "ui/ipainteritem.h"
#include "ui/keyframesitem.h"

#include <core/global.h>
#include <core/warp.h>

class SampleEditor;
class ClipItem;
class ClipArea;
class AreaInfo;

class TrackItem;
class AutomationLaneModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int totalHeight READ totalHeight NOTIFY totalHeightChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum AutomationLaneEnum {
        ROLE_AutomationLaneIndex = Qt::UserRole + 1,
        ROLE_AutomationLaneMenuTitle,
        ROLE_AutomationLaneSubMenuTitle,
        ROLE_AutomationLaneRowCount,
        ROLE_AutomationLaneHeight,

        ROLE_Value,
        ROLE_Normalized,
        ROLE_AutomatedNormalized,
        ROLE_AutomatedValue,
        ROLE_AutomatedActive,
        ROLE_AutomatedEnabled,

        ROLE_DefaultNormalized,

        ROLE_Clear,
        // m_roleNames[TrackProperty::ROLE_Clear] = "clear";

    };
    Q_ENUM(AutomationLaneEnum)

    explicit AutomationLaneModel(AreaInfo* areaInfo, TrackItem* trackItem, QObject* parent = Q_NULLPTR);
    ~AutomationLaneModel();

    struct AutomationLane {
        int automationLaneIndex;
        QString automationLaneMenuTitle;
        QString automationLaneSubMenuTitle;
        int automationLaneRowCount;
    };

    Q_INVOKABLE void insert(QString menu, QString subMenu, int index, int rowCount);
    Q_INVOKABLE void append(QString menu, QString subMenu, int rowCount);
    Q_INVOKABLE void remove(QString menu);
    Q_INVOKABLE void remove(QString menu, QString subMenu);
    Q_INVOKABLE void remove(int index);

    Q_INVOKABLE bool isContained(QString menu, QString subMenu);

    int count();

    int size();

    int totalHeight();

    AutomationLane* getItem(int index)
    {

        if (index < 0 || index >= m_automationLanes.size()) {
            return nullptr;
        }

        return &m_automationLanes[index];
    }

    int getIndex(QString menu, QString subMenu)
    {
        QMutexLocker locker(&m_mutex);

        if (mapSubMenu.contains(menu + subMenu)) {
            return mapSubMenu[menu + subMenu];
        }
        return -1;
    }

    void removeIndex(QString menu, QString subMenu)
    {
        QMutexLocker locker(&m_mutex);

        if (mapSubMenu.contains(menu + subMenu)) {
            mapSubMenu.remove(menu + subMenu);
        }
    }

    void setIndex(QString menu, QString subMenu, int index)
    {
        QMutexLocker locker(&m_mutex);

        mapSubMenu[menu + subMenu] = index;
    }

    bool contains(QString menu, QString subMenu)
    {
        QMutexLocker locker(&m_mutex);

        return mapSubMenu.contains(menu + subMenu);
    }

    Q_INVOKABLE int getIndex(QPointF point);
    Q_INVOKABLE int getIndexGlobal(QPointF point);

    Q_INVOKABLE int totalRowCount();
    Q_INVOKABLE int totalRowCountBefore(int _index);
    Q_INVOKABLE int totalRowCountBefore2(int _index);

    Q_INVOKABLE void decrease(int i);

    Q_INVOKABLE void increase(int i);

    Q_INVOKABLE void setLaneHeight(int _index, int posY);

    void updateLanesHeight();

signals:
    void sigRowAdded(int index, int count, QString menu, QString subMenu);
    void sigRowRemoved(int count, QString menu, QString subMenu);
    void sigRowChanged();
    void countChanged();
    void totalHeightChanged();

    void sigValueChanged();

private:
    QMutex m_mutex;
    int _currentSelected = -1;
    QList<AutomationLane> m_automationLanes;
    QHash<int, QByteArray> m_roleNames;

    QMap<QString, int> mapSubMenu;
    AreaInfo* _areaInfo;
    TrackItem* _trackItem;
    int _titleHeight;
    int _seperatorHeight;

private:
    int findIndex(QString menu, QString submenu);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

    virtual int rowCount(const QModelIndex& parent) const;

    virtual QVariant data(const QModelIndex& index, int role) const;

    // QAbstractItemModel interface
public:
    bool setData(const QModelIndex& index, const QVariant& value, int role);

    int currentSelected() const;
    void setCurrentSelected(int newCurrentSelected);

private slots:
    void sltAutomationTrackVolumeChanged();
    void sltAutomationTrackPanChanged();
    void volumeAutomatedLineChanged();
    void panAutomatedLineChanged();
    void sltVolumeEnabledChanged();
    void sltPanEnabledChanged();
    void sltEnableChanged();
    void sltLineChanged();
    void sltValueChanged();
    void sltNormalizedValueChanged();

    void sltAutomationTrackSpeakerOnChanged();
    void speakerOnAutomatedLineChanged();
    void sltSpeakerOnEnabledChanged();
};

class TruePeakModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(double maxTruePeak READ maxTruePeak NOTIFY maxTruePeakChanged)
public:
    enum TruePeakRole {
        ROLE_AverageMeterLevel = Qt::UserRole + 1,
        ROLE_AverageMeterPeakLevel,
        ROLE_PeakMeterLevel,
        ROLE_PeakMeterPeakLevel,
        ROLE_MaximumPeakLevel,
        ROLE_MaximumTruePeakLevel,
        ROLE_MaximumTruePeak,
        ROLE_Muted,
    };
    Q_ENUM(TruePeakRole)

    explicit TruePeakModel(int channsls, QObject* parent = Q_NULLPTR)
        : QAbstractListModel(parent)
    {
        m_roleNames[ROLE_AverageMeterLevel] = "AverageMeterLevel";
        m_roleNames[ROLE_AverageMeterPeakLevel] = "AverageMeterPeakLevel";
        m_roleNames[ROLE_PeakMeterLevel] = "PeakMeterLevel";
        m_roleNames[ROLE_PeakMeterPeakLevel] = "PeakMeterPeakLevel";
        m_roleNames[ROLE_MaximumPeakLevel] = "MaximumPeakLevel";
        m_roleNames[ROLE_MaximumTruePeakLevel] = "MaximumTruePeakLevel";

        m_roleNames[ROLE_MaximumTruePeak] = "MaximumTruePeak";

        m_roleNames[ROLE_Muted] = "Muted";

        for (int i = 0; i < channsls; i++) {
            _truePeakValues.append(TruePeak { 1, 1, 1, 1, 1, 1, false });
        }
    }

    struct TruePeak {
        double AverageMeterLevel;
        double AverageMeterPeakLevel;
        double PeakMeterLevel;
        double PeakMeterPeakLevel;
        double MaximumPeakLevel;
        double MaximumTruePeakLevel;

        double MaximumTruePeak;

        bool Muted;
    };

    double maxTruePeak()
    {
        return _maxTruePeak;
    }

    void insert(int index, const TruePeak& peak)
    {
        if (index < 0 || index >= _truePeakValues.size())
            return;

        // Update value
        _truePeakValues[index] = peak;
        _truePeakValues[index].Muted = _isMuted;

        // Recalculate max true peak
        _maxTruePeak = std::numeric_limits<float>::lowest();
        for (const auto& value : _truePeakValues) {
            if (value.PeakMeterPeakLevel > _maxTruePeak) {
                _maxTruePeak = value.PeakMeterPeakLevel;
            }
        }

        _truePeakValues[index].MaximumTruePeak = _maxTruePeak;

        // Notify model reset correctly
        beginResetModel();
        endResetModel();
    }

    void setMuted(bool muted)
    {
        _isMuted = muted;
        for (int i = 0; i < _truePeakValues.size(); i++) {
            _truePeakValues[i].Muted = muted;
            Q_EMIT dataChanged(index(i, 0), index(i, 0), QVector<int>() << ROLE_Muted);
        }
    }

    int size()
    {
        return _truePeakValues.size();
    }

    TruePeak get(int _index)
    {
        if (_index >= 0 && _index < _truePeakValues.size()) {
            return _truePeakValues[_index];
        }
        return TruePeak {
            0,
            0,
            0,
            0,
            0,
            0,
            0,
        };
    }
signals:
    void maxTruePeakChanged();

private:
    double _maxTruePeak = -1;
    bool _isMuted = false;
    QList<TruePeak> _truePeakValues;
    QHash<int, QByteArray> m_roleNames;

protected:
    virtual QHash<int, QByteArray> roleNames() const override
    {
        return m_roleNames;
    }

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const
    {
        return _truePeakValues.size();
    }

    virtual QVariant data(const QModelIndex& index, int role) const
    {

        if (!index.isValid())
            return QVariant();

        if (_truePeakValues.size() == 0 || index.row() < 0)
            return QVariant();

        auto value = _truePeakValues[index.row()];

        switch (role) {
        case ROLE_AverageMeterLevel:
            return value.AverageMeterLevel;
        case ROLE_AverageMeterPeakLevel:
            return value.AverageMeterPeakLevel;
        case ROLE_PeakMeterLevel:
            return value.PeakMeterLevel;
        case ROLE_PeakMeterPeakLevel:
            return value.PeakMeterPeakLevel;
        case ROLE_MaximumPeakLevel:
            return value.MaximumPeakLevel;
        case ROLE_MaximumTruePeakLevel:
            return value.MaximumTruePeakLevel;

        case ROLE_MaximumTruePeak:
            return value.MaximumTruePeak;
        case ROLE_Muted:
            return value.Muted;
        }
        return QVariant();
        //        qDebug() << "P2";
    }
};

class PanAutomationItem : public AutomationItem {

public:
    explicit PanAutomationItem(int trackIndex, QSharedPointer<Automation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
    {

        automation->setDefaultNormalized(0.5);
        automation->setAutomatedNormalized(0.5);
        automation->setNormalized(0.5);
        automation->setMinValue(-50);
        automation->setMaxValue(50);
    }
    // AutomationItem interface
public:
    double normalized2Value(double y)
    {
        return 100 * y - 50;
    }
    QString normalized2Label(double y)
    {
        auto value = int(normalized2Value(y));
        if (y < 0.5) {
            return QString::number(abs(value)) + "R";
        } else if (y > 0.5) {
            return QString::number(abs(value)) + "L";
        } else {
            return "C";
        }
    }
    double value2Normalized(double value)
    {
        return value / 100 + 1 / 2.0;
    }
};

class VisrAutomationItem : public AutomationItem {

public:
    explicit VisrAutomationItem(int trackIndex, QSharedPointer<Automation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
    {

        automation->setDefaultNormalized(0.5);
        automation->setAutomatedNormalized(0.5);
        automation->setNormalized(0.5);
        automation->setMinValue(0);
        automation->setMaxValue(1);
    }
    // AutomationItem interface
public:
    double normalized2Value(double y)
    {
        return y;
    }
    QString normalized2Label(double y)
    {
        return QString::number(y);
    }
    double value2Normalized(double value)
    {
        return value;
    }
};

class VolumeAutomationItem : public AutomationItem {

public:
    explicit VolumeAutomationItem(int trackIndex, QSharedPointer<Automation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
    {

        automation->setDefaultNormalized(MathHelper1::decibel2yPixel(0));
        automation->setNormalized(MathHelper1::decibel2yPixel(0));
        automation->setAutomatedNormalized(MathHelper1::decibel2yPixel(0));
        automation->setMinValue(MathHelper1::yPixel2Decibel(0));
        automation->setMaxValue(MathHelper1::yPixel2Decibel(1));
    }

    // AutomationItem interface
public:
    double normalized2Value(double y)
    {
        return qRound(MathHelper1::yPixel2Decibel(y) * 100) / 100.0;
    }
    QString normalized2Label(double y)
    {
        return QString::number(normalized2Value(y), 'f', 1) + " db";
    }
    double value2Normalized(double value)
    {
        return MathHelper1::decibel2yPixel(value);
    }
};

class SpeakerOnAutomationItem : public AutomationItem {

public:
    explicit SpeakerOnAutomationItem(int trackIndex, QSharedPointer<Automation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
    {

        automation->setDiscreted(true);
        automation->setDefaultNormalized(0.25);
        automation->setNormalized(0.25);
        automation->setAutomatedNormalized(0.25);
        automation->setMinValue(0.25);
        automation->setMaxValue(0.75);
    }

    // AutomationItem interface
public:
    double normalized2Value(double y)
    {

        if (y > 0.5) {
            return 0;
        } else {
            return 1;
        }
    }
    QString normalized2Label(double y)
    {
        if (y < 0.5) {
            return "On";
        } else {
            return "Off";
        }
    }
    double value2Normalized(double value)
    {
        if (value == 0) {
            return 0.75;
        } else {
            return 0.25;
        }
    }
};

class KeyFramesAutomation : public Automation {

    // Automation interface
public:
    explicit KeyFramesAutomation(AreaInfo* areaInfo, int trackIndex, QString type, QString subType, float minY = 0, float maxY = 1, QObject* parent = Q_NULLPTR)
        : Automation(areaInfo, trackIndex, type, subType, minY, maxY, 0.5, parent)
    {
    }

    int addLine2(double time, float Ypos)
    {

        return 0;
    }
    bool removeLine2(int index)
    {

        return true;
    }

    int addLine(double time, float Ypos)
    {

        int inserted_index = 0;

        return inserted_index;
    }
    bool removeLine(int index)
    {

        return true;
    }
};

class KeyFramesAutomationItem : public AutomationItem {

public:
    explicit KeyFramesAutomationItem(int trackIndex, QSharedPointer<KeyFramesAutomation> automation, AreaInfo* areaInfo)
        : AutomationItem(trackIndex, automation, areaInfo)
    {

        // automation->setPointShape(Automation::PointShape_Dimond);
        automation->setDiscreted(false);
        automation->setFixedVertical(true);
        automation->setDefaultNormalized(0.75);
        automation->setNormalized(0.75);
        automation->setAutomatedNormalized(0.75);
        automation->setMinValue(0.75);
        automation->setMaxValue(0.75);
    }

    void saveInUndoItems()
    {
    }

    void saveInUndoItems(QJsonArray state)
    {
    }

    QMap<qint64, int> keyFrames()
    {
        QMap<qint64, int> list;
        QMapIterator<quint64, KeyFramesPoint> i(_points);
        while (i.hasNext()) {
            i.next();
            list[i.key()] = i.value().type;
        }
        return list;
    }

    QMap<qint64, double> objectPositionX() const
    {
        QMap<qint64, double> list;
        for (int i = 0; i < _points.size(); i++) {
            list.insert(qint64(_points[i].time), _points[i].posX);
        }

        return list;
    }
    QMap<qint64, double> objectPositionY() const
    {
        QMap<qint64, double> list;
        for (int i = 0; i < _points.size(); i++) {
            list.insert(qint64(_points[i].time), _points[i].posY);
        }

        return list;
    }
    QMap<qint64, double> objectPositionZ() const
    {
        QMap<qint64, double> list;
        for (int i = 0; i < _points.size(); i++) {
            list.insert(qint64(_points[i].time), _points[i].posZ);
        }

        return list;
    }
    void clear()
    {
        QSharedPointer<KeyFramesAutomation> derivedPtr = qSharedPointerCast<KeyFramesAutomation>(automation());
        derivedPtr->clear();
        _points.clear();
    }
    void addKeyFrame(quint64 time, int type)
    {
        float Y = _automation->getYPixel(_automation->defaultNormalized());
        QSharedPointer<KeyFramesAutomation> derivedPtr = qSharedPointerCast<KeyFramesAutomation>(automation());
        derivedPtr->addLine2(time, Y);

        // Q_EMIT _areaInfo->sigAddKeyFrame(_trackIndex, time, int(automation()->pointShape()));

        _points.insert(time, KeyFramesPoint { time, type, m_objectPositionX, m_objectPositionY, m_objectPositionZ });
        Q_EMIT sigUpdate();
    }
    void removeKeyFrame(qint64 time)
    {
        int index = automation()->find_line(time);
        if (index >= 0) {
            QSharedPointer<KeyFramesAutomation> derivedPtr = qSharedPointerCast<KeyFramesAutomation>(automation());
            derivedPtr->removeLine2(index);
            // Q_EMIT _areaInfo->sigRemoveKeyFrame(_trackIndex, time);
            Q_EMIT sigUpdate();
        }

        if (_points.contains(time))
            _points.remove(time);
    }

    int getKeyFrameType(qint64 time)
    {

        if (_points.contains(time)) {
            return _points[time].type;
        }
        return 0;
    }

    // Returns KeyFramesType for the kf at the given time. Falls back to the
    // local _interpByTime cache (populated by right-click cycling), then to
    // _points (legacy), then defaults to Linear.
    // Numbers: 0 = Bezier (circle), 1 = Linear (diamond), 2 = Hold (square).
    int interpTypeAt(qint64 time) const
    {
        auto cit = _interpByTime.constFind(time);
        if (cit != _interpByTime.constEnd()) return cit.value();
        auto pit = _points.constFind(quint64(time));
        if (pit != _points.constEnd()) return pit.value().type;
        return 1; // Linear default
    }

    // Right-click on an existing keyframe in the lane cycles its interpolation
    // type Bezier(0) → Linear(1) → Hold(2) → Bezier. The change is propagated to
    // ObjectCreator's per-track ObjectPosAutomation via AreaInfo::sigInterpChanged
    // so the 3D path reshapes (linear ↔ curved ↔ stepped) immediately.
    int mousePressEvent(QMouseEvent* event)
    {
        qDebug() << "[KFLane] PRESS button=" << int(event->button())
                 << " track=" << _trackIndex << " pos=" << event->pos();
        if (event->button() == Qt::RightButton) {
            _rightPressX = event->pos().x();
            _rightPressY = event->pos().y();
            _rightPressed = true;
            return AutomationItem::AutomationItemEvent_InnerSelect;
        }
        return AutomationItem::mousePressEvent(event);
    }

    int mouseReleaseEvent(QMouseEvent* event)
    {
        qDebug() << "[KFLane] RELEASE button=" << int(event->button())
                 << " track=" << _trackIndex << " pos=" << event->pos()
                 << " wasPressed=" << _rightPressed;
        if (event->button() == Qt::RightButton) {
            const bool wasPressed = _rightPressed;
            _rightPressed = false;
            if (!wasPressed)
                return AutomationItem::AutomationItemEvent_NotSelect;

            // Hit-test against the lane's fixed Y (lane is fixedVertical).
            const double clickX = event->pos().x();
            const double pxThr = double(ScreenInterface::scaleSize2(20, _areaInfo->zoomFactor()));
            const double laneY = _automation->getYPixel(_automation->defaultNormalized());
            auto hit = _automation->hitTestLines(clickX, laneY, pxThr, pxThr * 2.0);
            qint64 hitTime = -1;
            if (hit.state == Automation::OnPoint) {
                CPoint* cp = (hit.endpoint == 0) ? hit.index.p1() : hit.index.p2();
                if (cp) hitTime = qint64(cp->time());
            }
            if (hitTime >= 0) {
                int curType = _interpByTime.value(hitTime, 1 /*Linear*/);
                const int nextType = (curType + 1) % 3;
                _interpByTime.insert(hitTime, nextType);
                if (_points.contains(quint64(hitTime)))
                    _points[quint64(hitTime)].type = nextType;
                // KeyFramesType (0=Bezier, 1=Linear, 2=Hold) → KeyInterp (0=Hold, 1=Linear, 2=Bezier)
                const int interpInt = (nextType == 0) ? 2
                                    : (nextType == 2) ? 0 : 1;
                Q_EMIT _areaInfo->sigInterpChanged(_trackIndex, hitTime, interpInt);
                Q_EMIT sigUpdate();
            }
            return AutomationItem::AutomationItemEvent_InnerSelect;
        }
        return AutomationItem::mouseReleaseEvent(event);
    }
    void setObjectPosition(double x, double y, double z)
    {
        m_objectPositionX = x;
        m_objectPositionY = y;
        m_objectPositionZ = z;
    }

    void getObjectPosition(double& x, double& y, double& z)
    {
        x = m_objectPositionX;
        y = m_objectPositionY;
        z = m_objectPositionZ;
    }
    struct KeyFramesPoint {
        quint64 time;
        int type;
        double posX;
        double posY;
        double posZ;
    };

private:
    double m_objectPositionX = 0;
    double m_objectPositionY = 0;
    double m_objectPositionZ = 0;
    QMap<quint64, KeyFramesPoint> _points;
    QMap<qint64, int> _interpByTime;   // local cache of cycled interp type per kf time
    bool   _rightPressed = false;
    int    _rightPressX  = 0;
    int    _rightPressY  = 0;

public:
    double normalized2Value(double y)
    {

        if (y > 0.5) {
            return 0;
        } else {
            return 1;
        }
    }
    QString normalized2Label(double y)
    {
        if (y < 0.5) {
            return "On";
        } else {
            return "Off";
        }
    }
    double value2Normalized(double value)
    {
        if (value == 0) {
            return 0.75;
        } else {
            return 0.25;
        }
    }
};

class TrackItem : public IPainterItem, public JournallingObject {
    Q_OBJECT

    Q_PROPERTY(TruePeakModel* truePeakModel READ getTruePeakModel NOTIFY truePeakModelChanged)
    Q_PROPERTY(AutomationLaneModel* automationLaneModel READ getAutomationLaneModel NOTIFY truePeakModelChanged)
    Q_PROPERTY(bool rowsExpanded READ rowsExpanded WRITE setRowsExpanded NOTIFY rowsExpandedChanged)

    Q_PROPERTY(QRect rect READ rect NOTIFY rowsExpandedChanged)
    Q_PROPERTY(QRect totalRect READ totalRect NOTIFY rowsExpandedChanged)

    Q_PROPERTY(int channelCount READ channelCount NOTIFY channelCountChanged)

    Q_PROPERTY(float masterTrackVolume READ masterTrackVolume WRITE setMasterTrackVolume NOTIFY masterTrackVolumeChanged)

public:
    enum TrackItemEvent {
        TrackItemEvent_NotSelect,
        TrackItemEvent_TrackSelect,
        TrackItemEvent_AutomationSelect,
        TrackItemEvent_AutomationLineSelect,
        TrackItemEvent_InnerSelect,
        TrackItemEvent_OuterPointSelect,
        TrackItemEvent_ClipSelect,
        TrackItemEvent_KeyFramesSelect,
        TrackItemEvent_ObjectXSelect,
        TrackItemEvent_ObjectYSelect,
        TrackItemEvent_ObjectZSelect,
    };
    Q_ENUM(TrackItemEvent)

    enum IndicatorType {
        IndicatorType_Disable = 0,
        IndicatorType_Track,
        IndicatorType_Title,
        IndicatorType_Lane,

    };

    struct CopyItem {
        double startTime;
        double endTime;
        double length;
        double startTimeDiff;
        double endTimeDiff;
        int trackIndex;
        QSharedPointer<ClipItem> clipItem;
        double height;
        int type = -1;
    };
    struct CutList {
        qint64 index;
        double startTime;
        double endTime;
    };

    QString nodeName() const override
    {
        return "TrackItem";
    }

    explicit TrackItem(int trackIndex = 0, Track* track = Q_NULLPTR, AreaInfo* areaInfo = Q_NULLPTR);
    ~TrackItem();

    QSharedPointer<ClipItem> clone(QString clipIndex, bool fake = false);

    static QSharedPointer<ClipItem> createNewSample(QString filePath, QColor _titleColor, AreaInfo* areaInfo, int trackIndex, QString clipIndex, Track* track, QRect rect, bool isFake);

    QSharedPointer<ClipItem> createDragSample(QString filePath, QColor titleColor, AreaInfo* areaInfo, int trackIndex, QString clipIndex, Track* track, QRect rect, bool isFake);
    void deleteDragSample();
    void editDragSample();
    QJsonObject removeSampleItem(QString clipIndex);

    QString addSampleClip(double time, QSharedPointer<ClipItem> item);

    QString addDragSampleClip(QSharedPointer<ClipItem> item);

    QString addSampleClip(QSharedPointer<ClipItem> item);

    QString addSampleClip(double startTime, double endTime, double startOffset, double endOffset, QSharedPointer<ClipItem> item);
    QJsonObject resizeSampleClip(QString clipIndex, QRect new_rect, double startTime, double endTime, double startOffset, double endOffset);
    QJsonArray cutClipItem(QString clipIndex, double startTime, double endTime);
    QJsonArray splitClipItem(QString clipIndex, double time);

    QList<QString> setPositionChanged(double duration_in_seconds, bool toPlay);
    QString setPositionChanged(QString clipIndex, double duration_in_seconds, bool toPlay);
    QList<QString> countPositionChanged(double duration_in_seconds);

    int numberOfClips();
    QSharedPointer<ClipItem> clipItems(QString index);
    QSharedPointer<ClipItem> getFirstClipItem();
    QSharedPointer<ClipItem> getLastClipItem();

    bool containTitleGlobal(QPointF pos);
    bool containTitle(QPointF pos);
    void serGlobalRect(const QRect& rect);
    void setRect(const QRect& rect) override;
    QRect rect() const;
    QRect totalRect();

    Q_INVOKABLE QRect getTotalRect();

    QSharedPointer<AutomationItem> effectAutomationItem();
    QSharedPointer<AutomationItem> effectAutomationItem(QString menuTitle, QString subMenuTitle);

    QSharedPointer<AutomationItem> visrAutomationItem();
    QSharedPointer<AutomationItem> visrAutomationItem(QString menuTitle, QString subMenuTitle);

    int isSelected(QString menuTitle, QString subMenuTitle);

public:
    void draw(IPainter* painter) override;

public:
    int trackIndex() const;

    void setSelectedClipIndex(QString clipIndex, bool selected);
    void deSelecteAll();

    // bool contains(QPoint point);
    bool contains_global(QPoint point);
    bool selectedAreaContains(SelectedArea selectedArea);

    QString findClipItemPixel(double pixel);
    QString findClipItemTime(double time);

    QList<QString> findOverlayItems(double startTime, double endTime);
    QList<QString> findOverlayItems(QRectF selectedRect);

    QJsonArray deleteSelectedArea(SelectedArea selectedRect);
    QVector<CopyItem> copySelectedArea(SelectedArea selectedRect);

    void setKeyFrameEnabled(bool enabled);
    bool keyFramesEnabled();

    bool automationLaneEnabled() const;
    void setAutomationLaneEnabled(bool automationLaneEnabled);

private:
    bool _automationLaneEnabled = false;

    int _inputChannelCount = 2;
    int _outputChannelCount = 2;

    QRect _baseRect;
    bool _isMasterTrack = false;
    AreaInfo* _areaInfo;
    Track* _track;
    QSharedPointer<ClipItem> _dragClipItem;

    QMap<QString, QSharedPointer<ClipItem>> _clipItems;
    QMutex _mutex;
    QSharedPointer<VolumeAutomationItem> _volumeAutomationItem;
    QSharedPointer<PanAutomationItem> _panAutomationItem;
    QSharedPointer<SpeakerOnAutomationItem> _speakerOnAutomationItem;

    QSharedPointer<KeyFramesAutomationItem> _keyFramesAutomationItem;

    QSharedPointer<KeyFramesAutomation> _keyFrameAutomation;

    QSharedPointer<TruePeakModel> _truePeakModel;

    // Automation::AutomationType _automationSelected;

    int _lastRowExpanded = 0;
    int _rowsExpandedCount = 0;
    bool _isCollapsed = true;
    bool _isSelected = false;
    bool _selectSolo = false;
    bool _partialySolo = false;
    bool _soloOnAudio = false;
    bool _selectTrackDependSolo = true;
    bool _coverBound = false;
    bool _editName = false;
    int _inDraggingCounter = -1;
    bool _rowsExpanded = true;
    bool _objectEnabled = false;

    QString _currentClipItem = "";

    QString _imageUrl = "qrc:/resources/svg/track_list/Triangle.svg";

    int _trackIndex = -1;
    double _currentIndicator = -1;
    IndicatorType _indicatorEnabled = IndicatorType_Disable;

    QString _selectedClipIndex = "";
    QString _hoverClipIndex = "";
    QTimer _timerBlink;
    QNanoColor _indicatorColor = "#A1A1FF";

    QColor _titleColor;

    QSharedPointer<AutomationLaneModel> _automationLaneModel;

    QString _automationMenuTitle1 = "None";
    QString _automationSubMenuTitle = "None";

    int _globalYPosition = 0;

private:
    void drawClips(IPainter* painter);
    void drawIndicator(IPainter* painter);

    void drawAutomationLane(IPainter* painter);

public:
    int mousePressEvent(QMouseEvent* event);
    int hoverMoveEvent(QHoverEvent* event);

    int mouseReleaseEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int keyPressEvent(int modifier, int key);
    int keyReleaseEvent(int modifier, int key);

public:
    Track* track() const;
    double currentIndicator() const;
    IndicatorType indicatorEnabled() const;
    void setIndicatorEnable(IndicatorType indicatorEnabled);
    void setIndicator(double currentIndicator);

    int globalYPosition()
    {
        return _globalYPosition;
    }

    QString selectedClipIndex() const;
    QString hoverClipIndex() const;

    // Automation* automation() const;
    QSharedPointer<AutomationItem> automationItem() const;

    QSharedPointer<AutomationItem> automationItem(QString menu, QString submenu) const;

    void drawTitleLine(IPainter* painter);

    QSharedPointer<AutomationItem> volumeAutomationItem() const;

    QSharedPointer<AutomationItem> panAutomationItem() const;

    QSharedPointer<AutomationItem> speakerOnAutomationItem() const;

    QMap<QString, QSharedPointer<ClipItem>> getClipItems() const;

    bool isCollapsed() const;

    bool selectSolo() const;

    bool partialySolo() const;

    bool isSolo() const;

    bool isMuted() const;

    bool selectTrackDependSolo() const;

    bool coverBound() const;
    void setCoverBound(bool coverBound);

    bool editName() const;

    int inDraggingCounter() const;

    bool rowsExpanded() const;

    void setRowsExpanded(bool rowsExpanded);

    Q_INVOKABLE void updateRect();

    QString imageUrl() const;
    void setImageUrl(const QString& imageUrl);

    //    double trackVolume() const;
    //    void setTrackVolume(double trackVolume);
    //    double automatedTrackVolume() const;

    //    double trackPan() const;
    //    void setTrackPan(double pan);
    //    double automatedTrackPan() const;

    QString currentClipItem() const;
    void setCurrentClipItem(const QString& currentClipItem);

    int channelCount() const;

    void setMasterTrackVolume(float value);
    float masterTrackVolume() const;

    QSharedPointer<TruePeakModel> truePeakModel();
    TruePeakModel* getTruePeakModel();

    void updateTruePeak();
    void playBackUpdate();

    bool isSelected() const;
    void setSelected(bool isSelected);

    void drawSelectedTrackItem(int T, int lastWidth, IPainter* painter, int B);

    QColor getTitleColor() const;
    void setTitleColor(const QColor& newTitleColor);

    int rowsExpandedCount() const;
    void setRowsExpandedCount(int currentHeight);

    QSharedPointer<AutomationLaneModel> automationLaneModel() const;
    AutomationLaneModel* getAutomationLaneModel() const;

    int getLastRowExpanded() const;
    void setLastRowExpanded(int lastRowExpanded);

    int getRowsExpandedCount() const;

    QString getAutomationMenuTitle() const;
    void setAutomationMenuTitle(const QString& automationMenuTitle);

    void totalRowExpandedChanged();

    bool createCopyList(SelectedArea selectedArea);
    int insertCopiedList();
    void setCopyList(const QJsonArray copyList, int subTrackIndex);
    void setCopyList2(const QJsonArray copyList, QString menu, QString subMenu);
    bool cutCopiedList(SelectedArea selectedArea);
    void clearCopyList();

    bool isMasterTrack() const;
    void setMasterTrack(bool isMasterTrack);

    // QSharedPointer<KeyFramesItem> keyFramesItem() const;
    QMap<qint64, int> keyFrames() const;

    QString automationSubMenuTitle() const;
    void setAutomationSubMenuTitle(const QString& newAutomationSubMenuTitle);

    bool copyAllAutomationItemsInArea(SelectedArea area, QVector<AutomationList>& list, int trackIndex);
    bool copyCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, int trackIndex);
    bool copyCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QString menu, QString submenu, int trackIndex, int laneIndex);
    void copyEmptyAutomationItem(SelectedArea area, QVector<AutomationList>& list, int trackIndex);
    void copyEmptyAutomationItem(SelectedArea area, QVector<AutomationList>& list, QString menu, QString submenu, int trackIndex, int laneIndex);

    bool copyKeyFramesInArea(SelectedArea area, QList<KeyFramesItem::KeyFramesPoint>& list, int trackIndex);
    bool copyKeyFramesItemInArea(SelectedArea area, QList<KeyFramesItem::KeyFramesPoint>& list, int trackIndex);

    bool cutAllAutomationItemsInArea(SelectedArea area, QVector<AutomationList>& list, QJsonArray& array, int trackIndex);
    bool cutCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex);
    bool cutCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, QString menu, QString submenu, int trackIndex, int laneIndex);
    void cutEmptyAutomationItem(SelectedArea area, QVector<AutomationList>& list, int trackIndex);

    void deleteAllAutomationItemsInArea(SelectedArea area, QJsonArray& array, int trackIndex);
    void deleteCurrentAutomationItemInArea(SelectedArea area, QJsonArray& jArray, int trackIndex);
    void deleteCurrentAutomationItemInArea(SelectedArea area, QJsonArray& jArray, QString menu, QString submenu, int trackIndex, int laneIndex);

    double getStartTimeSelectAllAutomation();
    double getStartTimeSelectAllAutomation(QString menu, QString submenu);

    double getEndTimeSelectAllAutomation();
    double getEndTimeSelectAllAutomation(QString menu, QString submenu);

    void insertAllAutomationItems(QJsonArray& jArray, int trackIndex, bool dont_copy);
    void insertCurrentAutomationItem(QJsonArray& jArray, int trackIndex, bool dont_copy);
    void insertCurrentAutomationItemInArea(QJsonArray& jArray, QString menu, QString submenu, int trackIndex, int laneIndex);

    void duplicateAllAutomationItems(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex, int subTrackIndex);
    void duplicateCurrentAutomationItem(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex, int subTrackIndex);
    void duplicateCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QJsonArray& jArray, int trackIndex, int laneIndex, QString menu, QString submenu);

    void movingAllAutomationItems(SelectedArea area, QVector<AutomationList>& list, int trackIndex);
    void movingCurrentAutomationItem(SelectedArea area, QVector<AutomationList>& list, int trackIndex);
    void movingCurrentAutomationItemInArea(SelectedArea area, QVector<AutomationList>& list, QString menu, QString submenu, int trackIndex, int laneIndex);

    void clearAutomationItems();

    bool objectEnabled() const;
    void setObjectEnabled(bool newObjectEnabled);

    QSharedPointer<Automation> keyFrameAutomation() const;

    QSharedPointer<KeyFramesAutomationItem> keyFramesAutomationItem() const;

    int globalYPosition() const;
    void setGlobalYPosition(int newGlobalYPosition);

    int inputChannelCount() const;
    void setInputChannelCount(int newInputChannelCount);

    int outputChannelCount() const;
    void setOutputChannelCount(int newOutputChannelCount);

    QSharedPointer<ClipItem> dragClipItem() const;

    QRectF drawSelectedArea();
private slots:

    void sltRowAdded(int index, int count, QString menu, QString subMenu);
    void sltRowRemoved(int index, QString menu, QString subMenu);
    void sltRowChanged();
signals:

    void sigAutomationLaneCountChanged(int index);
    void sigSelectedChanged(int index);
    void sigFirstBufferFilled(QString clipItem, bool toPlay);
    void sigSampleChanged();
    void sigPositionChanged(int _trackIndex, QString clipIndex);
    void sigMaxEndTimeChanged(double time);

    void sigPlayingChanged(bool state, int trackIndex, QString clipIndex);
    void rowsExpandedChanged(int index);
    void volumeAutomatedValueChanged();
    void volumeAutomatedLineChanged();
    void volumeEnabledChanged();

    void panAutomatedValueChanged();
    void panAutomatedLineChanged();
    void panEnabledChanged();

    void visrAutomatedValueChanged();
    void visrAutomatedLineChanged();
    void visrEnabledChanged();

    void channelCountChanged();
    void masterTrackVolumeChanged();
    void truePeakModelChanged();

    void sigClipChanged();
    void sigClipAdded(QString id);
    void sigClipRemoved(QString id);
    void sigDragClipAdded();
    void sigDragClipRemoved(QString id);
    void sigDragClipMoved(QString id);
};

#endif // CLIPITEM_H
