#ifndef TIMEPOS_H
#define TIMEPOS_H
#include "audio_global.h"

class TimeSig {
public:
    TimeSig(int num, int denom);
    int numerator() const;
    int denominator() const;

private:
    int m_num;
    int m_denom;
};

class TimePos {
public:
    TimePos(const bar_t bar, const tick_t ticks);
    TimePos(const tick_t ticks = 0);

    TimePos quantize(float) const;
    TimePos toAbsoluteBar() const;

    TimePos& operator+=(const TimePos& time);
    TimePos& operator-=(const TimePos& time);

    // return the bar, rounded down and 0-based
    bar_t getBar() const;
    // return the bar, rounded up and 0-based
    bar_t nextFullBar() const;

    void setFirstTicks(tick_t ticks);
    tick_t getFirstTicks() const;

    void setLastTicks(tick_t ticks);
    tick_t getlastTicks() const;

    void setTicks(tick_t ticks);
    tick_t getTicks() const;

    operator double() const;

    tick_t ticksPerBeat(const TimeSig& sig) const;
    // Remainder ticks after bar is removed
    tick_t getTickWithinBar(const TimeSig& sig) const;
    // Returns the beat position inside the bar, 0-based
    tick_t getBeatWithinBar(const TimeSig& sig) const;
    // Remainder ticks after bar and beat are removed
    tick_t getTickWithinBeat(const TimeSig& sig) const;

    // calculate number of frame that are needed this time
    f_cnt_t frames(const float framesPerTick) const;

    double getTimeInMilliseconds(bpm_t beatsPerMinute) const;

    static TimePos fromFrames(const f_cnt_t frames, const float framesPerTick);
    static tick_t ticksPerBar();
    static tick_t ticksPerBar(const TimeSig& sig);
    static int stepsPerBar();
    static void setTicksPerBar(tick_t tpt);
    static TimePos stepPosition(int step);
    static double ticksToMilliseconds(tick_t ticks, bpm_t beatsPerMinute);
    // static double ticksToMilliseconds(double ticks, bpm_t beatsPerMinute);

private:
    tick_t m_ticks;
    tick_t m_firsTicks;
    tick_t m_lastTicks;

    static tick_t s_ticksPerBar;
};

#endif // TIMEPOS_H
