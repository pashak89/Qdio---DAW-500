#include "juce.h"

JuceEngine::JuceEngine()
    : QObject()
{

    m_ebu128 = new Ebu128LoudnessMeter();
    m_truePeak = new TruePeakAudioProcessor();

    m_ebu128->prepareToPlay(44100, 2, 1000, 10);

    m_truePeak->prepareToPlay(44100, 2, 1000, 10);

    formatManager.registerBasicFormats();

    // Init default device
    audioDeviceManager.initialise(2, 2, nullptr, true, {}, nullptr);

    // Lock MessageManager for safety intall listner
    MessageManagerLock lock(Thread::getCurrentThread());

    audioDeviceManager.addAudioCallback(&audioSourcePlayer);

    audioSourcePlayer.setSource(&transportSource);

    timer_analyse.setInterval(10);

    connect(&timer_analyse, &QTimer::timeout, this, &JuceEngine::sltTimeout);
}

JuceEngine::~JuceEngine()
{
    transportSource.setSource(nullptr);
    audioSourcePlayer.setSource(nullptr);

    audioDeviceManager.removeAudioCallback(&audioSourcePlayer);
}
void JuceEngine::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &transportSource) {
        if (transportSource.isPlaying()) {
            emit startPlaying();
        } else {
            transportSource.setPosition(0.0);
            emit stopPlaying();
        }
    }
}
void JuceEngine::setAudioSource(QString url)
{
    stop();
    transportSource.setSource(nullptr);
    currentAudioFileSource.reset();

    AudioFormatReader* reader = nullptr;
    reader_analyse = nullptr;
    reader = formatManager.createReaderFor(String(url.toStdString()));

    buffer.clear();
    reader_analyse = formatManager.createReaderFor(String(url.toStdString()));
    buffer = AudioSampleBuffer(reader->numChannels, reader->lengthInSamples);
    reader_analyse->read(&buffer, 0, reader->lengthInSamples, 0, true, true);

    if (reader != nullptr) {
        currentAudioFileSource.reset(new AudioFormatReaderSource(reader, true));

        // ..and plug it into our transport source
        transportSource.setSource(currentAudioFileSource.get(),
            0, // tells it to buffer this many samples ahead
            nullptr, // this is the background thread to use for reading-ahead
            reader->sampleRate); // allows for sample rate correction

        qint64 result = (reader->sampleRate * 20 / 1000);

        m_ebu128->prepareToPlay(reader->sampleRate, reader->numChannels, result, 10);

        m_truePeak->prepareToPlay(reader->sampleRate, reader->numChannels, result, 10);
    }
}

int JuceEngine::getNumberOfChannel()
{
    if (reader_analyse)
        return reader_analyse->numChannels;
    else
        return 0;
}

void JuceEngine::stop()
{
    timer_analyse.stop();
    if (transportSource.isPlaying()) {
        transportSource.stop();
        transportSource.setPosition(0.0);
        m_bufferPosition = 0;
        m_playPosition = 0;
        m_truePeak->reset();
        m_ebu128->reset();
    }
}

void JuceEngine::play()
{

    buffer.clear();
    buffer = AudioSampleBuffer(reader_analyse->numChannels, reader_analyse->lengthInSamples);
    reader_analyse->read(&buffer, 0, reader_analyse->lengthInSamples, 0, true, true);

    m_truePeak->reset();
    m_ebu128->reset();
    transportSource.setPosition(0.0);
    m_bufferPosition = 0;
    m_playPosition = 0;

    timer_analyse.start();
    transportSource.start();
}

double JuceEngine::getCurrentPosition() const
{
    return getCurrentPosition();
}

double JuceEngine::getTotalTime() const
{
    return transportSource.getLengthInSeconds();
}

float JuceEngine::getIntegratedLoudness()
{
    return getEbu128()->getIntegratedLoudness();
}

float JuceEngine::getMomentaryLoudness()
{
    return getEbu128()->getMomentaryLoudness();
}

float JuceEngine::getMaximumMomentaryLoudness()
{
    return getEbu128()->getMaximumMomentaryLoudness();
}

float JuceEngine::getShortTermLoudness()
{
    return getEbu128()->getShortTermLoudness();
}

float JuceEngine::getMaximumShortTermLoudness()
{
    return getEbu128()->getMaximumShortTermLoudness();
}

float JuceEngine::getLoudnessRange()
{
    return getEbu128()->getLoudnessRange();
}

float JuceEngine::getMeasurementDuration()
{
    return getEbu128()->getMeasurementDuration();
}

float JuceEngine::getLoudnessRangeStart()
{
    return getEbu128()->getLoudnessRangeStart();
}

float JuceEngine::getLoudnessRangeEnd()
{
    return getEbu128()->getLoudnessRangeEnd();
}

float JuceEngine::getPeakMeterLevel(int channel)
{
    return getTruePeak()->getLevels()->getPeakMeterLevel(channel);
}

float JuceEngine::getMaximumPeakLevel(int channel)
{
    return getTruePeak()->getLevels()->getMaximumPeakLevel(channel);
}

float JuceEngine::getPhaseCorrelation()
{
    return getTruePeak()->getLevels()->getPhaseCorrelation();
}

float JuceEngine::getStereoMeterValue()
{
    return getTruePeak()->getLevels()->getStereoMeterValue();
}

float JuceEngine::getAverageMeterLevel(int channel)
{
    return getTruePeak()->getLevels()->getAverageMeterLevel(channel);
}

float JuceEngine::getNumberOfOverflows(int channel)
{
    return getTruePeak()->getLevels()->getNumberOfOverflows(channel);
}

float JuceEngine::getPeakMeterPeakLevel(int channel)
{
    return getTruePeak()->getLevels()->getPeakMeterPeakLevel(channel);
}

float JuceEngine::getTruePeakMeterLevel(int channel)
{
    return getTruePeak()->getLevels()->getTruePeakMeterLevel(channel);
}

float JuceEngine::getMaximumTruePeakLevel(int channel)
{
    return getTruePeak()->getLevels()->getMaximumTruePeakLevel(channel);
}

float JuceEngine::getAverageMeterPeakLevel(int channel)
{
    return getTruePeak()->getLevels()->getAverageMeterPeakLevel(channel);
}

float JuceEngine::getTruePeakMeterPeakLevel(int channel)
{
    return getTruePeak()->getLevels()->getTruePeakMeterPeakLevel(channel);
}

float JuceEngine::getMeterMinimumDecibel()
{
    return getTruePeak()->getLevels()->getMeterMinimumDecibel();
}

void JuceEngine::setActiveRms()
{
    m_truePeak->setAverageAlgorithm(0);
}

void JuceEngine::setActiveItu()
{
    m_truePeak->setAverageAlgorithm(1);
}

void JuceEngine::sltTimeout()
{
    qint64 playPosition = (reader_analyse->sampleRate * transportSource.getCurrentPosition());

    const qint64 readLen = (playPosition - m_playPosition);

    juce::AudioSampleBuffer calcBuffer(buffer.getArrayOfWritePointers(), reader_analyse->numChannels, m_bufferPosition, readLen);
    m_ebu128->processBlock(calcBuffer);
    m_truePeak->processBlock(calcBuffer);

    m_bufferPosition += readLen;
    m_playPosition = playPosition;
}

Ebu128LoudnessMeter* JuceEngine::getEbu128() const
{
    return m_ebu128;
}

TruePeakAudioProcessor* JuceEngine::getTruePeak() const
{
    return m_truePeak;
}
