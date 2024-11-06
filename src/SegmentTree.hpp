#pragma once
#include <Siv3D.hpp>
#include <functional>
#include "utility.hpp"
#include "Segment.hpp"
#include "KeyFrame.hpp"

struct SegmentTree {
	Mat3x2 affine;
	const int32_t stairs = 4;
    const double query_height = 0.030;
    const double segment_height = 0.080;

	const double y_max = 0.40;
	const double y_min = -0.20;
	const double width = 0.90;
	const Color query_color = HSV(273, 0, 1.00);
    const Array<Color> segment_color = {
		HSV(0, 0.44, 0.70),
		HSV(55, 0.44, 0.70),
		HSV(121, 0.44, 0.70),
		HSV(173, 0.45, 0.80),
	};
    double locate_y(int32_t i) const {
        return (y_max - y_min) * double(i)/(stairs-1) + y_min; 
    }
	SegmentTree() {
		affine = Mat3x2::Identity();
    }
	Segment default_segment(const Color& color) {
		return {.rect = {}, .c = color};
	}

    void draw(const double t) {
        Transformer2D transformer{affine};
        draw_segment_tree(t);
        draw_query(t);
    }
    void draw_bloom(const double t) { }

    // クエリが落下する時に引っかかる時刻のリスト
    Array<double> qfalling_y_keytime = {1.5, 2.1, 2.7, 3.3};
    const double queries_margin = 0.02;
    // クエリが落ちていき、セグメントと衝突した際にどのように変位するかを示した遷移
    KeyAnimation kf_qfalling_deflecting {
        ease_iplt<EaseInOutQuad>,
        {0, 0.2, 0.4, 0.5, 0.7},
        {0., 0.08, -0.02, 0.01, 0.}
    };
    // クエリが落ちていき、セグメントと衝突した際に、クエリの色をどのように変えるかを示した遷移
    KeyAnimation kf_qfalling_deflecting_color {
        ease_iplt<EaseInOutQuad>,
        {0, 0.2, 0.4},
        {0, 0.05, 0.015}
    };
    void draw_deflecting_segments_on_line(
        const double t,
        const Segment& base_segment,
        const int64_t stair
    ) const { 
        const int32_t segment_count = (1ull << stair);
        for (int32_t j = 0; j < segment_count; j++) {
            // セグメントのの基本的な位置配置を決定する。
            const double begin = double(j) / segment_count;
            const double end = double(j + 1) / segment_count;
            Segment s = 
                base_segment
                .DevideSegment(begin, end)
                .TakeMargin(0.005);
            
            // アニメーション中のクエリが着地するときのたわみを考慮した配置になるように、セグメントの位置を調節する。
            const auto deflector = [&](int32_t key_time_idx, int32 j, int32 top_j) {
                double displace_degree = kf_qfalling_deflecting.refer(t, qfalling_y_keytime[key_time_idx]);
                double color_degree = kf_qfalling_deflecting_color.refer(t, qfalling_y_keytime[key_time_idx]);
                if (j != top_j){
                    displace_degree *= 0.5;
                    color_degree *= 0.3;
                }
                HSV color(s.c);
                color.v *= (1 + Clamp(color_degree, 0., 1.0) * 20);
                return s.MoveCenter({0., displace_degree}).ChangeColor(color);
            };
            if (stair == 1)         { s = deflector(1, j, 0); }
            else if (stair == 2)    { s = deflector(2, j, 2); }
            else if (stair == 3)    { s = deflector(3, j, 6); }
            
            s.draw();
        }
    } 
    void draw_segment_tree(const double t) {
        // セグメントツリーの区間が時間経過に合わせてどう移動するかを示す。
        KeyAnimation kf_stappear {
            ease_iplt<EaseOutBack>, {0.7, 1.2}, {0, 1.0}
        };
        // セグメントツリーの各区間が時間経過に合わせてどう不透明度が変化するかを示す。
        KeyAnimation kf_stappear_color {
            ease_iplt<EaseOutBack>, {0.7, 1.2}, {0, 1.0}
        };
		for (int32_t stair = 0; stair < stairs; stair++) {
            const double y0 = 0;
            // 登場時に座標0から現れるように調節。終わった後は、キーフレームkf_stappearは常に値1を出し続ける。
            double y = std::lerp(y0, locate_y(stair), Math::Square(kf_stappear.refer(t)));
            const double alpha = std::lerp(y0, locate_y(stair), kf_stappear_color.refer(t));
            Segment base_segment =
                default_segment(segment_color[stair])
                .Locate(-width/2, width/2, y, segment_height);
            // 最終段以外、最初からフェードインするようにする。
            if (stair != 3) {
                HSV color(base_segment.c); color.v *= kf_stappear_color.refer(t);;
                base_segment.c = color;
                base_segment.c.a = kf_stappear_color.refer(t);
            }
            draw_deflecting_segments_on_line(t, base_segment, stair);
		}
    }

    void draw_query_first(const double t) {
        // 初めてクエリが登場する時の幅の遷移
        KeyAnimation kf_qappear_width {
            ease_iplt<EaseInOutQuint>, {0, 0.5},  {0, 7.0/8}
        };
        // クエリが登場したあと、上方向に移動する遷移
        KeyAnimation kf_qappear_y {
            ease_iplt<EaseOutBack>, {0.7, 1.2}, {0.0, 1.0}
        };

        const double x_left = (-0.5) * width;
        const double x_length = width * kf_qappear_width.refer(t);
        const double y = std::lerp(-0.10, -0.40, Math::Square(kf_qappear_y.refer(t)));
        Segment s =
            default_segment(query_color)
            .Locate(x_left, x_left + x_length, y, query_height);
        s.draw();
        
    }
    void draw_query_second(const double t) {
        // クエリが落下する時の遷移
        KeyAnimation kf_qfalling_y {
            ease_iplt<EaseInExpo>,
            qfalling_y_keytime,
            {-0.40, locate_y(1), locate_y(2), locate_y(3)}
        };
        // クエリが落下する時に、幅の広がりがどれほど取られるかの遷移を示したもの。
        KeyAnimation kf_qfalling_margin {
            binary_iplt,
            {-1, 0},
            {-queries_margin, 0.}
        };
        Array<std::pair<double, double>> x_segment = {
            {-width/2, 0}, {0, width/4}, {width/4, width*3/8}
        };
        
		for (int32_t i = 0; i < x_segment.size(); i++) {
            const auto [left_x, right_x] = x_segment[i];
            const double t1             = Clamp(t, 0., qfalling_y_keytime[i+1]);
            const double y              = kf_qfalling_y.refer(t1);
            // クエリが着地した時の押し下げ量
            const double deflecting     = kf_qfalling_deflecting.refer(t, qfalling_y_keytime[i+1]);
            const double query_margin   = kf_qfalling_margin.refer(t, qfalling_y_keytime[i]);
            const double margin_left    = (i == 0) ? 0 : query_margin;
            const double margin_right   = (i == x_segment.size() - 1) ? 0 : query_margin;
            // 基本的な配置
            default_segment(query_color)
                .Locate(left_x, right_x, y, query_height)
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
};
