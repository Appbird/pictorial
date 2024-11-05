#pragma once
#include <Siv3D.hpp>

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