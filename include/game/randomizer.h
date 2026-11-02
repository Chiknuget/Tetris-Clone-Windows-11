#pragma once

#include <deque>
#include <random>
#include <vector>

// 7 bag randomizer: every 7 pieces contain each piece exactly once
class Randomizer {
public:
	static constexpr int kPreviewCount = 5;

	Randomizer();
	void reset();
	int next();
	int peek(int index) const;

private:
	int from_bag();
	void refill();

	std::mt19937 rng_;
	std::vector<int> bag_;
	std::deque<int> queue_;
};
