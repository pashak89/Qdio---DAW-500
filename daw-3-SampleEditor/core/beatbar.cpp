//#include "beatbar.h"

//#include <QEvent>
//#include <QTime>
//#include <QtMath>
//BeatBar::BeatBar(QObject* parent)
//    : QObject(parent)
//{

//    _rulerTimeMap.insert(RulerTimeLevel_1, 1);
//    _rulerTimeMap.insert(RulerTimeLevel_2, 2);
//    _rulerTimeMap.insert(RulerTimeLevel_5, 5);
//    _rulerTimeMap.insert(RulerTimeLevel_10, 10);
//    _rulerTimeMap.insert(RulerTimeLevel_20, 20);
//    _rulerTimeMap.insert(RulerTimeLevel_50, 50);
//    _rulerTimeMap.insert(RulerTimeLevel_100, 100);
//    _rulerTimeMap.insert(RulerTimeLevel_200, 200);
//    _rulerTimeMap.insert(RulerTimeLevel_500, 500);
//    _rulerTimeMap.insert(RulerTimeLevel_1000, 1000);
//    _rulerTimeMap.insert(RulerTimeLevel_2000, 2000);
//    _rulerTimeMap.insert(RulerTimeLevel_5000, 5000);
//    _rulerTimeMap.insert(RulerTimeLevel_10000, 10000);
//    _rulerTimeMap.insert(RulerTimeLevel_20000, 20000);
//    _rulerTimeMap.insert(RulerTimeLevel_15000, 15000);
//    _rulerTimeMap.insert(RulerTimeLevel_30000, 30000);
//    _rulerTimeMap.insert(RulerTimeLevel_60000, 60000);

//    _startMeasure = timeToMeasure(0);

//    _playMovingMode = PlayMovingMode_Indicator;
//    _isPlaying = false;
//}

//void BeatBar::update(int width)
//{

//    if (_currentWidth == 0.0) {

//        setLastWidth(width);
//        _currentWidth = width;

//    } else {
//        if (width > _currentWidth) {
//            _update = true;
//        }
//    }
//    _currentWidth = width;
//    if (_update == false) {
//        return;
//    }

//    setMinBarWidth(ScreenInterface::instance()->freeResolution(30));
//    setMaxBarWidth(ScreenInterface::instance()->freeResolution(60));

//    setCurrentWidth(width);

//    if (_timeDuration == 0.0) {
//        _currentClipDuration = 120000.0;
//        _timeDuration = 120000.0;
//        _maxEndTime = 120000.0;
//        _currentStartTime = 0;
//        _currentEndTime = _timeDuration;
//        _endBraceMarker = _timeDuration;
//    }

//    _update = false;
//}

//double BeatBar::timeDuration() const
//{
//    return _timeDuration;
//}

//void BeatBar::setTimeDuration(double timeDuration)
//{
//    _timeDuration = timeDuration;
//    //    _currentClipDuration = _timeDuration;
//    //    _tempDuration = _timeDuration;
//    //    _endBraceMarker = _timeDuration;
//    //    update(_lastWidth);
//}

//void BeatBar::play()
//{
//    if (!_baseTimer.isActive()) {
//        _isPlaying = true;
//        _baseTimer.start(_timerValue, Qt::PreciseTimer, this);
//    }
//}

//void BeatBar::pause()
//{
//    _isPlaying = false;
//    _baseTimer.stop();
//}

//void BeatBar::stop()
//{
//    _isPlaying = false;
//    _baseTimer.stop();
//}

//bool BeatBar::isPlaying()
//{
//    return _isPlaying;
//}

//void BeatBar::forward()
//{
//    _playbackMarker += _timerValue;
//    if (_currentClipDuration >= _timeDuration) {
//        _currentClipDuration = _timeDuration;
//    }
//    Q_EMIT sigBaseTimerUpdate();
//}

//int BeatBar::timeFind(double time, QVector<BeatBar::Info>* info)
//{

//    QString str = "";
//    for (int i = 0; i < info->count(); i++) {
//        str += QString::number(info->at(i).time) + "(" + info->at(i).text + ")" + " , ";
//    }

//    for (int i = 0; i < info->count(); i++) {
//        if (time <= info->at(i).time) {
//            return i;
//        }
//    }

//    return -1;
//}

//int BeatBar::timeFindInRulerTime(double time, QVector<BeatBar::TimeRulerInfo>* info)
//{

//    for (int i = 0; i < info->count(); i++) {
//        if (time <= info->at(i).time) {
//            return i;
//        }
//    }
//    return -1;
//}

//Automation* BeatBar::getAutomation_wave_editor() const
//{
//    return _automation_wave_editor;
//}

//Automation* BeatBar::getAutomation_clip_area() const
//{
//    return _automation_clip_area;
//}

//bool BeatBar::getGreenWindowZoomStop() const
//{
//    return _greenWindowZoomStop;
//}

//void BeatBar::setGreenWindowZoomStop(bool greenWindowZoomStop)
//{
//    _greenWindowZoomStop = greenWindowZoomStop;
//}

//int BeatBar::getFirstFromLeft(QList<QPoint> list) const
//{
//    int preValue = -1;
//    for (int i = 0; i < list.size(); i++) {

//        if (i == 0) {
//            preValue = list[i].x();
//            continue;
//        }
//        if (list[i].x() < preValue && list[i].x() != -1) {
//            return i;
//        }
//        preValue = list[i].x();
//    }

//    return -1;
//}

//bool BeatBar::getAutomation() const
//{
//    return _automation;
//}

//void BeatBar::setAutomation(bool automation)
//{
//    _automation = automation;
//}

//bool BeatBar::getAutomationPressAndHold() const
//{
//    return _automationPressAndHold;
//}

//void BeatBar::setAutomationPressAndHold(bool automationPressAndHold)
//{
//    _automationPressAndHold = automationPressAndHold;
//}
//void BeatBar::setEnvelopePressAndHold(bool active)
//{
//    _envelopePressAndHold = active;
//}
//bool BeatBar::envelopePressAndHold()
//{
//    return _envelopePressAndHold;
//}

//double BeatBar::getMaxEndTime() const
//{
//    return _maxEndTime;
//}

//void BeatBar::setMaxEndTime(double maxEndTime)
//{
//    _maxEndTime = maxEndTime;
//}

//double BeatBar::getMinStartTime() const
//{
//    return _minStartTime;
//}

//void BeatBar::setMinStartTime(double minStartTime)
//{
//    _minStartTime = minStartTime;
//}

//double BeatBar::currentFileDuration() const
//{
//    return _fileDuration;
//}

//void BeatBar::setFileDuration(double fileDuration)
//{
//    _fileDuration = fileDuration;
//}

//double BeatBar::getEndPixel() const
//{
//    return _endPixel;
//}

//void BeatBar::setEndPixel(double endPixel)
//{
//    _endPixel = endPixel;
//}

//double BeatBar::getStartPixel() const
//{
//    return _startPixel;
//}

//void BeatBar::setStartPixel(double startPixel)
//{
//    _startPixel = startPixel;
//}

//double BeatBar::getSamplePerPixel() const
//{
//    return _samplePerPixel;
//}

//void BeatBar::setSamplePerPixel(double samplePerPixel)
//{
//    _samplePerPixel = samplePerPixel;
//}

//double BeatBar::getCurrentBaseEndTime() const
//{
//    return _currentBaseEndTime;
//}

//void BeatBar::setBaseEndTime(double currentBaseEndTime)
//{
//    _currentBaseEndTime = currentBaseEndTime;
//}

//double BeatBar::getCurrentBaseStartTime() const
//{
//    return _currentBaseStartTime;
//}

//void BeatBar::setBaseStartTime(double currentBaseStartTime)
//{
//    _currentBaseStartTime = currentBaseStartTime;
//}

//double BeatBar::getMovePixel() const
//{
//    return _movePixel;
//}

//void BeatBar::setMovePixel(double movePixel)
//{
//    _movePixel = movePixel;
//}

//SampleClip* BeatBar::getSampleClip() const
//{
//    return _sampleClip;
//}

//void BeatBar::setSampleClip(SampleClip* sampleClip)
//{
//    _sampleClip = sampleClip;
//}

//QMap<int, double*> BeatBar::getRms()
//{
//    return _rms;
//}

//QMap<int, double*> BeatBar::getMax()
//{
//    return _max;
//}

//QMap<int, double*> BeatBar::getMin()
//{
//    return _min;
//}

//int BeatBar::getChannelCount() const
//{
//    return _channelCount;
//}

//void BeatBar::setChannelCount(int channelCount)
//{
//    _channelCount = channelCount;

//    for (int i = 0; i < channelCount; i++) {
//        if (_min[i])
//            free(_min[i]);
//        if (_max[i])
//            free(_max[i]);
//        if (_rms[i])
//            free(_rms[i]);
//    }

//    _min.clear();
//    _max.clear();
//    _rms.clear();

//    for (int i = 0; i < channelCount; i++) {
//        _min.insert(i, nullptr);
//        _max.insert(i, nullptr);
//        _rms.insert(i, nullptr);
//    }
//}

//QVector<int>* BeatBar::getPlotLines()
//{
//    return &_plotLines;
//}

//double BeatBar::getTimerValue() const
//{
//    return _timerValue;
//}

//int BeatBar::getBackward() const
//{
//    return _backward;
//}

//void BeatBar::setBackward(int backward)
//{
//    _backward = backward;
//}

//double BeatBar::getCurrentEndTime() const
//{
//    return _currentEndTime;
//}

//void BeatBar::setEndTime(double currentEndTime)
//{
//    _currentEndTime = currentEndTime;
//}

//double BeatBar::getCurrentStartTime() const
//{
//    return _currentStartTime;
//}

//void BeatBar::setStartTime(double currentStartTime)
//{
//    _currentStartTime = currentStartTime;
//}

//quint32 BeatBar::getSampleRate() const
//{
//    return _sampleRate;
//}

//void BeatBar::setSampleRate(const quint32& sampleRate)
//{
//    _sampleRate = sampleRate;
//}

//double BeatBar::getLastHotSpotPressTime() const
//{
//    return _lastHotSpotPressTime;
//}

//void BeatBar::setLastHotSpotPressTime(double lastHotSpotPressTime)
//{
//    _lastHotSpotPressTime = lastHotSpotPressTime;
//}

//double BeatBar::getLastBeatBarPressX() const
//{
//    return _lastBeatBarPressX;
//}

//void BeatBar::setLastBeatBarPressX(double lastBeatBarPressX)
//{
//    _lastBeatBarPressX = lastBeatBarPressX;
//}

//double BeatBar::getLastHotSpotTimeEnd() const
//{
//    return _lastHotSpotTimeEnd;
//}

//void BeatBar::setLastHotSpotTimeEnd(double lastHotSpotTimeEnd)
//{
//    _lastHotSpotTimeEnd = lastHotSpotTimeEnd;
//}

//double BeatBar::getLastHostSpotTimeStart() const
//{
//    return _lastHostSpotTimeStart;
//}

//void BeatBar::setLastHostSpotTimeStart(double lastHostSpotTimeStart)
//{
//    _lastHostSpotTimeStart = lastHostSpotTimeStart;
//}

//bool BeatBar::getLoopFocus() const
//{
//    return _loopFocus;
//}

//void BeatBar::setLoopFocus(bool loopFocus)
//{
//    _loopFocus = loopFocus;
//}

//QColor BeatBar::getLoopBgColor() const
//{
//    return _loopBgColor;
//}

//void BeatBar::setLoopBgColor(const QColor& loopBgColor)
//{
//    _loopBgColor = loopBgColor;
//}

//double BeatBar::getEndBraceMarker() const
//{
//    return _endBraceMarker;
//}

//void BeatBar::setEndBraceMarker(double endBraceMarker)
//{
//    _endBraceMarker = endBraceMarker;
//}

//double BeatBar::getStartBraceMarker() const
//{
//    return _startBraceMarker;
//}

//void BeatBar::setStartBraceMarker(double startBraceMarker)
//{
//    _startBraceMarker = startBraceMarker;
//}

//double BeatBar::getEndMarker() const
//{
//    return _endMarker;
//}

//void BeatBar::setEndMarker(double endMarker)
//{
//    _endMarker = endMarker;
//    Q_EMIT sigMarkerChanged();
//}

//double BeatBar::getStartMarker() const
//{

//    return _startMarker;
//}

//void BeatBar::setStartMarker(double startMarker)
//{
//    _startMarker = startMarker;

//    Q_EMIT sigMarkerChanged();
//}

//double BeatBar::getPlaybackMarker() const
//{
//    return _playbackMarker;
//}

//void BeatBar::setPlaybackMarker(double playbackMarker)
//{
//    _playbackMarker = playbackMarker;
//}

//QString BeatBar::getStartMarkerString()
//{
//    Measure m = timeToMeasure2(_startMarker);
//    return QString::number(m.bar) + "." + QString::number(m.beat) + "." + QString::number(m.sixteenth);
//}

//double BeatBar::getStepPlotDuration() const
//{
//    return _stepPlotDuration;
//}

//void BeatBar::setStepPlotDuration(double stepPlotDuration)
//{

//    _stepPlotDuration = stepPlotDuration;
//}

//qint64 BeatBar::getLastEndFrame() const
//{
//    return _lastEndFrame;
//}

//void BeatBar::setLastEndFrame(const qint64& lastEndFrame)
//{
//    _lastEndFrame = lastEndFrame;
//}

//qint64 BeatBar::getLastStartFrame() const
//{
//    return _lastStartFrame;
//}

//void BeatBar::setLastStartFrame(const qint64& lastStartFrame)
//{
//    _lastStartFrame = lastStartFrame;
//}

//BeatBar::PlayMovingMode BeatBar::getPlayMovingMode() const
//{
//    return _playMovingMode;
//}

//void BeatBar::setPlayMovingMode(const PlayMovingMode& playMovingMode)
//{
//    _playMovingMode = playMovingMode;
//}

//void BeatBar::timerEvent(QTimerEvent* event)
//{
//    if (event->timerId() == _baseTimer.timerId()) {
//        _playbackMarker += _timerValue;
//        if (_currentClipDuration >= _timeDuration) {
//            _currentClipDuration = _timeDuration;
//        }
//        Q_EMIT sigBaseTimerUpdate();
//    } else {
//        QObject::timerEvent(event);
//    }
//}

//double BeatBar::getTempDuration() const
//{
//    return _tempDuration;
//}

//void BeatBar::setTempDuration(double tempDuration)
//{
//    _tempDuration = tempDuration;
//}

//bool BeatBar::getUpdate() const
//{
//    return _update;
//}

//void BeatBar::setUpdate(bool update)
//{
//    _update = update;
//    Q_EMIT updateChanged();
//}

//int BeatBar::adaptiveGrid() const
//{
//    return _adaptiveGrid;
//}

//void BeatBar::setAdaptiveGrid(int adaptiveGrid)
//{
//    _adaptiveGrid = adaptiveGrid;
//    Q_EMIT adaptiveGridCahnged();
//}

//double BeatBar::getCurrentWidth() const
//{
//    return _currentWidth;
//}

//void BeatBar::setCurrentWidth(double currentWidth)
//{
//    _currentWidth = currentWidth;
//}

//double BeatBar::getLastWidth() const
//{
//    return _lastWidth;
//}

//void BeatBar::setLastWidth(double lastWidth)
//{
//    _lastWidth = lastWidth;
//}

//// double BeatBar::getBeatDuration() const
////{
////     return _beatDuration;
//// }

//// double BeatBar::getSixteenthDuration() const
////{
////     return _sixteenthDuration;
//// }

//// double BeatBar::getBarDuration() const
////{
////     return _barDuration;
//// }

//int BeatBar::fixedGrid() const
//{
//    return _fixedGrid;
//}

//void BeatBar::setFixedGrid(int fixedGrid)
//{
//    _fixedGrid = fixedGrid;
//    Q_EMIT fixedGridChanged();
//}

//BeatBar::ZoomLevel BeatBar::getMinZoomLevel() const
//{
//    return _minZoomLevel;
//}

//void BeatBar::setMaxBarWidth(int maxBarWidth)
//{
//    _maxBarWidth = maxBarWidth;
//}

//void BeatBar::setMinBarWidth(int minBarWidth)
//{
//    _minBarWidth = minBarWidth;
//}

//int BeatBar::getRulerTimeStartIndex() const
//{
//    return _rulerTimeStartIndex;
//}

//double BeatBar::getRulerTimeStartPixelOffset() const
//{
//    return _rulerTimeStartPixelOffset;
//}

//double BeatBar::getRulerTimeBarSize() const
//{
//    return _rulerTimeBarSize;
//}

//QVector<BeatBar::TimeRulerInfo> BeatBar::getTimeRulerInfos() const
//{
//    return _timeRulerInfos;
//}

//void BeatBar::setZoomEndded(bool zoomEndded)
//{
//    _zoomEndded = zoomEndded;
//}

//bool BeatBar::getZoomEndded() const
//{
//    return _zoomEndded;
//}

//int BeatBar::getBeatLevel() const
//{
//    return _beatLevel;
//}

//void BeatBar::setBeatLevel(int beatLevel)
//{
//    _beatLevel = beatLevel;
//}

//int BeatBar::lenghtOfBeat() const
//{
//    return _lenghtOfBeat;
//}

//void BeatBar::setLenghtOfBeat(int lenghtOfBeat)
//{
//    _lenghtOfBeat = lenghtOfBeat;
//    timeToMeasure(_timeDuration);
//}

//int BeatBar::beatPerBar() const
//{
//    return _beatPerBar;
//}

//void BeatBar::setBeatPerBar(int beatPerBar)
//{
//    _beatPerBar = beatPerBar;
//    timeToMeasure(_timeDuration);
//}

//double BeatBar::tempo() const
//{
//    return _tempo;
//}

//double BeatBar::getCurrentTempoIndex() const
//{
//    return _currentTempoIndex;
//}

//void BeatBar::setCurrentTempoIndex(double currentTempoIndex)
//{
//    _currentTempoIndex = currentTempoIndex;

//    Q_EMIT currentTempoChanged();
//}

//void BeatBar::setTempo(double tempo)
//{

//    //    _startMeasure = timeToMeasure(_startTimeOffset);
//    //    _durationMeasure = timeToMeasure(_currentClipDuration);
//    //    _endMeasure = timeToMeasure(_timeDuration);
//    _tempo = tempo;
//    //    _startTimeOffset = measureToTime(_startMeasure);
//    //    _currentClipDuration = measureToTime(_durationMeasure);
//    //    _endTimeOffset = measureToTime(_endMeasure);
//    //    _timeDuration = measureToTime(_endMeasure);
//}

//double BeatBar::currentClipDuration() const
//{
//    return _currentClipDuration;
//}

//void BeatBar::setcurrentClipDuration(double currentClipDuration)
//{
//    _currentClipDuration = currentClipDuration;
//}

//int BeatBar::startIndex() const
//{
//    return _startIndex;
//}

//QVector<BeatBar::Info> BeatBar::getZoomRulerInfos() const
//{
//    return _zoomRulerInfos;
//}

//double BeatBar::offsetPixel() const
//{
//    return _startPixelOffset;
//}

//double BeatBar::finalSize() const
//{
//    return _finalSize;
//}

//void BeatBar::setFinalSize(double finalSize)
//{
//    _finalSize = finalSize;
//}

//double BeatBar::measureToTime(Measure measure)
//{
//    return measureToTime(measure.bar, measure.beat, measure.sixteenth, measure.subSixteenth);
//}

//void BeatBar::setZoomLevel(const ZoomLevel& zoomLevel)
//{
//    _zoomLevel = zoomLevel;
//}

//BeatBar::ZoomLevel BeatBar::zoomLevel() const
//{
//    return _zoomLevel;
//}

//int BeatBar::maxBarWidth() const
//{
//    return _maxBarWidth;
//}


//int BeatBar::minBarWidth() const
//{
//    return _minBarWidth;
//}
