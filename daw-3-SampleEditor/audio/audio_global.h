#ifndef AUDIO_GLOBAL_H
#define AUDIO_GLOBAL_H

//#include "sampleframe.h"

#define JUCE_STANDALONE_APPLICATION 1
#include <JuceLibraryCode/JuceHeader.h>

#include <QtGlobal>

using sampleFrame = juce::AudioBuffer<float>;
using fpp_t = int32_t; // frames per period (0-16384)
using sample_rate_t = double; // sample-rate
using ch_cnt_t = uint8_t; // channel-count (0-SURROUND_CHANNELS)
using f_cnt_t = int64_t; // standard frame-count
using mix_ch_t = uint16_t; // Mixer-channel (0 to MAX_CHANNEL)
using tick_t = double;
using bar_t = int32_t;
using bpm_t = uint16_t; // tempo (MIN_BPM to MAX_BPM)
using panning_t = int8_t;
using jo_id_t = uint32_t; // (unique) ID of a journalling object
using bitrate_t = uint16_t; // bitrate in kbps
using int_sample_t = int16_t; // 16-bit-int-sample
using sample_t = float; // standard sample-type

const int DefaultTicksPerBar = 192;
const int DefaultStepsPerBar = 16;
const int DefaultBeatsPerBar = DefaultTicksPerBar / DefaultStepsPerBar;
const bpm_t DefaultTempo = 120;
const fpp_t MINIMUM_BUFFER_SIZE = 32;
const fpp_t DEFAULT_BUFFER_SIZE = 256;

const fpp_t MINIMUM_FRAME_COUNT = 65536;
// const fpp_t BUFFER_COUNET = 5;

inline bool isLittleEndian()
{
    return (QSysInfo::ByteOrder == QSysInfo::LittleEndian);
}

#ifdef __SSE__
#include <immintrin.h>
#ifdef __GNUC__
#include <x86intrin.h>
#endif // __GNUC__
#endif // __SSE__

#ifdef __SSE__

// Intel® 64 and IA-32 Architectures Software Developer’s Manual,
// Volume 1: Basic Architecture,
// 11.6.3 Checking for the DAZ Flag in the MXCSR Register
int inline can_we_daz()
{
    alignas(16) unsigned char buffer[512] = { 0 };
#if defined(LMMS_HOST_X86)
    _fxsave(buffer);
#elif defined(LMMS_HOST_X86_64)
    _fxsave64(buffer);
#endif // defined(LLMS_HOST_X86)
    // Bit 6 of the MXCSR_MASK, i.e. in the lowest byte,
    // tells if we can use the DAZ flag.
    return ((buffer[28] & (1 << 6)) != 0);
}

#endif // __SSE__

// Set denormal protection for this thread.
void inline disable_denormals()
{
#ifdef __SSE__
    /* Setting DAZ might freeze systems not supporting it */
    if (can_we_daz()) {
        _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
    }
    /* FTZ flag */
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif // __SSE__
}

#endif // AUDIO_GLOBAL_H
