# pragma once
# include <Siv3D.hpp> // Siv3D v0.6.15

Line Connect(const Circle& v1, const Circle& v2) {
    Vec2 edge_vector = v2.center - v1.center;
    if (edge_vector.isZero()) { throw new Error(U"edge length is 0."); }
    double arg = Math::Atan2(edge_vector.y, edge_vector.x);
    return Line{
        OffsetCircular(v1.center, v1.r, arg+Math::Pi/2),
        OffsetCircular(v2.center, v2.r, arg-Math::Pi/2)
    };
}

struct BFS {
    const double anime_length = 5.0;
    SimpleAnimation s;
    Array<Vec2> vertices = {
        {0.3, 0},
        {-0.3, 0}
    };
    BFS() {

    }
    void draw(const double t) {
        
    }

    void draw_bloom(const double t) {

    }
};

