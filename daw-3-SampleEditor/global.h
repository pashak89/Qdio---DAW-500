#ifndef GLOBAL_H
#define GLOBAL_H
#include <Qt>
typedef int16_t fpp_t; // frames per period (0-16384)
typedef int32_t f_cnt_t; // standard frame-count
typedef uint32_t sample_rate_t; // sample-rate
typedef float sample_t; // standard sample-type
typedef uint8_t ch_cnt_t; // channel-count (0-SURROUND_CHANNELS)
typedef uint16_t fx_ch_t; // FX-channel (0 to MAX_EFFECT_CHANNEL)
typedef int32_t bar_t;
typedef int32_t tick_t;
typedef uint8_t volume_t;
typedef int8_t panning_t;

typedef uint16_t bpm_t; // tempo (MIN_BPM to MAX_BPM)
#endif // GLOBAL_H
