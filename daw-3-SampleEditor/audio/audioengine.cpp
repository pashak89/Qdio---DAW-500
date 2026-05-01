#include "audioengine.h"
#include "audiodevice.h"
#include "audiodummy.h"
#include "audioengineworkerthread.h"
#include "audioport.h"
#include "engine.h"
#include "mixer.h"
#include "playhandle.h"
#include "portaudiodevice.h"
#include "song.h"

#include <QDebug>

AudioEngine::AudioEngine(int channels, QObject* parent)
    : QObject(parent)
    , _outputSampleRate(44100)
    , m_framesPerPeriod(DEFAULT_BUFFER_SIZE)
    , m_renderOnly(false)
    , m_isProcessing(false)
    , m_channels(channels)
    , m_changes(0)
    , m_clearSignal(false)
    , m_changesSignal(false)
    , m_waitingForWrite(false)
    , m_qualitySettings(qualitySettings::Mode_Draft)
    , m_doChangesMutex(QMutex::Recursive)
    , m_newPlayHandles(PlayHandle::MaxNumber)
    , m_numWorkers(QThread::idealThreadCount() - 1)

{

    for (int i = 0; i < m_numWorkers + 1; ++i) {
        auto wt = new AudioEngineWorkerThread(this);
        if (i < m_numWorkers) {
            wt->start(QThread::TimeCriticalPriority);
        }
        m_workers.push_back(wt);
    }

    int fifoSize = 1;
    m_fifo = new Fifo(fifoSize);

    connect(this, &AudioEngine::sampleRateChanged, this, &AudioEngine::sltSampleRateChanged);
}

AudioEngine::~AudioEngine()
{

    runChangesInModel();

    for (int w = 0; w < m_numWorkers; ++w) {
        m_workers[w]->quit();
    }

    AudioEngineWorkerThread::startAndWaitForJobs();

    for (int w = 0; w < m_numWorkers; ++w) {
        m_workers[w]->wait(500);
    }

    delete m_audioDev;

    delete m_outputBufferRead;
    delete m_outputBufferWrite;
}

AudioDevice* AudioEngine::tryAudioDevices(int channels, int deviceIndex)
{

    bool success_ful = false;
    AudioDevice* dev = nullptr;

    m_audioDevStartFailed = false;
    dev = new PortAudioDevice(success_ful, channels, this, deviceIndex);
    qDebug() << "success_ful" << success_ful;
    if (success_ful) {
        return dev;
    }
    return Q_NULLPTR;
}

bool AudioEngine::initDevices(bool renderOnly, int channels, int deviceIndex)
{

    bool success_ful = false;

    if (renderOnly) {
        m_audioDev = new AudioDummy(success_ful, this);
        Q_EMIT aduioDeviceChanged();
        return false;
    } else {
        freeDevices();
        m_audioDev = tryAudioDevices(channels, deviceIndex);

        if (m_audioDev == Q_NULLPTR) {
            m_audioDev = new AudioDummy(success_ful, this);
            Q_EMIT aduioDeviceChanged();
            return false;
        } else {
            m_channels = m_audioDev->channels();
            m_outputBufferRead = new sampleFrame(m_channels, m_framesPerPeriod);
            m_outputBufferWrite = new sampleFrame(m_channels, m_framesPerPeriod);
            Q_EMIT aduioDeviceChanged();
            return true;
        }
    }
}

void AudioEngine::freeDevices()
{
    if (m_audioDev) {
        delete m_audioDev;
        m_audioDev = Q_NULLPTR;
    }
    if (m_outputBufferRead) {
        delete m_outputBufferRead;
        m_outputBufferRead = Q_NULLPTR;
    }
    if (m_outputBufferWrite) {
        delete m_outputBufferWrite;
        m_outputBufferWrite = Q_NULLPTR;
    }
}

void AudioEngine::startProcessing(bool needsFifo)
{

    if (needsFifo) {
        m_fifoWriter = new fifoWriter(this, m_fifo);
        m_fifoWriter->start(QThread::HighPriority);
    } else {
        m_fifoWriter = nullptr;
    }

    m_audioDev->startProcessing();

    m_isProcessing = true;
}

void AudioEngine::stopProcessing()
{
    m_isProcessing = false;

    if (m_fifoWriter != nullptr) {
        m_fifoWriter->finish();
        m_fifoWriter->wait();
        m_audioDev->stopProcessing();
        delete m_fifoWriter;
        m_fifoWriter = nullptr;
    } else {
        m_audioDev->stopProcessing();
    }
}

void AudioEngine::clear()
{
    m_clearSignal = true;
}

void AudioEngine::removeAudioPort(AudioPort* port)
{
    requestChangeInModel();

    QVector<AudioPort*>::Iterator it = std::find(m_audioPorts.begin(), m_audioPorts.end(), port);
    if (it != m_audioPorts.end()) {
        m_audioPorts.erase(it);
    }
    doneChangeInModel();
}

bool AudioEngine::criticalXRuns() const
{
    return AudioManager::getSong()->isExporting() == false;
}

bool AudioEngine::addPlayHandle(PlayHandle* handle)
{
    m_newPlayHandles.push(handle);
    handle->audioPort()->addPlayHandle(handle);
    return true;
}

void AudioEngine::removePlayHandle(PlayHandle* ph)
{
    requestChangeInModel();
    // check thread affinity as we must not delete play-handles
    // which were created in a thread different than the audio engine thread
    if (ph->affinityMatters() && ph->affinity() == QThread::currentThread()) {
        ph->audioPort()->removePlayHandle(ph);
        bool removedFromList = false;
        // Check m_newPlayHandles first because doing it the other way around
        // creates a race condition
        for (LocklessListElement *e = m_newPlayHandles.first(),
                                 *ePrev = nullptr;
             e; ePrev = e, e = e->next) {
            if (e->value == ph) {
                if (ePrev) {
                    ePrev->next = e->next;
                } else {
                    m_newPlayHandles.setFirst(e->next);
                }
                m_newPlayHandles.free(e);
                removedFromList = true;
                break;
            }
        }
        // Now check m_playHandles
        PlayHandleList::Iterator it = std::find(m_playHandles.begin(), m_playHandles.end(), ph);
        if (it != m_playHandles.end()) {
            m_playHandles.erase(it);
            removedFromList = true;
        }
        // Only deleting PlayHandles that were actually found in the list
        // "fixes crash when previewing a preset under high load"
        // (See tobydox's 2008 commit 4583e48)
        if (removedFromList) {
            delete ph;
        }
    } else {
        m_playHandlesToRemove.push_back(ph);
    }
    doneChangeInModel();
}

void AudioEngine::changeQuality(const qualitySettings& qs)
{

    stopProcessing();

    m_qualitySettings = qs;
    m_audioDev->applyQualitySettings();

    emit sampleRateChanged();
    emit qualitySettingsChanged();

    // startProcessing();
}

sample_rate_t AudioEngine::outputSampleRate() const
{
    return _outputSampleRate;
}

sample_rate_t AudioEngine::processingSampleRate() const
{
    return outputSampleRate();
}

void AudioEngine::removePlayHandlesOfTypes(Track* track, const quint8 types)
{

    //    if (m_playHandles.size() == 0)
    //        return;

    requestChangeInModel();

    PlayHandleList::Iterator it = m_playHandles.begin();
    while (it != m_playHandles.end()) {
        if ((*it)->isFromTrack(track) && ((*it)->type() & types)) {
            (*it)->audioPort()->removePlayHandle((*it));

            delete *it;
            it = m_playHandles.erase(it);

        } else {
            ++it;
        }
    }

    doneChangeInModel();
}

// removes all play-handles. this is necessary, when the song is stopped ->
// all remaining notes etc. would be played until their end
void AudioEngine::clearInternal()
{
    // TODO: m_midiClient->noteOffAll();
    for (auto ph : m_playHandles) {
        if (ph->type() != PlayHandle::TypeInstrumentPlayHandle) {
            m_playHandlesToRemove.push_back(ph);
        }
    }
}

void AudioEngine::swapBuffers()
{
    m_inputBufferWrite = (m_inputBufferWrite + 1) % 2;
    m_inputBufferRead = (m_inputBufferRead + 1) % 2;
    m_inputBufferFrames[m_inputBufferWrite] = 0;

    std::swap(m_outputBufferRead, m_outputBufferWrite);
    m_outputBufferWrite->clear();
}

sampleFrame* AudioEngine::renderNextBuffer()
{

    s_renderingThread = true;

    if (m_clearSignal) {
        m_clearSignal = false;
        clearInternal();
    }

    // remove all play-handles that have to be deleted and delete
    // them if they still exist...
    // maybe this algorithm could be optimized...
    ConstPlayHandleList::Iterator it_rem = m_playHandlesToRemove.begin();
    while (it_rem != m_playHandlesToRemove.end()) {
        PlayHandleList::Iterator it = std::find(m_playHandles.begin(), m_playHandles.end(), *it_rem);

        if (it != m_playHandles.end()) {
            (*it)->audioPort()->removePlayHandle((*it));
            delete *it;
            m_playHandles.erase(it);
        }

        it_rem = m_playHandlesToRemove.erase(it_rem);
    }

    swapBuffers();

    // prepare master mix (clear internal buffers etc.)
    auto mixer = AudioManager::mixer();
    mixer->prepareMasterMix();

    // handleMetronome();

    // create play-handles for new notes, samples etc.
    AudioManager::getSong()->processNextBuffer();

    // add all play-handles that have to be added
    for (LocklessListElement* e = m_newPlayHandles.popList(); e;) {

        m_playHandles += e->value;
        LocklessListElement* next = e->next;
        m_newPlayHandles.free(e);
        e = next;
    }

    // STAGE 1: run and render all play handles
    AudioEngineWorkerThread::fillJobQueue<PlayHandleList>(m_playHandles);
    AudioEngineWorkerThread::startAndWaitForJobs();

    // removed all play handles which are done

    for (PlayHandleList::Iterator it = m_playHandles.begin();
         it != m_playHandles.end();) {
        if ((*it)->affinityMatters() && (*it)->affinity() != QThread::currentThread()) {
            ++it;
            continue;
        }
        if ((*it)->isFinished()) {

            (*it)->audioPort()->removePlayHandle((*it));

            delete *it;
            it = m_playHandles.erase(it);
        } else {
            ++it;
        }
    }

    // STAGE 2: process effects of all instrument- and sampletracks
    AudioEngineWorkerThread::fillJobQueue<QVector<AudioPort*>>(m_audioPorts);
    AudioEngineWorkerThread::startAndWaitForJobs();

    // STAGE 3: do master mix in mixer
    mixer->masterMix(m_outputBufferWrite);

    emit nextAudioBuffer(m_outputBufferRead);

    runChangesInModel();

    AudioManager::getSong()->processLoadNewTracks();

    s_renderingThread = false;

    return m_outputBufferRead;
}

void AudioEngine::doneChangeInModel()
{
    if (s_renderingThread)
        return;

    m_changesMutex.lock();
    bool moreChanges = --m_changes;
    m_changesMutex.unlock();

    if (!moreChanges) {
        m_changesSignal = false;

        m_changesAudioEngineCondition.wakeOne();
    }
    m_doChangesMutex.unlock();
}

void AudioEngine::requestChangeInModel()
{
    if (s_renderingThread)
        return;

    m_changesMutex.lock();
    m_changes++;
    m_changesMutex.unlock();

    m_doChangesMutex.lock();
    m_waitChangesMutex.lock();

    if (m_isProcessing && !m_waitingForWrite && !m_changesSignal) {
        m_changesSignal = true;
        m_changesRequestCondition.wait(&m_waitChangesMutex);
    }

    m_waitChangesMutex.unlock();
}
void AudioEngine::runChangesInModel()
{

    if (m_changesSignal) {
        m_waitChangesMutex.lock();
        // allow changes in the model from other threads ...
        m_changesRequestCondition.wakeOne();
        // ... and wait until they are done
        m_changesAudioEngineCondition.wait(&m_waitChangesMutex);
        m_waitChangesMutex.unlock();
    }
}

void AudioEngine::setOutputSampleRate(int outputSampleRate)
{
    _outputSampleRate = outputSampleRate;
    emit sampleRateChanged();
}

void AudioEngine::sltSampleRateChanged()
{

    freeDevices();
    initDevices();
    startProcessing();
}

void AudioEngine::setFramesPerPeriod(fpp_t framesPerPeriod)
{

    m_framesPerPeriod = framesPerPeriod;
    freeDevices();
    initDevices();
    startProcessing();
}

void AudioEngine::setOutputLatency(double outputLatency)
{

    m_outputLatency = outputLatency;
    freeDevices();
    initDevices();
    startProcessing();
    Q_EMIT outputLatencyChanged();
}

AudioDevice* AudioEngine::audioDev() const
{
    return m_audioDev;
}

void AudioEngine::doSetAudioDevice(AudioDevice* _dev)
{
    // TODO: Use shared_ptr here in the future.
    // Currently, this is safe, because this is only called by
    // ProjectRenderer, and after ProjectRenderer calls this function,
    // it does not access the old device anymore.
    if (m_audioDev != m_oldAudioDev) {
        delete m_audioDev;
    }

    if (_dev) {
        m_audioDev = _dev;
    } else {
        printf("param _dev == NULL in AudioEngine::setAudioDevice(...). "
               "Trying any working audio-device\n");
        m_audioDev = tryAudioDevices();
    }
}

void AudioEngine::setAudioDevice(AudioDevice* _dev, const qualitySettings& _qs, bool _needs_fifo, bool startNow)
{

    stopProcessing();
    // freeDevices();

    m_qualitySettings = _qs;

    doSetAudioDevice(_dev);

    emit qualitySettingsChanged();
    //  emit sampleRateChanged();

    if (startNow) {

        startProcessing(_needs_fifo);
    }
}

void AudioEngine::restoreAudioDevice()
{

    if (m_oldAudioDev && m_audioDev != m_oldAudioDev) {
        stopProcessing();

        delete m_audioDev;

        m_audioDev = m_oldAudioDev;
        // emit sampleRateChanged();

        if (initDevices(false, m_oldAudioDev->channels(), m_oldAudioDev->getOutDevIdx())) {
            startProcessing();
        }
    }
    m_oldAudioDev = nullptr;
}

void AudioEngine::storeAudioDevice()
{
    if (!m_oldAudioDev) {
        m_oldAudioDev = m_audioDev;
    }
}

AudioEngine::fifoWriter::fifoWriter(AudioEngine* audioEngine, Fifo* fifo)
    : m_audioEngine(audioEngine)
    , m_fifo(fifo)
    , m_writing(true)
{
    setObjectName("AudioEngine::fifoWriter");
}

void AudioEngine::fifoWriter::finish()
{
    m_writing = false;
}

void AudioEngine::fifoWriter::run()
{
    disable_denormals();

    const fpp_t frames = m_audioEngine->framesPerPeriod();
    while (m_writing) {
        auto buffer = new sampleFrame(2, frames);
        const sampleFrame* b = m_audioEngine->renderNextBuffer();
        memcpy(buffer, b, frames * sizeof(sampleFrame));
        write(buffer);
    }

    // Let audio backend stop processing
    write(nullptr);
    m_fifo->waitUntilRead();
}

void AudioEngine::fifoWriter::write(sampleFrame* buffer)
{
    m_audioEngine->m_waitChangesMutex.lock();
    m_audioEngine->m_waitingForWrite = true;
    m_audioEngine->m_waitChangesMutex.unlock();
    m_audioEngine->runChangesInModel();

    m_fifo->write(buffer);

    m_audioEngine->m_doChangesMutex.lock();
    m_audioEngine->m_waitingForWrite = false;
    m_audioEngine->m_doChangesMutex.unlock();
}
