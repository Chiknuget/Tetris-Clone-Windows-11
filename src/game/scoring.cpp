#include "game/scoring.h"

#include <algorithm>
#include <cmath>

namespace scoring {

	long long line_clear_points(int lines, int level) {
		static constexpr int kBase[5] = { 0, 100, 300, 500, 800 };
		if (lines < 0 || lines > 4) return 0;
		return static_cast<long long>(kBase[lines]) * level;
	}

	const char* line_clear_name(int lines) {
		switch (lines) {
		case 1: return "SINGLE";
		case 2: return "DOUBLE";
		case 3: return "TRIPLE";
		case 4: return "TETRIS!";
		default: return "";
		}
	}

	double gravity_interval(int level) {
		const int lv = std::clamp(level, 1, 20);
		return std::pow(0.8 - (lv - 1) * 0.007, lv - 1);
	}

	int level_for_lines(int lines, int start_level) {
		return start_level + lines / 10;
	}

} // namespace sorting
