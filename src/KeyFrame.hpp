#pragma once
#include <Siv3D.hpp>
#include "utility.hpp"

using Interpolator = std::function<double(double, double, double)>;

struct KeyAnimation {
    Array<double> time;
    Array<double> frames;
    Interpolator interpolator;
    KeyAnimation(const Interpolator& itplt) : interpolator(itplt) {}
    KeyAnimation(const Interpolator& itplt, const Array<double>& t, const Array<double>& frame) :
        interpolator(itplt),
        time(t),
        frames(frame)
    {}
    // @return time in [0, 1]
    double refer(const double t) const {
        int64_t cei = -1;
        for (int64_t i = 0; i < time.size(); i++) {
            if (time[i] <= t) { cei = i; }
        }
        const int64_t current_idx = Clamp(cei, 0ll, int64_t(time.size() - 1));
        const int64_t next_idx    = Clamp(cei + 1, 0ll, int64_t(time.size() - 1));
        const double t1 = time_01(t, time[current_idx], time[next_idx] - time[current_idx]);
        assert(0 <= t1 and t1 <= 1);
        return interpolator(frames[current_idx], frames[next_idx], t1);
    }
    bool in_range(const double t) const { return InRange(t, time.front(), time.back()); }
    double refer(const double t, const double t_start) const {
        return refer(t - t_start);
    }
    void register_frames(const Array<double>&& t, const Array<double>&& frame) {
        time = t;
        frames = frame;
        assert(time.size() == frames.size());
    }
};

template<double (* easing)(double)>
double ease_iplt(double current, double next, double t) {
    return std::lerp(current, next, easing(t));
}

double binary_iplt(double current, double next, double t) {
    return (t > 1 - 1e-6) ? next : current;
};
