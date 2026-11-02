#include <chrono>
#include <thread>
#include <memory>

#include "app.h"
#include "engine/canvas.h"
#include "engine/input.h"
#include "platform/window.h"
#include "screens/screen.h"

int run_app() {
	if (!platform::open_window("Tetris", kScreenW, kScreenH)) return 1;

	Canvas canvas(kScreenW, kScreenH);
	Input input;
	auto app = std::make_unique<App>();

	using Clock = std::chrono::steady_clock;
	const auto frame_time = std::chrono::microseconds(16667); // roughly 60 FPS
	auto last = Clock::now();
	auto next_frame = last;

	while (platform::pump_messages(input)) {
		const auto now = Clock::now();
		double dt = std::chrono::duration<double>(now - last).count();
		last = now;
		if (dt > 0.1) dt = 0.1; // avoid huge jumps after dragging or minimizing the window

		if (!app->update(input, dt)) break;
		input.end_frame();

		app->draw(canvas);
		platform::present(canvas);

		next_frame += frame_time;
		if (next_frame < Clock::now()) next_frame = Clock::now();
		std::this_thread::sleep_until(next_frame);
	}

	platform::close_window();
	return 0;
}
