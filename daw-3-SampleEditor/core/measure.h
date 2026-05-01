#ifndef MEASURE_H
#define MEASURE_H
#include <QDebug>
#include <QtMath>
struct Measure {

    Measure() { }
    Measure(int bar, int beat, int sixteenth, double subSixteenth, double barDuration, double beatDuration, double sixDuration)
    {
        this->bar = bar;
        this->beat = beat;
        this->sixteenth = sixteenth;
        this->subSixteenth = subSixteenth;

        this->barDuration = barDuration;
        this->beatDuration = beatDuration;
        this->sixteenthDuration = sixDuration;
    }

    int bar = -1;
    int beat = -1;
    int sixteenth = -1;
    double subSixteenth = -1;

    double barDuration;
    double beatDuration;
    double sixteenthDuration;
    double subSixteenDuration;

public:
    double getBarDuration() const
    {
        return barDuration;
    }
    double getBeatDuration() const
    {
        return beatDuration;
    }
    double getSixteenthDuration() const
    {
        return sixteenthDuration;
    }

    static double measureToTime(int bar, int beat, int sixteenth, double subSixteenth,
        int _lenghtOfBeat, int _beatPerBar, float _tempo)
    {
        const double beatDuration = (60.0 / _tempo) * (4.0 / (double)_lenghtOfBeat); // seconds
        const double barDuration = beatDuration * (double)_beatPerBar; // seconds
        const double sixteenthDuration = beatDuration / (16.0 / (double)_lenghtOfBeat); // seconds
        const double tickDuration = sixteenthDuration / 2048.0; // seconds

        const double tSec = (double)(bar - 1) * barDuration + (double)(beat - 1) * beatDuration + (double)(sixteenth - 1) * sixteenthDuration + subSixteenth * tickDuration;

        return tSec * 1000.0; // milliseconds
    }

    static Measure timeToMeasure(double timeMs, int _lenghtOfBeat, int _beatPerBar, float _tempo)
    {
        const bool isNegative = timeMs < 0.0;
        double tSec = qAbs(timeMs) / 1000.0;

        const double beatDuration = (60.0 / _tempo) * (4.0 / (double)_lenghtOfBeat); // seconds
        const double barDuration = beatDuration * (double)_beatPerBar; // seconds
        const double sixteenthDuration = beatDuration / (16.0 / (double)_lenghtOfBeat); // seconds
        const double tickDuration = sixteenthDuration / 2048.0; // seconds

        auto snapDown = [](double x) {
            // protects against values like 0.99999999999996 from fp error
            const double eps = 1e-12;
            return (x < 0.0) ? 0.0 : (x + eps);
        };

        tSec = snapDown(tSec);

        // 0-based indices
        long long barIdx = (long long)std::floor(tSec / barDuration);
        tSec -= (double)barIdx * barDuration;

        long long beatIdx = (long long)std::floor(tSec / beatDuration);
        tSec -= (double)beatIdx * beatDuration;

        long long sixIdx = (long long)std::floor(tSec / sixteenthDuration);
        tSec -= (double)sixIdx * sixteenthDuration;

        // sub in [0..2047] ideally
        long long sub = (long long)std::llround(tSec / tickDuration);

        // carry / normalize (handles rounding to boundary)
        if (sub >= 2048) {
            sub = 0;
            sixIdx += 1;
        }
        const long long sixPerBeat = (long long)std::llround(beatDuration / sixteenthDuration); // typically 4
        if (sixIdx >= sixPerBeat) {
            sixIdx = 0;
            beatIdx += 1;
        }
        if (beatIdx >= _beatPerBar) {
            beatIdx = 0;
            barIdx += 1;
        }

        // Convert to 1-based
        int bar = (int)barIdx + 1;
        int beat = (int)beatIdx + 1;
        int sixteenth = (int)sixIdx + 1;

        // Apply sign consistently (so negative time maps to negative measure positions)
        if (isNegative) {
            bar = -bar;
            beat = -beat;
            sixteenth = -sixteenth;
        }

        return Measure(bar, beat, sixteenth, (double)sub, barDuration, beatDuration, sixteenthDuration);
    }

    static Measure timeToMeasure2(double time, int _lenghtOfBeat, int _beatPerBar, float _tempo)
    {
        bool isNegetive = time < 0 ? true : false;
        time = qAbs(time);

        double _beatDuration = 60.0 / _tempo * (4 / (double)_lenghtOfBeat);

        double _barDuration = _beatDuration * _beatPerBar;

        double _sixteenthDuration = _beatDuration / (16 / _lenghtOfBeat);

        double measurePerMin = (time / 1000.0) / _barDuration;

        double real = measurePerMin - floor(measurePerMin);

        int bar = isNegetive ? floor(measurePerMin) : floor(measurePerMin) + 1;

        if (real > 0.9999999 || real < 0.0000001) {
            bar = isNegetive ? qRound(measurePerMin) : qRound(measurePerMin) + 1;
            measurePerMin = qRound(measurePerMin);
        }

        measurePerMin = measurePerMin - floor(measurePerMin);

        measurePerMin = _barDuration * measurePerMin / _beatDuration;

        measurePerMin = qRound(measurePerMin * 10000) / 10000.0;

        int beat = floor(measurePerMin) + 1;
        if (real > 0.9999999 || real < 0.0000001) {
            beat = qRound(measurePerMin) + 1;
            measurePerMin = qRound(measurePerMin);
        }

        measurePerMin = measurePerMin - floor(measurePerMin);

        measurePerMin = measurePerMin * _beatDuration / _sixteenthDuration;

        measurePerMin = qRound(measurePerMin * 10000) / 10000.0;

        int sixteenth = floor(measurePerMin) + 1;
        if (real > 0.9999999 || real < 0.0000001) {
            sixteenth = qRound(measurePerMin) + 1;
            measurePerMin = qRound(measurePerMin);
        }

        measurePerMin = measurePerMin - floor(measurePerMin);

        measurePerMin = qRound(measurePerMin * 10000) / 10000.0;

        double sub = (measurePerMin * _sixteenthDuration) / (_sixteenthDuration / 2048.0);

        return Measure(isNegetive ? -bar : bar, beat, sixteenth, sub, _barDuration, _beatDuration, _sixteenthDuration);
    }

    static Measure timeToMeasure3(double time, int _lenghtOfBeat, int _beatPerBar, float _tempo)
    {
        bool isNegetive = time < 0 ? true : false;
        time = qAbs(time);

        double _beatDuration = 60.0 / _tempo * (4 / (double)_lenghtOfBeat);

        double _barDuration = _beatDuration * _beatPerBar;

        double _sixteenthDuration = _beatDuration / (16 / _lenghtOfBeat);

        double measurePerMin = (time / 1000.0) / _barDuration;

        double real = measurePerMin - floor(measurePerMin);

        int bar = isNegetive ? floor(measurePerMin) : floor(measurePerMin);

        if (real > 0.9999999 || real < 0.0000001) {
            bar = isNegetive ? qRound(measurePerMin) : qRound(measurePerMin);
            measurePerMin = qRound(measurePerMin);
        }

        measurePerMin = measurePerMin - floor(measurePerMin);

        measurePerMin = _barDuration * measurePerMin / _beatDuration;

        measurePerMin = qRound(measurePerMin * 10000) / 10000.0;

        int beat = floor(measurePerMin);
        if (real > 0.9999999 || real < 0.0000001) {
            beat = qRound(measurePerMin);
            measurePerMin = qRound(measurePerMin);
        }

        measurePerMin = measurePerMin - floor(measurePerMin);

        measurePerMin = measurePerMin * _beatDuration / _sixteenthDuration;

        measurePerMin = qRound(measurePerMin * 10000) / 10000.0;

        int sixteenth = floor(measurePerMin);
        if (real > 0.9999999 || real < 0.0000001) {
            sixteenth = qRound(measurePerMin);
            measurePerMin = qRound(measurePerMin);
        }

        measurePerMin = measurePerMin - floor(measurePerMin);

        measurePerMin = qRound(measurePerMin * 10000) / 10000.0;

        double sub = (measurePerMin * _sixteenthDuration) / (_sixteenthDuration / 2048.0);

        return Measure(isNegetive ? -bar : bar, beat, sixteenth, sub, _barDuration, _beatDuration, _sixteenthDuration);
    }
};

#endif // MEASURE_H
