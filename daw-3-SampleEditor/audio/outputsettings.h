#ifndef OUTPUTSETTINGS_H
#define OUTPUTSETTINGS_H
#include "audio_global.h"
#include <QDebug>
class OutputSettings {
public:
    enum BitDepth {
        Depth_16Bit = 16,
        Depth_24Bit = 24,
        Depth_32Bit = 32,
    };

    enum StereoMode {
        StereoMode_Stereo,
        StereoMode_Mono
    };

    class BitRateSettings {
    public:
        BitRateSettings(bitrate_t bitRate, bool isVariableBitRate)
            : m_bitRate(bitRate)
            , m_isVariableBitRate(isVariableBitRate)
        {
        }

        bool isVariableBitRate() const { return m_isVariableBitRate; }
        void setVariableBitrate(bool variableBitRate = true) { m_isVariableBitRate = variableBitRate; }

        bitrate_t getBitRate() const { return m_bitRate; }
        void setBitRate(bitrate_t bitRate) { m_bitRate = bitRate; }

    private:
        bitrate_t m_bitRate;
        bool m_isVariableBitRate;
    };

public:
    OutputSettings(sample_rate_t sampleRate,
        BitRateSettings const& bitRateSettings,
        BitDepth bitDepth,
        StereoMode stereoMode)
        : m_sampleRate(sampleRate)
        , m_bitRateSettings(bitRateSettings)
        , m_bitDepth(bitDepth)
        , m_stereoMode(stereoMode)
        , m_compressionLevel(0.625) // 5/8
    {
    }

    OutputSettings(sample_rate_t sampleRate,
        BitRateSettings const& bitRateSettings,
        BitDepth bitDepth)
        : OutputSettings(sampleRate, bitRateSettings, bitDepth, StereoMode_Stereo)
    {
    }

    sample_rate_t getSampleRate() const { return m_sampleRate; }
    void setSampleRate(sample_rate_t sampleRate) { m_sampleRate = sampleRate; }

    BitRateSettings const& getBitRateSettings() const { return m_bitRateSettings; }
    void setBitRateSettings(BitRateSettings const& bitRateSettings) { m_bitRateSettings = bitRateSettings; }

    BitDepth getBitDepth() const { return m_bitDepth; }
    void setBitDepth(BitDepth bitDepth) { m_bitDepth = bitDepth; }

    StereoMode getStereoMode() const { return m_stereoMode; }
    void setStereoMode(StereoMode stereoMode) { m_stereoMode = stereoMode; }

    double getCompressionLevel() const { return m_compressionLevel; }
    void setCompressionLevel(double level)
    {
        // legal range is 0.0 to 1.0.
        m_compressionLevel = level;
    }

private:
    sample_rate_t m_sampleRate;
    BitRateSettings m_bitRateSettings;
    BitDepth m_bitDepth;
    StereoMode m_stereoMode;
    double m_compressionLevel;
};

#endif // OUTPUTSETTINGS_H
