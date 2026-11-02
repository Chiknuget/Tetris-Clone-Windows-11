#include "engine/canvas.h"

#include <algorithm>
#include <cstddef>

Canvas::Canvas(int width, int height)
	: w_(width),
	h_(height),
	pixels_(static_cast<std::size_t>(width)* static_cast<std::size_t>(height), 0u) {}

void Canvas::clear(Color c) {
	std::fill(pixels_.begin(), pixels_.end(), c);
}

bool Canvas::clip(int& x0, int& y0, int& x1, int& y1) const {
	x0 += off_x_;
	x1 += off_x_;
	y0 += off_y_;
	y1 += off_y_;
	x0 = std::max(x0, 0);
	y0 = std::max(y0, 0);
	x1 = std::min(x1, w_);
	y1 = std::min(y1, h_);
	return x0 < x1 && y0 < y1;
}

void Canvas::put_pixel(int x, int y, Color c) {
	fill_rect(x, y, 1, 1, c);
}

void Canvas::fill_rect(int x, int y, int w, int h, Color c) {
	int x0 = x, y0 = y, x1 = x + w, y1 = y + h;
	if (!clip(x0, y0, x1, y1)) return;
	for (int py = y0; py < y1; ++py) {
		std::uint32_t* row = &pixels_[static_cast<std::size_t>(py) * w_];
		for (int px = x0; px < x1; ++px) {
			row[px] = c;
		}
	}
}

void Canvas::draw_rect(int x, int y, int w, int h, Color c, int thickness) {
	fill_rect(x, y, w, thickness, c);
	fill_rect(x, y + h - thickness, w, thickness, c);
	fill_rect(x, y, thickness, h, c);
	fill_rect(x + w - thickness, y, thickness, h, c);
}

void Canvas::blend_rect(int x, int y, int w, int h, Color c, float alpha) {
	int x0 = x, y0 = y, x1 = x + w, y1 = y + h;
	if (!clip(x0, y0, x1, y1)) return;
	for (int py = y0; py < y1; ++py) {
		std::uint32_t* row = &pixels_[static_cast<std::size_t>(py) * w_];
		for (int px = x0; px < x1; ++px) {
			row[px] = mix(row[px], c, alpha);
		}
	}
}

void Canvas::draw_block(int x, int y, int size, Color c) {
	const int b = std::max(2, size / 8);
	fill_rect(x, y, size, size, mix(c, rgb(0, 0, 0), 0.45f));
	fill_rect(x, y, size, b, mix(c, rgb(255, 255, 255), 0.45f));
	fill_rect(x, y, b, size, mix(c, rgb(255, 255, 255), 0.30f));
	fill_rect(x + b, y + b, size - 2 * b, size - 2 * b, c);
}
