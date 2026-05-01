#ifndef JUCE_H
#define JUCE_H

#define JUCE_PLUGINHOST_VST 1
#define JUCE_PLUGINHOST_VST3 1
#define JUCE_PLUGINHOST_AU 1
#define JUCE_PLUGINHOST_LADSPA 1
#include "JuceLibraryCode/JuceHeader.h"
#include "Klangfreund/Ebu128LoudnessMeter.h"

#include <QDebug>
#include <QObject>
#include <QScopedPointer>
#include <QTimer>
#include <QUrl>
#include <qdatetime.h>

#include <kmeter/kmeter.h>

class Q_DECL_EXPORT JuceEngine : public QObject, public ChangeListener {
    Q_OBJECT

public:
    JuceEngine();
    ~JuceEngine();

    //! \note Callback for AudioTransportSource
    void changeListenerCallback(ChangeBroadcaster* source) override;

    //! \note Set raw audio file data or another audio data in MP3 format
    void setAudioSource(QString url);
    int getNumberOfChannel();

    void stop();
    void play();

    double getCurrentPosition() const;
    double getTotalTime() const;

    float getIntegratedLoudness();
    float getMaximumMomentaryLoudness();
    float getMaximumShortTermLoudness();
    float getLoudnessRange();
    float getMeasurementDuration();
    float getMomentaryLoudness();
    float getShortTermLoudness();
    float getLoudnessRangeStart();
    float getLoudnessRangeEnd();

    float getPeakMeterLevel(int channel);
    float getMaximumPeakLevel(int channel);
    float getPhaseCorrelation();
    float getStereoMeterValue();
    float getAverageMeterLevel(int channel);
    float getNumberOfOverflows(int channel);
    float getPeakMeterPeakLevel(int channel);
    float getTruePeakMeterLevel(int channel);
    float getMaximumTruePeakLevel(int channel);
    float getAverageMeterPeakLevel(int channel);
    float getTruePeakMeterPeakLevel(int channel);
    float getMeterMinimumDecibel();

    void setActiveRms();
    void setActiveItu();

    void sltTimeout();
    Ebu128LoudnessMeter* getEbu128() const;

    TruePeakAudioProcessor* getTruePeak() const;

signals:
    void startPlaying();
    void stopPlaying();

private:
    AudioSourcePlayer audioSourcePlayer;
    AudioTransportSource transportSource;
    std::unique_ptr<AudioFormatReaderSource> currentAudioFileSource;
    AudioDeviceManager audioDeviceManager;
    AudioFormatManager formatManager;

    AudioFormatReader* reader_analyse = nullptr;

    AudioSampleBuffer buffer;
    Ebu128LoudnessMeter* m_ebu128;
    TruePeakAudioProcessor* m_truePeak;
    QTimer timer_analyse;
    qint64 m_bufferPosition = 0;
    qint64 m_playPosition = 0;
};

#endif // JUCE_H
