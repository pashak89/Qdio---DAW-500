/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define FRUT_DSP_USE_FFTW 1

#include "average_level_filtered.h"
#include "frut/FrutHeader.h"
#include "meter_ballistics.h"
#include "plugin_parameters.h"
//==============================================================================
/**
*/
class TruePeakAudioProcessor : virtual public frut::audio::RingBufferProcessor<float> {
public:
    //==============================================================================
    TruePeakAudioProcessor();
    ~TruePeakAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate,
        int numberOfInputChannels,
        int estimatedSamplesPerBlock,
        int expectedRequestRate);

    void releaseResources();

    void processBlock(juce::AudioBuffer<float>& buffer);

    std::shared_ptr<MeterBallistics> getLevels();

    int getAverageAlgorithm();
    void setAverageAlgorithmFinal(const int averageAlgorithm);
    void setAverageAlgorithm(const int averageAlgorithm);

    void reset();

private:
    int countOverflows(const juce::AudioBuffer<float>& buffer,
        const int channel,
        const int numberOfSamples,
        const float limitOverflow) const;

    KmeterPluginParameters pluginParameters_;

    std::unique_ptr<frut::audio::RingBuffer<float>> ringBuffer_;
    std::unique_ptr<frut::audio::RingBuffer<double>> ringBufferDouble_;

    std::unique_ptr<AverageLevelFiltered> averageLevelFiltered_;
    std::unique_ptr<frut::dsp::TruePeakMeter> truePeakMeter_;
    std::shared_ptr<MeterBallistics> meterBallistics_;

    const int kmeterBufferSize_;

    int _numInputChannels;
    bool isStereo_;
    bool sampleRateIsValid_;
    bool isSilent_;
    bool hasStopped_;

    int averageAlgorithmId_;
    float processedSeconds_;

    double attenuationDecibel_;
    double currentAttenuationDecibel_;

    double outputGain_;
    double outputFadeRate_;

    double _sampleRate;

    juce::Array<float> peakLevels_;
    juce::Array<float> rmsLevels_;
    juce::Array<float> averageLevelsFiltered_;
    juce::Array<float> truePeakLevels_;

    juce::Array<int> overflowCounts_;

    frut::dsp::Dither dither_;

    // Inherited via RingBufferProcessor
    virtual bool processBufferChunk(AudioBuffer<float>& buffer) override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TruePeakAudioProcessor)
};
