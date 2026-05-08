#ifndef AUTOMATIONITEM_H
#define AUTOMATIONITEM_H

#include <QCursor>
#include <QQuickPaintedItem>
#include <QTimer>

#include "core/automation.h"
#include "core/beatbar.h"

#include "ui/ipainteritem.h"

class TrackItem;
struct AutomationList {
    int trackIndex;
    int subtrackIndex;
    QString type;
    QString sub_type;
    QJsonArray aline;
    SelectedArea selectedArea;
};

class AutomationItem : public IPainterItem {
    Q_OBJECT

public:
    enum AutomationItemEvent {
        AutomationItemEvent_NotSelect,
        AutomationItemEvent_Select,
        AutomationItemEvent_InnerSelect,
        AutomationItemEvent_OuterPointSelect,
    };

    enum MoveDirection {
        MoveDirection_None,
        MoveDirection_Left,
        MoveDirection_Right,

    };

    struct UndoItem {
        QVector<Automation::ALine> lines;
        QVector<Automation::ALine> selected_lines;
        int startIndex;
        int endIndex;
        Automation::ALine _startPoint;
        Automation::ALine _endPoint;
        Automation::ALine _first_point;
        Automation::ALine _last_point;
        QVector<Automation::ALine> _first_list;
        QVector<Automation::ALine> _last_list;
        bool _init_selected_line_move;
        int _selected_lines_temp_size;
        int _temp_lines_size;
    };
    AutomationItem(int trackIndex, QSharedPointer<Automation> automation, AreaInfo* areaInfo);
    ~AutomationItem();

    bool createCopyList(SelectedArea selectedArea);
    bool insertCopiedList();
    bool cutCopiedList(SelectedArea selectedArea);
    bool removedCopiedList(SelectedArea selectedArea);
    void setLinesState(QJsonArray states);
    QJsonArray getLinesState();

    void setCurrentIndicator(double indicator);

    bool isInsideSelectedArea(SelectedArea& selectedArea)
    {
        int firstTrack = selectedArea.firstTrackIndex();
        int lastTrack = selectedArea.lastTrackIndex();
        int firstSub = selectedArea._firstSubTrackIndex;
        int lastSub = selectedArea._lastSubTrackIndex;

        int trackIndex = automation()->trackIndex();
        int subTrackIndex = automation()->getSubTrackIndex();

        const bool multipleTracks = (firstTrack != lastTrack);

        //
        // Case 1: Track strictly between first and last
        //
        if (trackIndex > firstTrack && trackIndex < lastTrack)
            return true;

        //
        // Case 2: On the first track of a multi-track selection
        //
        if (trackIndex == firstTrack && multipleTracks && subTrackIndex >= firstSub)
            return true;

        //
        // Case 3: On the last track of a multi-track selection
        //
        if (trackIndex == lastTrack && multipleTracks && subTrackIndex <= lastSub)
            return true;

        //
        // Case 4: Single-track selection
        //
        if (!multipleTracks && trackIndex == firstTrack && subTrackIndex >= firstSub && subTrackIndex <= lastSub) {
            return true;
        }

        return false;
    }

public:
    void draw(IPainter* painter);

protected:
    Automation::LineInfo _lineInfo;
    QSizeF _diffPressed;

    //-----

    Automation::HitState _hitPressedState = Automation::None;
    QPointF _hitPressedPoint = QPointF(-1, -1);

    Automation::HitState _hitHoverState = Automation::None;
    QPointF _hitHoverPoint = QPointF(-1, -1);

    double _currentIndicator = 0;
    QString _currentValue = "";
    double _maxYInSelectedcLines;
    double _minYInSelectedcLines;
    bool _altKeyPressed;
    int _trackIndex = 0;
    QSharedPointer<Automation> _automation;
    AreaInfo* _areaInfo;
    QPoint _lastMouseMovingPoint;
    QMutex _mutex;
    bool _saveingLastTemplatesLines = false;
    bool _mouseHover = false;
    bool _mousePress = false;
    bool _mouseRelease = false;
    bool _mouseMoving = false;
    bool _mouseClick = false;
    bool _mousePressAndHold = false;

    bool _curveChangingEnabled = false;
    // When false, a stationary left-click on a kf point will NOT delete it.
    // Used by KeyFramesAutomationItem so left-click only selects (delete is in
    // the right-click context menu). Default true preserves existing behavior
    // for all other automation lanes.
    bool _clickDeleteEnabled = true;
    bool _mouseDoubleClick = false;
    int _mouseDoubleClickCounter = 0;

    int _currentMouseHoverX = 0;
    int _currentMouseHoverY = 0;

    int _preMouseX = 0;
    int _preMouseY = 0;

    int _releaseMouseX = -1;
    int _releaseMouseY = -1;

    int _startDiffX = 0;
    int _startDiffY = 0;

    float _currentMouseX = -1;
    float _currentMouseY = -1;

    int _mousePressX = 0;
    int _mousePressY = 0;

    int _currentSelectedPointIndex = -1;
    int _currentIndex = -1;
    int _currentRangeIndex = -1;

    float _currentLineSlope = 0;
    int _currentMovingIndex1 = -1;
    int _currentMovingIndex2 = -1;
    QPointF _currentLinePoint;
    float _currentLinePointD1;
    float _currentLinePointD2;
    float _currentLinePointRatio = 0.0;

    int _currentMovingIndex = -1;
    bool _mouseShapeChangingEvent = false;
    int _currentCursorShape = Qt::ArrowCursor;

    bool _outerActive;
    bool _innerActive;
    bool _pointActive;

    bool _selectAllRangeEvent = false;

    MoveDirection _moveDirectionLeft = MoveDirection_None;
    bool _autoScrollLeft = false;
    bool _autoScrollRight = false;
    bool _autoScrollFirstOne = false;
    QTimer _timerCursorPosition;
    QTimer _timerCheckPressAndHold;
    QTimer _timerCheckDoubleClick;
    QTimer _timeMoving;

    bool _liveMovingEnabled = false;
    bool _selectedLinesMoving = false;
    bool _selectedLines = false;

    bool _selectedPointsMoving = false;

    float _start_time;

    QSizeF _diff;

    float distance_for_label1, distance_for_label2;

    int _excludePoint1, _excludePoint2;

    bool _checkStrachingActive = true;

    double _lastStartMarker = 0;
    double _lastEndMarker = 0;
    bool _saveEditState = true;

    bool _showEnvelopes = true;

    bool _isSelected = false;
    // bool _isSelected = false;

    virtual void saveInUndoItems();
    virtual void saveInUndoItems(QJsonArray state);

public:
    bool isSelectedLines();

    void checkEvents();

    void setAutomatedNormalized(double y);
    void setNormalized(double y);

    virtual double normalized2Value(double y) = 0;
    virtual double value2Normalized(double value) = 0;
    virtual QString normalized2Label(double y) = 0;

    virtual double value()
    {

        if (!_automation->isAutomated()) {

            return normalized2Value(_automation->normalized());
        } else {
            if (_automation->enabled()) {

                return normalized2Value(_automation->automatedNormalized());
            } else {

                return normalized2Value(_automation->normalized());
            }
        }

        // return normalized2Value(_automation->normalized());
    }
    virtual void setValue(double value)
    {
        _automation->setNormalized(value2Normalized(value));
    }
    virtual double automatedValue()
    {
        return normalized2Value(_automation->automatedNormalized());
    }

    int mousePressEvent(QMouseEvent* event);
    int mouseMoveEvent(QMouseEvent* event);
    int mouseReleaseEvent(QMouseEvent* event);
    int hoverMoveEvent(QHoverEvent* event);
    int mouseDoubleClickEvent(QMouseEvent* event);
    int keyPressEvent(int modifier, int key);
    int keyReleaseEvent(int modifier, int key);
    bool undo();
    bool redo();
    void sltTimerMoving();

    QSharedPointer<Automation> automation() const;

    bool showEnvelopes() const;
    void setShowEnvelopes(bool showEnvelopes);

    int trackIndex() const;

    void setEnabled(bool enabled);

    void setCurrentValue(const QString& newCurrentValue);

    int liveMovingEnabled();

    bool outerActive() const;

    bool innerActive() const;

    bool pointActive() const;

    int currentRangeIndex() const;

    bool mousePressAndHold() const;

    bool showEnvelopes();
    int currentMouseHoverX() const;

    int currentMouseHoverY() const;

    bool mouseHover() const;

    bool mouseClick() const;

    bool mouseDoubleClick() const;

    bool saveingLastTemplatesLines() const;
    void setSaveingLastTemplatesLines(bool newSaveingLastTemplatesLines);

    bool selectedLinesMoving() const;
    void setSelectedLinesMoving(bool newSelectedLinesMoving);

    bool selectedPointsMoving() const;
    void setSelectedPointsMoving(bool newSelectedPointsMoving);

    void setLiveMovingEnabled(bool newLiveMovingEnabled);

    double lastStartMarker() const;

    double lastEndMarker() const;

    void setLastEndMarker(double newLastEndMarker);

    void setLastStartMarker(double newLastStartMarker);

    bool checkStrachingActive() const;
    void setCheckStrachingActive(bool newCheckStrachingActive);

    void setOuterActive(bool newOuterActive);

    void setInnerActive(bool newInnerActive);

    void setPointActive(bool newPointActive);

    void setCurrentRangeIndex(int newCurrentRangeIndex);

    bool selectAllRangeEvent() const;
    void setSelectAllRangeEvent(bool newSelectAllRangeEvent);

    bool mouseShapeChangingEvent() const;
    void setMouseShapeChangingEvent(bool newMouseShapeChangingEvent);

    int currentIndex() const;

    int currentMovingIndex() const;

    int currentSelectedPointIndex() const;

    QString currentValue() const;

    QPointF hitPressedPoint() const;

    Automation::HitState hitHoverState() const;

    QPointF hitHoverPoint() const;

    void holdTriggered();

    Automation::HitState hitPressedState() const;

public Q_SLOTS:
    void sltTimerMouse();
    void sltTimerDoubleClicked();
    void sltLinesChanged();

    // IPainterItem interface
private slots:
    void sltTimerCheckPosition();

signals:
    void sigLineChanged();
    void sigNormalizedChanged();
    void sigAutomatedNormalizedChanged();
};

#endif // AUTOMATIONITEM_H
