#pragma once
#include <Siv3D.hpp>
#include "KeyFrame.hpp"
#include "Segment.hpp"
const double anime_length = 8.0;


struct Pointer {
    Vec2 to;
    Texture icon;
    ColorF color;

    Vec2 size = {0.09, 0.09};
    Vec2 margin_icon_tri = Vec2{0., -0.15};
    
    Triangle base_triangle() const {
        return Triangle{to - Vec2{0, 0.070}, 0.04, Math::Pi};
    }
    TextureRegion base_icon() const {
        return icon.resized(size);
    }
    void draw() {
        base_triangle().draw(color);
        Vec2 icon_center = to + margin_icon_tri;
        base_icon().draw(Arg::center = icon_center, color);
    }
    void Falling(const double t, const double y_from) {
        Vec2 icon_center = to + margin_icon_tri;
        const double y_tri  = std::lerp(y_from, base_triangle().centroid().y, t);
        const double y_icon = std::lerp(y_from, icon_center.y, t);
        Triangle base = base_triangle();
        TextureRegion tex = base_icon();
        base.setCentroid({base.centroid().x, y_tri}).draw(color);
        tex.draw(Arg::center = Vec2{icon_center.x, y_icon}, color);
    }
    void Oscilating(const double t) {
        Vec2 icon_center = to + margin_icon_tri;
        Triangle base = base_triangle();
        const double amp = 0.05 * Math::Square(1-t);
        const double T = 0.3;
        const double y_tri  = base.centroid().y + amp * 0.2 * Math::Sin(2*Math::Pi*t/T);
        const double y_icon = icon_center.y + amp  * Math::Sin(2*Math::Pi*t/T + 0.01);
        base.setCentroid({base.centroid().x, y_tri}).draw(color);
        base_icon().draw(Arg::center = Vec2{icon_center.x, y_icon}, color);

    }
    void Emphasising(const double t) {
        KeyAnimation scaling{ease_iplt<EaseInQuint>, {0, 0.3, 0.5, 1.0}, {0.1, 2.0, 1.9, 1.0 }};
        Vec2 icon_center = to + margin_icon_tri;
        base_triangle().draw(color);

        TextureRegion tex = base_icon();
        const double t1 = scaling.refer(t);
        tex.resized(t1 * tex.size.x).draw(Arg::center = icon_center + Vec2{0, 0.05 * (1-t1)}, color);
    }

};

ColorF reset_alpha(ColorF c, double alpha) {
    c.a = alpha;
    return c;
}

struct Interval {
    Vec2 from;
    Color c;
    double height = 0.15;
    double x_left_edge = -0.6;
    double x_right_edge = 0.6;
    void draw_right() {
        Polygon region = Polygon{
            from + Vec2{0.05, height},
            from,
            { x_right_edge, from.y },
            { x_right_edge, from.y + height },
        };
        region.draw( reset_alpha(c, 0.3) );
        HSV color{c};
        color.v *= 2; color.s *= 0.8;
        Line{from, from + Vec2{0.05, height}}.draw(0.005, color);
        Line{from + Vec2{0.05, height}, {x_right_edge, from.y + height}}.draw(0.005, color);
        //Circle{from, 0.018}.draw(c);
    }
    void draw_left() {
        Polygon region = Polygon{
            {x_left_edge, from.y + height},
            {x_left_edge, from.y},
            from,
            from + Vec2{0, height},
        };
        region.draw( reset_alpha(c, 0.3) );
        HSV color{c};
        color.v *= 2; color.s *= 0.8;
        Line{from, from + Vec2{0, height}}.draw(0.005, color);
        Line{from + Vec2{0, height}, {x_left_edge, from.y + height}}.draw(0.005, color);
        Circle{from, 0.018}.draw(c);
    }
private:
};

struct Axis {
    double width;
    double axis_y;
    double center;
    double x_min = 0.0;
    double x_max = 1.0;
    HSV axis_color;
    void draw() {
        Line{{-width/2, axis_y}, {width/2, axis_y}}.draw(0.010, axis_color);
    }
    Vec2 point_on_axis(const double t) {
        return Vec2{std::lerp(-width/2, width/2, scaled_t(t)) + center, axis_y};
    }
    void draw_segment(double t1, double t2, ColorF color) {
        Line{point_on_axis(t1), point_on_axis(t2)}.draw(0.03, color);
    }
    double scaled_t(const double t) {
        return (t - x_min) / (x_max - x_min);
    }
    void draw_scaling(const double t, const double height = 0.015) {
        const Vec2 start = point_on_axis(t) + Vec2{0, -height};
        const Vec2 end = point_on_axis(t) + Vec2{0, height};
        Line{start, end}.draw(0.01, axis_color);
    }
    // 半開区間？
    // #TODO: p2 目盛りをつける。
    // #TODO: p2 目盛を拡大縮小できるようにして、一定以上拡大したら新しい目盛りが加わるようにする。
    //  （それに伴ってどう他のオブジェクトを動かすかという問題があるが....）
        // 拡大率を持っておいて、全体がそれを参照する？ありかも。
};

struct BinarySearch {
    Mat3x2 affine;
    
    Texture ok_icon{0xF05E0_icon, 160};
    Texture ng_icon{0xF0159_icon, 160};
    Texture unresolved_icon{0xF02D7_icon, 160};

    HSV ok_color = HSV{129, 0.18, 0.94};
    HSV ng_color = HSV{331, 0.19, 1.00};
    HSV axis_color = HSV{42, 0.54, 1.00};
    
    Axis axis{ .width = 1.0, .axis_y = 0.15, .axis_color = axis_color, .center = 0.0 };
    
    Pointer pointer_ok;
    Pointer pointer_ng;
    Pointer pointer_mid1;
    Pointer pointer_mid2;
    
    Interval ok_interval;
    Interval ng_interval;
    
    BinarySearch() {
        const Vec2 p_ok = axis.point_on_axis(0.1);
        const Vec2 p_mid1 = axis.point_on_axis(0.5);
        const Vec2 p_mid2 =  axis.point_on_axis(0.75);
        const Vec2 p_ng = axis.point_on_axis(0.9);
        pointer_ok = Pointer{ .to = p_ok,   .icon = ok_icon,    .color = ok_color };
        pointer_ng = Pointer{ .to = p_ng,   .icon = ng_icon,    .color = ng_color };
        pointer_mid1 = Pointer{ .to = p_mid1, .icon = unresolved_icon, .color = axis_color };
        pointer_mid2 = Pointer{ .to = p_mid2, .icon = unresolved_icon, .color = axis_color };
        ok_interval = Interval{.from = p_ok, .c = ok_color, .height = 0.15};
        ng_interval = Interval{.from = p_ng, .c = ng_color, .height = 0.15};
    }
    void draw_mid_points(const double t) {
        // point1
        match(t, {0, 0.8}, [&](double t, double start, double end) {
            const double t_fall = EaseInQuint(time_01(t, start, end - start));
            pointer_mid1.icon = unresolved_icon;
            pointer_mid1.color = axis_color;
            pointer_mid1.Falling(t_fall, -0.6);
        });
        match(t, {0.8, 1.8}, [&](double t, double start, double end) {
            const double t_osc      = time_01(t, start, end - start);
            pointer_mid1.Oscilating(t_osc);
        });
        match(t, {1.8, 2.9}, [&](double t, double start, double end) {
            const double t_emp      = time_01(t, start, end - start);
            pointer_mid1.icon = ok_icon;
            pointer_mid1.color = ok_color;
            pointer_mid1.Emphasising(t_emp);
        });
        match(t, {2.9, 10.0}, [&](double t, double start, double end) {
            pointer_mid1.draw();
        });

        // point2
        match(t, {4.0, 4.8}, [&](double t, double start, double end) {
            const double t_fall = EaseInQuint(time_01(t, start, end - start));
            pointer_mid2.icon = unresolved_icon;
            pointer_mid2.color = axis_color;
            pointer_mid2.Falling(t_fall, -0.6);
        });
        match(t, {4.8, 5.8}, [&](double t, double start, double end) {
            const double t_osc      = time_01(t, start, end - start);
            pointer_mid2.Oscilating(t_osc);
        });
        match(t, {5.8, 6.9}, [&](double t, double start, double end) {
            const double t_emp      = time_01(t, start, end - start);
            pointer_mid2.icon = ng_icon;
            pointer_mid2.color = ng_color;
            pointer_mid2.Emphasising(t_emp);
        });
        match(t, {6.9, 10.0}, [&](double t, double start, double end) {
            pointer_mid2.draw();
        });
    }
    // 5秒で1ループ
    void draw(const double t) {
        // ポインタの描画
        const double t_scale_start = 7.0;
        const double t_scale    = EaseInOutQuad(time_01(t, t_scale_start, 1.0));
        const double margin = std::lerp(0.10, 0.10 * (0.20 / 0.8), t_scale);
        const double min_x_after = 0.50;
        const double max_x_after = 0.70; 
        axis.x_min = std::lerp(0.0, min_x_after - margin, t_scale);
        axis.x_max = std::lerp(1.0, max_x_after + margin, t_scale);
        
        pointer_ok.to.x = axis.point_on_axis(0.1).x;
        pointer_mid1.to.x = axis.point_on_axis(0.5).x;
        pointer_mid2.to.x = axis.point_on_axis(0.70).x;
        pointer_ng.to.x = axis.point_on_axis(0.9).x;
        axis.draw();
        pointer_ok.draw();
        draw_mid_points(t);
        pointer_ng.draw();
        // 目盛
        {
            const int32_t scale_count = 6;
            for (int32_t i = 0; i <= scale_count; i++) {
                const double s = std::lerp(0.1, 0.9, double(i)/scale_count);
                axis.draw_scaling(s);
            }
        }
        // 小さい目盛
        {
            const int32_t scale_count = 24;
            for (int32_t i = 0; i <= scale_count; i++) {
                const double s = std::lerp(0.1, 0.9, double(i)/scale_count);
                axis.draw_scaling(s, std::lerp(0, 0.015, t_scale));
            }
        }
        // 区間の描画
        {   
            const double t_segment1_start = 2.9;
            const double t_segment2_start = 6.9;
            const double t1 = EaseInOutQuint(time_01(t, t_segment1_start, 1.0));
            const double t2 = EaseInOutQuint(time_01(t, t_segment2_start, 1.0));
            ok_interval.from.x = axis.point_on_axis(std::lerp(0.1, 0.5, t1)).x;
            ng_interval.from.x = axis.point_on_axis(std::lerp(0.9, 0.70, t2)).x;
            
            ok_interval.draw_left();
            ng_interval.draw_right();    
        }
        // 区間の下の線の描画
        {
            const double t_segment1_start = 1.8;
            const double t_segment1  = EaseInQuart(time_01(t, t_segment1_start, 1.0));
            const double t_segment2_start = 5.8;
            const double t_segment2  = EaseInQuart(time_01(t, t_segment2_start, 1.0));
            axis.draw_segment(0.0, 0.1, ok_color);
            axis.draw_segment(0.9, 1.0, ng_color);
            axis.draw_segment(0.5 * (1 - t_segment1), 0.5, ok_color);
            axis.draw_segment(0.70, 0.70 + 0.25 * t_segment2, ng_color);
        }

    }
    void draw_bloom(const double t) {

    }
};