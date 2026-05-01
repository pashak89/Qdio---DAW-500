#ifndef MICROTIMER_H
#define MICROTIMER_H
#include <chrono>

class MicroTimer {
    using time_point = std::chrono::steady_clock::time_point;

public:
    MicroTimer();
    ~MicroTimer() = default;

    void reset();
    int elapsed() const;

private:
    time_point begin;
};

#endif // MICROTIMER_H
