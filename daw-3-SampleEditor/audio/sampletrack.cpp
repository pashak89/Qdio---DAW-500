#include "sampletrack.h"
#include "audioengine.h"
#include "clip.h"
#include "engine.h"

#include "sampleclip.h"
#include "sampleplayhandle.h"

#include <core/automation.h>
SampleTrack::SampleTrack(AreaInfo* areaInfo, int trackIndex)
    : Track(trackIndex, Track::TrackTypes_SampleTrack, areaInfo)
    , m_audioPort(tr("Sample track"), this)
    , m_mixerChannelModel(0)
    , m_isPlaying(false)

{

    if (AudioManager::audioEngine() == Q_NULLPTR) {
        return;
    }

    int oversamplingFactor = 8;

    if (AudioManager::audioEngine()->outputSampleRate() >= 176400) {
        oversamplingFactor /= 4;
    } else if (AudioManager::audioEngine()->outputSampleRate() >= 88200) {
        oversamplingFactor /= 2;
    }

    m_truePeakMeter = QSharedPointer<TruePeakAudioProcessor>(new TruePeakAudioProcessor());
    qint64 result = (AudioManager::audioEngine()->outputSampleRate() * 20 / 1000);
    m_truePeakMeter->prepareToPlay(AudioManager::audioEngine()->outputSampleRate(),
        AudioManager::audioEngine()->channels(),
        AudioManager::audioEngine()->framesPerPeriod(),
        10);

    m_truePeakMeter->setAverageAlgorithm(0);

    connect(AudioManager::audioEngine(), &AudioEngine::aduioDeviceChanged, this, &SampleTrack::audioEngineChanged);

    setName(tr("Sample track"));

    _volumeAutomation = QSharedPointer<Automation>(new Automation(_areaInfo, trackIndex, "Mixer", "Volume", 0, 1, MathHelper1::decibel2yPixel(0)));
    _panAutomation = QSharedPointer<Automation>(new Automation(_areaInfo, trackIndex, "Mixer", "Pan", 0, 1, 0.5));
    _speakerAutomation = QSharedPointer<Automation>(new Automation(_areaInfo, trackIndex, "Mixer", "SpeakerOn", 0.25, 0.75));
}

void SampleTrack::audioEngineChanged()
{
    if (m_truePeakMeter) {
        m_truePeakMeter.reset();
    }
    m_truePeakMeter = QSharedPointer<TruePeakAudioProcessor>(new TruePeakAudioProcessor());
    qint64 result = (AudioManager::audioEngine()->outputSampleRate() * 20 / 1000);
    m_truePeakMeter->prepareToPlay(AudioManager::audioEngine()->outputSampleRate(),
        AudioManager::audioEngine()->channels(),
        AudioManager::audioEngine()->framesPerPeriod(),
        10);

    m_truePeakMeter->setAverageAlgorithm(0);
}

SampleTrack::~SampleTrack()
{

    _speakerAutomation.clear();
    _panAutomation.clear();
    _volumeAutomation.clear();

    // CHECK THIS delete _automation;
    if (AudioManager::audioEngine())
        AudioManager::audioEngine()->removePlayHandlesOfTypes(this, PlayHandle::TypeSamplePlayHandle);
}

bool SampleTrack::play(const TimePos& _start, const fpp_t _frames,
    const f_cnt_t _offset, int _clip_num)
{
    // m_audioPort.effects()->startRunning();
    bool played_a_note = false; // will be return variable

    clipVector clips;
    class PatternTrack* pattern_track = nullptr;
    if (_clip_num >= 0) {
        if (_start > getClip(_clip_num)->length()) {
            setPlaying(false);
        }
        if (_start != 0) {
            return false;
        }
        clips.push_back(getClip(_clip_num));

    } else {
        bool nowPlaying = false;
        auto n = numOfClips();

        for (int i = 0; i < n; ++i) {
            Clip* clip = getClip(i);

            auto sClip = dynamic_cast<SampleClip*>(clip);

            if (sClip == Q_NULLPTR)
                continue;

            if (sClip->isFake())
                continue;

            // qDebug() << "_start_start" << qSetRealNumberPrecision(100) << _start << sClip->startPosition() << sClip->endPosition();
            if (_start >= sClip->startPosition() && _start < sClip->endPosition()) {

                if (sClip->isPlaying() == false && _start >= (sClip->startPosition())) {

                    double bufferFramesPerTick = 256.0; // AudioManager::framesPerTick(sClip->sampleBuffer()->sampleRate());

                    double start_time = (_start - sClip->startPosition() + sClip->startTimeOffset());
                    double tmp_start = start_time * bufferFramesPerTick;
                    f_cnt_t sampleStart = qRound(tmp_start);

                    auto p1 = sClip->endPosition();
                    auto p2 = sClip->startPosition();

                    double tmp_end = (start_time + ((sClip->endPosition() - sClip->startPosition()) - (_start - sClip->startPosition()))) * bufferFramesPerTick;
                    f_cnt_t clipFrameLength = qRound(tmp_end);

                    // qDebug() << "_start_start1" << qSetRealNumberPrecision(100) << start_time << sClip->endPosition();
                    if (start_time < sClip->endPosition()) {

                        sClip->setSampleStartFrame(sampleStart);

                        sClip->setSamplePlayLength(clipFrameLength);
                        clips.push_back(sClip);
                        sClip->setIsPlaying(true);
                        nowPlaying = true;
                    }
                }
            } else {

                sClip->setIsPlaying(false);
            }
            nowPlaying = nowPlaying || sClip->isPlaying();
        }
        setPlaying(nowPlaying);
    }

    for (const auto& clip : clips) {

        auto st = dynamic_cast<SampleClip*>(clip);
        if (!st->isMuted()) {
            PlayHandle* handle;
            auto smpHandle = new SamplePlayHandle(st);
            handle = smpHandle;
            handle->setOffset(_offset);
            // send it to the audio engine

            AudioManager::audioEngine()->addPlayHandle(handle);
            played_a_note = true;
        }
    }

    return played_a_note;
}

Clip* SampleTrack::createClip(const TimePos& pos, bool isFake)
{
    auto sClip = new SampleClip(this, isFake);
    sClip->movePosition(pos);
    return sClip;
}

Clip* SampleTrack::createClip(const TimePos& pos, QString filename, bool isFake)
{
    auto sClip = new SampleClip(this, filename, isFake);
    sClip->movePosition(pos);
    return sClip;
}

void SampleTrack::updateClips()
{

    AudioManager::audioEngine()->removePlayHandlesOfTypes(this, PlayHandle::TypeSamplePlayHandle);
    setPlayingClips(false);
}

void SampleTrack::setPlayingClips(bool isPlaying)
{

    for (int i = 0; i < numOfClips(); ++i) {
        Clip* clip = getClip(i);
        if (clip) {
            auto sClip = dynamic_cast<SampleClip*>(clip);
            sClip->setIsPlaying(isPlaying);
        }
    }
}

QSharedPointer<TruePeakAudioProcessor> SampleTrack::truePeakMeter() const
{
    return m_truePeakMeter;
}

QJsonDocument SampleTrack::saveState(QJsonDocument _doc)
{

    JournallingObject::saveState(_doc);

    return _doc;
}
