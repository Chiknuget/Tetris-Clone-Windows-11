#pragma once

#include <string_view>

#include "canvas.h"

namespace font {

	constexpr int kGlyphW = 5;
	constexpr int kGlyphH = 7;
	constexpr int kAdvance = 6; // glyph width + 1 pixel gap

	void draw_char(Canvas& canvas, int x, int y, char ch, Color c, int scale);
	void draw_text(Canvas& canvas, int x, int y, std::string_view text, Color c, int scale);
	int text_width(std::string_view text, int scale);

} // namespace font
