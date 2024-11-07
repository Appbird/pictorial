#pragma once
#include <Siv3D.hpp>
#include "KeyFrame.hpp"
#include "Segment.hpp"

struct Pointer {
    Vec2 to;
    Texture icon;
    ColorF color;

    Vec2 size = {0.06, 0.06};
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
        base.setCentroid({base.centroid().x, y_tri});
        base.draw(color);
        tex.draw(Arg::center = Vec2{icon_center.x, y_icon}, color);
    }
    // # TODO: p2 落下した後、ちょっと揺らぐアニメーションを作る。

};

struct LeftInterval {
    Vec2 from;
    Color c;
    double height = 0.15;
    void draw() {
        Polygon ok_region = Polygon{
            {-0.6, from.y + height},
            {-0.6, from.y},
            from,
            from + Vec2{0, height},
        };
        ok_region.draw( HSV{129, 0.78, 0.54, 0.3} );

        Line{from, from + Vec2{0, height}}.draw(0.005, HSV{129, 0.28, 0.84});
        Line{from + Vec2{0, height}, {-0.6, from.y + height}}.draw(0.005, HSV{ 129, 0.28, 0.84});
        Circle{from, 0.018}.draw(HSV{129, 0.18, 0.94});
    }
    // #TODO: p1 落下した後、区間を移動できるように。
};

struct Axis {
    double width;
    double axis_y;
    double center;
    HSV axis_color;
    void draw() {
        Line{{-width/2, axis_y}, {width/2, axis_y}}.draw(0.010, axis_color);
    }
    Vec2 point_on_axis(const double t) {
        return Vec2{std::lerp(-width/2, width/2, t) + center, axis_y};
    }

    // #TODO: p1 落下した後、落下した領域から片方を特定の色で塗りつぶす処理を追加する。
    // 半開区間？
    // #TODO: p2 目盛りをつける。
    // #TODO: p2 目盛を拡大縮小できるようにして、一定以上拡大したら新しい目盛りが加わるようにする。
    //  （それに伴ってどう他のオブジェクトを動かすかという問題があるが....）
        // 拡大率を持っておいて、全体がそれを参照する？ありかも。
};

struct BinarySearch {
    Mat3x2 affine;
    
    Texture ok_icon{0xF05E0_icon, 80};
    Texture ng_icon{0xF0159_icon, 80};
    Texture unresolved_icon{0xF02D7_icon, 80};

    HSV ok_color = HSV{129, 0.18, 0.94};
    HSV ng_color = HSV{331, 0.19, 1.00};
    HSV axis_color = HSV{42, 0.34, 1.00};
    
    Axis axis{ .width = 1.0, .axis_y = 0.05, .axis_color = axis_color, .center = 0.0 };
    
    Pointer pointer_ok;
    Pointer pointer_ng;
    Pointer pointer_mid;
    
    LeftInterval interval;
    
    BinarySearch() {
        const Vec2 p_ok = axis.point_on_axis(0.1);
        const Vec2 p_ng = axis.point_on_axis(0.9);
        const Vec2 p_mid = (p_ok + p_ng) / 2;
        pointer_ok = Pointer{ .to = p_ok, .icon = ok_icon, .color = ok_color };
        pointer_ng = Pointer{ .to = p_ng, .icon = ng_icon, .color = ng_color };
        pointer_mid = Pointer{ .to = p_mid, .icon = unresolved_icon, .color = axis_color };
        interval = LeftInterval{.from = p_ok, .c = ok_color, .height = 0.15};
    }
    void draw(const double t) {
        axis.draw();
        interval.draw();
        pointer_ok.draw();
        pointer_ng.draw();
        const double t_fall = EaseInQuint(Clamp(t-0.5/1.0, 0., 1.));
        pointer_mid.Falling(t_fall, -0.7);
    }
    void draw_bloom(const double t) {

    }
};