#include <Siv3D.hpp>
#include "SegmentTree.hpp"
const double anime_length = 5.0;
using AnimationClass = SegmentTree;

struct LightBloom {
    const Size size;
    const RenderTexture blur1;
    const RenderTexture internal1;
    const RenderTexture blur4;
    const RenderTexture internal4;
    const RenderTexture blur8;
    const RenderTexture internal8;
    LightBloom(Size size):
        size(size),
        blur1(size), internal1(size),
        blur4(size/4), internal4(size/4),
        blur8(size/8), internal8(size/8)
    {}

    const RenderTexture& get_render_target() {
        blur1.clear(ColorF{0.0});
        return blur1;
    }
    void draw_bloom() const {
        const ScopedRenderStates2D blend {BlendState::Additive};
        blur1.resized(size).draw(ColorF{0.8});
        blur4.resized(size).draw(ColorF{0.4});
        blur8.resized(size).draw(ColorF{0.25});
    }
    void blur() {
        Shader::GaussianBlur(blur1, internal1, blur1);
        
        Shader::Downsample(blur1, blur4);
        Shader::GaussianBlur(blur4, internal4, blur4);

        Shader::Downsample(blur4, blur8);
        Shader::GaussianBlur(blur8, internal8, blur8);
    }

};


void Main() {
	const HSV outerframe_color = HSV{ 210, 0.10, 0.60 };
	const HSV drawable_region_color = HSV{ 224, 0.44, 0.21 };	
	Scene::SetBackground(drawable_region_color);
	OuterFrame outer_frame;
	Recorder recorder;
	AnimationClass animation;
	LightBloom lightbloom{ recorder.frame_rect.size };

    const auto draw_mainloop = [&](double t) {
        {
            const Transformer2D transformer2d{ NormalizedCoord(recorder.frame_rect) };
		    animation.draw(t);
            {
                const ScopedRenderTarget2D target{ lightbloom.get_render_target() };
                animation.draw_bloom(t);
            }
        }
        lightbloom.blur();
        lightbloom.draw_bloom();
        {
            const Transformer2D transformer2d{ NormalizedCoord(recorder.frame_rect) };
            outer_frame.draw(outerframe_color);
        }
        
    };

	while (System::Update()) {
		ClearPrint();
		const double t = Math::Fmod(Scene::Time(), anime_length);
        draw_mainloop(t);
	}
	
	for (int32_t f = 0; f < recorder.fps*anime_length; f++)
	{
        const ScopedRenderTarget2D target{ recorder.frame };
        recorder.clear_frame(ColorF{0.0});
		const double t = Math::Fmod(f / recorder.fps, anime_length);
        draw_mainloop(t);
		recorder.shot();
	}
}