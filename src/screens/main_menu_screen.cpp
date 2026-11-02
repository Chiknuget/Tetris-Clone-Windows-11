#include "screens/main_menu_screen.h"

#include <string>

#include "game/piece.h"
#include "ui/theme.h"

namespace {
    constexpr int kDecoCell = 24;
    constexpr int kDecoCount = 14;
}  // namespace

MainMenuScreen::MainMenuScreen(const HighScoreTable& scores)
    : scores_(scores), rng_(std::random_device{}()) {
    decos_.resize(kDecoCount);
    for (Deco& d : decos_) {
        respawn(d, true);
    }
}

void MainMenuScreen::respawn(Deco& d, bool anywhere) {
    std::uniform_int_distribution<int> type_dist(0, PIECE_COUNT - 1);
    std::uniform_int_distribution<int> rot_dist(0, 3);
    std::uniform_real_distribution<float> x_dist(-40.0f, static_cast<float>(kScreenW));
    std::uniform_real_distribution<float> speed_dist(25.0f, 70.0f);

    d.type = type_dist(rng_);
    d.rot = rot_dist(rng_);
    d.x = x_dist(rng_);
    if (anywhere) {
        std::uniform_real_distribution<float> y_dist(-static_cast<float>(kScreenH), static_cast<float>(kScreenH));
        d.y = y_dist(rng_);
    }
    else {
        std::uniform_real_distribution<float> y_dist(-200.0f, -80.0f);
        d.y = y_dist(rng_);
    }
    d.speed = speed_dist(rng_);
}

ScreenId MainMenuScreen::update(const Input& in, double dt) {
    for (Deco& d : decos_) {
        d.y += d.speed * static_cast<float>(dt);
        if (d.y > static_cast<float>(kScreenH)) respawn(d, false);
    }

    if (in.pressed[KeyEscape]) return ScreenId::Quit;

    if (menu_.update(in, 4)) {
        switch (menu_.selected) {
        case 0:  return ScreenId::Game;
        case 1:  return ScreenId::Settings;
        case 2:  return ScreenId::HighScores;
        default: return ScreenId::Quit;
        }
    }
    return ScreenId::None;
}

void MainMenuScreen::draw(Canvas& c) const {
    c.clear(theme::kBackground);

    for (const Deco& d : decos_) {
        const Color color = mix(pieces::color(d.type), theme::kBackground, 0.75f);
        for (const Cell& cell : pieces::shape(d.type, d.rot)) {
            c.draw_block(static_cast<int>(d.x) + cell.x * kDecoCell,
                static_cast<int>(d.y) + cell.y * kDecoCell,
                kDecoCell - 1, color);
        }
    }

    ui::draw_title(c, kScreenW / 2, 120, 8);
    ui::draw_panel(c, 190, 280, 320, 240);
    menu_.draw(c, kScreenW / 2, 310, { "PLAY", "SETTINGS", "HIGH SCORES", "QUIT" }, 3, 50);

    if (scores_.best() > 0) {
        ui::draw_text_centered(c, kScreenW / 2, 560, "BEST " + std::to_string(scores_.best()),
            theme::kAccent, 2);
    }
    ui::draw_text_centered(c, kScreenW / 2, 640, "ARROWS + ENTER", theme::kDim, 2);
}
