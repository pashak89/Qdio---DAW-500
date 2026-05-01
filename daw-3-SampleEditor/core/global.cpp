#include "global.h"
#include "audio/audioengine.h"
#include <audio/engine.h>
#include <audio/song.h>
#include <core/measure.h>
#include <core/screeninterface.h>
ActionStack* ActionStack::s_instanceOfMe = nullptr;
AreaInfo::AreaInfo(KeyEventFilter* filter, AreaType areaType, QObject* parent)
    : QObject(parent)
    , _areaType(areaType)
    , m_keyEventFilter(filter)
{
    _minTitlebarCount = 4;
    _titleHeight = ScreenInterface::scaleSize2(52, zoomFactor());
    _expandedHeight = _titleHeight * _minTitlebarCount;

    _selectionArea = new SelectedArea(this);
    _lastSelectedArea = new SelectedArea(this);
    _trackMasterSelectionArea = new SelectedArea(this);

    _playbackUpdateTimer.setInterval(20);
    connect(&_playbackUpdateTimer, &QTimer::timeout, this, &AreaInfo::sigPlayBackUpdateTimeout);
    _playbackUpdateTimer.start();

    connect(AudioManager::inst()->getSong(), &Song::playbackStateChanged, this, &AreaInfo::playbackStateChanged);
}

double AreaInfo::findSnap(double posX)
{
    const double step = stepPlotDuration();
    if (step <= 0.0)
        return posX;

    // Convert pixel -> time
    const double t = pixel2Time(posX);

    // startTime() + pixel / (double)lastWidth() * clipDuration();
    // qDebug() << "TTT" << qSetRealNumberPrecision(100) << step << t << posX << lastWidth() << clipDuration() << startTime();

    // Snap to nearest multiple of step
    const double snappedT = std::round(t / step) * step;

    // Clamp to valid range (optional but usually good)
    const double maxT = maxEndTime();
    const double clampedT = std::clamp(snappedT, 0.0, maxT);

    // Convert snapped time -> pixel

    // qDebug() << "TTT1" << qSetRealNumberPrecision(100) << snappedT << t << posX << lastWidth() << clipDuration() << startTime() << time2Pixel(clampedT);

    return time2Pixel(clampedT);
}

double AreaInfo::findSnapLeft(double posX)
{
    const double step = stepPlotDuration();
    if (step <= 0.0)
        return posX;

    const double t = pixel2Time(posX);

    // previous multiple of step
    double snappedT = std::floor(t / step) * step;

    // clamp (optional, but usually correct)
    snappedT = std::clamp(snappedT, 0.0, maxEndTime());

    return time2Pixel(snappedT);
}

double AreaInfo::findSnapRight(double posX)
{
    const double step = stepPlotDuration();
    if (step <= 0.0)
        return posX;

    const double t = pixel2Time(posX);

    // next multiple of step
    double snappedT = std::ceil(t / step) * step;

    // clamp
    snappedT = std::clamp(snappedT, 0.0, maxEndTime());

    return time2Pixel(snappedT);
}

void AreaInfo::setObjectPosition(int trackIndex, double x, double y, double z)
{
    Q_EMIT sigObjectPosition(trackIndex, x, y, z);
}

void AreaInfo::playbackStateChanged()
{
    if (AudioManager::inst()->getSong()) {
        if (AudioManager::inst()->getSong()->isPlaying() == false) {
            _playbackUpdateTimer.stop();
        } else {
            _playbackUpdateTimer.start();
        }
    }
}

void AreaInfo::setKeyPressEvent(QKeyEvent* event)
{
    Q_EMIT sigKeyPressEvent(event);
}

void AreaInfo::init(int lastWidth, double final_size, double barDuration)
{

    if (lastWidth == 0) {
        qDebug() << "CTACH";
    }
    setLastWidth(lastWidth);

    setClipProperties(_startTime, _lastWidth / (double)final_size * barDuration * 1000.0 + _startTime, _lastWidth / (double)final_size * barDuration * 1000.0);

    setFileDuration(_clipDuration);
    setTimeDuration(_clipDuration);

    _startBraceMarker = Measure::measureToTime(3, 1, 1, 0, _lenghtOfBeat, _beatPerBar, _tempo);
    _endBraceMarker = Measure::measureToTime(7, 1, 1, 0, _lenghtOfBeat, _beatPerBar, _tempo);

    if (_maxEndTime <= 0 || _clipDuration > _maxEndTime)
        _maxEndTime = _clipDuration;

    _endMeasure = Measure::timeToMeasure(_clipDuration, _lenghtOfBeat, _beatPerBar, _tempo);
    _startMeasure = Measure::timeToMeasure(0, _lenghtOfBeat, _beatPerBar, _tempo);
}

int AreaInfo::lastWidth() const
{
    return _lastWidth;
}

void AreaInfo::setLastWidth(int lastWidth)
{
    _lastWidth = lastWidth;
    Q_EMIT sigLastWidthChanged();
}

void AreaInfo::setClipProperties(double startTime, double endTime, double clipDuration)
{

    // lf.push(startTime, endTime, clipDuration);

    _clipDuration = clipDuration;
    _startTime = startTime;
    _endTime = endTime;

    _startMeasure = Measure::timeToMeasure(_startTime, _lenghtOfBeat, _beatPerBar, _tempo);
    _endMeasure = Measure::timeToMeasure(_endTime, _lenghtOfBeat, _beatPerBar, _tempo);

    Q_EMIT timeArrangementStartChanged();
    Q_EMIT sigClipPropertiesChanged1();
}

void AreaInfo::setSelectionAreaRect(QRect rect)
{
    m_selectionAreaRect = rect;
    Q_EMIT sigSelectionAreaRectChanged();
}

QRect AreaInfo::selectionAreaRect() const
{
    return m_selectionAreaRect;
}

QRect AreaInfo::plotRect() const
{
    return m_plotRect;
}

void AreaInfo::setPlotRect(const QRect& newPlotRect)
{
    if (m_plotRect == newPlotRect)
        return;
    m_plotRect = newPlotRect;
    emit plotRectChanged();
}

double AreaInfo::clipDuration() const
{
    return _clipDuration;
}

double AreaInfo::startTime() const
{
    return _startTime;
}

double AreaInfo::endTime() const
{
    return _endTime;
}

double AreaInfo::lenghtOfBeat() const
{
    return _lenghtOfBeat;
}

void AreaInfo::setLenghtOfBeat(double lenghtOfBeat)
{
    _lenghtOfBeat = lenghtOfBeat;
    Q_EMIT timeArrangementStartChanged();
    Q_EMIT sigClipPropertiesChanged1();
}

double AreaInfo::beatPerBar() const
{
    return _beatPerBar;
}

void AreaInfo::setBeatPerBar(double beatPerBar)
{
    _beatPerBar = beatPerBar;
    Q_EMIT timeArrangementStartChanged();
    Q_EMIT sigClipPropertiesChanged1();
}

double AreaInfo::tempo() const
{
    return _tempo;
}

void AreaInfo::setTempo(double tempo)
{
    _tempo = tempo;
    Q_EMIT timeArrangementStartChanged();
    Q_EMIT sigClipPropertiesChanged1();
}

double AreaInfo::maxEndTime() const
{
    return _maxEndTime;
}

void AreaInfo::setMaxEndTime(double maxEndTime)
{
    _maxEndTime = maxEndTime;
}

double AreaInfo::minStartTime() const
{
    return _minStartTime;
}

void AreaInfo::setMinStartTime(double minStartTime)
{
    _minStartTime = minStartTime;
}

int AreaInfo::fixedGrid() const
{
    return _fixedGrid;
}

void AreaInfo::setFixedGrid(int fixedGrid)
{
    if (_fixedGrid != fixedGrid) {
        _fixedGrid = fixedGrid;
        Q_EMIT timeArrangementStartChanged();
        Q_EMIT sigClipPropertiesChanged1();
    }
}

int AreaInfo::adaptiveGrid() const
{
    return _adaptiveGrid;
}

void AreaInfo::setAdaptiveGrid(int adaptiveGrid)
{

    if (_adaptiveGrid != adaptiveGrid) {
        _adaptiveGrid = adaptiveGrid;
        Q_EMIT timeArrangementStartChanged();
        Q_EMIT sigClipPropertiesChanged1();
    }
}

double AreaInfo::timeDuration() const
{
    return _timeDuration;
}

void AreaInfo::setTimeDuration(double timeDuration)
{
    _timeDuration = timeDuration;
}

double AreaInfo::stepDuration() const
{
    return _stepDuration;
}

void AreaInfo::setStepDuration(double stepDuration)
{

    if (stepDuration != _stepDuration) {
        _stepDuration = stepDuration;
    }
}

double AreaInfo::stepPlotDuration() const
{
    return _stepPlotDuration;
}

void AreaInfo::setStepPlotDuration(double stepPlotDuration)
{

    if (stepPlotDuration != _stepPlotDuration) {
        _stepPlotDuration = stepPlotDuration;
    }
}

double AreaInfo::fileDuration() const
{
    return _fileDuration;
}

void AreaInfo::setFileDuration(double fileDuration)
{
    _fileDuration = fileDuration;
}

double AreaInfo::startBraceMarker() const
{
    return _startBraceMarker;
}

void AreaInfo::setStartBraceMarker(double startBraceMarker)
{
    _startBraceMarker = startBraceMarker;
    Q_EMIT startBraceMarkerChanged();
}

double AreaInfo::endBraceMarker() const
{
    return _endBraceMarker;
}

void AreaInfo::setEndBraceMarker(double endBraceMarker)
{
    _endBraceMarker = endBraceMarker;
    Q_EMIT endBraceMarkerChanged();
}

double AreaInfo::startMarker() const
{
    return _startMarker;
}

void AreaInfo::setStartMarker(double startMarker)
{
    _startMarker = startMarker;
}

double AreaInfo::endMarker() const
{
    return _endMarker;
}

void AreaInfo::setEndMarker(double endMarker)
{
    _endMarker = endMarker;
}

int AreaInfo::titleHeight() const
{
    return ScreenInterface::scaleSize2(52, m_zoomFactor);
}

void AreaInfo::setTitleHeight(int titleHeight)
{
    _titleHeight = titleHeight;
}

bool AreaInfo::altKeyPressed() const
{
    return _altKeyPressed;
}

void AreaInfo::setAltKeyPressed(bool altKeyPressed)
{

    _altKeyPressed = altKeyPressed;
}

void ActionStack::clearAllRedo()
{
    _redoItems.clear();
}

void ActionStack::pushInUndo(QJsonArray array)
{
    if (array.size() > 0) {
        _undoItems.push(Action { array });
    }
}

void ActionStack::pushInRedo(QJsonArray array)
{
    if (array.size() > 0) {
        _redoItems.push(Action { array });
    }
}

ActionStack::Action ActionStack::popFromUndo()
{
    return _undoItems.pop();
}

ActionStack::Action ActionStack::popFromRedo()
{
    return _redoItems.pop();
}

bool ActionStack::undoIsEmpty()
{
    return _undoItems.isEmpty();
}

bool ActionStack::redoIsEmpty()
{
    return _redoItems.isEmpty();
}

double AreaInfo::time2Pixel(double timeMs)
{
    const qint64 timeUs = std::llround(timeMs * 1000.0); // ms -> us
    const qint64 startUs = std::llround(startTime() * 1000.0); // ms -> us
    const qint64 durationUs = std::llround(clipDuration() * 1000.0); // ms -> us

    const qint64 deltaUs = timeUs - startUs;

    const double px = static_cast<double>(deltaUs) * static_cast<double>(lastWidth()) / static_cast<double>(durationUs);

    return px;
}

double AreaInfo::pixel2Time(double pixel)
{
    const qint64 startUs = std::llround(startTime() * 1000.0); // ms -> us
    const qint64 durationUs = std::llround(clipDuration() * 1000.0); // ms -> us

    const qint64 deltaUs = std::llround(pixel * static_cast<double>(durationUs) / static_cast<double>(lastWidth()));

    const qint64 timeUs = startUs + deltaUs;

    return static_cast<double>(timeUs) / 1000.0; // us -> ms
}

ZoomLevel AreaInfo::minZoomLevel() const
{
    return _minZoomLevel;
}

void AreaInfo::setMinZoomLevel(const ZoomLevel& minZoomLevel)
{
    _minZoomLevel = minZoomLevel;
}

ZoomLevel AreaInfo::zoomLevel() const
{
    return _zoomLevel;
}

void AreaInfo::setZoomLevel(const ZoomLevel& zoomLevel)
{
    if (_zoomLevel != zoomLevel) {
        _zoomLevel = zoomLevel;

        Q_EMIT sigZoomLevelChanged();
    }
}

bool AreaInfo::zoomEndded() const
{
    return _zoomEndded;
}

void AreaInfo::setZoomEndded(bool zoomEndded)
{
    _zoomEndded = zoomEndded;
}

double AreaInfo::timePerPixel() const
{
    if (clipDuration() > 0)
        return lastWidth() / clipDuration();
    return 1;
}

SelectedArea* AreaInfo::masterTrackSelectedArea()
{
    // I commented this line beacuse without this, app will crash,
    //_selectionArea->update();
    return _trackMasterSelectionArea;
}

SelectedArea* AreaInfo::selectionArea()
{
    // I commented this line beacuse without this, app will crash,
    //_selectionArea->update();
    return _selectionArea;
}

void AreaInfo::setSelectionArea(SelectedArea* selectionArea)
{
    _selectionArea = selectionArea;
}

SelectedArea* AreaInfo::lastSelectedArea() const
{
    return _lastSelectedArea;
}

void AreaInfo::setLastSelectedArea(SelectedArea* lastSelectedArea)
{
    _lastSelectedArea = lastSelectedArea;
}

bool AreaInfo::shiftkeyPressed() const
{
    return _shiftkeyPressed;
}

void AreaInfo::setShiftkeyPressed(bool shiftkeyPressed)
{
    _shiftkeyPressed = shiftkeyPressed;
}

bool AreaInfo::ctrlkeyPressed() const
{
    return _ctrlkeyPressed;
}

void AreaInfo::setCtrlkeyPressed(bool ctrlkeyPressed)
{
    _ctrlkeyPressed = ctrlkeyPressed;
    Q_EMIT ctrlkeyPressedChanged();
}

int AreaInfo::expandedHeight() const
{
    return titleHeight() * _minTitlebarCount;
}

void AreaInfo::setExpandedHeight(int expandedHeight)
{
    _expandedHeight = expandedHeight;
}

AreaInfo::ThemeType AreaInfo::themeType() const
{
    return _themeType;
}

void AreaInfo::setThemeType(const ThemeType& themeType)
{
    _themeType = themeType;
    Q_EMIT themeTypeChanged();
    Q_EMIT sigUpdateUI();
}

double AreaInfo::startOffsetX() const
{
    return _startOffsetX;
}

void AreaInfo::setStartOffsetX(double startOffsetX)
{
    _startOffsetX = startOffsetX;
}

double AreaInfo::stepColorDuration() const
{
    return _stepColorDuration;
}

void AreaInfo::setStepColorDuration(double stepColorDuration)
{
    _stepColorDuration = stepColorDuration;
}

double AreaInfo::stepColorSize() const
{
    return _stepColorSize;
}

void AreaInfo::setStepColorSize(double stepColorSize)
{
    _stepColorSize = stepColorSize;
}

double AreaInfo::stepPlotSize() const
{
    return _stepPlotSize;
}

void AreaInfo::setStepPlotSize(double stepPlotSize)
{
    _stepPlotSize = stepPlotSize;
}

bool AreaInfo::gridOff() const
{
    return _gridOff;
}

void AreaInfo::setGridOff(bool gridOff)
{
    _gridOff = gridOff;
}

void AreaInfo::setSelectedTrackItem(int index)
{
    m_selectedTrackItem = index;
    Q_EMIT sigSelectedTrackItemChanged();
}

int AreaInfo::selectedTrackItem() const
{
    return m_selectedTrackItem;
}

QRectF AreaInfo::selectedTrackItemRect() const
{
    return m_selectedTrackItemRect;
}

void AreaInfo::setSelectedTrackItemRect(const QRectF& selectedTrackItemRect)
{
    m_selectedTrackItemRect = selectedTrackItemRect;
    Q_EMIT sigSelectedTrackItemRectChanged();
}

AreaInfo::AreaType AreaInfo::areaType() const
{
    return _areaType;
}

float AreaInfo::zoomFactor() const
{
    return m_zoomFactor;
}

void AreaInfo::setZoomFactor(float zoomFactor)
{

    if (qAbs(m_zoomFactor - zoomFactor) > 0.001) {
        m_zoomFactor = zoomFactor;
        Q_EMIT zoomFactorChanged();
    }
}

int AreaInfo::minTitlebarCount() const
{
    return _minTitlebarCount;
}

int AreaInfo::seperatorHeight() const
{
    return ScreenInterface::scaleSize2(5, m_zoomFactor);
}

int AreaInfo::startContentY() const
{
    return _startContentY;
}

void AreaInfo::setStartContentY(int startContentY)
{

    if (_startContentY != startContentY) {
        _startContentY = startContentY;
        Q_EMIT startContentYChanged();
    }
}

int AreaInfo::trackListStartY() const
{
    return _trackListStartY;
}

void AreaInfo::setTrackListStartY(int trackListStartY)
{
    _trackListStartY = trackListStartY;
}

void AreaInfo::updateUI()
{

    Q_EMIT sigUpdateUI();
}

KeyEventFilter* AreaInfo::keyEventFilter() const
{
    return m_keyEventFilter;
}

bool AreaInfo::loopFocusActive() const
{
    return _loopFocusActive;
}

void AreaInfo::setLoopFocusActive(bool newLoopFocusActive)
{
    _loopFocusActive = newLoopFocusActive;
    Q_EMIT loopFocusActiveChanged();
}

bool AreaInfo::loopActive() const
{
    return _loopActive;
}

void AreaInfo::setLoopActive(bool newLoopFocusActive)
{
    _loopActive = newLoopFocusActive;
    Q_EMIT loopActiveChanged();
}

double AreaInfo::arrangementStartTime() const
{
    return _arrangementStartTime;
}

void AreaInfo::setArrangementStartTime(double arrangement)
{

    _arrangementStartTime = arrangement;

    Q_EMIT timeArrangementStartChanged();
}

void AreaInfo::setArrangementStartTime(int bar, int beat, int sixteenth)
{

    double time = Measure::measureToTime(bar, beat, sixteenth, 0, _lenghtOfBeat, _beatPerBar, _tempo);

    auto paused = AudioManager::inst()->getSong()->isPause();
    auto playing = AudioManager::inst()->getSong()->isPlaying();

    if (!paused && playing) {

        auto tick
            = time / 1000.0 * AudioManager::audioEngine()->outputSampleRate() / 256.0;

        // AudioManager::getSong()->setToTime(tick);
        emit playheadMarkerChanged();
    }

    if (paused || !playing) {
        setPlayheadMarkerVisible(false);
        _arrangementStartTime = time;
        Q_EMIT arrangementStartTimeChanged();
        Q_EMIT timeArrangementStartChanged();
    }
}

void AreaInfo::changeArrangementStartTimeBar(bool increase)
{

    const double beatDuration = (60.0 / _tempo) * (4.0 / (double)_lenghtOfBeat); // seconds
    const double barDuration = beatDuration * (double)_beatPerBar; // seconds
    const double sixteenthDuration = beatDuration / (16.0 / (double)_lenghtOfBeat); // seconds
    const double tickDuration = sixteenthDuration / 2048.0; // seconds

    int64_t time = barDuration * 1000.0;
    auto paused = AudioManager::inst()->getSong()->isPause();
    auto playing = AudioManager::inst()->getSong()->isPlaying();

    if (paused || !playing) {
        if (increase)
            _arrangementStartTime += time;
        else {
            _arrangementStartTime -= time;
        }
        if (_arrangementStartTime < 0) {
            _arrangementStartTime = 0;
        }

        _playheadMarker = _arrangementStartTime;

        Q_EMIT arrangementStartTimeChanged();
        Q_EMIT timeArrangementStartChanged();
    }
}

void AreaInfo::changeArrangementStartTimeBeat(bool increase)
{
    const double beatDuration = (60.0 / _tempo) * (4.0 / (double)_lenghtOfBeat); // seconds
    const double barDuration = beatDuration * (double)_beatPerBar; // seconds
    const double sixteenthDuration = beatDuration / (16.0 / (double)_lenghtOfBeat); // seconds
    const double tickDuration = sixteenthDuration / 2048.0; // seconds

    int64_t time = beatDuration * 1000.0;

    auto paused = AudioManager::inst()->getSong()->isPause();
    auto playing = AudioManager::inst()->getSong()->isPlaying();

    if (paused || !playing) {
        if (increase)
            _arrangementStartTime += time;
        else
            _arrangementStartTime -= time;

        if (_arrangementStartTime < 0) {
            _arrangementStartTime = 0;
        }

        _playheadMarker = _arrangementStartTime;

        Q_EMIT arrangementStartTimeChanged();
        Q_EMIT timeArrangementStartChanged();
    }
}

void AreaInfo::changeArrangementStartTimeSixteenth(bool increase)
{
    const double beatDuration = (60.0 / _tempo) * (4.0 / (double)_lenghtOfBeat); // seconds
    const double barDuration = beatDuration * (double)_beatPerBar; // seconds
    const double sixteenthDuration = beatDuration / (16.0 / (double)_lenghtOfBeat); // seconds
    const double tickDuration = sixteenthDuration / 2048.0; // seconds

    int64_t time = sixteenthDuration * 1000.0;

    auto paused = AudioManager::inst()->getSong()->isPause();
    auto playing = AudioManager::inst()->getSong()->isPlaying();

    if (paused || !playing) {
        if (increase)
            _arrangementStartTime += time;
        else
            _arrangementStartTime -= time;

        if (_arrangementStartTime < 0) {
            _arrangementStartTime = 0;
        }

        _playheadMarker = _arrangementStartTime;

        Q_EMIT arrangementStartTimeChanged();
        Q_EMIT timeArrangementStartChanged();
    }
}

bool AreaInfo::playheadMarkerVisible() const
{
    return _playheadMarkerVisible;
}

void AreaInfo::setPlayheadMarkerVisible(bool newPlayheadMarkerVisible)
{
    if (_playheadMarkerVisible == newPlayheadMarkerVisible)
        return;
    _playheadMarkerVisible = newPlayheadMarkerVisible;
    emit playheadMarkerVisibleChanged();
}

double AreaInfo::playheadMarker() const
{
    return _playheadMarker;
}

void AreaInfo::setPlayheadMarker(double newPlayheadMarker)
{
    if (qFuzzyCompare(_playheadMarker, newPlayheadMarker))
        return;
    _playheadMarker = newPlayheadMarker;
    emit playheadMarkerChanged();

    Q_EMIT timeArrangementStartChanged();
}

int AreaInfo::arrangementStartPixel()
{
    auto time = _arrangementStartTime;

    return time2Pixel(time);
}

double AreaInfo::editPointIndicator() const
{
    return _editPointIndicator;
}

void AreaInfo::setEditPointIndicator(double newEditPointIndicator)
{
    _editPointIndicator = newEditPointIndicator;
}

int AreaInfo::scrubAreaTop() const
{
    return _setScrubAreaTop;
}

void AreaInfo::setScrubAreaTop(int newSetScrubAreaTop)
{
    _setScrubAreaTop = newSetScrubAreaTop;
}

bool AreaInfo::selectedAreaActive() const
{
    return _selectedAreaActive;
}

void AreaInfo::setSelectedAreaActive(bool newSelectedAreaActive)
{
    if (newSelectedAreaActive != _selectedAreaActive) {
        _selectedAreaActive = newSelectedAreaActive;
        Q_EMIT selectedAreaActiveChanged();
    }
}

QStringList AreaInfo::timeArrangementStart()
{
    QStringList list;

    auto paused = AudioManager::inst()->getSong()->isPause();
    auto playing = AudioManager::inst()->getSong()->isPlaying();

    auto time = _arrangementStartTime;

    if (!paused && playing) {
        time = _playheadMarker;
    }

    auto m = Measure::timeToMeasure(time, lenghtOfBeat(), beatPerBar(), tempo());
    list << QString::number(m.bar) << QString::number(m.beat) << QString::number(m.sixteenth);

    return list;
}

QStringList AreaInfo::timeSelectionStart()
{
    QStringList list;

    auto time = _selectionArea->area().left();

    auto m = Measure::timeToMeasure(time, lenghtOfBeat(), beatPerBar(), tempo());
    list << QString::number(m.bar) << QString::number(m.beat) << QString::number(m.sixteenth);

    return list;
}

QStringList AreaInfo::timeSelectionEnd()
{
    QStringList list;

    auto time = _selectionArea->area().right();

    auto m = Measure::timeToMeasure(time, lenghtOfBeat(), beatPerBar(), tempo());
    list << QString::number(m.bar) << QString::number(m.beat) << QString::number(m.sixteenth);

    return list;
}

QStringList AreaInfo::timeSelectionLength()
{
    QStringList list;

    auto time2 = _selectionArea->area().right();
    auto time1 = _selectionArea->area().left();

    if (time1 == time2) {
        list << "1"
             << "0"
             << "0";
        return list;
    }

    auto m2 = Measure::timeToMeasure3(time2 - time1, lenghtOfBeat(), beatPerBar(), tempo());

    list << QString::number(m2.bar) << QString::number(m2.beat) << QString::number(m2.sixteenth);

    return list;
}

int AreaInfo::currentHoveTrackIndex() const
{
    return _currentHoveTrackIndex;
}

void AreaInfo::setCurrentHoveTrackIndex(int newCurrentHoveTrackIndex)
{
    _currentHoveTrackIndex = newCurrentHoveTrackIndex;
}

SelectedArea::SelectedArea(AreaInfo* areaInfo)
{
    _areaInfo = areaInfo;
}

void SelectedArea::setTracksRect(int index, QRectF track_area)
{
    _tracks_area[index] = track_area;
    update();
}
void SelectedArea::setTracksRect(QList<QRectF> tracks_area)
{
    _tracks_area = tracks_area;
    update();
}

void SelectedArea::update()
{

    if (_firstTrackIndex >= 0 && _lastTrackIndex >= 0 && _firstTrackIndex < _tracks_area.size() && _lastTrackIndex < _tracks_area.size()) {
        auto top = _tracks_area[_firstTrackIndex].top();
        auto bottom = _tracks_area[_lastTrackIndex].bottom();

        _area.setMyBottom(bottom);
        _area.setMyTop(top);

        if (_area.left() == _area.right()) {
            _areaInfo->setSelectedAreaActive(false);
            _areaInfo->setSelectionAreaRect(QRect(0, 0, 0, 0));
        } else {
            _areaInfo->setSelectedAreaActive(true);
        }

        Q_EMIT _areaInfo->timeSelectionChanged();
    }
}
