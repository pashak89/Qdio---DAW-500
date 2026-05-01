#include "audiofilewave.h"

AudioFileWave::AudioFileWave(OutputSettings const& outputSettings,
    const ch_cnt_t channels, bool& successful,
    const QString& file,
    AudioEngine* audioEngine)
    : AudioFileDevice(outputSettings, channels, file, audioEngine)
    , m_sf(nullptr)
{

    if (outputFileOpened()) {
        successful = startEncoding();
    } else {
        successful = false;
    }
}

AudioFileWave::~AudioFileWave()
{
    qDebug() << "AudioFileWave::~AudioFileWave()";
    finishEncoding();
}

bool AudioFileWave::startEncoding()
{
    qDebug() << "startEncoding()";
    m_si.samplerate = sampleRate();
    m_si.channels = channels();

    m_si.frames = (audioEngine()->framesPerPeriod() * m_si.samplerate / (double)audioEngine()->outputSampleRate() + 1);

    m_si.sections = 1;
    m_si.seekable = 0;

    m_si.format = SF_FORMAT_WAV;

    switch (getOutputSettings().getBitDepth()) {
    case OutputSettings::Depth_32Bit:
        m_si.format |= SF_FORMAT_FLOAT;
        break;
    case OutputSettings::Depth_24Bit:
        m_si.format |= SF_FORMAT_PCM_24;

        break;
    case OutputSettings::Depth_16Bit:
    default:
        m_si.format |= SF_FORMAT_PCM_16;
        break;
    }

    m_sf = sf_open_fd(outputFileHandle(), SFM_WRITE, &m_si, false);

    if (!m_sf) {
        qWarning("Error: AudioFileWave::startEncoding: %s", sf_strerror(nullptr));
        return false;
    }

    // Prevent fold overs when encountering clipped data
    sf_command(m_sf, SFC_SET_CLIPPING, nullptr, SF_TRUE);

    sf_set_string(m_sf, SF_STR_SOFTWARE, "DAW");

    qDebug() << "startEncoding()2";
    return true;
}
void AudioFileWave::float32_to_int16(const float* input, int16_t* output, size_t samples)
{
    for (size_t i = 0; i < samples; ++i) {
        float sample = input[i];
        if (sample > 1.0f)
            sample = 1.0f;
        else if (sample < -1.0f)
            sample = -1.0f;
        output[i] = static_cast<int16_t>(sample * 32767.0f + (sample >= 0 ? 0.5f : -0.5f));
    }
}

void AudioFileWave::float32_to_int24(const float* input, int32_t* output, size_t samples)
{
    constexpr float scale = 8388608.0f; // 2^23
    constexpr float max_pos = 8388607.0f; // 2^23 - 1
    constexpr float max_neg = -8388608.0f; // -2^23

    for (size_t i = 0; i < samples; ++i) {
        float sample = input[i];

        // Clamp to [-1.0, 1.0] range
        sample = std::max(-1.0f, std::min(1.0f, sample));

        // Scale and round
        float scaled = sample * scale;

        // Proper rounding with overflow protection
        int32_t value;
        if (scaled >= 0) {
            value = static_cast<int32_t>(scaled + 0.5f);
            if (value > max_pos)
                value = max_pos;
        } else {
            value = static_cast<int32_t>(scaled - 0.5f);
            if (value < max_neg)
                value = max_neg;
        }

        output[i] = value;
    }
}
void AudioFileWave::store_as_packed_24bit(const int32_t* input, uint8_t* output, size_t samples)
{
    for (size_t i = 0; i < samples; ++i) {
        output[i * 3] = static_cast<uint8_t>(input[i] & 0xFF);
        output[i * 3 + 1] = static_cast<uint8_t>((input[i] >> 8) & 0xFF);
        output[i * 3 + 2] = static_cast<uint8_t>((input[i] >> 16) & 0xFF);
    }
}

void AudioFileWave::writeBuffer(const sampleFrame* _ab,
    const fpp_t _frames,
    const float _master_gain)
{
    OutputSettings::BitDepth bitDepth = getOutputSettings().getBitDepth();

    if (bitDepth == OutputSettings::Depth_32Bit) {
        auto buf = new float[_frames * channels()];
        for (fpp_t frame = 0; frame < _frames; ++frame) {
            for (ch_cnt_t chnl = 0; chnl < channels(); ++chnl) {
                buf[frame * channels() + chnl] = _ab->getSample(chnl, frame) * _master_gain;
            }
        }
        sf_writef_float(m_sf, buf, _frames);
        delete[] buf;

    } else if (bitDepth == OutputSettings::Depth_24Bit) {

        auto buf = new float[_frames * channels()];
        for (fpp_t frame = 0; frame < _frames; ++frame) {
            for (ch_cnt_t chnl = 0; chnl < channels(); ++chnl) {
                buf[frame * channels() + chnl] = _ab->getSample(chnl, frame) * _master_gain;
            }
        }

        int32_t* int24_dst = new int32_t[_frames * channels()];

        soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_INT32_I);
        soxr_quality_spec_t quality_spec = soxr_quality_spec(SOXR_HQ, 0);

        size_t ilen = _frames;
        size_t olen;
        soxr_oneshot(
            sampleRate(), sampleRate(), 2,
            buf, _frames, nullptr, // Input
            int24_dst, ilen, &olen, // Output
            &io_spec, &quality_spec, nullptr);

        sf_writef_int(m_sf, int24_dst, _frames);
        delete[] buf;
        delete[] int24_dst;

    } else { // 16bit
        auto buf = new float[_frames * channels()];
        for (fpp_t frame = 0; frame < _frames; ++frame) {
            for (ch_cnt_t chnl = 0; chnl < channels(); ++chnl) {
                buf[frame * channels() + chnl] = _ab->getSample(chnl, frame) * _master_gain;
            }
        }
        int16_t* int16_dst = new int16_t[_frames * channels()];

        soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_INT16_I);
        soxr_quality_spec_t quality_spec = soxr_quality_spec(SOXR_HQ, 0);

        size_t ilen = _frames;
        size_t olen;
        soxr_oneshot(
            sampleRate(), sampleRate(), 2,
            buf, _frames, nullptr, // Input
            int16_dst, ilen, &olen, // Output
            &io_spec, &quality_spec, nullptr);

        sf_writef_short(m_sf, int16_dst, _frames);
        delete[] buf;
        delete[] int16_dst;
    }
}
void AudioFileWave::finishEncoding()
{
    if (m_sf) {

        auto result = sf_close(m_sf);
        qDebug() << "finishEncoding" << result;
    }
}
