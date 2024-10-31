# include <Siv3D.hpp> // Siv3D v0.6.15


struct Region {
	RectF bound;
};

struct Item {
	void draw_item(const Region& r) {
		r.bound.draw(Palette::White);
	}
	Item(){}
};

// Locator
struct LineUp {
	Array<Item> items;
	void draw(const Region& r) {
		const size_t count = items.size();
		const double item_space = r.bound.w / count;
		
		for (size_t i = 0; i < count; i++) {
			Region region = { RectF{
				Vec2{(double)i / count * r.bound.w, r.bound.h / 2},
				Vec2{item_space * 0.8, r.bound.h / 2}
			}};
			Item{}.draw_item(region);
		}
	}
};


void Tester() {
	const auto Mat = Mat3x2::Scale(Scene::Size().x);
	Region region = {{{0.0, 0.40}, {1.0, 0.50}}};
	LineUp lineup{ {{}, {}, {}, {}, {}} };
	while (System::Update())
	{
		{
			Transformer2D transformer2d{Mat};
			lineup.draw(region);
		}
	}
}