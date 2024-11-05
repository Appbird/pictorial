#pragma once
# include <Siv3D.hpp> // Siv3D v0.6.15

Mat3x2 NormalizedCoord(RectF region) {
	return Mat3x2::Scale(region.size.y) * Mat3x2::Translate(region.center());
}
double time_01(double t, double min, double length) {
    if (length == 0) { return 0; }
	return Clamp(t - min, 0., length) / (length);
}


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