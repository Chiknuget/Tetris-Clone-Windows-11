#pragma once

enum Key : int {
    KeyLeft,
    KeyRight,
    KeyUp,
    KeyDown,
    KeySpace,
    KeyEnter,
    KeyEscape,
    KeyZ,
    KeyX,
    KeyC,
    KeyP,
    KeyR,
    KeyCount
};

struct Input {
    bool down[KeyCount] = {};    // key is currently held
    bool pressed[KeyCount] = {}; // key went down this frame

    void end_frame() {
        for (bool& p : pressed) p = false;
    }
    void release_all() {
        for (bool& d : down) d = false;
    }
};
