#pragma once

#include "engine/canvas.h"
#include "engine/input.h"

// The program's real entry point.
// Defined in main.cpp, called by the platform layer.
int run_app();

namespace platform {

	bool open_window(const char* title, int width, int height);
	bool pump_messages(Input& input); // returns false once the window has been closed
	void present(const Canvas& canvas);
	void close_window();

} // namespace platform
