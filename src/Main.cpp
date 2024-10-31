# include <Siv3D.hpp> // Siv3D v0.6.15

struct Region {
	RectF bound;
	double angle;
};
double arg(const Vec2& v) {
	return atan2(v.y, v.x);
}


struct Item {
	void draw_item(const Region& r) {
		r.bound.rotated(r.angle).draw(Palette::White);
	}
	Item(){}
};


struct LineUp {
	Array<Item> items;
	void draw(const Region& r) {
		const size_t count = items.size();
		const double item_space = r.bound.w / count;
		Transformer2D transformer2d{
			Mat3x2::Translate(r.bound.centerX())
			* Mat3x2::Rotate( r.angle )
			* Mat3x2::Scale()
		};
		for (size_t i = 0; i < count; i++) {
			Region region = { {Vec2((double)i / count, 0), {0.8 / item_space, 0.2}}, 0};
			Item{}.draw_item(region);
		}
	}
};

void Main()
{
	// 背景の色を設定する | Set the background color
	Scene::SetBackground(HSV{ 224, 0.44, 0.21 });
	const auto Mat = Mat3x2::Scale(Scene::Size().x);
	while (System::Update())
	{
		{
			Transformer2D transformer2d{Mat};
			LineUp{Line{{0.10, 0.50}, {0.90, 0.50}}, Array<Item>{ {}, {}, {} }}.draw();
		}
	}
}
