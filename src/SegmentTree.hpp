#pragma once
#include <Siv3D.hpp>
#include "utility.hpp"

struct Segment {
	RectF rect;
	ColorF c;
	void draw(const double t) {
		RoundRect{rect, 0.005}.draw(c);
	}
    Segment DevideSegment(
        const double t1, const double t2
    ) {
        Segment segment = *this;
        const double x_min = segment.rect.leftX();
        const double x_max = segment.rect.rightX();
        const double x = std::lerp(x_min, x_max, t1);
        const double y = segment.rect.topY();
        const double w = std::lerp(x_min, x_max, t2) - x;
        const double h = segment.rect.h;
        segment.rect = {{x, y}, {w, h}};
        return segment;
    }
    Segment LocateSegment(
        const double x1, const double x2, const double y,
        const double width
    ) {
        Segment segment = *this;
        segment.rect = RectF{x1, y - width/2, x2 - x1, width};
        return segment;
    }
    Segment GoDown(
        const double t, const double amp
    ) {
        Segment segment = *this;
        segment.rect.moveBy({0, amp*Clamp(t, 0., 1.)});
        return segment;
    }

};



struct SegmentTree {
	Mat3x2 affine;
	SegmentTree() {
		const Mat3x2 translate = Mat3x2::Translate({0, 0.216});
		const Mat3x2 rotate1 = Mat3x2::Rotate(Math::Pi / 5);
		affine = translate * rotate1;
	}
	void draw(const double t) {
		
	}	
};
