#include "mixer.h"
#include "audioengine.h"
#include "audioengineworkerthread.h"
#include "engine.h"
#include "mixhelpers.h"
#include <QDebug>
#include <QtMath>
Mixer::Mixer(QObject* parent)
    : QObject(parent)
    , m_mixerChannels()
    , m_lastSoloed(-1)
{

    // create master channel
    createChannel();
}

int Mixer::createChannel()
{
    const int index = m_mixerChannels.size();
    // create new channel
    m_mixerChannels.push_back(new MixerChannel(index));

    // reset channel state
    clearChannel(index);

    // if there is a soloed channel, mute the new track
    if (m_lastSoloed != -1 && m_mixerChannels[m_lastSoloed]->m_soloModel) {
        m_mixerChannels[index]->m_muteBeforeSolo = m_mixerChannels[index]->m_muteModel;
        m_mixerChannels[index]->m_muteModel = true;
    }

    m_truePeakMeter = new TruePeakAudioProcessor();
    m_truePeakMeter->setAverageAlgorithm(0);
    m_truePeakMeter->prepareToPlay(AudioManager::audioEngine()->outputSampleRate(),
        AudioManager::audioEngine()->channels(),
        AudioManager::audioEngine()->framesPerPeriod(),
        10);

    return index;
}

void Mixer::clearChannel(mix_ch_t index)
{
    MixerChannel* ch = m_mixerChannels[index];
    // ch->m_fxChain.clear();
    ch->m_volumeModel = 1.0f;
    ch->m_muteModel = false;
    ch->m_soloModel = false;
    ch->m_name = (index == 0) ? tr("Master") : tr("Channel %1").arg(index);

    // send only to master
    if (index > 0) {
        // delete existing sends
        while (!ch->m_sends.isEmpty()) {
            deleteChannelSend(ch->m_sends.first());
        }

        // add send to master
        createChannelSend(index, 0);
    }

    // delete receives
    while (!ch->m_receives.isEmpty()) {
        deleteChannelSend(ch->m_receives.first());
    }
}

void Mixer::clearChannels()
{
    delete m_truePeakMeter;
    while (!m_mixerRoutes.isEmpty()) {
        deleteChannelSend(m_mixerRoutes.first());
    }
    while (m_mixerChannels.size()) {
        MixerChannel* f = m_mixerChannels[m_mixerChannels.size() - 1];
        m_mixerChannels.pop_back();
        delete f;
    }
}

// delete the connection made by createChannelSend
void Mixer::deleteChannelSend(mix_ch_t fromChannel, mix_ch_t toChannel)
{
    // delete the send
    MixerChannel* from = m_mixerChannels[fromChannel];
    MixerChannel* to = m_mixerChannels[toChannel];

    // find and delete the send entry
    for (const auto& send : from->m_sends) {
        if (send->receiver() == to) {
            deleteChannelSend(send);
            break;
        }
    }
}

MixerRoute* Mixer::createChannelSend(mix_ch_t fromChannel, mix_ch_t toChannel,
    float amount)
{
    //	qDebug( "requested: %d to %d", fromChannel, toChannel );
    // find the existing connection
    MixerChannel* from = m_mixerChannels[fromChannel];
    MixerChannel* to = m_mixerChannels[toChannel];

    for (const auto& send : from->m_sends) {
        if (send->receiver() == to) {
            // simply adjust the amount
            send->setAmount(amount);
            return send;
        }
    }

    // connection does not exist. create a new one
    return createRoute(from, to, amount);
}

MixerRoute* Mixer::createRoute(MixerChannel* from, MixerChannel* to, float amount)
{
    if (from == to) {
        return nullptr;
    }
    AudioManager::audioEngine()->requestChangeInModel();
    auto route = new MixerRoute(from, to, amount);

    // add us to from's sends
    from->m_sends.append(route);

    // add us to to's receives
    to->m_receives.append(route);

    // add us to mixer's list
    AudioManager::mixer()->m_mixerRoutes.append(route);
    AudioManager::audioEngine()->doneChangeInModel();

    return route;
}

void Mixer::deleteChannelSend(MixerRoute* route)
{
    AudioManager::audioEngine()->requestChangeInModel();
    // remove us from from's sends
    route->sender()->m_sends.remove(route->sender()->m_sends.indexOf(route));
    // remove us from to's receives
    route->receiver()->m_receives.remove(route->receiver()->m_receives.indexOf(route));
    // remove us from mixer's list
    AudioManager::mixer()->m_mixerRoutes.remove(AudioManager::mixer()->m_mixerRoutes.indexOf(route));
    delete route;
    AudioManager::audioEngine()->doneChangeInModel();
}

void Mixer::prepareMasterMix()
{
    if (m_mixerChannels.size() > 0) {
        m_mixerChannels[0]->m_buffer->clear();
    }
}

void Mixer::mixToChannel(sampleFrame* _buf, mix_ch_t _ch)
{
    m_mixerChannels[_ch]->m_lock.lock();
    for (int i = 0; i < m_mixerChannels[_ch]->m_buffer->getNumChannels(); i++)
        m_mixerChannels[_ch]->m_buffer->addFrom(i, 0, _buf->getReadPointer(i), AudioManager::audioEngine()->framesPerPeriod());

    m_mixerChannels[_ch]->m_hasInput = true;
    m_mixerChannels[_ch]->m_lock.unlock();
}

TruePeakAudioProcessor* Mixer::truePeakMeter() const
{
    return m_truePeakMeter;
}

float Mixer::masterTrackVolume() const
{
    return m_masterTrackVolume;
}

void Mixer::setMasterTrackVolume(float masterTrackVolume)
{
    m_masterTrackVolume = masterTrackVolume;
}

void Mixer::masterMix(sampleFrame* _buf)
{
    const int fpp = AudioManager::audioEngine()->framesPerPeriod();

    AudioEngineWorkerThread::resetJobQueue(AudioEngineWorkerThread::JobQueue::Dynamic);
    for (MixerChannel* ch : m_mixerChannels) {
        ch->m_queued = true;
        AudioEngineWorkerThread::addJob(ch);
    }
    while (m_mixerChannels[0]->state() != ThreadableJob::ProcessingState::Done) {
        bool found = false;
        for (MixerChannel* ch : m_mixerChannels) {
            const auto s = ch->state();
            if (s == ThreadableJob::ProcessingState::Queued
                || s == ThreadableJob::ProcessingState::InProgress) {
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }
        AudioEngineWorkerThread::startAndWaitForJobs();
    }

    for (int i = 0; i < _buf->getNumChannels(); i++)
        _buf->addFrom(i, 0, m_mixerChannels[0]->m_buffer->getReadPointer(i), fpp);

    for (f_cnt_t f = 0; f < fpp; ++f) {
        double v0 = qPow(10, m_masterTrackVolume / 20.0);
        double v1 = qPow(10, m_masterTrackVolume / 20.0);
        _buf->setSample(0, f, _buf->getSample(0, f) * v0);
        _buf->setSample(1, f, _buf->getSample(1, f) * v1);
    }

    auto tmp_buffer = new sampleFrame(_buf->getNumChannels(), _buf->getNumSamples());
    for (int i = 0; i < _buf->getNumChannels(); i++) {
        tmp_buffer->copyFrom(i, 0, _buf->getReadPointer(i), _buf->getNumSamples());
    }
    m_truePeakMeter->processBlock(*tmp_buffer);
    delete tmp_buffer;

    // clear all channel buffers and
    // reset channel process state
    for (int i = 0; i < numChannels(); ++i) {
        m_mixerChannels[i]->m_buffer->clear();
        m_mixerChannels[i]->reset();
        m_mixerChannels[i]->m_queued = false;
        // also reset hasInput
        m_mixerChannels[i]->m_hasInput = false;
        m_mixerChannels[i]->m_dependenciesMet = 0;
    }
}

Mixer::~Mixer()
{
    delete m_truePeakMeter;
    while (!m_mixerRoutes.isEmpty()) {
        deleteChannelSend(m_mixerRoutes.first());
    }
    while (m_mixerChannels.size()) {
        MixerChannel* f = m_mixerChannels[m_mixerChannels.size() - 1];
        m_mixerChannels.pop_back();
        delete f;
    }
}

MixerChannel::MixerChannel(int idx)
    : m_hasInput(false)
    , m_stillRunning(false)
    , m_peakLeft(0.0f)
    , m_peakRight(0.0f)
    , m_buffer(new sampleFrame(AudioManager::audioEngine()->channels(),
          AudioManager::audioEngine()->framesPerPeriod()))
    , m_muteModel(false)
    , m_soloModel(false)
    , m_volumeModel(1.0)
    , m_name()
    , m_lock()
    , m_channelIndex(idx)
    , m_queued(false)
    , m_dependenciesMet(0)
    , m_muted(false)
{
}

MixerChannel::~MixerChannel()
{
    delete m_buffer;
}

void MixerChannel::unmuteForSolo()
{
    m_muteModel = false;
}

void MixerChannel::doProcessing()
{

    //    for (int i = 0; i < AudioManager::audioEngine()->channels(); i++) {
    //        m_truePeakLevel[i] = m_truePeakMeter->getLevel(i);
    //    }

    //    if (m_muted == false) {
    //        for (MixerRoute* senderRoute : m_receives) {
    //            MixerChannel* sender = senderRoute->sender();
    //            FloatModel* sendModel = senderRoute->amount();
    //            if (!sendModel)
    //                qFatal("Error: no send model found from %d to %d", senderRoute->senderIndex(), m_channelIndex);

    //            if (sender->m_hasInput || sender->m_stillRunning) {
    //                // figure out if we're getting sample-exact input
    //                ValueBuffer* sendBuf = sendModel->valueBuffer();
    //                ValueBuffer* volBuf = sender->m_volumeModel.valueBuffer();

    //                // mix it's output with this one's output
    //                sampleFrame* ch_buf = sender->m_buffer;

    //                // use sample-exact mixing if sample-exact values are available
    //                if (!volBuf && !sendBuf) // neither volume nor send has sample-exact data...
    //                {
    //                    const float v = sender->m_volumeModel.value() * sendModel->value();
    //                    MixHelpers::addSanitizedMultiplied(m_buffer, ch_buf, v, fpp);
    //                } else if (volBuf && sendBuf) // both volume and send have sample-exact data
    //                {
    //                    MixHelpers::addSanitizedMultipliedByBuffers(m_buffer, ch_buf, volBuf, sendBuf, fpp);
    //                } else if (volBuf) // volume has sample-exact data but send does not
    //                {
    //                    const float v = sendModel->value();
    //                    MixHelpers::addSanitizedMultipliedByBuffer(m_buffer, ch_buf, v, volBuf, fpp);
    //                } else // vice versa
    //                {
    //                    const float v = sender->m_volumeModel.value();
    //                    MixHelpers::addSanitizedMultipliedByBuffer(m_buffer, ch_buf, v, sendBuf, fpp);
    //                }
    //                m_hasInput = true;
    //            }
    //        }

    //        const float v = m_volumeModel.value();

    //        if (m_hasInput) {
    //            // only start fxchain when we have input...
    //            m_fxChain.startRunning();
    //        }

    //        m_stillRunning = m_fxChain.processAudioBuffer(m_buffer, fpp, m_hasInput);

    //        AudioEngine::StereoSample peakSamples = AudioManager::audioEngine()->getPeakValues(m_buffer, fpp);
    //        m_peakLeft = qMax(m_peakLeft, peakSamples.left * v);
    //        m_peakRight = qMax(m_peakRight, peakSamples.right * v);
    //    } else {
    //        m_peakLeft = m_peakRight = 0.0f;
    //    }

    //    // increment dependency counter of all receivers
    //    processed();
}

MixerRoute::MixerRoute(MixerChannel* from, MixerChannel* to, float amount)
    : m_from(from)
    , m_to(to)
    , m_amount(amount)

{
}
