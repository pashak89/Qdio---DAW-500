#ifndef SONG_H
#define SONG_H

#include "audio_global.h"
#include "metermodel.h"
#include "timepos.h"
#include "trackcontainer.h"
#include <QDateTime>
#include <QObject>
#include <QReadWriteLock>
class AreaInfo;
class Song : public TrackContainer {
    Q_OBJECT
    Q_PROPERTY(int tempoModel READ getTempo WRITE setTempo NOTIFY tempoChanged)
public:
    enum PlayModes {
        Mode_None,
        Mode_PlaySong,
        Mode_PlayPattern,
        Mode_PlayMidiClip,
        Mode_PlayAutomationClip,
        Mode_Count
    };

    class PlayPos : public TimePos {
    public:
        PlayPos(const int abs = 0)
            : TimePos(abs)
            , m_currentFrame(0.0f)
        {
        }
        inline void setCurrentFrame(const float f)
        {
            m_currentFrame = f;
        }
        inline float currentFrame() const
        {
            return m_currentFrame;
        }
        inline void setJumped(const bool jumped)
        {
            m_jumped = jumped;
        }
        inline bool jumped() const
        {
            return m_jumped;
        }

    private:
        float m_currentFrame;
        bool m_jumped;
    };

    explicit Song(QObject* parent = Q_NULLPTR);
    ~Song() override;

    int getTempo() const;
    void setTempo(int newTempoModel);

    void processNextBuffer();

    inline PlayPos& getPlayPos(PlayModes pm)
    {
        return m_playPos[pm];
    }
    inline const PlayPos& getPlayPos(PlayModes pm) const
    {
        return m_playPos[pm];
    }
    inline PlayPos& getPlayPos()
    {
        return getPlayPos(m_playMode);
    }
    inline const PlayPos& getPlayPos() const
    {
        return getPlayPos(m_playMode);
    }

    inline void setToTime(TimePos const& pos)
    {
        m_elapsedMilliSeconds[m_playMode] = pos.getTimeInMilliseconds(getTempo());
        m_playPos[m_playMode].setTicks(pos.getTicks());
        m_playPos[m_playMode].setFirstTicks(pos.getTicks());
    }

    MeterModel& getTimeSigModel()
    {
        return m_timeSigModel;
    }

    inline bool isExporting() const
    {
        return m_exporting;
    }

    inline double ticksPerBar() const
    {
        return TimePos::ticksPerBar();
    }

    inline bool isPause() const
    {
        return m_paused;
    }

    inline bool isPlaying() const
    {
        return m_playing;
    }

    inline void setExportLoop(bool exportLoop)
    {
        m_exportLoop = exportLoop;
    }

    void processAutomations(const TrackList& tracks, TimePos timeStart, fpp_t frames);

    void processLoadNewTracks();

    void updateLength();

    void setModified();
    void setModified(bool value);

    Track* addSampleTrack(AreaInfo* areaInfo, int trackIndex);

    void playSong();

    void stop();
    void setPause();
    void setPlay();
    void readyToPlay();

    void startExport();
    void stopExport();
    bool isExportDone() const;
    int getExportProgress() const;

    volatile bool renderBetweenMarkers() const;
    void setRenderBetweenMarkers(volatile bool newRenderBetweenMarkers);

    void setExportSongBegin(const TimePos& newExportSongBegin);

    void setExportSongEnd(const TimePos& newExportSongEnd);

signals:
    void tempoChanged();
    void updateSampleTracks();
    void modified();
    void lengthChanged(int bars);
    void stopped();
    void playbackStateChanged();
    void positionMarkerMoved();
    void sigReadyToPlay();

private:
    int m_tempo = 120;
    bool m_loadingProject;
    bool m_modified;
    volatile bool m_recording;
    volatile bool m_exporting;
    volatile bool m_exportLoop;
    volatile bool m_renderBetweenMarkers;
    volatile bool m_playing = false;
    volatile bool m_paused = true;
    volatile bool m_requestToPause = true;
    int m_counterToPause = true;

    PlayPos m_playPos[Mode_Count];
    PlayModes m_playMode;

    int m_loopRenderCount;
    int m_loopRenderRemaining;
    TimePos m_exportSongBegin;
    TimePos m_exportLoopBegin;
    TimePos m_exportLoopEnd;
    TimePos m_exportSongEnd;
    TimePos m_exportEffectiveLength;

    double m_elapsedMilliSeconds[Mode_Count];
    tick_t m_elapsedTicks;
    bar_t m_elapsedBars;

    TimePos m_length;
    mutable QReadWriteLock m_tracksMutex;

    QVector<Track*> m_async_track_list;
    QMutex m_track_list_mutex;

    MeterModel m_timeSigModel;
};

#endif // SONG_H
