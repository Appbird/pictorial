# include <Siv3D.hpp> // Siv3D v0.6.15

struct LineArrow{
	Line line;
	LineArrow(const Line& _line):
		line{_line}
	{}
	void draw(
		const double t
	) const {	
		const Line current_arrow {
			line.begin, line.position(t)
		};
		current_arrow.drawArrow(0.03, SizeF{0.04, 0.04}, Palette::White);
	}
};

struct ArrowsOnLines {
	ArrowsOnLines(Line line, const size_t N) {
		const double total_len = 1.0 / N;
		for (int32_t i = 0; i < N; i++) {
			const double start = double(i) / N;
			const double end = double(i+1) / N;
			arrows.emplace_back(Line{line.position(start), line.position(end)});
		}
	}
	void draw(const double t) const {
		for (const LineArrow arrow : arrows) {
			arrow.draw(t);
		}
	}
	Array<LineArrow> arrows;
};

double time_01(double t, double min, double length) {
	return Clamp(t - min, 0., length) / (length);
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
	Scene::SetBackground(HSV{ 224, 0.44, 0.21 });
	const auto Mat = Mat3x2::Scale(Scene::Size().x) * Mat3x2::Translate(Scene::Size() / 2.0);
	Array<ArrowsOnLines> arrows_on_lines;
	int32_t N = 4;
	const double width = 0.6;
	const double height = 0.6;
	for (int32_t i = 0; i < N; i++) {
		const double y = (double(i)/(N) - 0.5) * height;
		Line line{{-width/2, y}, {width/2, y}};
		arrows_on_lines.emplace_back(line, 1 << (N - 1 - i));
	}
	double prepare_time = 3.0;
	while (System::Update())
	{
		const double t = Math::Fmod(Scene::Time(), 5.0);
		SimpleGUI::Slider(ToString(x), x, {0, 0}, 120.0);
		
		{
			Transformer2D transformer2d{Mat};
			const Mat3x2 transformed = Mat3x2::Translate(0, 0.20);
			const Mat3x2 rotate1 = Mat3x2::Rotate(Math::Pi / 5);
			Transformer2D mat{transformed * rotate1};
			for (const auto& [i, arrows_on_line] : Indexed(arrows_on_lines)) {
				const double t1 = time_01(t, 1 + (i * 0.5), 0.7);
				arrows_on_line.draw(EaseOutCubic(t1));
			}
		}
	}
}
