#ifndef SAMPLECLIP_H
#define SAMPLECLIP_H

#include "clip.h"

#include <core/warp.h>

#include <QTimer>

class SampleBuffer;
class SampleClip : public Clip {
    Q_OBJECT

public:
    SampleClip(Track* _track, bool isFake = false);
    SampleClip(Track* _track, QString fileName, bool isFake = false);

    void initialize();

    ~SampleClip() override;

    SampleClip(SampleClip& item, Track* track, bool fake = false);
    SampleClip* clone(Track* track, bool isFake = false)
    {
        return new SampleClip(*this, track, isFake);
    }

    SampleClip& operator=(const SampleClip& that) = delete;

    void changeLength(const TimePos& _length) override;
    const QString& sampleFile() const;

    SampleBuffer* sampleBuffer()
    {
        return m_sampleBuffer;
    }

    TimePos sampleLength() const;
    void setSampleStartFrame(f_cnt_t startFrame);
    void setSamplePlayLength(f_cnt_t length);

    bool isPlaying() const;
    void setIsPlaying(bool isPlaying);

    double baseStartTime() const;
    void setBaseStartTime(double baseStartTime);

    double baseEndTime() const;
    void setBaseEndTime(double baseEndTime);

    double startTime() const;
    void setStartTime(double startTime);

    double endTime() const;
    void setEndTime(double endTime);

    double currentFileDuration() const;
    void setCurrentFileDuration(double currentFileDuration);

    //    double offsetStartTime() const;
    //    void setOffsetStartTime(double offsetStartTime);

    //    double offsetEndTime() const;
    //    void setOffsetEndTime(double offsetEndTime);

public slots:
    void setSampleFile(const QString& _sf);
    void playbackPositionChanged();
    void updateTrackClips(QString clipIndex);

private:
    SampleBuffer* m_sampleBuffer = Q_NULLPTR;
    bool m_recordModel;
    bool m_isPlaying;

    double _baseStartTime;
    double _baseEndTime;

    double _startTime = 0;
    double _endTime = 0;
    double _offsetStartTime = 0;
    double _offsetEndTime = 0;

    double _currentClipDuration = -1;
    double _currentFileDuration = -1;

    QList<Warp*> _warpMarkers;

signals:

    void wasReversed();
    void sigPlayingChanged(bool state, QString index);

    // Clip interface
public:
    void setClipIndex(const QString& clipIndex);
    void sampleChange();
    // SerializingObject interface
public:
    inline QString nodeName() const override
    {
        return "sampleclip";
    }
    QJsonObject state();
    QList<Warp*> warpMarkers() const;
    void addWarpMarker(int index, double p0, double p1, double t0, double base0, double t1, double base1, double tempo1);
    void removeWarpMarker(int index);
    int findWarpByTime(double time);
};

#endif // SAMPLECLIP_H
