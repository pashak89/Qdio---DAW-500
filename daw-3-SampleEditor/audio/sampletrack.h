#ifndef SAMPLETRACK_H
#define SAMPLETRACK_H

#define FRUT_DSP_USE_FFTW 1
#include "audio_global.h"
#include "audioport.h"

#include "track.h"
#include <QObject>
#include <kmeter/kmeter.h>

constexpr panning_t PanningRight = (0 + 100);
constexpr panning_t PanningLeft = -PanningRight;
constexpr panning_t PanningCenter = 0;
constexpr panning_t DefaultPanning = PanningCenter;

class Automation;
class AreaInfo;
class SampleTrack : public Track {
    Q_OBJECT
public:
    SampleTrack(AreaInfo *areaInfo, int trackIndex);
    ~SampleTrack() override;

    bool play(const TimePos& _start, const fpp_t _frames,
        const f_cnt_t _frame_base, int _clip_num = -1) override;
    Clip* createClip(const TimePos& pos, bool isFake = false) override;
    Clip* createClip(const TimePos& pos, QString filename, bool isFake = false) override;

    inline int mixerChannelModel()
    {
        return m_mixerChannelModel;
    }

    inline AudioPort* audioPort()
    {
        return &m_audioPort;
    }

    bool isPlaying()
    {
        return m_isPlaying;
    }

    void setPlaying(bool playing)
    {
        if (m_isPlaying != playing) {
            emit playingChanged();
        }
        m_isPlaying = playing;
    }

signals:
    void playingChanged();

public slots:
    void updateClips();
    void setPlayingClips(bool isPlaying);

private:
    int m_mixerChannelModel;
    AudioPort m_audioPort;

    bool m_isPlaying;

    QSharedPointer<TruePeakAudioProcessor> m_truePeakMeter;

    // SerializingObject interface
public:
    QString nodeName() const
    {
        return "sampletrack";
    }

    // SerializingObject interface
public:
    QJsonDocument saveState(QJsonDocument _doc) override;

    QSharedPointer<TruePeakAudioProcessor> truePeakMeter() const;
private slots:
    void audioEngineChanged();
};

#endif // SAMPLETRACK_H
