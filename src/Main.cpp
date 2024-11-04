#include <Siv3D.hpp>
#include "SegmentTree.hpp"
const double anime_length = 5.0;
using AnimationClass = SegmentTree;

void Main() {
	const HSV outerframe_color = HSV{ 210, 0.10, 0.60 };
	const HSV drawable_region_color = HSV{ 224, 0.44, 0.21 };	
	Scene::SetBackground(drawable_region_color);
	OuterFrame outer_frame;
	Recorder recorder;
	AnimationClass animation;
	
	while (System::Update()) {
		ClearPrint();
		const Transformer2D transformer2d{ NormalizedCoord(recorder.frame_rect) };
		const double t = Math::Fmod(Scene::Time(), anime_length);
		animation.draw(t);
		outer_frame.draw(outerframe_color);
	}
	
	for (int32_t f = 0; f < recorder.fps*anime_length; f++)
	{
		const ScopedRenderTarget2D target{ recorder.frame };
		const Transformer2D transformer2d{ NormalizedCoord(recorder.frame_rect) };
		const double t = Math::Fmod(f / recorder.fps, anime_length);

		recorder.clear_frame(drawable_region_color);
		animation.draw(t);
		outer_frame.draw(outerframe_color);
		
		recorder.shot();
	}
}