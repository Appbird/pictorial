#pragma once
#include <Siv3D.hpp>
#include <functional>
#include "utility.hpp"

struct Segment {
	RectF rect;
	ColorF c;
	void draw() const {
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
    ) const {
        Segment segment = *this;
        segment.rect = RectF{x1, y - width/2, x2 - x1, width};
        return segment;
    }
    Segment LocateByLength(
        const double x_center, const double length, const double y,
        const double width
    ) const {
        Segment segment = *this;
        segment.rect = RectF{x_center - length/2, y - width/2, length, width};
        return segment;
    }
    Segment MoveCenter(
        const Vec2& delta
    ) const {
        Segment segment = *this;
        segment.rect.moveBy(delta);
        return segment;
    }
};


struct KeyAnimation {
    Array<double> time;
    Array<double> frames;
    using Interpolator = std::function<double(double, const double&, const double&)>;
    Interpolator interpolator;
    KeyAnimation(const Interpolator& itplt) : interpolator(itplt) {}
    // @return time in [0, 1]
    double refer(const double t) {
        int64_t cei = -1;
        for (int64_t i = 0; i < time.size() - 1; i++) {
            if (time[i] <= t and t <= time[i+1]) { cei = i; }
        }
        if (time[time.size() - 1] <= t) { cei = time.size() - 1; }
        const int64_t current_idx = Clamp(cei, 0ll, int64_t(time.size() - 1));
        const int64_t next_idx    = Clamp(cei + 1, 0ll, int64_t(time.size() - 1));
        const double t1 = time_01(t, time[current_idx], time[next_idx] - time[current_idx]);
        assert(0 <= t1 and t1 <= 1);
        return interpolator(frames[current_idx], frames[next_idx], t1);
    }
    double refer(const double t, const double t_start) {
        return refer(t - t_start);
    }
    void register_frames(const Array<double>& t, const Array<double>& frame) {
        time = t;
        frames = frame;
        assert(time.size() == frames.size());
    }
};

double easeinexpo_iplt(double current, double next, double t) {
    return std::lerp(current, next, EaseInExpo(t));
};
double easeinoutquad_iplt(double current, double next, double t) {
    return std::lerp(current, next, EaseInOutQuad(t));
};
double binary_iplt(double current, double next, double t) {
    return (t > 1 - 1e-6) ? next : current;
};

struct SegmentTree {
	Mat3x2 affine;
	const int32_t stairs = 4;
	const double y_max = 0.30;
	const double y_min = -0.15;
	const double width = 0.80;
	const Color query_color = HSV(273, 0, 1.00);
    const Array<Color> segment_color = {
		HSV(0, 0.44, 0.50),
		HSV(55, 0.44, 0.50),
		HSV(121, 0.44, 0.50),
		HSV(173, 0.45, 0.70),
	};

    Array<double> qappear_y_keytime;

    Array<double> qfalling_y_keytime;
    const double queries_margin = 0.02;
    // 落ちていく時のアニメーション
    KeyAnimation kf_qfalling_y { easeinexpo_iplt };
    KeyAnimation kf_qfalling_margin { binary_iplt };
    KeyAnimation kf_qfalling_deflecting { easeinoutquad_iplt };

    KeyAnimation kf_qappear_width { easeinoutquad_iplt };
    KeyAnimation kf_qappear_y { easeinoutquad_iplt };
    KeyAnimation kf_stappear { easeinoutquad_iplt };

    double locate_y(int32_t i) const {
        return (y_max - y_min) * double(i)/(stairs-1) + y_min; 
    }

	SegmentTree() {
		affine = Mat3x2::Identity();
        qfalling_y_keytime = {1.5, 2.1, 2.7, 3.3};
        kf_qfalling_y.register_frames(qfalling_y_keytime,
            {-0.30, locate_y(1), locate_y(2), locate_y(3)}
        );
        kf_qfalling_margin.register_frames({-1, 0}, {-queries_margin, 0.});
        kf_qfalling_deflecting.register_frames({0, 0.2, 0.4}, {0., 0.03, 0.0});
        kf_qappear_width.register_frames({0, 0.5},  {0, 7.0/8});
        kf_qappear_y.register_frames({0.7, 1.2},    {-0.10, -0.30});
        kf_stappear.register_frames({0.7, 1.2},     {0, 1.0});
	}
	Segment default_segment(const Color& color) {
		return {.rect = {}, .c = color};
	}
    void draw_segment_tree(const double t) {
        // セグ木
		for (int32_t stair = 0; stair < stairs; stair++) {
            const int32_t segment_count = (1ull << stair);
            const double y0 = 0;
            const double y = (locate_y(stair) - y0) * kf_stappear.refer(t) + y0;
            Segment base_segment =
                default_segment(segment_color[stair])
                .Locate(-width/2, width/2, y, 0.05);
            if (stair != 3) {
                base_segment.c.a = kf_stappear.refer(t);
            }
            
            
            for (int32_t j = 0; j < segment_count; j++) {
                const double begin = double(j) / segment_count;
                const double end = double(j + 1) / segment_count;
                // 基本的な位置配置
                Segment s = base_segment
                    .DevideSegment(begin, end)
                    .TakeMargin(0.005);
                
                // 着地時のたわみ
                const auto deflector = [&](int32_t key_time_idx) {
                    const double degree = kf_qfalling_deflecting.refer(t, qfalling_y_keytime[key_time_idx]);
                    HSV color(s.c); color.v *= (1 + degree * 30);
                    return s.MoveCenter({0., degree}).ChangeColor(color);
                };
                if (stair == 1 and j == 0) { s = deflector(1); }
                else if (stair == 2 and j == 2) { s = deflector(2); }
                else if (stair == 3 and j == 6) { s = deflector(3); }
                s.draw();
            }
		}
    }

    void draw_query_first(const double t) {
        const double x_left = (-0.5) * width;
        const double x_length = width * kf_qappear_width.refer(t);
        const double y = kf_qappear_y.refer(t);
        Segment s = default_segment(query_color)
            .Locate(x_left, x_left + x_length, y, 0.015);
        s.draw();
        
    }
    void draw_query_second(const double t) {
        Array<std::pair<double, double>> x_segment = {
            {-width/2, 0},
            {0, width/4},
            {width/4, width*3/8}
        };
		for (int32_t i = 0; i < x_segment.size(); i++) {
            const auto [left_x, right_x] = x_segment[i];
            const double t1             = Clamp(t, 0., qfalling_y_keytime[i+1]);
            const double y              = kf_qfalling_y.refer(t1);
            if (y > 0.5) { exit(1); }
            const double deflecting = kf_qfalling_deflecting.refer(t, qfalling_y_keytime[i+1]);
            const double query_margin   = kf_qfalling_margin.refer(t, qfalling_y_keytime[i]);
            const double margin_left    = (i == 0) ? 0 : query_margin;
            const double margin_right   = (i == x_segment.size() - 1) ? 0 : query_margin;
            default_segment(query_color)
                .Locate(left_x, right_x, y, 0.015)
                .TakeMargin(margin_left, margin_right)
                // 押し下げ分
                .MoveCenter({0., deflecting}) 
                .draw();
        }
    }
    void draw_query(const double t) {
        if (t < qfalling_y_keytime[0]) { draw_query_first(t); }
        else { draw_query_second(t); }
    }

	void draw(const double t) {
        Transformer2D transformer{affine};
        draw_segment_tree(t);
        draw_query(t);
    }
    void draw_bloom(const double t) {
    }
};
