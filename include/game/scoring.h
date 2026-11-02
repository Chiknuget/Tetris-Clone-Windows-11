#pragma once

namespace scoring {

	constexpr int kSoftDropPerCell = 1;
	constexpr int kHardDropPerCell = 2;

	long long line_clear_points(int lines, int level);
	const char* line_clear_name(int lines);
	double gravity_interval(int level); // seconds per row
	int level_for_lines(int lines, int start_level);

} // namespace scoring
