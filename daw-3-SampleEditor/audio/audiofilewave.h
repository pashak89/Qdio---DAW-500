#ifndef AUDIOFILEWAVE_H
#define AUDIOFILEWAVE_H

#include "audio_global.h"
#include "audiofiledevice.h"
#include <SNDFILE.h>

class AudioFileWave : public AudioFileDevice {
public:
    AudioFileWave(OutputSettings const& outputSettings,
        const ch_cnt_t channels,
        bool& successful,
        const QString& file,
        AudioEngine* audioEngine);
    ~AudioFileWave() override;

    static AudioFileDevice* getInst(const QString& outputFilename,
        OutputSettings const& outputSettings,
        const ch_cnt_t channels,
        AudioEngine* audioEngine,
        bool& successful)
    {
        return new AudioFileWave(outputSettings, channels, successful,
            outputFilename, audioEngine);
    }

private:
    void writeBuffer(const sampleFrame* _ab,
        const fpp_t _frames,
        float _master_gain) override;

    bool startEncoding();
    void finishEncoding();

private:
    SF_INFO m_si;
    SNDFILE* m_sf;
    void float32_to_int16(const float* input, int16_t* output, size_t samples);
    void float32_to_int24(const float* input, int32_t* output, size_t samples);
    void store_as_packed_24bit(const int32_t *input, uint8_t *output, size_t samples);
};

#endif // AUDIOFILEWAVE_H
