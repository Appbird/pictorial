#pragma once
#include <Siv3D.hpp>

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