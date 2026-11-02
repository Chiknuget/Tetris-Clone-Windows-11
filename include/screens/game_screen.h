#pragma once

#include <array>

#include "game/effects.h"
#include "game/gameplay.h"
#include "game/highscores.h"
#include "game/settings.h"
#include "screen.h"
#include "ui/widgets.h"

class GameScreen : public Screen {
public:
    GameScreen(Settings& settings, HighScoreTable& scores);

    void enter() override;
    ScreenId update(const Input& in, double dt) override;
    void draw(Canvas& canvas) const override;

private:
    enum class Mode { Playing, Paused, EnterName, GameOver };

    void new_game();
    void handle_events();
    void on_game_over();

    void draw_board(Canvas& c) const;
    void draw_side_panels(Canvas& c) const;
    void draw_overlay(Canvas& c) const;

    Settings& settings_;
    HighScoreTable& scores_;

    Gameplay game_;
    Effects effects_;
    Mode mode_ = Mode::Playing;

    ui::Menu pause_menu_;
    ui::Menu over_menu_;

    std::array<int, 3> name_{ 0, 0, 0 };  // letters as 0-25
    int name_slot_ = 0;
    int final_rank_ = -1;
};
