#include "sampleplayhandle.h"
#include "audioengine.h"
#include "audioport.h"
#include "clip.h"
#include "engine.h"
#include "samplebuffer.h"
#include "sampleclip.h"
#include "sampletrack.h"
#include "sharedobject.h"
#include <memory>
SamplePlayHandle::SamplePlayHandle(SampleBuffer* sampleBuffer, bool ownAudioPort)
    : PlayHandle(TypeSamplePlayHandle)
    , m_sampleBuffer(sampleBuffer)
    , m_doneMayReturnTrue(true)
    , m_frame(0)
    , m_ownAudioPort(ownAudioPort)
    , m_track(nullptr)
{
    if (ownAudioPort) {
        setAudioPort(new AudioPort("SamplePlayHandle", Q_NULLPTR));
    }
}

SamplePlayHandle::SamplePlayHandle(SampleClip* clip)
    : SamplePlayHandle(clip->sampleBuffer(), false)
{

    m_track = (SampleTrack*)clip->getTrack();
    setAudioPort(((SampleTrack*)clip->getTrack())->audioPort());
}

SamplePlayHandle::~SamplePlayHandle()
{

    // delete m_sampleBuffer;

    if (m_ownAudioPort) {
        delete audioPort();
    }
}

void SamplePlayHandle::play(sampleFrame* buffer)
{

    const fpp_t fpp = AudioManager::audioEngine()->framesPerPeriod();

    // play( 0, _try_parallelizing );
    if (framesDone() >= totalFrames()) {
        memset(buffer, 0, sizeof(sampleFrame) * fpp);
        return;
    }

    sampleFrame* workingBuffer = buffer;
    f_cnt_t frames = fpp;

    // apply offset for the first period
    if (framesDone() == 0) {
        memset(buffer, 0, sizeof(sampleFrame) * offset());
        workingBuffer += offset();
        frames -= offset();
    }

    if ((m_track)) {

        if (!m_sampleBuffer->play(workingBuffer, &m_state, frames, DefaultBaseFreq)) {
            qDebug() << "CLEAR";
            workingBuffer->clear();
        }
    }

    m_frame += frames;
}

bool SamplePlayHandle::isFinished() const
{

    return framesDone() >= totalFrames() && m_doneMayReturnTrue == true;
}

bool SamplePlayHandle::isFromTrack(const Track* _track) const
{
    return m_track == _track;
}

f_cnt_t SamplePlayHandle::totalFrames() const
{
    return (m_sampleBuffer->endFrame() - m_sampleBuffer->startFrame()); // * (AudioManager::audioEngine()->processingSampleRate() / m_sampleBuffer->sampleRate());
}
