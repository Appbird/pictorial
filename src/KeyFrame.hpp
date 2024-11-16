#pragma once
#include <Siv3D.hpp>
#include "utility.hpp"

template<typename T>
using Interpolator = std::function<double(T, T, double)>;
template<typename T>
struct KeyAnimatorShifted;

template<typename T>
struct KeyAnimator {
    struct Frame {
        double time;
        T x;
        Interpolator<T> f;
    };
    Array<Frame> frames;
    KeyAnimator(Array<Frame> frames_): frames(frames_) {}
    T refer(double t) const {
        int64_t cei = -1;
        for (int64_t i = 0; i < frames.size(); i++) {
            if (frames[i].time <= t) { cei = i; }
        }
        const int64_t current_idx = Clamp(cei, 0ll, int64_t(frames.size() - 1));
        const int64_t next_idx    = Clamp(cei + 1, 0ll, int64_t(frames.size() - 1));
        const double t1 = MapInto01(t, frames[current_idx].time, frames[next_idx].time);
        assert(InRange(t1, 0., 1.));
        return frames[current_idx].f(frames[current_idx].x, frames[next_idx].x, t1);
    }
    KeyAnimatorShifted<T> shift(double t) const;
};

template <typename T>
struct KeyAnimatorShifted {
    const KeyAnimator<T>& animator;
    double time_shift;
    T refer(const double t) {
        return animator.refer(t - time_shift);
    }
    KeyAnimatorShifted<T> shift(const double shift) const {
        return {animator, time_shift + shift};
    }
};

template <typename T>
KeyAnimatorShifted<T> KeyAnimator<T>::shift(double s) const {
    return KeyAnimatorShifted<T>{*this, s};
}


/// @deprecated TODO: KeyAnimatorに置き換えたい
struct KeyAnimation {
    Array<double> time;
    Array<double> frames;
    Interpolator<double> interpolator;
    KeyAnimation(const Interpolator<double>& itplt) : interpolator(itplt) {}
    KeyAnimation(const Interpolator<double>& itplt, const Array<double>& t, const Array<double>& frame) :
        time(t),
        frames(frame),
        interpolator(itplt)
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

template<double (*easing)(double)>
ColorF ease_color_iplt(ColorF current, ColorF next, double t) {
    return current.lerp(next, easing(t));
}

template<typename T>
T binary_iplt(T current, T next, double t) {
    return (t > 1 - 1e-6) ? next : current;
};

void match(double t, std::pair<double, double> time, std::function<void(double, double, double)> animator) {
    auto [start, end] = time;
    if (InRange(t, start, end)) { animator(t, start, end); }
}