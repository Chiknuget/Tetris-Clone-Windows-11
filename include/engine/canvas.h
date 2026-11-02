#pragma once

#include <cstdint>
#include <vector>

#include "color.h"

// A block of pixels in memory that everything draws into.
class Canvas {
public:
	Canvas(int width, int height);

	int width() const { return w_; }
	int height() const { return h_; }
	const std::uint32_t* data()const { return pixels_.data(); }

	// Shifts everything drawn afterwards (used for screen shake).
	void set_offset(int dx, int dy) {
		off_x_ = dx;
		off_y_ = dy;
	}

	void clear(Color c);
	void put_pixel(int x, int y, Color c);
	void fill_rect(int x, int y, int w, int h, Color c);
	void draw_rect(int x, int y, int w, int h, Color c, int thickness = 1);
	void blend_rect(int x, int y, int w, int h, Color c, float alpha);
	void draw_block(int x, int y, int size, Color c); // bevelled title

private:
	// Applies the offset and clips to the canvas. Returns false if nothing is left.
	bool clip(int& x0, int& y0, int& x1, int& y1) const;

	int w_;
	int h_;
	int off_y_ = 0;
	int off_x_ = 0;
	std::vector<std::uint32_t> pixels_;
};
