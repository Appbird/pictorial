#include <Siv3D.hpp>
#include "LightBloom.hpp"
#include "Dubling.hpp"
using AnimationClass = Dubling;

// 描画された最大のアルファ成分を保持するブレンドステートを作成する
BlendState MakeBlendState()
{
	BlendState blendState = BlendState::Default2D;
	blendState.srcAlpha = Blend::SrcAlpha;
	blendState.dstAlpha = Blend::DestAlpha;
	blendState.opAlpha = BlendOp::Max;
	return blendState;
}

void Main() {
	const HSV drawable_region_color = HSV{ 235, 0.35, 0.20 };	
	Scene::SetBackground(drawable_region_color);
	Recorder recorder;
	AnimationClass animation;
	LightBloom lightbloom{ recorder.frame_rect.size };
    MSRenderTexture render_texture{ recorder.frame_rect.size };
    RoundRect mini_window{RectF{-0.45, -0.45, 0.9, 0.9}, 0.01};

    const auto draw_mainloop = [&](double t) {
        {
            const ScopedRenderTarget2D target{ render_texture.clear(drawable_region_color) };
            {
                const Transformer2D transformer2d{ NormalizedCoord(recorder.frame_rect) };
                animation.draw(t);
                // Bloomを書く。
                {
                    const ScopedRenderTarget2D target{ lightbloom.get_render_target() };
                    animation.draw_bloom(t);
                }
            }
            lightbloom.blur();
            lightbloom.draw_bloom();
        }
        Graphics2D::Flush();
        render_texture.resolve();
        {
            const Transformer2D transformer2d{ NormalizedCoord(recorder.frame_rect) };
            mini_window(render_texture.uv(RectF{0, 0, 1, 1})).draw();
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
        const ScopedRenderStates2D state{ MakeBlendState() };
        recorder.clear_frame(ColorF{0.0, 0.0});
		const double t = Math::Fmod(f / recorder.fps, anime_length);
        draw_mainloop(t);
		recorder.shot();
	}
}