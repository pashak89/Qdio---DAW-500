#ifndef CLIPAREA_H
#define CLIPAREA_H

#include "audio/audioengine.h"
#include "audio/engine.h"
#include "audio/rendermanager.h"
#include "audio/sampletrack.h"
#include "core/tracksmodel.h"
#include "eq/multieqeffect.h"
#include "eq/visreffect.h"
#include "measure.h"
#include "sampleeditor.h"
#include "ui/automationitem.h"
#include "ui/backgrounditem.h"
#include "ui/beatbaritem.h"
#include "ui/clipitem.h"
#include "ui/envelopeitem.h"
#include "ui/hotspotitem.h"
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
#include <QStandardPaths>
#include <QTimer>
#include <ui/timeruleritem.h>

class ClipArea;

struct RangeItem {
    double start;
    double end;
    int value;
};
struct LineSeg {
    QPointF p1;
    QPointF p2;
    QColor color; // unique per line
};

struct RectSeg {
    QRectF rect;
    QColor color; // unique per line
};

class ClipArea : public QObject {

    Q_OBJECT
    Q_PROPERTY(int fixedGrid READ fixedGrid WRITE setFixedGrid NOTIFY fixedGridChanged)
    Q_PROPERTY(int adaptiveGrid READ adaptiveGrid WRITE setAdaptiveGrid NOTIFY adaptiveGridChanged)

    Q_PROPERTY(int beatPerBar READ beatPerBar WRITE setBeatPerBar NOTIFY beatPerBarChanged)
    Q_PROPERTY(int lenghtOfBeat READ lenghtOfBeat WRITE setLenghtOfBeat NOTIFY lenghtOfBeatChanged)
    Q_PROPERTY(double tempo READ tempo WRITE setTempo NOTIFY tempoChanged)

    Q_PROPERTY(int currentSelectedTime READ currentSelectedTime WRITE setCurrentSelectedTime NOTIFY currentSelectedTimeChanged)

    Q_PROPERTY(TracksModel* tracksModel READ tracksModel NOTIFY sigTrackModelCreated)

    Q_PROPERTY(int trackListStartY READ trackListStartY NOTIFY sigTrackChanged)
    Q_PROPERTY(int trackListStartHeight READ trackListStartHeight NOTIFY sigTrackHeightChanged)

    Q_PROPERTY(bool automationEnabled READ automationEnabled WRITE setAutomationEnabled NOTIFY automationEnabledChanged)

    Q_PROPERTY(bool shiftPressed READ shiftPressed NOTIFY shiftPressedChanged)

    Q_PROPERTY(bool paused READ paused NOTIFY pausedChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)

    Q_PROPERTY(float zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)

    Q_PROPERTY(int minStepSize READ minStepSize NOTIFY zoomFactorChanged)
    Q_PROPERTY(int seperatorHeight READ seperatorHeight NOTIFY zoomFactorChanged)

    Q_PROPERTY(bool coverBound WRITE setCoverBound NOTIFY sigCoverBoundChanged)

    Q_PROPERTY(QString defaultSavePath READ defaultSavePath WRITE setDefaultSavePath NOTIFY defaultSavePathChanged)

    Q_PROPERTY(int exportProgress READ exportProgress NOTIFY exportProgressChanged)

public:
    explicit ClipArea(AreaInfo* areaInfo = nullptr, QObject* parent = nullptr);
    ~ClipArea();

    Q_INVOKABLE void initialize();

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();

    int _trackListHeight;
    int _trackListStartY;

    Q_INVOKABLE void closeAllEffectWindow();
    Q_INVOKABLE void addSampleClip();
    Q_INVOKABLE void removeSampleClip(int trackIndex);
    Q_INVOKABLE void setCurrentIndicator(qint64 time);

    int fixedGrid() const;
    void setFixedGrid(int fixedGrid);

    int adaptiveGrid() const;
    void setAdaptiveGrid(int adaptiveGrid);

    float tempo() const;
    void setTempo(float tempo);

    int lenghtOfBeat() const;
    void setLenghtOfBeat(int lenghtOfBeat);

    int beatPerBar() const;
    void setBeatPerBar(int beatPerBar);

    QSharedPointer<ClipItem> draggingItem() const;

    QVector<TrackItem::CopyItem> movingItems() const;

    double stepPlotDuration() const;

    double endPixel() const;

    double startPixel() const;

    ZoomLevel zoomLevel() const;

    QVector<QPair<double, double>> snapPoints() const;

    bool automationEnabled() const;
    void setAutomationEnabled(bool automationEnabled);

    void saveInUndoItems(QJsonArray array);
    void saveInRedoItems(QJsonArray array);

    AreaInfo* areaInfo() const;

    QString defaultSavePath() const;
    void setDefaultSavePath(QString defaultSavePath);

    TracksModel* tracksModel() const;

    int trackListStartY() const;

    int trackListStartHeight() const;

    Q_INVOKABLE bool isCtrlPressed();

    bool shiftPressed() const;

    void setZoomFactor(float factor);
    float zoomFactor();

    Q_INVOKABLE void setOverrideCursor(Qt::CursorShape cursorShape);
    Q_INVOKABLE void setTrackHeight(int _index, int posY);

    Q_INVOKABLE int findTrackResizing(int posY);

    int minStepSize();

    int seperatorHeight();

    int _currentMouseX = -1;
    int _currentMouseY = -1;
    bool _mouseMoveEvent = false;

    Q_INVOKABLE void addVisr(int trackIndex, bool enabled = false, bool reverbEnabled = false);
    Q_INVOKABLE void removeVisr(int trackIndex);

    Q_INVOKABLE void removeMultiEqEffect(int trackIndex);

    Q_INVOKABLE bool hasEffect(int trackIndex);

    Q_INVOKABLE void addVst3Effect(int trackIndex, QString path, QString name, int index = 0);
    Q_INVOKABLE void removeVst3Effect(int trackIndex, int effectIndex, int index = 0);

    Q_INVOKABLE void saveAddEffectInUndo(int trackIndex, QString effect_name, int effectIndex = -1);
    Q_INVOKABLE void saveRemoveEffectInUndo(int trackIndex, QString effect_name, int effect_index = -1);

    Q_INVOKABLE void saveAsyncProject(QStringList objects, QString projectName);
    Q_INVOKABLE void loadAsyncProject(QString projectName);

    void setCoverBound(bool status);

    void undoPerformOperation();

    void redoPerformOperation();

    bool paused();
    bool playing();

    Q_INVOKABLE void add3dObject(int trackIndex, bool enabled);

    int currentSelectedTime() const;
    void setCurrentSelectedTime(int newCurrentSelectedTime);

    Q_INVOKABLE void exportAudio(QString fileName, int samplerate, int bitDepth, bool mongo);

    int exportProgress();

    Q_INVOKABLE void setCursorPosition(int x, int y);
    Q_INVOKABLE QPoint globalPos();

    Q_INVOKABLE double findSnap(double posX);

    Q_INVOKABLE void clearMasterTrackSelection();
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void handleMasterTrackMousePressed(QPoint localPos, int button);
    Q_INVOKABLE void handleMasterTrackMouseMoved(QPoint localPos, int button);

    Q_INVOKABLE bool handleMousePressed(QPoint localPos, QPoint globalPos, int button);
    Q_INVOKABLE void handleMouseMoved(QPoint localPos, QPoint globalPos, int button);
    Q_INVOKABLE void handleMouseReleased(QPoint localPos, QPoint globalPos, int button);
    Q_INVOKABLE void handleHoverMoved(QPoint localPos, QPoint globalPos, int button);

    // I think This function has problem, for that I have commented, beacuse I should load all samples for all trackes
    void playbackSampleClipPositionChanged(int trackIndex, QString clipIndex);

    void drawGrids();

    void addMultiEqEffect(int trackIndex);

    Q_INVOKABLE void setWindowRootItem(QQuickItem* item)
    {
        qDebug() << "setWindowRootItem" << item;
        m_windowRootitem = item;
    }

private:
    QQuickItem* m_windowRootitem = nullptr;
    QSharedPointer<TrackItem> _masterTrack = Q_NULLPTR;

    std::unique_ptr<AudioRenderManager> m_renderManager;

    QPointF _oldHoverPos;
    QString _defaultSavePath;
    QString _project_name = "";
    int _lastKey = 0;
    AreaInfo* _areaInfo;

    QList<KeyFramesItem::KeyFramesPoint> _copeiedKeyFramesItems;

    QVector<TrackItem::CopyItem> _movingItems;
    QSharedPointer<ClipItem> _draggingItem;

    QVector<TrackItem::CopyItem> _copiedItems;
    QVector<TrackItem::CopyItem> _duplicatedItems;

    QVector<AutomationList> _copiedAutomationPoint;
    QVector<AutomationList> _duplicatedAutomationPoint;
    QVector<AutomationList> _movingAutomationPoint;

    QList<Warp*> _warpMarkers;
    QSharedPointer<TracksModel> _tracksModel;

    bool _automationEnabled = false;

    double _timeDuration = 0;
    double _startPixel = 0;
    double _endPixel = 0;

    double _baseStartTime = 0;
    double _baseEndTime = 0;

    PlayMovingMode _playMovingMode = PlayMovingMode_Indicator;

    QTimer _timerWidth;
    QTimer _timerCheckStartContentYChanged;

    double _startBraceMarker = 0;
    double _endBraceMarker = 0;
    QRect _boundingRect;

    QList<QString> _numberOfSampleBuffer;

    std::atomic_bool _requestForLoading = false;
    std::atomic_bool _exporing = false;

    Qt::DropAction dropAction;
    QJsonArray _selectingStateBeforeResizing;
    int _startSelectedTrackIndex = -1;
    int _startSelectedSubTrackIndex = -1;
    QString _startSelectedClipIndex = "";
    double _startSelectedTime = -1;
    double _endSelectedTime = -1;
    int _pre_base_track_indicator = -1;
    int _trackLineResizing = -1; // -1 :Disable,0:hover,1:pressed
    int _trackLineResizingStartY = 0;
    bool _selectingAreaCreating = false;
    bool _selectingAreaMoving = false;
    bool _selectingAreaResizing = false;
    bool _automationChanging = false;
    bool _keyFramesPointSelected = false;
    bool _objectPointXSelected = false;
    bool _objectPointYSelected = false;
    bool _objectPointZSelected = false;

    double _pre_paste_indicator = -1;
    double _startPressedPointX;

    bool _selectedAreaMoved = false;

    void clearCopiedItems();
    void clearDuplicatedItems();

    QTimer _timerCursorPosition;
    QPoint _lastMouseMovingPoint;

    QJsonObject performOperation(QJsonObject obj);

    int _lastStartContentY = 0;

    int _listViewPositionChanged = 0;

    QMap<QString, QSharedPointer<Effect>> m_loadingEffects;
    QMap<QString, QJsonObject> m_loadingEffectsAutomations;

    int _currentSelectedTime = 0;
    bool _selectedAreaActive = false;
    int _exportProgress = 0;

    QStringList _savedObjects;
    QStringList _loadedObjects;

    QVector<RectSeg> m_plotRects;
    QVector<LineSeg> m_plotLines;

    QVector<QVector<RectSeg>> m_trackSegRects;
    QVector<QVector<RectSeg>> m_trackSelectedRects;
    QVector<RectSeg> m_masterTrackSelectedRects;
    QVector<QVector<LineSeg>> m_trackSegLines;

signals:
    void sigUpdateUI();
    void sigSelectionAreaChanged();
    void sigLoad3dView();
    void currentSelectedTimeChanged();
    void defaultSavePathChanged();
    void sigProjectSaving();
    void sigProjectLoading();
    void sigProjectSaved();
    void sigProjectLoaded(QStringList objects);

    void sigSaveAsProject();
    void sigLoadProject();
    void pausedChanged();
    void playingChanged();
    void sigEffectLoading(bool progress);

    void sigCoverBoundChanged();
    void sigInitialized();
    void effectsChanged(int trackIndex, int index);
    void selectedTrackIndexChanged();
    void areaInfoChanged();
    void zoomFactorChanged();
    void sigTrackHeightChanged();

    void shiftPressedChanged();

    void sigTrackChanged();

    void beatPerBarChanged();
    void lenghtOfBeatChanged();

    void fixedGridChanged();
    void adaptiveGridChanged();
    void tempoChanged();

    void lastWidthChanged();
    void sigClipDurationChanged();
    void sigPlaybackMarkerChanged(double time);
    void sigStartTimeChanged();
    void sigEndTimeChanged();
    void automationEnabledChanged();

    void sigClipSelected(SampleClip* sampleClip);
    void sigWheelEvent(int deltay);

    void sigListViewPositionChanged(int value);

    void speakerCountChanged();
    void sigTrackModelCreated();
    void exportProgressChanged();
    void sigExportFinished();

    void sigAddVisr(int trackIndex, bool enabled, bool reverbEnabled);

    void sigAddEffect(int trackIndex, QString name, bool enabled);

    void sigClipItemMoving();
    void sigClipItemRemoved();
    void sigClipItemEdited();
public slots:

    void sltKeyPressed(int modifier, int key);
    void sltKeyReleased(int modifier, int key);
    void sltFocusOut();

    void sltEffectLoaded(int trackIndex);
    void sltEffectUpdated(int trackIndex, int effectIndex);

    void sltPlayBackUpdateTimeout();
    void sltStartContentYChanged();
    void sltTimerCheckStartContentYChanged();
    void sltSampleChanged();
    void sltFirstBufferFilled(QString clipIndex, bool toPlay);

    void sltReadyToPlay();

    void sltTimerCheckPosition();

    void sltRowsExpandedChanged(int _index);

    void sltZoomFactorChanged();

    void sltControlVisibleChanged(int trackIndex, QString menu);
    void sltEffectEnableChanged(int trackIndex, QString menu);

    void sltFocusActiveChanged();

    void sltExportRenderProgress(int p);

    void sltExportRenderFinished();

    void slt3dEngineStarted();

public:
    QPair<int, int> findTrackIndexByPoint(QPointF point);

    void createSelectedArea(QSharedPointer<ClipItem> clipItem, int firstTrack, int lastTrack, double startTime,
        double endTime, int top, int bottom, int stratSubTrack = -1, int lastSubTrack = -1);
    bool selectingAreaResizing() const;
    void setSelectingAreaResizing(bool newSelectingAreaResizing);

    QString startSelectedClipIndex() const;
    void setStartSelectedClipIndex(const QString& newStartSelectedClipIndex);

    bool selectingAreaCreating() const;
    void setSelectingAreaCreating(bool newSelectingAreaCreating);

    bool automationChanging() const;
    void setAutomationChanging(bool newAutomationChanging);

    bool selectingAreaMoving() const;
    void setSelectingAreaMoving(bool newSelectingAreaMoving);

    bool keyFramesPointSelected() const;
    void setKeyFramesPointSelected(bool newKeyFramesPointSelected);

    int startSelectedTrackIndex() const;
    void setStartSelectedTrackIndex(int newStartSelectedTrackIndex);

    int startSelectedSubTrackIndex() const;
    void setStartSelectedSubTrackIndex(int newStartSelectedSubTrackIndex);

    bool selectedAreaMoved() const;
    void setSelectedAreaMoved(bool newSelectedAreaMoved);

    int trackLineResizing() const;
    void setTrackLineResizing(int newTrackLineResizing);

    double startSelectedTime() const;
    void setStartSelectedTime(double newStartSelectedTime);

    QPoint lastMouseMovingPoint() const;
    void setLastMouseMovingPoint(QPoint newLastMouseMovingPoint);

    QVector<AutomationList> movingAutomationPoint() const;

    double time2Frame(double time);
    void clearMovingItems();

    int trackLineResizingStartY() const;
    void setTrackLineResizingStartY(int newTrackLineResizingStartY);

    QJsonArray selectingStateBeforeResizing() const;

    void setSelectingStateBeforeResizing(const QJsonArray& newSelectingStateBeforeResizing);

    int listViewPositionChanged() const;
    void setListViewPositionChanged(int newListViewPositionChanged);

    void applySelectedAreaToTracks(double endSelectedTime, QPointF lastPos, int trackIndex, int subTrack = -1);

    void saveProject(QStringList object, QString project_name);
    void loadProject(QString project_name);

    int trackListHeight() const;
    void setTrackListHeight(int newTrackListHeight);
    QMap<QString, QSharedPointer<Effect>> loadingEffects() const;

    QVector<RectSeg> trackSegRects(int index);

    QVector<RectSeg> trackSelectedRects(int index);

    QVector<RectSeg> masterTrackSelectedRects();

    QVector<LineSeg> trackSegLines(int index);

    QSharedPointer<TrackItem> masterTrack() const;

private slots:
    void sltSelectedTrackItemChanged();
};

#endif // ClipArea_H
