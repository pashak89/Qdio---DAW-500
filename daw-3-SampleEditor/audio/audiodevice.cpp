#include "audiodevice.h"
#include "audioengine.h"
#include "sox.h"
#include "soxr.h"
#include <QDebug>

AudioDevice::AudioDevice(AudioEngine* _audioEngine)
    : m_channels(_audioEngine->channels())
    , m_audioEngine(_audioEngine)
    , m_sampleRate(_audioEngine->processingSampleRate())

{
    m_buffer = (new sampleFrame(audioEngine()->channels(), audioEngine()->framesPerPeriod()));

    soxr_error_t error;
    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32_I, SOXR_FLOAT32_I);
    soxr_quality_spec_t q_spec = soxr_quality_spec(SOXR_VHQ, 0);
    soxr_runtime_spec_t runtime_spec = soxr_runtime_spec(0);

    m_soxr_left = soxr_create(_audioEngine->processingSampleRate(), _audioEngine->processingSampleRate(), 1,
        &error, &io_spec, &q_spec, &runtime_spec);

    m_soxr_right = soxr_create(_audioEngine->processingSampleRate(), _audioEngine->processingSampleRate(), 1,
        &error, &io_spec, &q_spec, &runtime_spec);

    qDebug() << "CREAED AudioDevice::AudioDevice";
    if (error) {
        printf("SOXR right init error: %s\n", soxr_strerror(error));
    }
}

AudioDevice::~AudioDevice()
{
    qDebug() << "AudioDevice::~AudioDevice()";
    if (m_soxr_left)
        soxr_delete(m_soxr_left);
    if (m_soxr_right)
        soxr_delete(m_soxr_right);

    m_soxr_left = nullptr;
    m_soxr_right = nullptr;
    delete m_buffer;
}

void AudioDevice::processFlush()
{

    fpp_t frames = audioEngine()->framesPerPeriod();

    if (audioEngine()->processingSampleRate() != m_sampleRate) {
        for (int j = 0; j < 2; j++) {
            frames = resample_flush(j, frames, m_buffer);
        }

    } else {
        frames = 0;
    }

    if (frames > 0) {
        writeBuffer(m_buffer, frames, 1);
    } else {
        m_inProcess = false;
    }
}

void AudioDevice::processNextBuffer()
{

    const fpp_t frames = getNextBuffer(m_buffer);

    if (frames) {

        writeBuffer(m_buffer, frames, 1);

    } else {
        m_inProcess = false;
    }
}

void AudioDevice::stopProcessing()
{
    if (audioEngine()->hasFifoWriter()) {
        while (m_inProcess) {
            processNextBuffer();
        }
    }
}

fpp_t AudioDevice::resample(int channel, const sampleFrame* _src,
    const fpp_t _frames,
    sampleFrame* _dst,
    const sample_rate_t _src_sr,
    const sample_rate_t _dst_sr)
{

    size_t odone = _frames;
    if (_src->getReadPointer(channel)) {

        size_t outSize = (_frames * _dst_sr / (double)_src_sr + 1);

        if (channel == 0) {
            soxr_error_t error = soxr_process(m_soxr_left,
                (void*)_src->getReadPointer(channel), _frames, NULL,
                (void*)_dst->getWritePointer(channel), outSize, &odone);
        }

        if (channel == 1) {
            soxr_error_t error = soxr_process(m_soxr_right,
                (void*)_src->getReadPointer(channel), _frames, NULL,
                (void*)_dst->getWritePointer(channel), outSize, &odone);
        }
    }

    return odone;
}

fpp_t AudioDevice::resample_flush(int channel, const fpp_t _frames, sampleFrame* _dst)
{

    size_t odone = _frames;

    size_t outSize = _frames;

    soxr_error_t error;

    if (channel == 0) {
        error = soxr_process(m_soxr_left,
            NULL, 0, NULL,
            (void*)_dst->getWritePointer(channel), outSize, &odone);
    }

    if (channel == 1) {
        error = soxr_process(m_soxr_right,
            NULL, 0, NULL,
            (void*)_dst->getWritePointer(channel), outSize, &odone);
    }

    return odone;
}

fpp_t AudioDevice::getNextBuffer(sampleFrame* _ab)
{
    fpp_t frames = audioEngine()->framesPerPeriod();
    sampleFrame* b = audioEngine()->nextBuffer();

    if (!b) {
        return 0;
    }

    lock();

    if (audioEngine()->processingSampleRate() != m_sampleRate) {
        for (int j = 0; j < 2; j++) {
            frames = resample(j, b, b->getNumSamples(), _ab, audioEngine()->outputSampleRate(), m_sampleRate);
        }

    } else {

        for (int i = 0; i < _ab->getNumChannels(); i++) {
            _ab->copyFrom(i, 0, b->getReadPointer(i), b->getNumSamples());
        }
    }

    // frames = resample(b, frames, _ab, audioEngine()->processingSampleRate(), m_sampleRate);

    unlock();

    return frames;
}

void AudioDevice::stopProcessingThread(QThread* thread)
{
    if (!thread->wait(30000)) {
        fprintf(stderr, "Terminating audio device thread\n");
        thread->terminate();
        if (!thread->wait(1000)) {
            fprintf(stderr, "Thread not terminated yet\n");
        }
    }
}

int AudioDevice::convertToS16(const sampleFrame* _ab, const fpp_t _frames, const float _master_gain, int_sample_t* _output_buffer, const bool _convert_endian)
{

    if (_convert_endian) {
        int_sample_t temp;
        for (fpp_t frame = 0; frame < _frames; ++frame) {
            for (ch_cnt_t chnl = 0; chnl < channels(); ++chnl) {
                temp = static_cast<int_sample_t>(AudioEngine::clip(_ab->getSample(chnl, frame) * _master_gain) * OUTPUT_SAMPLE_MULTIPLIER);

                (_output_buffer + frame * channels())[chnl] = (temp & 0x00ff) << 8 | (temp & 0xff00) >> 8;
            }
        }
    } else {
        for (fpp_t frame = 0; frame < _frames; ++frame) {
            for (ch_cnt_t chnl = 0; chnl < channels(); ++chnl) {
                (_output_buffer + frame * channels())[chnl] = static_cast<int_sample_t>(
                    AudioEngine::clip(_ab->getSample(chnl, frame) * _master_gain) * OUTPUT_SAMPLE_MULTIPLIER);
            }
        }
    }

    return _frames * channels() * BYTES_PER_INT_SAMPLE;
}

void AudioDevice::setSampleRate(sample_rate_t newSampleRate)
{
    m_sampleRate = newSampleRate;
    size_t oexpframes = 0;
    delete m_buffer;
    if (audioEngine()->processingSampleRate() != m_sampleRate) {
        oexpframes = (audioEngine()->framesPerPeriod() * m_sampleRate / (double)audioEngine()->outputSampleRate() + 1);
        m_buffer = new sampleFrame(audioEngine()->channels(), oexpframes);

        if (m_soxr_left)
            soxr_delete(m_soxr_left);

        if (m_soxr_right)
            soxr_delete(m_soxr_right);

        soxr_error_t error;
        soxr_io_spec_t io_spec = soxr_io_spec(SOXR_FLOAT32, SOXR_FLOAT32);
        soxr_quality_spec_t q_spec = soxr_quality_spec(SOXR_HQ, 0);
        soxr_runtime_spec_t runtime_spec = soxr_runtime_spec(1);

        m_soxr_left = soxr_create(m_audioEngine->processingSampleRate(), newSampleRate, 1,
            &error, &io_spec, &q_spec, &runtime_spec);

        m_soxr_right = soxr_create(m_audioEngine->processingSampleRate(), newSampleRate, 1,
            &error, &io_spec, &q_spec, &runtime_spec);

        if (error) {
            printf("SOXR right init error: %s\n", soxr_strerror(error));
        }
        qDebug() << "setSampleRate" << newSampleRate;
    } else {
        m_buffer = (new sampleFrame(audioEngine()->channels(), audioEngine()->framesPerPeriod()));
    }
}

int AudioDevice::getMaxOutChannels() const
{
    return m_maxOutChannels;
}

void AudioDevice::applyQualitySettings()
{
    //    src_delete(m_srcState);

    //    int error;
    //    if ((m_srcState = src_new(
    //             audioEngine()->currentQualitySettings().libsrcInterpolation(),
    //             SURROUND_CHANNELS, &error))
    //        == nullptr) {
    //        printf("Error: src_new() failed in audio_device.cpp!\n");
    //    }
}

int AudioDevice::getOutDevIdx() const
{
    return m_outDevIdx;
}

QString AudioDevice::getHostApiName() const
{
    return _hostApiName;
}

QString AudioDevice::getDeviceName() const
{
    return _deviceName;
}
