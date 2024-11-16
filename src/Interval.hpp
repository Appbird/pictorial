# pragma once

struct Interval {
    double min;
    double max;
    double location(const double x) {
        if (max == min) { return 0; }
        return (x - min)/(max - min);
    }
};