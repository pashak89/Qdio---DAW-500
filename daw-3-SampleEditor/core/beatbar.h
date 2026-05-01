//#ifndef BEATBAR_H
//#define BEATBAR_H

//#include <QDebug>
//#include <QElapsedTimer>
//#include <QMutex>
//#include <QMutexLocker>
//#include <QStack>
//#include <QTimer>
//#include <QVector>
//#include <qcolor.h>
//#include <qobject.h>

//#include "audio/sampleclip.h"
//#include "core/automation.h"
//#include "core/screeninterface.h"

//class BeatBar : public QObject {
//    Q_OBJECT

//    Q_PROPERTY(int beatPerBar READ beatPerBar WRITE setBeatPerBar NOTIFY beatPerBarChanged)
//    Q_PROPERTY(int lenghtOfBeat READ lenghtOfBeat WRITE setLenghtOfBeat NOTIFY lenghtOfBeatChanged)
//    Q_PROPERTY(double tempo READ tempo WRITE setTempo NOTIFY tempoChanged)

//    Q_PROPERTY(int fixedGrid READ fixedGrid WRITE setFixedGrid NOTIFY fixedGridChanged)
//    Q_PROPERTY(int adaptiveGrid READ adaptiveGrid WRITE setAdaptiveGrid NOTIFY adaptiveGridCahnged)

//    Q_PROPERTY(bool updateEnable READ getUpdate WRITE setUpdate NOTIFY updateChanged)

//    Q_PROPERTY(double startMarker READ getStartMarker NOTIFY sigMarkerChanged)
//    Q_PROPERTY(QString startMarkerString READ getStartMarkerString NOTIFY sigMarkerChanged)

//    Q_PROPERTY(double currentTempo READ currentTempo WRITE setCurrentTempo NOTIFY currentTempoChanged)

//public:
//    struct Info {
//        double time;
//        QString text;
//        int type; // litle,big,hasnumber
//        Measure measure;
//    };

//    struct TimeRulerInfo {
//        double time;
//        QString text;
//        int type; // litle,big,hasnumber
//    };

//    explicit BeatBar(QObject* parent = nullptr);

//    Q_INVOKABLE void update(int width);

//    QVector<BeatBar::Info> getZoomRulerInfos() const;
//    QVector<TimeRulerInfo> getTimeRulerInfos() const;

//    Q_INVOKABLE double timeDuration() const;
//    Q_INVOKABLE void setTimeDuration(double timeDuration);
//    Q_INVOKABLE void play();
//    Q_INVOKABLE void pause();
//    Q_INVOKABLE void stop();
//    Q_INVOKABLE bool isPlaying();

//    Q_INVOKABLE void forward();

//    int minBarWidth() const;
//    int maxBarWidth() const;

//    ZoomLevel zoomLevel() const;
//    void setZoomLevel(const ZoomLevel& zoomLevel);

//    double finalSize() const;
//    void setFinalSize(double finalSize);

//    double offsetPixel() const;
//    int startIndex() const;

//    double currentClipDuration() const;
//    void setcurrentClipDuration(double currentClipDuration);

//    double tempo() const;
//    double currentTempo() const;
//    void setCurrentTempo(double tempo);

//    double getCurrentTempoIndex() const;
//    void setCurrentTempoIndex(double currentTempoIndex);

//    void setTempo(double tempo);

//    int beatPerBar() const;
//    void setBeatPerBar(int beatPerBar);

//    int lenghtOfBeat() const;
//    void setLenghtOfBeat(int lenghtOfBeat);

//    int getBeatLevel() const;
//    void setBeatLevel(int beatLevel);

//    bool getZoomEndded() const;
//    void setZoomEndded(bool zoomEndded);

//    double getRulerTimeBarSize() const;
//    double getRulerTimeStartPixelOffset() const;
//    int getRulerTimeStartIndex() const;

//    void setMinBarWidth(int minBarWidth);
//    void setMaxBarWidth(int maxBarWidth);

//    int fixedGrid() const;
//    void setFixedGrid(int fixedGrid);

//    double getLastWidth() const;
//    Q_INVOKABLE void setLastWidth(double lastWidth);

//    double getCurrentWidth() const;
//    void setCurrentWidth(double currentWidth);

//    //    double getBarDuration() const;
//    //    double getBeatDuration() const;
//    //    double getSixteenthDuration() const;

//    ZoomLevel getMinZoomLevel() const;

//    int adaptiveGrid() const;
//    void setAdaptiveGrid(int adaptiveGrid);

//    bool getUpdate() const;
//    void setUpdate(bool update);

//    double getTempDuration() const;
//    void setTempDuration(double tempDuration);

//    PlayMovingMode getPlayMovingMode() const;
//    void setPlayMovingMode(const PlayMovingMode& playMovingMode);

//    qint64 getLastStartFrame() const;
//    void setLastStartFrame(const qint64& lastStartFrame);

//    qint64 getLastEndFrame() const;
//    void setLastEndFrame(const qint64& lastEndFrame);

//    double getPlaybackMarker() const;
//    void setPlaybackMarker(double playbackMarker);

//    QString getStartMarkerString();
//    double getStartMarker() const;
//    void setStartMarker(double startMarker);

//    double getEndMarker() const;
//    void setEndMarker(double endMarker);

//    double getStartBraceMarker() const;
//    void setStartBraceMarker(double startBraceMarker);

//    double getEndBraceMarker() const;
//    void setEndBraceMarker(double endBraceMarker);

//    QColor getLoopBgColor() const;
//    void setLoopBgColor(const QColor& loopBgColor);

//    bool getLoopFocus() const;
//    void setLoopFocus(bool loopFocus);

//    double getLastHostSpotTimeStart() const;
//    void setLastHostSpotTimeStart(double lastHostSpotTimeStart);

//    double getLastHotSpotTimeEnd() const;
//    void setLastHotSpotTimeEnd(double lastHotSpotTimeEnd);

//    double getLastHotSpotPressTime() const;
//    void setLastHotSpotPressTime(double lastHotSpotPressTime);

//    quint32 getSampleRate() const;
//    void setSampleRate(const quint32& sampleRate);

//    double getCurrentStartTime() const;
//    void setStartTime(double currentStartTime);

//    double getCurrentEndTime() const;
//    void setEndTime(double currentEndTime);

//    int getBackward() const;
//    void setBackward(int backward);

//    double getTimerValue() const;

//    QVector<int>* getPlotLines();

//    int getChannelCount() const;
//    void setChannelCount(int channelCount);

//    QMap<int, double*> getMin();

//    QMap<int, double*> getMax();

//    QMap<int, double*> getRms();

//    SampleClip* getSampleClip() const;
//    void setSampleClip(SampleClip* sampleClip);

//    double getMovePixel() const;
//    void setMovePixel(double movePixel);

//    double getCurrentBaseStartTime() const;
//    void setBaseStartTime(double currentBaseStartTime);

//    double getCurrentBaseEndTime() const;
//    void setBaseEndTime(double currentBaseEndTime);

//    double getSamplePerPixel() const;
//    void setSamplePerPixel(double samplePerPixel);

//    double getStartPixel() const;
//    void setStartPixel(double startPixel);

//    double getEndPixel() const;
//    void setEndPixel(double endPixel);

//    double getLastBeatBarPressX() const;
//    void setLastBeatBarPressX(double lastBeatBarPressX);

//    bool getGreenWindowZoomStop() const;
//    void setGreenWindowZoomStop(bool greenWindowZoomStop);

//    int getFirstFromLeft(QList<QPoint> list) const;

//    bool getAutomationPressAndHold() const;
//    void setAutomationPressAndHold(bool automationPressAndHold);

//    void setEnvelopePressAndHold(bool active);
//    bool envelopePressAndHold();

//    bool getAutomation() const;
//    void setAutomation(bool automation);

//    Automation* getAutomation_clip_area() const;

//    Automation* getAutomation_wave_editor() const;

//    bool getIsContainsSelectedList() const;
//    void setIsContainsSelectedList(bool isContainsSelectedList);

//private:
//    int timeFind(double time, QVector<BeatBar::Info>* info);
//    int timeFindInRulerTime(double time, QVector<BeatBar::TimeRulerInfo>* info);

//private:
//    int _zoom = 1;

//    double _currentBaseStartTime;
//    double _currentBaseEndTime;

//    double _currentStartTime;
//    double _currentEndTime;
//    double _fileDuration = 0;
//    double _currentClipDuration = 1;
//    double _tempDuration = 0;

//    int _minBarWidth = 30;
//    int _maxBarWidth = 60;

//    Measure _startMeasure;
//    Measure _durationMeasure;
//    Measure _endMeasure;

//    double _finalSize = 0;
//    double _startPixelOffset = 0;
//    int _startIndex = 0;

//    Measure _baseMeasure;

//    RulerTimeLevel _rulerTimeLevel = RulerTimeLevel_1;
//    QVector<TimeRulerInfo> _timeRulerInfos;

//    double _rulerTimeStartPixelOffset = 0;
//    double _rulerTimeBarDuration = 0;
//    int _rulerTimeStartIndex = 0;
//    double _rulerTimeStep = 0;
//    double _rulerTimeBarSize = 0;

//    QMap<int, int> _rulerTimeMap;

//    //    double _barDuration;
//    //    double _beatDuration;
//    //    double _sixteenthDuration;


//    double _lastWidth = 0;
//    double _currentWidth = 0;

//    QBasicTimer _baseTimer;
//    double _playbackMarker = 0;

//    PlayMovingMode _playMovingMode;
//    bool _isPlaying = false;
//    qint64 _lastStartFrame = 0;
//    qint64 _lastEndFrame = 0;

//    double _timerValue = 40;





//    QMutex mutex;

//    quint32 _sampleRate;

//    int _backward = false;

//    QVector<int> _plotLines;

//    QMap<int, double*> _min;
//    QMap<int, double*> _max;
//    QMap<int, double*> _rms;

//    int _channelCount = 0;

//    SampleClip* _sampleClip;

//    double _movePixel;

//    double _samplePerPixel = 0;

//private:
//    bool _update = true;

//signals:
//    void beatPerBarChanged();
//    void lenghtOfBeatChanged();
//    void tempoChanged();
//    void fixedGridChanged();
//    void adaptiveGridCahnged();
//    void updateChanged();
//    void sigBaseTimerUpdate();
//    void sigStartOffsetChanged();
//    void sigMarkerChanged();
//    void sigWarpChanged();
//    void currentTempoChanged();

//    // QObject interface
//protected:
//    void timerEvent(QTimerEvent* event);
//};

//#endif // BEATBAR_H
