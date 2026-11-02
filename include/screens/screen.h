#pragma once

#include "engine/canvas.h"
#include "engine/input.h"

constexpr int kScreenW = 700;
constexpr int kScreenH = 700;

enum class ScreenId {
    None, 
    MainMenu,
    Settings,
    Game,
    HighScores,
    Quit
};

class Screen {
public:
    virtual ~Screen() = default;
    virtual void enter() {}
    // Returns the screen to switch to, or ScreenId::None to stay.
    virtual ScreenId update(const Input& in, double dt) = 0;
    virtual void draw(Canvas& canvas) const = 0;
};
