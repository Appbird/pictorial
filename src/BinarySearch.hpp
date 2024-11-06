#pragma once
#include <Siv3D.hpp>
#include "KeyFrame.hpp"
#include "Segment.hpp"

struct BinarySearch {
    void draw(const double t) {
        Line{{-0.5, 0}, {0.5, 0}}.draw(0.005, ColorF{42, 0.34, 100});
    }
    void draw_bloom(const double t) {

    }
};