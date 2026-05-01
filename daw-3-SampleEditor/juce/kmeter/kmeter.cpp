
#include "kmeter.h"

//==============================================================================
TruePeakAudioProcessor::TruePeakAudioProcessor()
    : kmeterBufferSize_(1024)
{

    meterBallistics_ = nullptr;
    averageLevelFiltered_ = nullptr;
    truePeakMeter_ = nullptr;

    ringBuffer_ = nullptr;
    ringBufferDouble_ = nullptr;

    sampleRateIsValid_ = false;
    isStereo_ = true;
    isSilent_ = false;
    hasStopped_ = true;

    attenuationDecibel_ = 0.0;
    currentAttenuationDecibel_ = attenuationDecibel_;

    outputGain_ = 1.0;
    outputFadeRate_ = 0.0;

    averageAlgorithmId_ = KmeterPluginParameters::selAlgorithmItuBs1770;

    processedSeconds_ = 0.0f;
}

TruePeakAudioProcessor::~TruePeakAudioProcessor()
{
}

void TruePeakAudioProcessor::prepareToPlay(double sampleRate, int numberOfInputChannels,
    int estimatedSamplesPerBlock, int expectedRequestRate)
{

    _sampleRate = sampleRate;
    sampleRateIsValid_ = true;
    isSilent_ = false;
    hasStopped_ = true;
    // force initialization of "outputGain_" in "processBlock()"
    currentAttenuationDecibel_ = attenuationDecibel_ + 1e-12;

    // output fade rate: 60 dB/s
    outputFadeRate_ = 60.0 / sampleRate;

    _numInputChannels = numberOfInputChannels;

    dither_.initialise(numberOfInputChannels, 24);

    isStereo_ = (_numInputChannels == 2);

    meterBallistics_ = std::make_shared<MeterBallistics>(
        _numInputChannels,
        averageAlgorithmId_,
        false,
        false);

    peakLevels_.clear();
    rmsLevels_.clear();
    averageLevelsFiltered_.clear();
    truePeakLevels_.clear();

    overflowCounts_.clear();

    for (int channel = 0; channel < _numInputChannels; ++channel) {
        peakLevels_.add(0.0f);
        rmsLevels_.add(0.0f);
        averageLevelsFiltered_.add(MeterBallistics::getMeterMinimumDecibel());
        truePeakLevels_.add(0.0f);

        overflowCounts_.add(0);
    }

    averageLevelFiltered_ = std::make_unique<AverageLevelFiltered>(
        _numInputChannels,
        (int)sampleRate,
        kmeterBufferSize_,
        averageAlgorithmId_);

    // maximum under-read of true peak measurement is 0.169 dB (see
    // Annex 2 of ITU-R BS.1770-4)
    int oversamplingFactor = 8;

    if (sampleRate >= 176400) {
        oversamplingFactor /= 4;
    } else if (sampleRate >= 88200) {
        oversamplingFactor /= 2;
    }

    truePeakMeter_ = std::make_unique<frut::dsp::TruePeakMeter>(
        _numInputChannels,
        kmeterBufferSize_,
        oversamplingFactor);

    // make sure that ring buffer can hold at least kmeterBufferSize_
    // samples and is large enough to receive a full block of audio
    int ringBufferSize = jmax(estimatedSamplesPerBlock, kmeterBufferSize_);

    int preDelay = kmeterBufferSize_;
    int chunkSize = kmeterBufferSize_;

    ringBuffer_ = std::make_unique<frut::audio::RingBuffer<float>>(
        _numInputChannels,
        ringBufferSize,
        preDelay,
        chunkSize);

    ringBuffer_->setCallbackClass(this);

    ringBufferDouble_ = std::make_unique<frut::audio::RingBuffer<double>>(
        _numInputChannels,
        ringBufferSize,
        preDelay,
        chunkSize);
}

void TruePeakAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer)
{

    // temporarily disable denormals
    ScopedNoDenormals noDenormals;

    int numberOfSamples = buffer.getNumSamples();

    // process two channels only
    if (isStereo_) {
        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);

        for (int sample = 0; sample < numberOfSamples; ++sample) {
            // flip stereo channels
            float oldleftChannel = leftChannel[sample];
            leftChannel[sample] = rightChannel[sample];
            rightChannel[sample] = oldleftChannel;
        }
    }

    // copy buffer to ring buffer (applies pre-delay)
    //
    // calls "processBufferChunk" each time chunkSize samples have
    // been added!
    ringBuffer_->addFrom(buffer, 0, numberOfSamples);

    // copy ring buffer back to buffer
    ringBuffer_->removeTo(buffer, 0, numberOfSamples);

    float** bufferSample = (float**)buffer.getArrayOfWritePointers();

    // fade to mute / dim
    for (int sample = 0; sample < numberOfSamples; ++sample) {
        // fade in
        if (currentAttenuationDecibel_ < attenuationDecibel_) {
            currentAttenuationDecibel_ += outputFadeRate_;

            if (currentAttenuationDecibel_ > attenuationDecibel_) {
                currentAttenuationDecibel_ = attenuationDecibel_;
            }

            if (currentAttenuationDecibel_ < 0.0) {
                outputGain_ = MeterBallistics::decibel2level_double(
                    currentAttenuationDecibel_);
            } else {
                outputGain_ = 1.0;
            }
        }
        // fade out
        else if (currentAttenuationDecibel_ > attenuationDecibel_) {
            currentAttenuationDecibel_ -= outputFadeRate_;

            if (currentAttenuationDecibel_ < attenuationDecibel_) {
                currentAttenuationDecibel_ = attenuationDecibel_;
            }

            if (currentAttenuationDecibel_ > -60.0) {
                outputGain_ = MeterBallistics::decibel2level_double(
                    currentAttenuationDecibel_);
            } else {
                outputGain_ = 0.0;
            }
        }

        if (outputGain_ != 1.0) {
            for (int channel = 0;
                 channel < _numInputChannels;
                 ++channel) {
                // apply fade to sample
                double fadedSample = outputGain_ * static_cast<double>(bufferSample[channel][sample]);

                // dither faded sample and store in buffer
                bufferSample[channel][sample] = dither_.ditherSample(
                    channel, fadedSample);
            }
        }
    }
}

std::shared_ptr<MeterBallistics> TruePeakAudioProcessor::getLevels()
{
    return meterBallistics_;
}

int TruePeakAudioProcessor::getAverageAlgorithm()
{
    return averageAlgorithmId_;
}

void TruePeakAudioProcessor::setAverageAlgorithm(
    const int averageAlgorithm)
{
    if (averageAlgorithm != averageAlgorithmId_) {
        if (averageLevelFiltered_ != nullptr) {
            averageLevelFiltered_->setAlgorithm(averageAlgorithm);
            setAverageAlgorithmFinal(averageLevelFiltered_->getAlgorithm());
        } else {
            averageAlgorithmId_ = averageAlgorithm;
        }
    }
}

void TruePeakAudioProcessor::reset()
{
    hasStopped_ = true;
    processedSeconds_ = 0.0f;

    ringBuffer_->clear();

    averageLevelFiltered_->reset();
    truePeakMeter_->reset();

    if (meterBallistics_) {
        meterBallistics_->reset();
    }
}

void TruePeakAudioProcessor::setAverageAlgorithmFinal(
    const int averageAlgorithm)
{
    averageAlgorithmId_ = averageAlgorithm;
    meterBallistics_->setAverageAlgorithm(averageAlgorithmId_);

    //  the level averaging alghorithm has been changed, so update the
    // "RMS" and "ITU-R" buttons to make sure that the correct button
    // is lit
    //
    // "AC" ==> algorithm changed
}

int TruePeakAudioProcessor::countOverflows(const juce::AudioBuffer<float>& buffer, const int channel, const int numberOfSamples, const float limitOverflow) const
{
    jassert(isPositiveAndBelow(channel, buffer.getNumChannels()));
    jassert(isPositiveAndNotGreaterThan(numberOfSamples,
        buffer.getNumSamples()));

    const float* audioData = buffer.getReadPointer(channel);
    int overflows = 0;

    for (int sample = 0; sample < numberOfSamples; ++sample) {
        // get sample value
        float amplitude = audioData[sample];

        // convert sample value to amplitude
        if (amplitude < 0.0f) {
            amplitude = -amplitude;
        }

        // an overflow has occurred if the amplitude lies above the
        // overflow limit
        if (amplitude > limitOverflow) {
            ++overflows;
        }
    }

    return overflows;
}

bool TruePeakAudioProcessor::processBufferChunk(AudioBuffer<float>& buffer)
{
    int chunkSize = buffer.getNumSamples();
    bool isMono = false;

    // length of buffer chunk in fractional seconds
    // (1024 samples / 44100 samples/s = 23.2 ms)
    processedSeconds_ = static_cast<float>(chunkSize) / static_cast<float>(_sampleRate);

    // copy buffer to determine average level
    averageLevelFiltered_->copyFrom(buffer, chunkSize);

    // copy buffer to determine true peak level
    truePeakMeter_->copyFrom(buffer, chunkSize);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        if (isMono && (channel == 1)) {
            peakLevels_.set(channel, peakLevels_[0]);
            rmsLevels_.set(channel, rmsLevels_[0]);
            averageLevelsFiltered_.set(channel, averageLevelsFiltered_[0]);
            truePeakLevels_.set(channel, truePeakLevels_[0]);

            overflowCounts_.set(channel, overflowCounts_[0]);
        } else {

            // determine peak level for chunkSize samples
            peakLevels_.set(
                channel,
                buffer.getMagnitude(channel, 0, chunkSize));

            // determine RMS level for chunkSize samples
            rmsLevels_.set(
                channel,
                buffer.getRMSLevel(channel, 0, chunkSize));

            // determine filtered average level for chunkSize samples
            // (please note that this level has already been converted
            // to decibels!)
            averageLevelsFiltered_.set(
                channel,
                averageLevelFiltered_->getLevel(channel));

            // determine true peak level for chunkSize samples
            truePeakLevels_.set(
                channel,
                truePeakMeter_->getLevel(channel));

            // determine overflows for chunkSize samples; treat all
            // samples above -0.001 dBFS as overflow
            //
            // in the 16-bit domain, full scale corresponds to an
            // absolute integer value of 32'767 or 32'768, so we'll
            // treat absolute levels of 32'767 and above as overflows;
            // this corresponds to a floating-point level of 32'767 /
            // 32'768 = 0.9999694 (approx. -0.001 dBFS).
            overflowCounts_.set(
                channel,
                countOverflows(buffer, channel, chunkSize, 0.9999f));
        }

        // apply meter ballistics and store values so that the editor
        // can access them
        meterBallistics_->updateChannel(channel,
            processedSeconds_,
            peakLevels_[channel],
            truePeakLevels_[channel],
            averageLevelsFiltered_[channel],
            overflowCounts_[channel]);
    }

    // phase correlation is only defined for stereo signals
    if (isStereo_) {
        float phaseCorrelation = 1.0f;

        // check whether the stereo signal has been mixed down to mono
        if (isMono) {
            phaseCorrelation = 1.0f;
        }
        // otherwise, process only RMS levels at or above -80 dB
        else if ((rmsLevels_[0] >= 0.0001f) || (rmsLevels_[1] >= 0.0001f)) {
            float sumOfProduct = 0.0f;
            float sumOfSquaresLeft = 0.0f;
            float sumOfSquaresRight = 0.0f;

            // determine correlation for chunkSize samples
            for (int sample = 0; sample < chunkSize; ++sample) {
                float leftChannel = buffer.getSample(0, sample);
                float rightChannel = buffer.getSample(1, sample);

                sumOfProduct += leftChannel * rightChannel;
                sumOfSquaresLeft += leftChannel * leftChannel;
                sumOfSquaresRight += rightChannel * rightChannel;
            }

            float sumsOfSquares = sumOfSquaresLeft * sumOfSquaresRight;

            // prevent division by zero and taking the square root of
            // a negative number
            if (sumsOfSquares > 0.0f) {
                phaseCorrelation = sumOfProduct / sqrtf(sumsOfSquares);
            } else {
                // this is mathematically incorrect, but "musically"
                // correct (i.e. signal is mono-compatible)
                phaseCorrelation = 1.0f;
            }
        }

        meterBallistics_->setPhaseCorrelation(processedSeconds_,
            phaseCorrelation);

        float stereoMeterValue = 0.0f;

        // do not process RMS levels below -80 dB
        if ((rmsLevels_[0] < 0.0001f) && (rmsLevels_[1] < 0.0001f)) {
            stereoMeterValue = 0.0f;
        } else if (rmsLevels_[1] >= rmsLevels_[0]) {
            stereoMeterValue = 1.0f - rmsLevels_[0] / rmsLevels_[1];
        } else {
            stereoMeterValue = rmsLevels_[1] / rmsLevels_[0] - 1.0f;
        }

        meterBallistics_->setStereoMeterValue(processedSeconds_,
            stereoMeterValue);
    }

    return false;
}
