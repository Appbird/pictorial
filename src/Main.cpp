# include <Siv3D.hpp>
# include "LightBloom.hpp"
# include "utility.hpp"
# include "BFS.hpp"
# include "Interval.hpp"
using AnimationClass = BFS;

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
	Recorder recorder;
	LightBloom lightbloom{ recorder.frame_rect.size };
    MSRenderTexture render_texture{ recorder.frame_rect.size };
    RoundRect mini_window{RectF::FromPoints({-0.475, -0.475}, {0.475, 0.475}), 0.01};
	AnimationClass animation;
	Scene::SetBackground(animation.drawable_region_color);

    const auto draw_mainloop = [&](double t) {
        {
            const ScopedRenderTarget2D target{ render_texture.clear(animation.drawable_region_color) };
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
		const double t = Math::Fmod(Scene::Time(), animation.anime_length);
        draw_mainloop(t);
	}
	
	for (int32_t f = 0; f < recorder.fps*animation.anime_length; f++)
	{
        const ScopedRenderTarget2D target{ recorder.frame };
        const ScopedRenderStates2D state{ MakeBlendState() };
        recorder.clear_frame(ColorF{0.0, 0.0});
		const double t = Math::Fmod(f / recorder.fps, animation.anime_length);
        draw_mainloop(t);
		recorder.shot();
	}
}