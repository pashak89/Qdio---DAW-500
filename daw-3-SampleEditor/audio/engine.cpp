#include "engine.h"
#include "audioengine.h"
#include "mixer.h"
#include "song.h"
#include <QDebug>
float AudioManager::s_framesPerTick;
AudioEngine* AudioManager::s_audioEngine = nullptr;
Song* AudioManager::s_song = nullptr;
Mixer* AudioManager::s_mixer = nullptr;
ProjectJournal* AudioManager::s_projectJournal = nullptr;
void AudioManager::init(bool renderOnly)
{
    inst();

    s_audioEngine = new AudioEngine(2);
    s_audioEngine->initDevices(renderOnly, 2);

    s_song = new Song;
    s_mixer = new Mixer;

    s_audioEngine->startProcessing();

    s_projectJournal = new ProjectJournal;

    updateFramesPerTick();
}

void AudioManager::destroy()
{
    s_audioEngine->stopProcessing();
    s_audioEngine->freeDevices();
    deleteHelper(&s_song);
    deleteHelper(&s_mixer);
    deleteHelper(&s_audioEngine);

    deleteHelper(&s_projectJournal);
}

void AudioManager::audioEngineOff()
{

    s_audioEngine->stopProcessing();
    s_audioEngine->freeDevices();
    s_audioEngine->initDevices(true);
    s_mixer->clearChannels();
    s_mixer->createChannel();
}

void AudioManager::audioEngineOn(int deviceIndex, int channel)
{

    s_audioEngine->stopProcessing();
    s_audioEngine->freeDevices();
    if (s_audioEngine->initDevices(false, channel, deviceIndex)) {
        s_audioEngine->startProcessing();
    }
    s_mixer->clearChannels();
    s_mixer->createChannel();
}

float AudioManager::framesPerTick(sample_rate_t sampleRate)
{
    return sampleRate * 60.0f * 4 / DefaultTicksPerBar / s_song->getTempo();
}

void AudioManager::updateFramesPerTick()
{
    s_framesPerTick = s_audioEngine->processingSampleRate() * 60.0f * 4 / DefaultTicksPerBar / s_song->getTempo();
    qDebug() << "GHELLO" << s_framesPerTick;
}

AudioManager* AudioManager::s_instanceOfMe = nullptr;
