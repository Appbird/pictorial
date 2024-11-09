#pragma once
# include <Siv3D.hpp> // Siv3D v0.6.15
# include "utility.hpp"
const double anime_length = 5.0;

struct Arrow{
	Line line;
	Color c;
	double width;
	void draw(const double t) const {
		if (not InRange(t, 0., 1.0 + 1e6)) { return; }
		const Line current_arrow { line.begin, line.position(t) };
		current_arrow.drawArrow(width, SizeF{width*1.2, width*1.2}, c);
	}
};
Arrow BasicArrow(const Line& line, const Color& c) {
	return {line, c, 0.08};
}

struct ArrowsOnLines {
	Line line;
	Color c;
	const double margin;
	const size_t N;
	ArrowsOnLines(const Line& _line, const double _margin, const size_t _N, const Color& c):
		line(_line), c(c), margin(_margin), N(_N)
	{}
	void draw(const double t) const {
		const double margin_in_t = margin / line.length();
		for (int32_t i = 0; i < N; i++) {
			const double start = double(i) / N;
			const double end = double(i+1) / N;
			Line target_line = {line.position(start + margin_in_t), line.position(end - margin_in_t)};
			BasicArrow(target_line, c).draw(t);
		}
	}
};
ArrowsOnLines BasicArrowsOnLine(const Line& line, size_t N, const Color& c) {
	return {line, 0.005, N, c};
}

struct Dubling {
	Mat3x2 affine;
	int32_t N = 4;
	const Array<HSV> arrow_color2 = {
		HSV(171, 0.15, 1.00),
		HSV(128, 0.15, 1.00),
		HSV(42, 0.15, 1.0),
        HSV(331, 0.15, 1.0),
    };
	Dubling(){
		const Mat3x2 translate = Mat3x2::Translate({0, 0.216});
		const Mat3x2 rotate1 = Mat3x2::Rotate(Math::Pi / 5);
		affine = translate * rotate1;
	}
	double line_y_at(int32_t i) {
		return (double(i)/(N) - 0.5) * 1.0 + 0.05;
	}
	ArrowsOnLines LerpingArrowsOnLine(
		int32_t i,  double t_position, double t_color
	) {
		const double x = 0.60;
		const double y = std::lerp(line_y_at(i-1), line_y_at(i), t_position);
		const Line line { {-x, y}, {x, y} };
		const size_t line_count = 1ull << (N - 1 - i);
        ColorF arrow_color1 = arrow_color2[i];
        arrow_color1.a = 0.0;
		const ColorF c = arrow_color1.lerp(arrow_color2[i], t_color);
		return BasicArrowsOnLine(line, line_count, c);
	}
	void draw(const double t) {
		Transformer2D transformer{affine};
		const double t1 = EaseOutCubic(time_01(t, 0.5 + (0 * 0.5), 0.8));
		LerpingArrowsOnLine(0, 1, t1).draw(t1);
		for (int32_t i = 1; i < N; i++) {
			const double t1 = EaseOutCubic(time_01(t, 0.5 + (i * 0.5), 0.8));
			LerpingArrowsOnLine(i, t1, t1).draw(t1);
		}
	}
    void draw_bloom(const double t) {
        
    }
};