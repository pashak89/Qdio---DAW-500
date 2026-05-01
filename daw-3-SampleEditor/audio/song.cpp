#include "song.h"
#include "audioengine.h"
#include "engine.h"
#include "sampletrack.h"
#include <QDateTime>
#include <QDebug>
#include <core/global.h>
tick_t TimePos::s_ticksPerBar = DefaultTicksPerBar;

Song::Song(QObject* parent)
    : TrackContainer(parent)
    , m_tempo(DefaultTempo)
    , m_timeSigModel(this)
    , m_playing(false)
    , m_paused(true)
    , m_playMode(Mode_None)
    , m_recording(false)
    , m_exporting(false)
    , m_exportLoop(false)
    , m_renderBetweenMarkers(false)
    , m_elapsedTicks(0)
    , m_elapsedBars(0)
    , m_modified(false)
    , m_loopRenderCount(1)
    , m_loopRenderRemaining(1)

{
    m_playMode = Mode_PlaySong;
    setToTime(TimePos(0));
    m_playPos[m_playMode].setTicks(0);
}

Song::~Song()
{
    qDebug() << "~Song()";
    stop();
    m_playing = false;
}

int Song::getTempo() const
{
    return m_tempo;
}

void Song::setTempo(int tempo)
{
    m_tempo = tempo;
    Q_EMIT tempoChanged();
}

void Song::processNextBuffer()
{

    // If nothing is playing, there is nothing to do
    if (!m_playing) {
        return;
    }
    if (m_requestToPause) {

        m_counterToPause--;
        if (m_counterToPause <= 0) {

            m_counterToPause = 1;
            m_paused = true;

            m_playPos[m_playMode].setCurrentFrame(0);
            AudioManager::audioEngine()->clear();

            m_playing = false;
            m_counterToPause = false;
            return;
        }
    }

    // At the beginning of the song, we have to reset the LFOs
    if (m_playMode == Mode_PlaySong && getPlayPos() == 0) {
        // EnvelopeAndLfoParameters::instances()->reset();
    }

    TrackList trackList;
    int clipNum = -1; // The number of the clip that will be played

    // Determine the list of tracks to play and the clip number
    switch (m_playMode) {
    case Mode_PlaySong:
        trackList = tracks();

        break;

    default:
        return;
    }

    // If we have no tracks to play, there is nothing to do
    if (trackList.empty()) {
        return;
    }

    // If the playback position is outside of the range [begin, end), move it to
    // begin and inform interested parties.
    // Returns true if the playback position was moved, else false.
    const auto enforceLoop = [this](const TimePos& begin, const TimePos& end) {
        if (getPlayPos() < begin || getPlayPos() >= end) {
            setToTime(begin);
            // m_vstSyncController.setPlaybackJumped(true);
            emit updateSampleTracks();

            return true;
        }
        return false;
    };

    // const auto timeline = getPlayPos().m_timeLine;
    // const auto loopEnabled = !m_exporting && timeline && timeline->loopPointsEnabled();

    // Ensure playback begins within the loop if it is enabled
    // if (loopEnabled) {
    //    enforceLoop(timeline->loopBegin(), timeline->loopEnd());
    //}

    // Inform VST plugins if the user moved the play head
    if (getPlayPos().jumped()) {
        //  m_vstSyncController.setPlaybackJumped(true);
        getPlayPos().setJumped(false);
    }

    const auto framesPerTick = 256;
    const auto framesPerPeriod = AudioManager::audioEngine()->framesPerPeriod();

    f_cnt_t frameOffsetInPeriod = 0;

    while (frameOffsetInPeriod < framesPerPeriod) {
        auto frameOffsetInTick = getPlayPos().currentFrame();

        // If a whole tick has elapsed, update the frame and tick count, and check any loops
        if (frameOffsetInTick >= framesPerTick) {

            const auto elapsedTicks = static_cast<int>(frameOffsetInTick / (double)framesPerTick);

            getPlayPos().setTicks(getPlayPos().getTicks() + elapsedTicks);

            frameOffsetInTick -= elapsedTicks * framesPerTick;
            getPlayPos().setCurrentFrame(frameOffsetInTick);
        }

        const f_cnt_t framesUntilNextPeriod = framesPerPeriod - frameOffsetInPeriod;
        const auto framesUntilNextTick = static_cast<f_cnt_t>(std::ceil(framesPerTick - frameOffsetInTick));

        // We want to proceed to the next buffer or tick, whichever is closer
        const auto framesToPlay = std::min(framesUntilNextPeriod, framesUntilNextTick);

        if (frameOffsetInPeriod == 0) {
            // First frame of buffer: update VST sync position.
            // This must be done after we've corrected the frame/tick count,
            // but before actually playing any frames.
            // m_vstSyncController.setAbsolutePosition(getPlayPos().getTicks()
            //     + getPlayPos().currentFrame() / static_cast<double>(framesPerTick));
            // m_vstSyncController.update();
        }

        if (static_cast<f_cnt_t>(frameOffsetInTick) == 0) {
            // First frame of tick: process automation and play tracks
            processAutomations(trackList, getPlayPos(), framesToPlay);
            for (const auto track : trackList) {
                auto p = getPlayPos();

                // qDebug() << "P1" << p;
                track->play(p, framesToPlay, frameOffsetInPeriod, clipNum);
            }
        }

        frameOffsetInPeriod += framesToPlay;
        frameOffsetInTick += framesToPlay;

        getPlayPos().setCurrentFrame(frameOffsetInTick);

        m_elapsedMilliSeconds[m_playMode] += TimePos::ticksToMilliseconds(framesToPlay / framesPerTick, getTempo());
        m_elapsedBars = m_playPos[Mode_PlaySong].getBar();
        m_elapsedTicks = ((int)m_playPos[Mode_PlaySong].getTicks() % (int)ticksPerBar()) / 48;
    }
}

void Song::processAutomations(const TrackList& tracks, TimePos timeStart, fpp_t frames)
{
    return;
}

void Song::processLoadNewTracks()
{

    m_track_list_mutex.lock();
    for (int i = 0; i < m_async_track_list.size(); i++) {
        AudioManager::audioEngine()->requestChangeInModel();
        addTrack(m_async_track_list[i]);
        AudioManager::audioEngine()->doneChangeInModel();
        AudioManager::audioEngine()->addAudioPort(((SampleTrack*)m_async_track_list[i])->audioPort());
    }
    m_async_track_list.clear();
    m_track_list_mutex.unlock();

    updateAfterTrackAdd();
}

void Song::updateLength()
{

    m_length = 0;
    m_tracksMutex.lockForRead();

    for (auto track : tracks()) {
        if (m_exporting && track->isMuted()) {
            continue;
        }

        const tick_t cur = track->length();
        if (cur > m_length) {
            m_length = cur;
        }
    }

    m_tracksMutex.unlock();

    emit lengthChanged(m_length);
}

void Song::setModified(bool value)
{
    if (!m_loadingProject && m_modified != value) {
        m_modified = value;
        emit modified();
    }
}

Track* Song::addSampleTrack(AreaInfo* areaInfo, int trackIndex)
{
    auto t = Track::create(Track::TrackTypes_SampleTrack, areaInfo, trackIndex);
    m_track_list_mutex.lock();
    m_async_track_list.append(t);
    m_track_list_mutex.unlock();
    return t;
}

void Song::playSong()
{

    m_recording = false;

    m_playMode = Mode_PlaySong;

    m_requestToPause = false;
    m_playing = true;
    m_paused = false;

    emit playbackStateChanged();
}

void Song::setPause()
{

    m_playPos[m_playMode].setLastTicks(m_playPos[m_playMode].getTicks());
    m_playMode = Mode_PlaySong;

    m_counterToPause = 1;
    m_requestToPause = true;
    m_paused = true;

    emit playbackStateChanged();
}

void Song::setPlay()
{
    m_playPos[m_playMode].setLastTicks(0);
    m_requestToPause = false;
    m_recording = false;
    m_playMode = Mode_PlaySong;
    m_playing = true;
    m_paused = false;
    m_playPos[m_playMode].setCurrentFrame(0);
    emit playbackStateChanged();
}

void Song::readyToPlay()
{
    m_playPos[m_playMode].setCurrentFrame(0);
    Q_EMIT sigReadyToPlay();
}

void Song::startExport()
{

    stop();

    m_exporting = true;
    updateLength();

    if (m_renderBetweenMarkers) {
        // m_exportSongBegin = m_exportLoopBegin = m_playPos[Mode_PlaySong].m_timeLine->loopBegin();
        // m_exportSongEnd = m_exportLoopEnd = m_playPos[Mode_PlaySong].m_timeLine->loopEnd();

        // m_playPos[Mode_PlaySong].setTicks(m_playPos[Mode_PlaySong].m_timeLine->loopBegin().getTicks());

        m_playPos[Mode_PlaySong].setTicks(m_exportSongBegin.getTicks());

    } else {
        m_exportSongEnd = m_length;

        // Handle potentially ridiculous loop points gracefully.
        //        if (m_loopRenderCount > 1 && m_playPos[Mode_PlaySong].m_timeLine->loopEnd() > m_exportSongEnd) {
        //            m_exportSongEnd = m_playPos[Mode_PlaySong].m_timeLine->loopEnd();
        //        }

        if (!m_exportLoop)
            m_exportSongEnd += TimePos(1, 0);

        m_exportSongBegin = 0;
        // FIXME: remove this check once we load timeline in headless mode
        //        if (m_playPos[Mode_PlaySong].m_timeLine) {
        //            m_exportLoopBegin = m_playPos[Mode_PlaySong].m_timeLine->loopBegin() < m_exportSongEnd && m_playPos[Mode_PlaySong].m_timeLine->loopEnd() <= m_exportSongEnd ? m_playPos[Mode_PlaySong].m_timeLine->loopBegin() : TimePos(0, 0);
        //            m_exportLoopEnd = m_playPos[Mode_PlaySong].m_timeLine->loopBegin() < m_exportSongEnd && m_playPos[Mode_PlaySong].m_timeLine->loopEnd() <= m_exportSongEnd ? m_playPos[Mode_PlaySong].m_timeLine->loopEnd() : TimePos(0, 0);
        //        }

        m_playPos[Mode_PlaySong].setTicks(0);
    }

    m_exportEffectiveLength = (m_exportLoopBegin - m_exportSongBegin) + (m_exportLoopEnd - m_exportLoopBegin) * m_loopRenderCount + (m_exportSongEnd - m_exportLoopEnd);
    m_loopRenderRemaining = m_loopRenderCount;

    playSong();
}

void Song::stopExport()
{
    stop();
    m_exporting = false;
}

bool Song::isExportDone() const
{
    return !isExporting() || m_playPos[m_playMode] >= m_exportSongEnd;
}

int Song::getExportProgress() const
{
    TimePos pos = m_playPos[m_playMode];

    if (pos >= m_exportSongEnd) {
        return 100;
    } else if (pos <= m_exportSongBegin) {
        return 0;
    } else if (pos >= m_exportLoopEnd) {
        pos = (m_exportLoopBegin - m_exportSongBegin) + (m_exportLoopEnd - m_exportLoopBegin) * m_loopRenderCount + (pos - m_exportLoopEnd);
    } else if (pos >= m_exportLoopBegin) {
        pos = (m_exportLoopBegin - m_exportSongBegin) + ((m_exportLoopEnd - m_exportLoopBegin) * (m_loopRenderCount - m_loopRenderRemaining)) + (pos - m_exportLoopBegin);
    } else {
        pos = (pos - m_exportSongBegin);
    }

    return (float)pos / (float)m_exportEffectiveLength * 100.0f;
}

volatile bool Song::renderBetweenMarkers() const
{
    return m_renderBetweenMarkers;
}

void Song::setRenderBetweenMarkers(volatile bool newRenderBetweenMarkers)
{
    m_renderBetweenMarkers = newRenderBetweenMarkers;
}

void Song::setExportSongBegin(const TimePos& newExportSongBegin)
{
    m_exportSongBegin = newExportSongBegin;
}

void Song::setExportSongEnd(const TimePos& newExportSongEnd)
{
    m_exportSongEnd = newExportSongEnd;
}

void Song::stop()
{

    // do not stop/reset things again if we're stopped already
    if (m_playMode == Mode_None) {
        return;
    }

    // To avoid race conditions with the processing threads
    AudioManager::audioEngine()->requestChangeInModel();

    m_paused = true;
    m_recording = true;

    m_playPos[m_playMode].setTicks(0);
    m_elapsedMilliSeconds[m_playMode] = 0;

    m_playing = false;

    m_elapsedMilliSeconds[Mode_None] = m_elapsedMilliSeconds[m_playMode];
    m_playPos[Mode_None].setTicks(m_playPos[m_playMode].getTicks());

    m_playPos[m_playMode].setCurrentFrame(0);

    // remove all note-play-handles that are active
    AudioManager::audioEngine()->clear();

    m_playMode = Mode_None;

    AudioManager::audioEngine()->doneChangeInModel();

    emit stopped();
    emit playbackStateChanged();
}

void Song::setModified()
{
    setModified(true);
}
