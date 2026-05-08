#ifndef GLOBAL_H
#define GLOBAL_H

#include "screeninterface.h"

#include "keyeventfilter.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QKeyEvent>
#include <QMutex>
#include <QQmlApplicationEngine>
#include <QRectF>
#include <QStack>
#include <QTimer>
#include <QVector>
#include <core/measure.h>
class Effect;
class AreaInfo;
enum FixedGrid {
    FixedGrid_Off = 0,
    FixedGrid_8 = 1,
    FixedGrid_4 = 2,
    FixedGrid_2 = 3,
    FixedGrid_1 = 4,
    FixedGrid_1_2 = 5,
    FixedGrid_1_4 = 6,
    FixedGrid_1_8 = 7,
    FixedGrid_1_16 = 8,
    FixedGrid_1_32 = 9,
};

enum AdaptiveGrid {
    AdaptiveGrid_Off = 0,
    AdaptiveGrid_Narrow = 1,
    AdaptiveGrid_Narrowest = 2,
    AdaptiveGrid_Wide = 3,
    AdaptiveGrid_Widest = 4,
    AdaptiveGrid_Noraml = 5,
};

enum ZoomLevel {
    ZoomLevel_Init = 0,
    ZoomLevel_Bar4096 = 5,
    ZoomLevel_Bar2048 = 6,
    ZoomLevel_Bar1024 = 7,
    ZoomLevel_Bar512 = 8,
    ZoomLevel_Bar256 = 9,
    ZoomLevel_Bar128 = 10,
    ZoomLevel_Bar64 = 11,
    ZoomLevel_Bar32 = 12,
    ZoomLevel_Bar16 = 13,
    ZoomLevel_Bar8 = 14,
    ZoomLevel_Bar4 = 15,
    ZoomLevel_Bar2 = 16,
    ZoomLevel_Bar = 17,
    ZoomLevel_Beat = 18,
    ZoomLevel_Beat2 = 19,
    ZoomLevel_Beat4 = 20,
    ZoomLevel_Beat8 = 21,
    ZoomLevel_Beat16 = 22,
    ZoomLevel_Beat32 = 23,
    ZoomLevel_Beat64 = 24,
    ZoomLevel_Beat128 = 25,
    ZoomLevel_Beat256 = 26,
    ZoomLevel_Beat512 = 27,
    ZoomLevel_Beat1024 = 28,
    ZoomLevel_Beat2048 = 29,
    ZoomLevel_Beat4096 = 30,
    ZoomLevel_Beat8192 = 31,
    ZoomLevel_Beat16384 = 32,
    ZoomLevel_Beat32768 = 33,
    ZoomLevel_Beat65536 = 34,
};

enum PlayMovingMode {
    PlayMovingMode_Indicator = 0,
    PlayMovingMode_Background = 1,
};

template <typename T>
class ThreadSafeQVector {
public:
    ThreadSafeQVector() { }

    // Add an element to the vector
    void push_back(const T& value)
    {
        QMutexLocker locker(&mutex);
        vector.push_back(value);
    }

    // Access an element in the vector by index
    T at(int index)
    {
        QMutexLocker locker(&mutex);
        if (index >= 0 && index < vector.size()) {
            return vector.at(index);
        } else {
            // Handle out-of-bounds access gracefully
            throw std::out_of_range("Index out of bounds");
        }
    }

    void remove(int index)
    {
        QMutexLocker locker(&mutex);
        if (index >= 0 && index < vector.size()) {
            vector.remove(index);
        } else {
            // Handle out-of-bounds access gracefully
            throw std::out_of_range("Index out of bounds");
        }
    }

    // Get the size of the vector
    int size()
    {
        QMutexLocker locker(&mutex);
        return vector.size();
    }

private:
    QVector<T> vector;
    QMutex mutex;
};

class ActionStack {

public:
    enum Operation {
        Operation_InsertClipItem,
        Operation_DeleteClipItem,
        Operation_ResizeClipItem,
        Operation_SelectionArea,
        Operation_InsertAutomationPoint,
        Operation_DeleteAutomationPoint,
        Operation_EditAutomationPoint,
        Operation_RefreshAutomationPoint,
        Operation_DisableAutomationPoint,
        Operation_EnableAutomationPoint,

        Operation_EffectStatus,
        Operation_NormalizedValueChanged,
        Operation_EffectAdded,
        Operation_EffectRemoved,

        Operation_EditKeyFramesPoint,
        Operation_ObjectPosition,

    };

    struct Action {
        QJsonArray _actions;
    };

    void clearAllRedo();

    void pushInUndo(QJsonArray array);
    void pushInRedo(QJsonArray array);
    Action popFromUndo();
    Action popFromRedo();

    bool undoIsEmpty();
    bool redoIsEmpty();

private:
    QStack<Action> _undoItems;
    QStack<Action> _redoItems;

public:
    static inline ActionStack* instance()
    {
        if (s_instanceOfMe == nullptr) {
            s_instanceOfMe = new ActionStack();
        }
        return s_instanceOfMe;
    }
    static inline void destroy()
    {
        delete s_instanceOfMe;
        s_instanceOfMe = Q_NULLPTR;
    }

private:
    static ActionStack* s_instanceOfMe;
};

class MyRectF : public QRectF {

public:
    MyRectF(qreal x = 0, qreal y = 0, qreal w = 0, qreal h = 0)
        : QRectF(x, y, w, h)
    {
    }

    void setMyTop(qreal pos)
    {
        QRectF::setTop(pos);
    }
    void setMyBottom(qreal pos)
    {
        QRectF::setBottom(pos);
    }

private:
    void setHeight(qreal pos)
    {
        QRectF::setHeight(pos);
    }
    void setY(qreal pos)
    {
        QRectF::setY(pos);
    }
    void setTop(qreal pos)
    {
        QRectF::setTop(pos);
    }
    void setBottom(qreal pos)
    {
        QRectF::setBottom(pos);
    }
};

class SelectedArea {

private:
    MyRectF _area;

    QList<QRectF> _tracks_area;
    AreaInfo* _areaInfo;

public:
    SelectedArea(AreaInfo* areaInfo);
    void setTracksRect(QList<QRectF> tracks_area);
    void setTracksRect(int index, QRectF track_area);
    void update();

    SelectedArea(const SelectedArea& other)

        : _area(other._area)
        , _tracks_area(other._tracks_area)
        , _areaInfo(other._areaInfo)
        , _firstTrackIndex(other._firstTrackIndex)
        , _lastTrackIndex(other._lastTrackIndex)
        , _firstSubTrackIndex(other._firstSubTrackIndex)
        , _lastSubTrackIndex(other._lastSubTrackIndex)
        , leftDiff_pixel(other.leftDiff_pixel)
        , leftDiff(other.leftDiff)
        , topDiff(other.topDiff)
        , last_width(other.last_width)
        , firstPoint(other.firstPoint)
        , lastPoint(other.lastPoint)
        , isClipSelectedAtFirst(other.isClipSelectedAtFirst)
        , isClipSelectedAtLast(other.isClipSelectedAtLast)
    {
        // Optionally, if you want to refresh state or perform post-copy actions:
        update();
    }

public:
    int _firstTrackIndex = -1;
    int _lastTrackIndex = -1;
    int _firstSubTrackIndex = 0;
    int _lastSubTrackIndex = 0;
    double leftDiff_pixel = 0;
    double leftDiff = 0;
    double topDiff = 0;

    int last_width = 0;
    QPointF firstPoint;
    QPointF lastPoint;
    bool isClipSelectedAtFirst = false;
    bool isClipSelectedAtLast = false;

    const MyRectF& area() const { return _area; }

    void setRight(double right)
    {

        _area.setRight(right);
    }

    void setLeft(double left)
    {

        _area.setLeft(left);
    }

    void setWidth(double width)
    {

        _area.setWidth(width);
    }

    void setRect(MyRectF rect)
    {
        _area.setLeft(rect.left());
        _area.setRight(rect.right());
        _area.setMyTop(rect.top());
        _area.setMyBottom(rect.bottom());
    }

    int firstTrackIndex()
    {
        return _firstTrackIndex;
    }
    int lastTrackIndex()
    {
        return _lastTrackIndex;
    }
    void setLastTrackIndex(int index)
    {

        if (index < 0)
            index = 0;

        if (index >= _tracks_area.size()) {
            index = _tracks_area.size() - 1;
        }
        if (_firstTrackIndex > index) {
            _lastTrackIndex = _firstTrackIndex;
            _firstTrackIndex = index;
        } else {
            _lastTrackIndex = index;
        }
    }

    void setfirstTrackIndex(int index)
    {

        if (index < 0)
            index = 0;
        if (index > _tracks_area.size()) {
            index = _tracks_area.size() - 1;
        }
        _firstTrackIndex = index;
        _lastTrackIndex = index;
    }

    void setfirstSubTrackIndex(int index)
    {

        _firstSubTrackIndex = index;
    }

    void setLastSubTrackIndex(int index)
    {

        _lastSubTrackIndex = index;
    }
    QJsonObject state()
    {
        QJsonObject _state;
        _state["operation"] = ActionStack::Operation_SelectionArea;
        _state["rect"] = QString("%1, %2, %3, %4").arg(_area.x()).arg(_area.y()).arg(_area.width()).arg(_area.height());
        _state["first_track_index"] = _firstTrackIndex;
        _state["last_track_index"] = _lastTrackIndex;
        _state["left_diff"] = leftDiff;
        _state["top_diff"] = topDiff;
        return _state;
    }
    void setState(QJsonObject _state)
    {

        auto rect_str = (_state["rect"]).toString();
        QStringList list1 = rect_str.split(',');
        _area = MyRectF(QString(list1[0]).toDouble(), QString(list1[1]).toDouble(), QString(list1[2]).toDouble(), QString(list1[3]).toDouble());
        _firstTrackIndex = _state["first_track_index"].toInt();
        _lastTrackIndex = _state["last_track_index"].toInt();
        leftDiff = _state["left_diff"].toDouble();
        topDiff = _state["top_diff"].toDouble();
    }
};

class CloseClass : public QObject {
    Q_OBJECT
public:
    explicit CloseClass(QObject* parent = nullptr, QGuiApplication* app = nullptr, QQmlApplicationEngine* engine = nullptr)
        : QObject(parent)
        , m_app(app)
        , m_engine(engine)
    {
    }

public slots:
    Q_INVOKABLE void onCloseFunction()
    {
        qDebug() << "C++ function called on closing";
        if (m_engine) {
            QObject* rootObject = m_engine->rootObjects().first();

            if (rootObject) {
                QMetaObject::invokeMethod(rootObject, "destroy");
            }
            m_engine->quit();
        }
        if (m_app) {
            m_app->quit();
        }
    }

private:
    QGuiApplication* m_app;
    QQmlApplicationEngine* m_engine;
};

struct Node {
    double _s, _e, _d;
    std::atomic<Node*> next;

    Node(double s, double e, double d)
        : _s(s)
        , _e(e)
        , _d(d)
        , next(nullptr)
    {
    }
};

class LockFreeFIFO {
    int numReader = 5;

    std::unordered_map<int, std::string> myDict;
    std::mutex dictMutex;

public:
    LockFreeFIFO()
    {
        // dummy node
        Node* n = new Node(0, 0, 0);
        head.store(n);
        tail.store(n);
    }

    ~LockFreeFIFO()
    {
        while (pop() != false) { } // drain
        delete head.load();
    }
    void removeItem(int key)
    {
        std::lock_guard<std::mutex> lock(dictMutex); // lock mutex
        auto it = myDict.find(key);
        if (it != myDict.end()) {
            myDict.erase(it);

        } else {
        }
    }
    void registerReaders()
    {
        for (int i = 0; i < numReader; i++) {
            myDict[i] = "value";
        }
    }

    void push(double s, double e, double d)
    {
        Node* n = new Node(s, e, d);
        Node* prev_head = head.exchange(n); // atomically swap
        prev_head->next.store(n);
    }

    void get(double& s, double& e, double& d)
    {
        Node* tail_node = tail.load();
        s = tail_node->_s;
        e = tail_node->_e;
        d = tail_node->_d;
    }

    bool pop()
    {
        Node* tail_node = tail.load();
        Node* next = tail_node->next.load();
        if (next == nullptr) {
            return false;
        }

        tail.store(next);
        delete tail_node;
        return true;
    }
    void finishRead(int i)
    {

        removeItem(i);
        if (myDict.empty()) {
            registerReaders();
            pop();
        }
    }

private:
    std::atomic<Node*> head; // where we push
    std::atomic<Node*> tail; // where we pop
};

class AreaInfo : public QObject {
    Q_OBJECT
    Q_PROPERTY(float startContentY READ startContentY WRITE setStartContentY NOTIFY startContentYChanged)
    Q_PROPERTY(int minStepSize READ titleHeight NOTIFY sigMinTrackHeightSizeChanged)
    Q_PROPERTY(int selectedTrackItem READ selectedTrackItem NOTIFY sigSelectedTrackItemChanged)
    Q_PROPERTY(float zoomFactor READ zoomFactor WRITE setZoomFactor NOTIFY zoomFactorChanged)
    Q_PROPERTY(int seperatorHeight READ seperatorHeight NOTIFY zoomFactorChanged)
    Q_PROPERTY(int lastWidth READ lastWidth NOTIFY sigLastWidthChanged)
    Q_PROPERTY(int clipDuration READ clipDuration NOTIFY sigClipDurationChanged)
    Q_PROPERTY(int startTime READ startTime NOTIFY sigStartTimeChanged)

    Q_PROPERTY(AreaInfo::ThemeType themeType READ themeType WRITE setThemeType NOTIFY themeTypeChanged)

    Q_PROPERTY(bool selectedAreaActive READ selectedAreaActive NOTIFY selectedAreaActiveChanged)
    Q_PROPERTY(bool loopActive READ loopActive WRITE setLoopActive NOTIFY loopActiveChanged)

    Q_PROPERTY(QStringList timeSelectionStart READ timeSelectionStart NOTIFY timeSelectionChanged)
    Q_PROPERTY(QStringList timeSelectionEnd READ timeSelectionEnd NOTIFY timeSelectionChanged)
    Q_PROPERTY(QStringList timeSelectionLength READ timeSelectionLength NOTIFY timeSelectionChanged)
    Q_PROPERTY(QStringList timeArrangementStart READ timeArrangementStart NOTIFY timeArrangementStartChanged)

    Q_PROPERTY(int arrangementStartPixel READ arrangementStartPixel NOTIFY timeArrangementStartChanged)

    Q_PROPERTY(QRect plotRect READ plotRect WRITE setPlotRect NOTIFY plotRectChanged)

    Q_PROPERTY(double arrangementStartTime READ arrangementStartTime WRITE setArrangementStartTime NOTIFY arrangementStartTimeChanged)

    Q_PROPERTY(bool playheadMarkerVisible READ playheadMarkerVisible WRITE setPlayheadMarkerVisible NOTIFY playheadMarkerVisibleChanged)

    Q_PROPERTY(double playheadMarker READ playheadMarker WRITE setPlayheadMarker NOTIFY playheadMarkerChanged)

    Q_PROPERTY(bool ctrlkeyPressed READ ctrlkeyPressed NOTIFY ctrlkeyPressedChanged)

public:
    LockFreeFIFO lf;
    enum AreaType {
        AreaType_SampleEditor,
        AreaType_ClipArea

    };
    Q_ENUM(AreaType)
    enum ThemeType {
        ThemeType_Theme1,
        ThemeType_Theme2
    };
    Q_ENUM(ThemeType)

    explicit AreaInfo(KeyEventFilter* filter = Q_NULLPTR, AreaType areaType = AreaType_ClipArea, QObject* parent = Q_NULLPTR);

    double findSnap(double posX);

    double findSnapLeft(double posX);

    double findSnapRight(double posX);

    void setObjectPosition(int trackIndex, double x, double y, double z);

    void setKeyPressEvent(QKeyEvent* event);

    void init(int lastWidth, double final_size, double barDuration);

    int lastWidth() const;
    void setLastWidth(int lastWidth);

    void setClipProperties(double startTime, double endTime, double clipDuration);

    double clipDuration() const;

    double startTime() const;

    double endTime() const;

    double lenghtOfBeat() const;
    void setLenghtOfBeat(double lenghtOfBeat);

    double beatPerBar() const;
    void setBeatPerBar(double beatPerBar);

    double tempo() const;
    void setTempo(double tempo);

    double maxEndTime() const;
    void setMaxEndTime(double maxEndTime);

    double minStartTime() const;
    void setMinStartTime(double minStartTime);

    int fixedGrid() const;
    void setFixedGrid(int fixedGrid);

    int adaptiveGrid() const;
    void setAdaptiveGrid(int adaptiveGrid);

    double timeDuration() const;
    void setTimeDuration(double timeDuration);

    double stepPlotDuration() const;
    void setStepPlotDuration(double stepPlotDuration);

    double stepDuration() const;
    void setStepDuration(double stepDuration);

    double fileDuration() const;
    void setFileDuration(double fileDuration);

    double startBraceMarker() const;
    void setStartBraceMarker(double startBraceMarker);

    double endBraceMarker() const;
    void setEndBraceMarker(double endBraceMarker);

    double startMarker() const;
    void setStartMarker(double startMarker);

    double endMarker() const;
    void setEndMarker(double endMarker);

    int titleHeight() const;
    void setTitleHeight(int titleHeight);

    bool altKeyPressed() const;
    void setAltKeyPressed(bool altKeyPressed);

    ZoomLevel minZoomLevel() const;
    void setMinZoomLevel(const ZoomLevel& minZoomLevel);

    ZoomLevel zoomLevel() const;
    void setZoomLevel(const ZoomLevel& zoomLevel);

    bool zoomEndded() const;
    void setZoomEndded(bool zoomEndded);

    double time2Pixel(double time);
    double pixel2Time(double pixel);

    double timePerPixel() const;

    SelectedArea* masterTrackSelectedArea();

    SelectedArea* selectionArea();
    void setSelectionArea(SelectedArea* selectionArea);

    SelectedArea* lastSelectedArea() const;
    void setLastSelectedArea(SelectedArea* lastSelectedArea);

    bool shiftkeyPressed() const;
    void setShiftkeyPressed(bool shiftkeyPressed);

    bool ctrlkeyPressed() const;
    void setCtrlkeyPressed(bool ctrlkeyPressed);

    int expandedHeight() const;
    void setExpandedHeight(int expandedHeight);

    ThemeType themeType() const;
    void setThemeType(const ThemeType& themeType);

    double startOffsetX() const;
    void setStartOffsetX(double startOffsetX);

    double stepColorDuration() const;
    void setStepColorDuration(double stepColorDuration);

    double stepColorSize() const;
    void setStepColorSize(double stepColorSize);

    double stepPlotSize() const;
    void setStepPlotSize(double stepPlotSize);

    bool gridOff() const;
    void setGridOff(bool gridOff);

    void setSelectedTrackItem(int index);
    int selectedTrackItem() const;

    QRectF selectedTrackItemRect() const;
    void setSelectedTrackItemRect(const QRectF& selectedTrackItemRect);

    AreaType areaType() const;

    float zoomFactor() const;
    void setZoomFactor(float zoomFactor);

    int minTitlebarCount() const;

    int seperatorHeight() const;

    int startContentY() const;
    void setStartContentY(int startContentY);

    int trackListStartY() const;
    void setTrackListStartY(int trackListStartY);

    Q_INVOKABLE void updateUI();

    KeyEventFilter* keyEventFilter() const;

    bool loopFocusActive() const;
    void setLoopFocusActive(bool newLoopFocusActive);

    int scrubAreaTop() const;
    void setScrubAreaTop(int newSetScrubAreaTop);

    bool selectedAreaActive() const;
    void setSelectedAreaActive(bool newSelectedAreaActive);

    QStringList timeArrangementStart();

    QStringList timeSelectionStart();
    QStringList timeSelectionEnd();
    QStringList timeSelectionLength();

    int currentHoveTrackIndex() const;
    void setCurrentHoveTrackIndex(int newCurrentHoveTrackIndex);

    void setSelectionAreaRect(QRect rect);
    QRect selectionAreaRect() const;

    QRect plotRect() const;
    void setPlotRect(const QRect& newPlotRect);

    bool loopActive() const;

    void setLoopActive(bool newLoopFocusActive);

    double arrangementStartTime() const;
    void setArrangementStartTime(double arrangement);

    Q_INVOKABLE void setArrangementStartTime(int bar, int beat, int sixteenth);
    Q_INVOKABLE void changeArrangementStartTimeBar(bool increase);
    Q_INVOKABLE void changeArrangementStartTimeBeat(bool increase);
    Q_INVOKABLE void changeArrangementStartTimeSixteenth(bool increase);

    // Called by the QML kf context menu after the user picks an interp option.
    // interp: 0=Hold, 1=Linear, 2=Bezier (matches KeyInterp enum).
    Q_INVOKABLE void kfSetInterp(int trackIndex, qint64 time, int interp) {
        Q_EMIT sigInterpChanged(trackIndex, time, interp);
    }
    // Called by the QML kf context menu when the user picks Delete.
    Q_INVOKABLE void kfDelete(int trackIndex, qint64 time) {
        Q_EMIT sigRemoveKeyFrame(trackIndex, quint64(time));
    }

    bool playheadMarkerVisible() const;
    void setPlayheadMarkerVisible(bool newPlayheadMarkerVisible);

    double playheadMarker() const;
    void setPlayheadMarker(double newPlayheadMarker);

    int arrangementStartPixel();

    double editPointIndicator() const;
    void setEditPointIndicator(double newEditPointIndicator);

private:
    QRect m_plotRect;
    QRect m_selectionAreaRect;
    KeyEventFilter* m_keyEventFilter;
    QTimer _playbackUpdateTimer;
    float m_zoomFactor = 0.7;
    ThemeType _themeType = ThemeType_Theme1;
    ZoomLevel _minZoomLevel = ZoomLevel_Init;
    ZoomLevel _zoomLevel = ZoomLevel_Init;
    int m_selectedTrackItem = 0;
    QRectF m_selectedTrackItemRect;

    AreaType _areaType = AreaType_ClipArea;
    int _beatLevel = 1;
    bool _zoomEndded = false;

    bool _altKeyPressed = false;
    bool _shiftkeyPressed = false;
    bool _ctrlkeyPressed = false;

    int _fixedGrid = FixedGrid_Off;
    int _adaptiveGrid = AdaptiveGrid_Narrow;
    Measure _endMeasure;
    Measure _startMeasure;
    double _lenghtOfBeat = 4;
    double _beatPerBar = 4;
    double _tempo = 120.0;
    int _lastWidth = 0;
    double _maxEndTime = 0;
    double _minStartTime = 0;
    double _clipDuration = 0;
    double _fileDuration = 0;
    double _startTime = 0;
    double _endTime = 0;
    double _timeDuration = 0;
    double _stepPlotDuration = 0;
    double _stepDuration = 0;
    double _startOffsetX;
    double _stepColorDuration;
    double _stepColorSize;
    double _stepPlotSize;
    bool _gridOff;

    double _startBraceMarker = 0;
    double _endBraceMarker = 0;

    double _startMarker;
    double _endMarker;

    float _startContentY = 0;

    SelectedArea* _trackMasterSelectionArea;
    SelectedArea* _selectionArea;
    SelectedArea* _lastSelectedArea;
    int _titleHeight = 20;
    int _expandedHeight = 20;
    int _minTitlebarCount = 3;

    double _arrangementStartTime = 0;

    // this is for when we are moving a point,live in cliparea we should findwhihc time is changing
    double _editPointIndicator = -1;

    bool _playheadMarkerVisible = false;
    double _playheadMarker = 0;

    int _trackListStartY;

    int _setScrubAreaTop = 0;
    bool _loopFocusActive = false;

    bool _loopActive = false;

    bool _selectedAreaActive = false;

    int _currentHoveTrackIndex = -1;

signals:
    void sigSelectionAreaRectChanged();
    void sig3dEngineStarted();
    void sigObjectPosition(int trackIndex, double x, double y, double z);

    void endBraceMarkerChanged();
    void startBraceMarkerChanged();
    void loopFocusActiveChanged();
    void timeSelectionChanged();
    void selectedAreaActiveChanged();
    void sigCurrentSelectedTime(qint64 time);
    void sigKeyFrameClear(int trackIndex);
    void sigAddObject(int trackIndex, bool enabled);
    void sigEditKeyFrame(int trackIndex, quint64 last_time, quint64 new_time, int type);
    void sigAddKeyFrame(int trackIndex, quint64 time, int type);

    void sigKeyFrameChanged(int trackIndex, QMap<quint64, int> list);

    void sigAddKeyFrameWithPosition(int trackIndex, QString status);
    void sigRemoveKeyFrameWithPosition(int trackIndex, QString status);
    void sigEditKeyFrameWithPosition(int trackIndex, QString status);

    void sigRemoveKeyFrame(int trackIndex, quint64 time);
    void themeTypeChanged();
    void zoomFactorChanged();
    void sigSelectedTrackItemRectChanged();
    void sigSelectedTrackItemChanged();

    void sigPlayBackUpdateTimeout();
    void sigLastWidthChanged();
    void sigKeyframeLaneToggled(int trackIndex, bool active);
    void sigKeyFrameLinesChanged(int trackIndex, QList<qint64> times);
    // Right-click cycle on a keyframe in the timeline lane changed its interp.
    // interp values match KeyInterp enum: 0=Hold, 1=Linear, 2=Bezier.
    void sigInterpChanged(int trackIndex, qint64 time, int interp);
    // Right-click on a kf in the lane: show context menu at global screen position.
    void sigKFContextMenu(int trackIndex, qint64 time, int globalX, int globalY);
    // Selection sync: emitted when the user left-clicks a kf in the lane OR
    // clicks a kf marker in the 2D/3D viewport. Updates lane halo + 3D/2D
    // tangent handles.
    void sigKFSelected(int trackIndex, qint64 time);
    void sigZoomLevelChanged();
    void sigClipDurationChanged();
    void sigStartTimeChanged();
    void sigEndTimeChanged();
    void sigKeyPressEvent(QKeyEvent* event);

    void startContentYChanged();
    void sigUpdateUI();
    void sigMinTrackHeightSizeChanged();
    void sigPlayChanged(bool play);

    void sigClipPropertiesChanged1();

    void plotRectChanged();

    void loopActiveChanged();
    void arrangementStartTimeChanged();
    void timeArrangementStartChanged();

    void playheadMarkerVisibleChanged();

    void playheadMarkerChanged();

    void ctrlkeyPressedChanged();

private slots:
    void playbackStateChanged();
};

#endif // GLOBAL_H
