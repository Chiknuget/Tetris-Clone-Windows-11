#pragma once

#include <algorithm>
#include <cstdint>

// 0x00RRGGBB, which matches the the byte order of a 32-bit Windows bitmap.
using Color = std::uint32_t;

constexpr Color rgb(int r, int g, int b) {
	return (static_cast<Color>(r & 0xFF) << 16) |
		(static_cast<Color>(g & 0xFF) << 8) |
		static_cast<Color>(b & 0xFF);
}

// Linear blend: t = 0 gives a, t = 1 gives b.
inline Color mix(Color a, Color b, float t) {
	auto channel = [&](int shift) {
		const float va = static_cast<float>((a >> shift) & 0xFF);
		const float vb = static_cast<float>((b >> shift) & 0xFF);
		const int v = static_cast<int>(va + (vb - va) * t + 0.5f);
		return static_cast<Color>(std::clamp(v, 0, 255));
		};
	return (channel(16) << 16) | (channel(8) << 8) | channel(0);
}
