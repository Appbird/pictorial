# pragma once
# include <Siv3D.hpp> // Siv3D v0.6.15
# include "Interval.hpp"
# include "KeyFrame.hpp"


/// 円v1とv2の外枠を繋ぐような線分を与えます。
Line Connect(const Circle& v1, const Circle& v2);

/// 線分の始点・終点をt:(1-t)に内分したときの始点側の線分を返す。
Line CutAt(double t, Line l) {
    return Line{l.begin, l.position(t)};
}

/// @brief 与えられた文字列の配列`map`で表された配置位置に従って頂点を設置します。10個までなら置けます。
const Array<Vec2> FromVerticesMap(const Array<String> map);

struct BFS {
    const double anime_length = 5.0;
    Array<Vec2> vertices = FromVerticesMap({
        U"                       ",
        U"    0                  ",
        U"                3      ",
        U"                       ",
        U"         2         6   ",
        U"  1                    ",
        U"            5          ",
        U"    4                  ",
        U"                       ",
        U"                7      ",
        U"                       ",
    });
    Array<std::pair<size_t, size_t>> edges = {
        {0, 1}, {0, 2}, {0, 3},
        {1, 4}, {2, 5}, {3, 6},
        {4, 7}, {5, 7}, {6, 7}
    };
    Array<bool> vertices_frame;
    
    BFS() {
        vertices_frame = Array<bool>(vertices.size(), false);
        vertices_frame.front() = true;
    }
    Circle vertex_circle(const size_t idx) {
        return Circle{vertices[idx], vertices_frame[idx] ? 0.040 : 0.030};
    }
    Circle vertex_circle(const size_t idx, const double edge_width) {
        return Circle{vertices[idx], vertices_frame[idx] ? 0.040 : 0.030 + edge_width / 2};
    }

    void draw(const double t) {
        KeyAnimator<double> kf_edge_painter {{
            {.time = 0.0, .x = 0, ease_iplt<EaseInOutExpo>},
            {.time = 0.5, .x = 1, binary_iplt<double>},
        }};
        KeyAnimator<double> kf_vertex_empasiser {{
            {.time = 0.0, .x = 0.0, ease_iplt<EaseInQuint>},
            {.time = 0.20, .x = 1.4, ease_iplt<EaseInQuint>},
            {.time = 0.30, .x = 0.8, ease_iplt<EaseInQuint>},
            {.time = 0.40, .x = 1.1, ease_iplt<EaseInQuint>},
            {.time = 0.50, .x = 1.0, binary_iplt<double>},
        }};

        //TODO: 生起時刻と起こるイベントという形で書ければコードの見通しが良くなりそう
        // イベント発生時刻を求めておく。
        Array<double> evoke_edge_paint;
        Array<double> evoke_points_paint;
        {
            const double start = 0.3;
            const double d = 0.15;
            const double wave1 = 0.4;
            const double wave2 = 1.4;
            const double wave3 = 2.4;
            evoke_edge_paint = {
                wave1, wave1 + d, wave1 + 2*d,
                wave2, wave2 + d, wave2 + 2*d,
                wave3, wave3 + d, wave3 + 2*d,
            };
            evoke_points_paint =  {
                start,
                wave2, wave2 + d, wave2 + 2*d,
                wave3, wave3 + d, wave3 + 2*d,
                3.4
            };
        }

        // 辺の色
        Array<HSV> map_edge_to_color = {
            HSV{ 5.14, 0.85, 0.99 },
            HSV{ 10.14, 0.85, 0.99 },
            HSV{ 15.14, 0.85, 0.99 },
            HSV{ 33.84, 0.88, 1.00 },
            HSV{ 38.84, 0.88, 1.00 },
            HSV{ 43.84, 0.88, 1.00 },
            HSV{ 55, 1.00, 1.00},
            HSV{ 60, 1.00, 1.00 },
            HSV{ 65, 1.00, 1.00 },
        };
        // 頂点の色の色
        Array<HSV> map_vertex_to_color = {
            HSV{ 10.14, 0.85, 0.99 },
            HSV{ 33.84, 0.88, 1.00 },
            HSV{ 38.84, 0.88, 1.00 },
            HSV{ 43.84, 0.88, 1.00 },
            HSV{ 55, 1.00, 1.00 },
            HSV{ 60, 1.00, 1.00 },
            HSV{ 65, 1.00, 1.00 },
            HSV{ 0, 0.00, 1.00 },
        };
        // 辺
        for (size_t idx = 0; idx < edges.size(); idx++) {
            const auto [i, j] = edges[idx];
            const double edge_width = 0.02;
            const HSV color = map_edge_to_color[idx];
            const Line basic_line = Connect(vertex_circle(i, 0.02), vertex_circle(j, 0.01));

            // 塗りつぶし量
            basic_line.draw(LineStyle::RoundDot, 0.01, ColorF{0.6});
            const double t1 = kf_edge_painter.shift(evoke_edge_paint[idx]).refer(t);
            CutAt(t1, basic_line).draw(edge_width, color);
        }

        // 頂点
        const HSV target_color = {0., 1., 1.};
        for (size_t i = 0; i < vertices.size(); i++) {
            const Circle C = vertex_circle(i);
            C.drawFrame(0.01, HSV{ 210.00, 0.22, 0.56 });
            const double t2 = kf_vertex_empasiser.shift(evoke_points_paint[i]).refer(t);
            Circle C1 = { C.center, C.r * t2 };
            HSV vert_color = map_vertex_to_color[i];
            C1.draw(vert_color);
        }
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