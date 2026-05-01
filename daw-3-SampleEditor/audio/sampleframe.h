//#ifndef SAMPLEFRAME_H
//#define SAMPLEFRAME_H

//#include <QDebug>
//#include <QPair>
//#include <Qt>
//#include <core/mathhelper.h>
//#include <cstdlib>
//#include <iostream>
//#include <vector>

//class SampleFrame {
//public:
//    SampleFrame(int channels, int frames, int sampleRate = 44100)
//        : _channels(channels)
//        , _frames(frames)
//        , _sampleRate(sampleRate)
//    {

//        _data = (float**)malloc(_channels * sizeof(*_data));

//        for (int i = 0; i < channels; i++) {
//            _data[i] = (float*)malloc(frames * sizeof(float));
//            memset(_data[i], 0, frames * sizeof(float));
//        }
//    }

//    float getMagnitude(int channel, int startSample, int numSamples) const noexcept
//    {
//        assert(isPositiveAndBelow(channel, channels()));
//        assert(startSample >= 0 && numSamples >= 0 && startSample + numSamples <= size());

//        auto r = findMinMax(channel, startSample, numSamples);

//        return jmax(r.first, -r.first, r.second, -r.second);
//    }
//    float getRMSLevel(int channel, int startSample, int numSamples) const noexcept
//    {
//        assert(isPositiveAndBelow(channel, channels()));
//        assert(startSample >= 0 && numSamples >= 0 && startSample + numSamples <= size());

//        if (numSamples <= 0 || channel < 0 || channel >= channels())
//            return float(0);

//        auto* data_ = data(channel) + startSample;
//        double sum = 0.0;

//        for (int i = 0; i < numSamples; ++i) {
//            auto sample = data_[i];
//            sum += sample * sample;
//        }

//        return static_cast<float>(std::sqrt(sum / numSamples));
//    }

//    QPair<float, float> findMinMax(int channel, int startSample, int numSamples) const noexcept
//    {
//        assert(isPositiveAndBelow(channel, channels()));
//        assert(startSample >= 0 && numSamples >= 0 && startSample + numSamples <= size());

//        return MathHelper1::findMinAndMax(_data[channel] + startSample, numSamples);
//    }

//    float magnitude(int channel, int startSample, int numSamples) const noexcept
//    {
//        assert(isPositiveAndBelow(channel, channels()));
//        assert(startSample >= 0 && numSamples >= 0 && startSample + numSamples <= size());

//        auto r = findMinMax(channel, startSample, numSamples);

//        return jmax(r.first, -r.first, r.second, -r.second);
//    }
//    size_t size() const
//    {
//        return _frames;
//    }
//    int getNumSamples() { return _frames; }
//    ~SampleFrame()
//    {
//        for (int i = 0; i < _channels; i++) {
//            free(_data[i]);
//        }
//        free(_data);
//    }

//    void set(int channel, int frame, float value)
//    {
//        _data[channel][frame] = value;
//        // data_[frame * _channels + channel] = value;
//    }

//    float get(int channel, int frame) const
//    {
//        return _data[channel][frame];
//    }

//    float& operator()(int channel, int frame)
//    {
//        return _data[channel][frame];
//    }

//    const float& operator()(int channel, int frame) const
//    {
//        return _data[channel][frame];
//    }

//    int channels() const
//    {
//        return _channels;
//    }

//    int frames() const
//    {
//        return _frames;
//    }

//    void clear()
//    {
//        for (int i = 0; i < _channels; i++) {

//            memset(_data[i], 0, _frames * sizeof(float));
//        }
//    }

//    void clear(int startSample, int numSamples)
//    {
//        assert(startSample >= 0 && numSamples >= 0 && startSample + numSamples <= size());

//        for (int i = 0; i < channels(); ++i)
//            memset(_data[i] + startSample, 0, numSamples * sizeof(float));
//    }

//    void copyTo(SampleFrame* other, int offset = 0)
//    {

//        for (int i = 0; i < _channels; i++) {
//            memcpy(other->data(i), data(i) + offset, (size_t)(other->frames() * sizeof(float)));
//        }
//    }
//    void copyTo(SampleFrame* other, int channel, int offset = 0)
//    {
//        memcpy(other->data(channel), data(channel) + offset, (size_t)(other->frames() * sizeof(float)));
//    }

//    void copyFrom(int destChannel,
//        int destStartSample,
//        const SampleFrame* other,
//        int sourceChannel,
//        int sourceStartSample,
//        int numSamples)
//    {

//        if (numSamples > 0) {
//            memcpy(data(destChannel) + destStartSample, other->data(sourceChannel) + sourceStartSample, (size_t)(numSamples * sizeof(float)));
//        }
//    }
//    void copyFrom(int destChannel,
//        int destStartSample, float* other,
//        int numSamples)
//    {

//        if (numSamples > 0) {

//            auto* d = data(destChannel) + destStartSample;
//            memcpy(d, other, (size_t)(numSamples * sizeof(float)));
//        }
//    }

//    void addFrom(int destChannel,
//        int destStartSample, SampleFrame* other,
//        int sourceChannel,
//        int sourceStartSample,
//        int numSamples)
//    {

//        if (numSamples > 0 && !other->isClear) {

//            auto* d = data(destChannel) + destStartSample;
//            auto* s = other->data(sourceChannel) + sourceStartSample;

//            memcpy(d, s, (size_t)(numSamples * sizeof(float)));
//        }
//    }

//    float* data(int channel) const
//    {
//        return _data[channel];
//    }
//    float** data() const
//    {
//        return _data;
//    }

//    int sampleRate() const
//    {
//        return _sampleRate;
//    }

//private:
//    std::atomic<bool> isClear { false };

//    int _channels;
//    int _frames;
//    int _sampleRate = 44100;
//    float** _data;
//};

//#endif // SAMPLEFRAME_H
