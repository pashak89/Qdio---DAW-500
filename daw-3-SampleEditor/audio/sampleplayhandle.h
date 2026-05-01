#ifndef SAMPLEPLAYHANDLE_H
#define SAMPLEPLAYHANDLE_H

#include "playhandle.h"
#include "samplebuffer.h"
class SampleBuffer;
class SampleClip;
class SampleTrack;
class SamplePlayHandle : public PlayHandle {
public:
    SamplePlayHandle(SampleBuffer* sampleBuffer, bool ownAudioPort = true);

    SamplePlayHandle(SampleClip* clip);
    ~SamplePlayHandle() override;

    inline bool affinityMatters() const override
    {
        return true;
    }

    void play(sampleFrame* buffer) override;
    bool isFinished() const override;

    bool isFromTrack(const Track* _track) const override;

    f_cnt_t totalFrames() const;
    inline f_cnt_t framesDone() const
    {
        return (m_frame);
    }
    void setDoneMayReturnTrue(bool _enable)
    {
        m_doneMayReturnTrue = _enable;
    }

private:
    SampleBuffer* m_sampleBuffer;
    bool m_doneMayReturnTrue;

    f_cnt_t m_frame;
    SampleBuffer::handleState m_state;

    const bool m_ownAudioPort;

    SampleTrack* m_track;
};
#endif // SAMPLEPLAYHANDLE_H
