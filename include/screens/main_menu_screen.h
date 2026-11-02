#pragma once

#include <random>
#include <vector>

#include "game/highscores.h"
#include "screen.h"
#include "ui/widgets.h"

class MainMenuScreen : public Screen {
public:
    explicit MainMenuScreen(const HighScoreTable& scores);

    ScreenId update(const Input& in, double dt) override;
    void draw(Canvas& canvas) const override;

private:
    struct Deco {
        int type;
        int rot;
        float x;
        float y;
        float speed;
    };

    void respawn(Deco& d, bool anywhere);

    const HighScoreTable& scores_;
    ui::Menu menu_;
    std::vector<Deco> decos_;
    std::mt19937 rng_;
};
