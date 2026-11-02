#pragma once

#include "game/highscores.h"
#include "screen.h"

class HighScoresScreen : public Screen {
public:
    explicit HighScoresScreen(const HighScoreTable& scores);

    ScreenId update(const Input& in, double dt) override;
    void draw(Canvas& canvas) const override;

private:
    const HighScoreTable& scores_;
};
