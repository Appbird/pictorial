# pragma once
# include <Siv3D.hpp> // Siv3D v0.6.15


struct BFS {
    const double anime_length = 5.0;
    void draw(const double t) {
        const Circle v1 = Circle{{-0.3, 0}, 0.05};
        const Circle v2 = Circle{{0.3, 0}, 0.03};
        v1.draw(Palette::White);
        v2.drawFrame(0.008, Palette::White);
        Vec2 edge_vector = v2.center - v1.center;
        if (edge_vector.isZero()) { throw new Error(U"edge length is 0."); }
        double arg = Math::Atan2(edge_vector.y, edge_vector.x);
        Line{
            OffsetCircular(v1.center, v1.r, arg+Math::Pi/2),
            OffsetCircular(v2.center, v2.r, arg-Math::Pi/2)
        }.draw(0.008, Palette::Whitesmoke);
    }

    void draw_bloom(const double t) {

    }
};

