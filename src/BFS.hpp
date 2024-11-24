# pragma once
# include <Siv3D.hpp> // Siv3D v0.6.15
# include "Interval.hpp"
# include "KeyFrame.hpp"


/// 円v1とv2の外枠を繋ぐような線分を与える。
Line Connect(const Circle& v1, const Circle& v2);

/// 線分の始点・終点をt:(1-t)に内分したときの始点側の線分を返す。
Line CutAt(double t, Line l) {
    return Line{l.begin, l.position(t)};
}
/// 円`C`を`dy`だけ上向きに動かした円を返す。
Circle MoveDown(const Circle& C, double dy) {
    return Circle{C.center + Vec2{0, dy}, C.r};
}

/// @brief 与えられた文字列の配列`map`で表された配置位置に従って頂点を設置します。10個までなら置けます。
const Array<Vec2> FromVerticesMap(const Array<String> map);

struct BFS {
    HSV drawable_region_color = HSV{ 235, 0.35, 0.20 };
    const double anime_length = 5.0;
    Vec2 origin_center = {-0.35, -0.35};
    Array<Vec2> vertices = {
        OffsetCircular(origin_center, 0, 0),

        OffsetCircular(origin_center, 0.33, Math::Pi*9/16),
        OffsetCircular(origin_center, 0.33, Math::Pi*12/16),
        OffsetCircular(origin_center, 0.33, Math::Pi*15/16),

        OffsetCircular(origin_center, 0.66, Math::Pi*9/16),
        OffsetCircular(origin_center, 0.66, Math::Pi*12/16),
        OffsetCircular(origin_center, 0.66, Math::Pi*15/16),

        OffsetCircular(origin_center, 1.0, Math::Pi*12/16),

    };
    Array<std::pair<size_t, size_t>> edges = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 4}, {2, 5}, {3, 6},
        {4, 7}, {5, 7}, {6, 7}
    };

    // 辺の色
    Array<HSV> map_edge_to_color = {
        HSV{ 5.14, 0.45, 0.90 },
        HSV{ 10.14, 0.45, 0.90 },
        HSV{ 15.14, 0.45, 0.90 },
        HSV{ 33.84, 0.48, 1.00 },
        HSV{ 38.84, 0.48, 1.00 },
        HSV{ 43.84, 0.48, 1.00 },
        HSV{ 55, 0.40, 1.00 },
        HSV{ 60, 0.40, 1.00 },
        HSV{ 65, 0.40, 1.00 },
    };
    // 頂点の色の色
    Array<HSV> map_vertex_to_color = {
        HSV{ 10.14, 0.75, 0.90 },
        HSV{ 33.84, 0.78, 1.00 },
        HSV{ 38.84, 0.78, 1.00 },
        HSV{ 43.84, 0.78, 1.00 },
        HSV{ 55, 0.70, 1.00 },
        HSV{ 60, 0.70, 1.00 },
        HSV{ 65, 0.70, 1.00 },
        HSV{ 0, 0.00, 1.00 },
    };

    KeyAnimator<double> kf_edge_painter {{
        {.time = 0.0, .x = 0, ease_iplt<EaseInOutExpo>},
        {.time = 0.5, .x = 1, binary_iplt<double>},
    }};
    KeyAnimator<double> kf_startend_vertex_empasiser {{
        {.time = 0.0, .x = 0.0, ease_iplt<EaseInOutQuint>},
        {.time = 0.10, .x = 0.3, ease_iplt<EaseInOutQuint>},
        {.time = 0.40, .x = 0.4, ease_iplt<EaseInOutQuint>},
        {.time = 0.50, .x = 1.4, ease_iplt<EaseInOutQuint>},
        {.time = 0.70, .x = 1.0, ease_iplt<EaseInOutQuint>},
    }};
    KeyAnimator<double> kf_end2_vertex_empasiser {{
        {.time = 0.0, .x = 0.5, ease_iplt<EaseInOutQuint>},
        {.time = 0.10, .x = 0.5, ease_iplt<EaseInOutQuint>},
        {.time = 0.40, .x = 0.6, ease_iplt<EaseInOutQuint>},
        {.time = 0.50, .x = 1.4, ease_iplt<EaseInOutQuint>},
        {.time = 0.70, .x = 1.0, ease_iplt<EaseInOutQuint>},
    }};
    KeyAnimator<double> kf_startend_vertex_dy {{
        {.time = 0.0, .x = 0.00, ease_iplt<EaseInOutQuad>},
        {.time = 0.30, .x = -0.06, ease_iplt<EaseInOutQuad>},
        {.time = 0.40, .x = 0.01, ease_iplt<EaseInOutQuad>},
        {.time = 0.60, .x = 0.00, ease_iplt<EaseInOutQuad>},
    }};
    KeyAnimator<double> kf_explored_area_radius {{
        {.time = 0.0, .x = 0.0, ease_iplt<EaseOutCubic>},
        {.time = 0.3, .x = 0.0, ease_iplt<EaseOutCubic>},
        {.time = 0.6, .x = 0.14, ease_iplt<EaseOutCubic>},
        {.time = 0.7, .x = 0.12, ease_iplt<EaseOutCubic>},
        {.time = 0.9, .x = 0.12, ease_iplt<EaseOutCubic>},
        {.time = 0.9 + 0.75, .x = 0.37, ease_iplt<EaseOutCubic>},
        {.time = 1.9, .x = 0.37, ease_iplt<EaseOutCubic>},
        {.time = 1.9+ 0.75, .x = 0.70, ease_iplt<EaseInOutCubic>},
        {.time = 2.9, .x = 0.70, ease_iplt<EaseOutCubic>},
        {.time = 2.9 + 0.75, .x = 1.04, ease_iplt<EaseOutCubic>},
        {.time = 3.9, .x = 1.04, ease_iplt<EaseOutCubic>},
        {.time = 3.9 + 0.75, .x = 1.3, ease_iplt<EaseOutCubic>},
    }};
    

    //TODO: 生起時刻と起こるイベントという形で書ければコードの見通しが良くなりそう
    Array<double> evoke_edge_paint;
    Array<double> evoke_points_paint;

    /// 中実な円で頂点を表現するかどうか
    Array<bool> vertices_frame;
    
    BFS() {
        vertices_frame = Array<bool>(vertices.size(), false);
        vertices_frame.front() = true;
        vertices_frame.back() = true;

        // イベント発生時刻を求めておく。
        const double start = 0.8;
        const double d = 0.05;
        const double wave1 = 0.9;
        const double wave2 = 1.9;
        const double wave3 = 2.9;
        evoke_edge_paint = {
            wave1, wave1 + d, wave1 + 2*d,
            wave2, wave2 + d, wave2 + 2*d,
            wave3, wave3 + d, wave3 + 2*d,
        };
        evoke_points_paint =  {
            start,
            wave2 - 0.4, wave2 + d - 0.4, wave2 + 2*d - 0.4,
            wave3 - 0.4, wave3 + d - 0.4, wave3 + 2*d - 0.4,
            3.0
        };
    }
    Circle vertex_circle(const size_t idx) {
        return Circle{vertices[idx], vertices_frame[idx] ? 0.040 : 0.030};
    }
    Circle vertex_circle(const size_t idx, const double edge_width) {
        return Circle{vertices[idx], vertices_frame[idx] ? 0.040 : 0.030 + edge_width / 2};
    }
    Circle anim_appear(const double t, const double shift, const Circle& C) {
        // FIXME: なんか円の半径がおかしい！
        const double t1 = kf_startend_vertex_empasiser.shift(shift).refer(t);
        const double dy = kf_startend_vertex_dy.shift(shift).refer(t);
        Circle C_prime = Circle{C.center, C.r * t1};
        C_prime = MoveDown(C_prime, dy);
        return C_prime;
    }

    void draw_edge(const double t) {
        for (size_t idx = 0; idx < edges.size(); idx++) {
            const auto [i, j] = edges[idx];
            const double edge_width = 0.02;
            const HSV color = map_edge_to_color[idx];
            const Line basic_line = Connect(vertex_circle(i, 0.02), vertex_circle(j, 0.01));

            // 塗りつぶし量
            // TODO: 未探索の辺はすごく薄く描く？
            basic_line.draw(LineStyle::RoundDot, 0.01, ColorF{0.6});
            const double t1 = kf_edge_painter.shift(evoke_edge_paint[idx]).refer(t);
            CutAt(t1, basic_line).draw(edge_width, color);
        }
    }
    void draw_vertex(const double t) {
        const HSV vertex_color = HSV{ 234.00, 0.05, 0.80 };
        for (size_t i = 1; i < vertices.size() - 1; i++) {
            const Circle C = vertex_circle(i);
            C.drawFrame(0.01, vertex_color);
            HSV vert_color = map_vertex_to_color[i];
            anim_appear(t, evoke_points_paint[i], C.stretched(0.01)).draw(vert_color);
        }
        vertex_circle(vertices.size() - 1).draw(drawable_region_color).drawFrame(0.01, vertex_color);
    }
    void draw_explored_region(const double t) {
        Circle explored{origin_center, kf_explored_area_radius.refer(t)};
        {
            RectF whole_area{-0.5, -0.5, 1.0, 1.0};
            const auto regions = Geometry2D::Xor(whole_area, explored.asPolygon());
            for (const auto region:regions) {
                region.draw(HSV{drawable_region_color, 0.9});
            }
        }
        explored.drawFrame(0.01, Palette::Lightsteelblue);
    }
    void draw_start_goal_vertex(const double t) {
        const HSV start_color =HSV{ 133.22, 0.24, 0.97 };
        const HSV goal_color = HSV{ 330.59, 0.20, 1.00 };
        anim_appear(t, 0, vertex_circle(0)).draw(start_color);
        if (t < 3.4) {
            Circle C = vertex_circle(vertices.size() - 1);
            C = {C.center, C.r * 0.5};
            anim_appear(t, 0, C).draw(goal_color);
        } else {
            Circle C = vertex_circle(vertices.size() - 1);
            const double t1 = kf_end2_vertex_empasiser.shift(3.9 - 0.5).refer(t);
            const double dy = kf_startend_vertex_dy.shift(3.9 - 0.5).refer(t);
            Circle C_prime = Circle{C.center, C.r * t1};
            C_prime = MoveDown(C_prime, dy);
            C_prime.draw(goal_color);
        }
    }
    void draw(const double t) {
        draw_edge(t);
        draw_vertex(t);
        // 未探索の範囲は暗くする。
        draw_explored_region(t);
        draw_start_goal_vertex(t);
    }

    void draw_bloom(const double t) {

    }
};

////////////////////////////////////////////////
//
// 以下、ユーティリティ関数群
//
////////////////////////////////////////////////


const Array<Vec2> FromVerticesMap(const Array<String> S) {
    size_t max_column = 0;
    size_t max_index = 0;
    Array<Vec2> points(10);
    for (int32_t i = 0; i < S.size(); i++) {
        for (int32_t j = 0; j < S[i].size(); j++) {
            if ('0' <= S[i][j] and S[i][j] <= '9') {
                const size_t idx = S[i][j] - '0';
                points[idx] = Vec2{j + 0.5, i + 0.5};
                max_index = Max(max_index, idx);
            }
        }
        max_column = Max(max_column, S[i].size());
    }
    Console << points;
    points.resize(max_index + 1);
    for (auto& point: points) {
        point.x = (point.x) / max_column - 0.5;
        point.y = (point.y) / S.size() - 0.5;
    }
    Console << points;
    return points;
};

Line Connect(const Circle& v1, const Circle& v2) {
    Vec2 edge_vector = v2.center - v1.center;
    if (edge_vector.isZero()) { throw new Error(U"edge length is 0."); }
    double arg = Math::Atan2(edge_vector.y, edge_vector.x);
    return Line{
        OffsetCircular(v1.center, v1.r, arg+Math::Pi/2),
        OffsetCircular(v2.center, v2.r, arg-Math::Pi/2)
    };
}