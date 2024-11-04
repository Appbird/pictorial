#pragma once
#include <Siv3D.hpp>
#include "utility.hpp"

struct Segment {
	RectF rect;
	ColorF c;
	void draw() {
		RoundRect{rect, 0.005}.draw(c);
	}
    Segment DevideSegment(
        const double t1, const double t2
    ) const {
        Segment segment = *this;
        const double x_min = segment.rect.leftX();
        const double x_max = segment.rect.rightX();
        const double x = std::lerp(x_min, x_max, t1);
        const double y = segment.rect.topY();
        const double w = std::lerp(x_min, x_max, t2) - x;
        const double h = segment.rect.h;
        segment.rect = {x, y, w, h};
        return segment;
    }
	Segment ChangeColor(const ColorF& color) const {
		Segment segment = *this;
		segment.c = color;
		return segment;
	}
	Segment TakeMargin(const double margin) const {
		Segment segment = *this;
		const Vec2 tl = segment.rect.tl();
		const Vec2 br = segment.rect.br();
		const Vec2 delta = {margin/2, 0};
		segment.rect = RectF::FromPoints(tl + delta, br - delta);
		return segment;
	}
	Segment TakeMargin(const double left, const double right) const {
		Segment segment = *this;
		const Vec2 tl = segment.rect.tl();
		const Vec2 br = segment.rect.br();
		segment.rect = RectF::FromPoints(tl + Vec2{left/2, 0}, br - Vec2{right/2, 0});
		return segment;
	}
    Segment Locate(
        const double x1, const double x2, const double y,
        const double width
    ) {
        Segment segment = *this;
        segment.rect = RectF{x1, y - width/2, x2 - x1, width};
        return segment;
    }
	Segment SetBaseY(
        const double Y
    ) {
        Segment segment = *this;
        segment.rect.setCenter(rect.centerX(), Y);
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


struct SegmentsOnLine {
	Segment base;
	double margin;
	int64_t N;
	void draw(std::function<Segment(const Segment& s, const int64_t i, const int64_t N)> f) const {
		for (int32_t i = 0; i < N; i++) {
			const double t1 = double(i)/N;
			const double t2 = double(i+1)/N;
			f(base.DevideSegment(t1, t2).TakeMargin(margin), i, N).draw();
		}
	}
	
};

struct Timer {
	double time;
	bool Update(double t, bool on) {
		if (t < time)
		if (on) {  }
		return on;
	}
	event() {
		
	}
};

struct SegmentTree {
	Mat3x2 affine;
	const int32_t stairs = 4;
	const Array<Color> segment_color = {
		HSV(0, 0.44, 0.70),
		HSV(55, 0.44, 0.70),
		HSV(121, 0.44, 0.70),
		HSV(173, 0.45, 0.70),
	};
	const Color query_color = HSV(255, 0.00, 1.00);
	const double y_max = 0.30;
	const double y_min = -0.25;
	const double width = 0.80;

	SegmentTree() {
		const Mat3x2 translate = Mat3x2::Translate({0, 0.216});
		const Mat3x2 rotate1 = Mat3x2::Rotate(Math::Pi / 5);
		affine = translate * rotate1;
	}
	Segment default_segment(const Color& color) {
		return {.rect = {}, .c = color};
	}
	void draw(const double t) {
		const auto locate_y = [&](int32_t i){ return (y_max - y_min) * double(i)/(stairs-1) + y_min; };
		Array<double> event_y = {y_min - 0.10, locate_y(1), locate_y(2), locate_y(3), 0.5};
		Array<double> event_time = { 0.0, 1.0, 1.8, 2.6, 5.0 };
		
		int32_t cei = 0;
		for (int32_t i = 0; i < event_time.size() - 1; i++) {
			if (InRange(t, event_time[i], event_time[i+1])) { cei = i; }
		}
		double t1 = time_01(t, event_time[cei], event_time[cei+1] - event_time[cei]);
		t1 = EaseInBack(t1);

		Array<Array<SegmentsOnLine>> segments_on_line(stairs);
		for (int32_t i = 0; i < stairs; i++) {
			const double y = locate_y(i);
			SegmentsOnLine segments{
				.base = default_segment(segment_color[i]).Locate(-width/2, width/2, y, 0.05),
				.margin = 0.005,
				.N = 1 << i
			};
			segments.draw([&](const Segment& s, auto i, auto N) -> Segment{ 
				
			});
		}
		
		const double margin = 0.02;
		Array<Segment> queries = {
			default_segment(query_color).Locate(-width*0.5, width*0, event_y[0], 0.025),
			default_segment(query_color).Locate(width*0, width*0.25, event_y[0], 0.025),
			default_segment(query_color).Locate(width*0.25, width*(0.25 + 0.125), event_y[0], 0.025),
		};
		if (cei < 0) { queries[0] = queries[0].TakeMargin(0, -margin); }
		if (cei < 1) { queries[1] = queries[1].TakeMargin(-margin); }
		if (cei < 2) { queries[2] = queries[2].TakeMargin(-margin, 0); }
		for (int32_t i = 0; i < queries.size(); i++) {
			if (cei <= i) {
				queries[i] = queries[i].SetBaseY(event_y[cei]).GoDown(t1, event_y[cei+1] - event_y[cei]);
			} else {
				queries[i] = queries[i].SetBaseY(event_y[i+1]);
			}
		}
		for (int32_t i = 0; i < queries.size(); i++) { queries[i].draw(); }
	}	
};
