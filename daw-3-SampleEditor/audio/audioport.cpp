#include "audioport.h"
#include "audioengine.h"
#include "engine.h"
#include "mixer.h"
#include "mixhelpers.h"
#include "sampletrack.h"
#include "song.h"
#include <core/automation.h>
AudioPort::AudioPort(const QString& _name, SampleTrack* sampleTrack)
    : m_name("unnamed port")
    , m_bufferUsage(false)
    , m_nextMixerChannel(0)
    , m_sampleTrack(sampleTrack)
    , mCaptureSampleBuffers(2, nullptr)
    , mPlaybackSampleBuffers(36, nullptr)

{

    if (AudioManager::audioEngine() == Q_NULLPTR)
        return;

    int fpp = AudioManager::audioEngine()->framesPerPeriod();
    m_channels = AudioManager::audioEngine()->channels();
    m_portBuffer = new sampleFrame(m_channels, fpp);

    for (int i = 0; i < 2; i++) {
        mCaptureSampleBuffers[i] = new visr::SampleType[fpp];
    }
    for (int i = 0; i < 36; i++) {
        mPlaybackSampleBuffers[i] = new visr::SampleType[fpp];
    }

    connect(AudioManager::audioEngine(), &AudioEngine::aduioDeviceChanged, this, &AudioPort::audioEngineChanged);

    m_effects = QSharedPointer<EffectChain>(new EffectChain(sampleTrack->trackIndex(), nullptr));

    // setExtOutputEnabled(true);
}

void AudioPort::audioEngineChanged()
{

    if (m_channels != AudioManager::audioEngine()->channels()) {
        m_channels = AudioManager::audioEngine()->channels();
        int fpp = AudioManager::audioEngine()->framesPerPeriod();
        if (m_portBuffer == Q_NULLPTR) {
            m_portBuffer = new sampleFrame(m_channels, fpp);
        } else {
            delete m_portBuffer;
            m_portBuffer = new sampleFrame(m_channels, fpp);
        }
    }
}
AudioPort::~AudioPort()
{

    for (int i = 0; i < 2; i++) {
        delete mCaptureSampleBuffers[i];
    }
    for (int i = 0; i < 36; i++) {
        delete mPlaybackSampleBuffers[i];
    }

    delete m_portBuffer;
}

void AudioPort::addPlayHandle(PlayHandle* handle)
{

    m_playHandleLock.lock();
    m_playHandles.append(handle);
    m_playHandleLock.unlock();
}

void AudioPort::removePlayHandle(PlayHandle* handle)
{

    m_playHandleLock.lock();
    PlayHandleList::Iterator it = std::find(m_playHandles.begin(), m_playHandles.end(), handle);
    if (it != m_playHandles.end()) {
        m_playHandles.erase(it);
    }
    m_playHandleLock.unlock();
}

void AudioPort::addVisr(QSharedPointer<VisrHelper> handle)
{
    m_visrHelper = handle;
}

void AudioPort::removeVisr()
{
    m_visrHelper.reset();
}

QSharedPointer<VisrHelper> AudioPort::visrHelper() const
{
    return m_visrHelper;
}

bool AudioPort::processEffects()
{
    if (m_effects) {
        bool more = m_effects->processAudioBuffer(m_portBuffer, AudioManager::audioEngine()->framesPerPeriod(), m_bufferUsage);
        return more;
    }
    return false;
}

volatile bool AudioPort::bufferUsed() const
{
    return m_bufferUsed;
}

volatile bool AudioPort::bufferUsage() const
{
    return m_bufferUsage;
}

void AudioPort::doProcessing()
{

    const fpp_t fpp = AudioManager::audioEngine()->framesPerPeriod();
    // clear the buffer
    m_portBuffer->clear();
    m_bufferUsed = false;
    m_bufferUsage = false;

    for (PlayHandle* ph : m_playHandles) // now we mix all playhandle buffers into the audioport buffer
    {
        if (ph->buffer()) {
            if (ph->usesBuffer()) {
                m_bufferUsage = true;
                m_bufferUsed = true;
                // MixHelpers::add(m_portBuffer, ph->buffer(), fpp);
                for (int i = 0; i < m_portBuffer->getNumChannels(); i++)
                    m_portBuffer->addFrom(i, 0, ph->buffer()->getReadPointer(i), fpp);
            }
            ph->releaseBuffer(); // gets rid of playhandle's buffer and sets
                                 // pointer to null, so if it doesn't get re-acquired we know to skip it next time
        }
    }

    if (m_bufferUsage) {

        processEffects();

        if (m_visrHelper) {

            if (m_visrHelper->enabled()) {
                for (int i = 0; i < 2; i++) {
                    visr::SampleType* outputPtr = mCaptureSampleBuffers[i];

                    for (f_cnt_t f = 0; f < fpp; ++f) {

                        auto value = m_portBuffer->getSample(i, f);

                        *outputPtr = value;
                        outputPtr++;
                    }
                }
                m_visrHelper->process(&mCaptureSampleBuffers[0], &mPlaybackSampleBuffers[0]);

                for (int i = 0; i < AudioManager::audioEngine()->channels(); i++) {
                    if (m_visrHelper->speakerModel()->isChecked(i)) {
                        visr::SampleType* outputPtr = mPlaybackSampleBuffers[i];

                        for (f_cnt_t f = 0; f < fpp; ++f) {

                            m_portBuffer->setSample(m_visrHelper->speakerModel()->getIndex(i), f, *outputPtr);
                            outputPtr++;
                        }
                    }
                }
            }

            double time = (AudioManager::getSong()->getPlayPos() * fpp) / AudioManager::audioEngine()->outputSampleRate() * 1000.0;
            double decibels = 0;
            double Y = 0;

            if (m_sampleTrack->volumeAutomation()->isAutomated()) {

                if (m_sampleTrack->volumeAutomation()->enabled()) {

                    Y = m_sampleTrack->volumeAutomation()->getY(time);
                    decibels = MathHelper1::yPixel2Decibel(Y);
                } else {
                    decibels = MathHelper1::yPixel2Decibel(m_sampleTrack->volumeAutomation()->normalized());
                }
            } else {
                decibels = MathHelper1::yPixel2Decibel(m_sampleTrack->volumeAutomation()->normalized());
            }

        } else {
            if (AudioManager::audioEngine()->channels() == 2) {
                if (m_sampleTrack->volumeAutomation()) {

                    double time = (AudioManager::getSong()->getPlayPos() * fpp) / AudioManager::audioEngine()->outputSampleRate() * 1000.0;

                    double decibels = 0;
                    double Y = 0;

                    if (m_sampleTrack->volumeAutomation()->isAutomated()) {

                        if (m_sampleTrack->volumeAutomation()->enabled()) {

                            Y = m_sampleTrack->volumeAutomation()->getY(time);

                            decibels = MathHelper1::yPixel2Decibel(Y);
                        } else {
                            decibels = MathHelper1::yPixel2Decibel(m_sampleTrack->volumeAutomation()->normalized());
                        }
                    } else {
                        decibels = MathHelper1::yPixel2Decibel(m_sampleTrack->volumeAutomation()->normalized());
                    }

                    for (f_cnt_t f = 0; f < fpp; ++f) {
                        double v0 = qPow(10, decibels / 20.0);
                        double v1 = qPow(10, decibels / 20.0);

                        m_portBuffer->setSample(0, f, m_portBuffer->getSample(0, f) * v0);
                        m_portBuffer->setSample(1, f, m_portBuffer->getSample(1, f) * v1);
                    }
                }

                if (m_sampleTrack->panAutomation()) {

                    double time = (AudioManager::getSong()->getPlayPos() * fpp) / AudioManager::audioEngine()->outputSampleRate() * 1000.0;

                    double lGain = 1;
                    double rGain = 1;
                    double Y = 0;

                    if (m_sampleTrack->panAutomation()->isAutomated()) {

                        if (m_sampleTrack->panAutomation()->enabled()) {

                            Y = m_sampleTrack->panAutomation()->getY(time);

                            if (Y != 0.5) {
                                lGain = MathHelper1::pan2leftGain(Y);
                                rGain = MathHelper1::pan2RightGain(Y);
                            }

                        } else {
                            Y = m_sampleTrack->panAutomation()->normalized();

                            if (Y != 0.5) {
                                lGain = MathHelper1::pan2leftGain(Y);
                                rGain = MathHelper1::pan2RightGain(Y);
                            }
                        }
                    } else {

                        Y = m_sampleTrack->panAutomation()->normalized();

                        if (Y != 0.5) {
                            lGain = MathHelper1::pan2leftGain(Y);
                            rGain = MathHelper1::pan2RightGain(Y);
                        }
                    }

                    for (f_cnt_t f = 0; f < fpp; ++f) {

                        m_portBuffer->setSample(1, f, m_portBuffer->getSample(1, f) * lGain);
                        m_portBuffer->setSample(0, f, m_portBuffer->getSample(0, f) * rGain);
                    }
                }
            }
        }

        auto tmp_buffer = new sampleFrame(m_portBuffer->getNumChannels(), m_portBuffer->getNumSamples());

        for (int i = 0; i < m_portBuffer->getNumChannels(); i++) {
            tmp_buffer->copyFrom(i, 0, m_portBuffer->getReadPointer(i), m_portBuffer->getNumSamples());
        }
        m_sampleTrack->truePeakMeter()->processBlock(*tmp_buffer);
        delete tmp_buffer;
        tmp_buffer = Q_NULLPTR;

        if (m_sampleTrack->speakerAutomation()) {

            double time = (AudioManager::getSong()->getPlayPos() * fpp) / AudioManager::audioEngine()->outputSampleRate() * 1000.0;

            double Y = 0;

            if (m_sampleTrack->speakerAutomation()->isAutomated()) {

                if (m_sampleTrack->speakerAutomation()->enabled()) {

                    Y = m_sampleTrack->speakerAutomation()->getY(time);

                    if (qAbs(Y - 0.75) < 0.1) {

                        for (f_cnt_t f = 0; f < fpp; ++f) {
                            m_portBuffer->setSample(1, f, 0);
                            m_portBuffer->setSample(0, f, 0);
                        }
                    }
                } else {
                    Y = m_sampleTrack->speakerAutomation()->normalized();
                    if (qAbs(Y - 0.75) < 0.1) {
                        for (f_cnt_t f = 0; f < fpp; ++f) {
                            m_portBuffer->setSample(1, f, 0);
                            m_portBuffer->setSample(0, f, 0);
                        }
                    }
                }
            } else {
                Y = m_sampleTrack->speakerAutomation()->normalized();
                if (qAbs(Y - 0.75) < 0.1) {
                    for (f_cnt_t f = 0; f < fpp; ++f) {
                        m_portBuffer->setSample(1, f, 0);
                        m_portBuffer->setSample(0, f, 0);
                    }
                }
            }
        }

        if (qAbs(AudioManager::getSong()->getPlayPos().getlastTicks() - AudioManager::getSong()->getPlayPos().getTicks()) <= 1) {

            for (int i = 0; i < m_portBuffer->getNumChannels(); i++) {
                for (int j = 0; j < m_portBuffer->getNumSamples(); j++) {

                    double fadeFactor = 1.0f - j / (double)m_portBuffer->getNumSamples();

                    m_portBuffer->setSample(i, j, m_portBuffer->getSample(i, j) * fadeFactor);
                }
            }
        }

        if (AudioManager::getSong()->getPlayPos().getTicks() == AudioManager::getSong()->getPlayPos().getFirstTicks()
            || AudioManager::getSong()->getPlayPos().getTicks() == AudioManager::getSong()->getPlayPos().getFirstTicks() + 1) {

            for (int i = 0; i < m_portBuffer->getNumChannels(); i++) {
                for (int j = 0; j < m_portBuffer->getNumSamples(); j++) {

                    double fadeFactor = j / (double)m_portBuffer->getNumSamples();

                    m_portBuffer->setSample(i, j, m_portBuffer->getSample(i, j) * fadeFactor);
                }
            }
        }

        // qDebug() << "AudioManager::mixer()->hasSolo()" << AudioManager::mixer()->hasSolo() << m_sampleTrack->isMuted() << m_sampleTrack->isSelected() << m_sampleTrack->isSolo();
        if (AudioManager::mixer()->hasSolo()) {
            if (m_sampleTrack->isSolo()) {
                AudioManager::mixer()->mixToChannel(m_portBuffer, m_nextMixerChannel);
            }
        } else if ((m_sampleTrack->isSelected() && !m_sampleTrack->isMuted())) {

            AudioManager::mixer()->mixToChannel(m_portBuffer, m_nextMixerChannel);
        }

        m_bufferUsage = false;
    } else {
    }
}
