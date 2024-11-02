# include <Siv3D.hpp> // Siv3D v0.6.15

Mat3x2 NormalizedCoord(RectF region) {
	return Mat3x2::Scale(region.size.y) * Mat3x2::Translate(region.center());
}
double time_01(double t, double min, double length) {
	return Clamp(t - min, 0., length) / (length);
}

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
	return {line, c, 0.05};
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

struct OuterFrame {
	Polygon outerframe_region;
	OuterFrame() {
		RoundRect drawable_region = RoundRect{RectF{-0.45, -0.45, 0.9, 0.9}, 0.01};
		outerframe_region = RectF{-1, -1, 2, 2}.asPolygon();
		assert(outerframe_region.addHole(drawable_region));
	}
	void draw(const Color& c) {
		outerframe_region.draw(c);
	}
};

struct Recorder {	
	const Rect frame_rect{0, 0, 600, 600};
	const double fps = 30;
	AnimatedGIFWriter writer{U"dst/a.gif", frame_rect.size};
	MSRenderTexture frame{frame_rect.size};
	Image frame_as_img{frame_rect.size};
	void clear_frame(const Color& c) {
		frame.clear(c);
	}
	void shot() {
		Graphics2D::Flush();
		frame.resolve();
		frame.readAsImage(frame_as_img);
		writer.writeFrame(frame_as_img, s3d::SecondsF(1/fps));
	}
};

struct Dubling {
	Mat3x2 affine;
	int32_t N = 6;
	const HSV arrow_color1 = HSV(175, 1.00, 0.87, 0.0);
	const HSV arrow_color2 = Color(22,62,100);
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
		const double x = (i >= N - 2) ? 0.4 : 1.7;
		const double y = std::lerp(line_y_at(i-1), line_y_at(i), t_position);
		const Line line { {-x, y}, {x, y} };
		const size_t line_count = 1ull << (N - 1 - i);
		const ColorF c = arrow_color1.lerp(arrow_color2, t_color);
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
};

void Main()
{
	const HSV outerframe_color = HSV{ 224, 0.44, 0.21 };
	const HSV drawable_region_color = HSV{ 216, 0.06, 1.00 };	
	Scene::SetBackground(drawable_region_color);
	OuterFrame outer_frame;
	Recorder recorder;
	const double anime_length = 5.0;
	Dubling dubling_animation;
	
	while (System::Update()) {
		const Transformer2D transformer2d{ NormalizedCoord(recorder.frame_rect) };
		const double t = Math::Fmod(Scene::Time(), anime_length);
		dubling_animation.draw(t);
		outer_frame.draw(outerframe_color);
	}

	for (int32_t f = 0; f < recorder.fps*anime_length; f++)
	{
		const ScopedRenderTarget2D target{ recorder.frame };
		const Transformer2D transformer2d{ NormalizedCoord(recorder.frame_rect) };
		const double t = Math::Fmod(f / recorder.fps, anime_length);
		
		recorder.clear_frame(drawable_region_color);
		dubling_animation.draw(t);
		outer_frame.draw(outerframe_color);
		
		recorder.shot();
	}
}
