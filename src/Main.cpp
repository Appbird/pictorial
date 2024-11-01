# include <Siv3D.hpp> // Siv3D v0.6.15

BlendState MakeBlendState()
{
	BlendState blendState = BlendState::Default2D;
	blendState.srcAlpha = Blend::SrcAlpha;
	blendState.dstAlpha = Blend::DestAlpha;
	blendState.opAlpha = BlendOp::Max;
	return blendState;
}

struct LineArrow{
	const Line line;
	const Color c;
	void draw(
		const double t
	) const {
		const Line current_arrow {
			line.begin, line.position(t)
		};
		current_arrow.drawArrow(0.05, SizeF{0.06, 0.06}, c);
	}

};


struct ArrowsOnLines {
	const Line line;
	const Color c;
	const int32_t margin;
	const size_t N;
	ArrowsOnLines(const Line& _line, const double _margin, const size_t _N, const Color& c):
		line(_line), margin(_margin), N(_N), c(c)
	{}
	void draw(const double t) const {
		for (int32_t i = 0; i < N; i++) {
			const double start = double(i) / N;
			const double end = double(i+1) / N;
			const double margin_in_t = margin / line.length();
			LineArrow{
				{line.position(start + margin_in_t), line.position(end - margin_in_t)},
				c
			}.draw(t);
		}
	}
};

double time_01(double t, double min, double length) {
	return Clamp(t - min, 0., length) / (length);
}

Mat3x2 DrawRegion(RectF region) {
	return Mat3x2::Scale(region.size.y) * Mat3x2::Translate(region.center());
}
RectF CenteredRect() {
	const Point p = Scene::Center();
	const Size s = Scene::Size();
	const double length = s.y;
	return RectF{
		{p.x - length / 2, 0},
		{length, length}
	};
}

template<class Target>
struct Animator {
	Stopwatch s{StartImmediately::No};
	Target target;
	Animator(const Target& _target): target(_target) {}
	void start() const { s.start(); }
	void draw() const { target.draw(s.sF()); }
	
};

struct TimePassenger {
	Stopwatch s;
	double previous_time;
	TimePassenger(StartImmediately yesno): s(yesno) {}
	void start() { s.start(); }
	bool pass_by(double t) {
		const bool result = (previous_time < s.sF() and s.sF() <= t);
		previous_time = t;
		return result;	
	}
};

double x = 0;
void Main()
{
	// 背景の色を設定する | Set the background color
	HSV background_color = HSV{ 224, 0.44, 0.21 };
	Scene::SetBackground(background_color);
	const auto Mat = DrawRegion(CenteredRect());
	Array<ArrowsOnLines> arrows_on_lines;
	int32_t N = 5;
	for (int32_t i = 0; i < N; i++) {
		const double y = (double(i)/(N) - 0.5) * 1.0 + 0.05;
		Line line{{-1.0, y}, {1.0, y}};
		arrows_on_lines.emplace_back(line, 0.005, 1 << (N - 1 - i), HSV{242, 0.62, 0.40});
	}
	const RoundRect drawable_region = RoundRect{RectF{-0.45, -0.45, 0.9, 0.9}, 0.01};
	Polygon indrawable_region = RectF{-1, -1, 2, 2}.asPolygon();
	assert(indrawable_region.addHole(drawable_region));

	const RenderTexture frame;
	AnimatedGIFWriter TextData{U"dst/a.gif", Size{300, 300}};
	for (int32_t f = 0; f < 100; f++)
	{
		frame.clear({0, 0, 0, 0});
		const double t = Math::Fmod(f / 100, 5.0);
		{
			const ScopedRenderTarget2D target{ frame };
			const ScopedRenderStates2D blend{ MakeBlendState() };
			Transformer2D transformer2d{ Mat };

			drawable_region.draw(Palette::White);
			{
				const Mat3x2 translate = Mat3x2::Translate({0, 0.216});
				const Mat3x2 rotate1 = Mat3x2::Rotate(Math::Pi / 5);
				Transformer2D mat{translate*rotate1};
				for (const auto& [i, arrows_on_line] : Indexed(arrows_on_lines)) {
					const double t1 = time_01(t, 1 + (i * 0.5), 0.7);
					arrows_on_line.draw(EaseOutCubic(t1));
				}
			}
			indrawable_region.draw(background_color);
		}
		SimpleGUI::Slider(ToString(x), x, {0, 0}, 120.0);
	}
}
