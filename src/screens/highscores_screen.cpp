#include "screens/highscores_screen.h"

#include <string>

#include "engine/font.h"
#include "ui/theme.h"
#include "ui/widgets.h"

HighScoresScreen::HighScoresScreen(const HighScoreTable& scores) : scores_(scores) {}

ScreenId HighScoresScreen::update(const Input& in, double) {
    if (in.pressed[KeyEnter] || in.pressed[KeyEscape]) return ScreenId::MainMenu;
    return ScreenId::None;
}

void HighScoresScreen::draw(Canvas& c) const {
    c.clear(theme::kBackground);
    ui::draw_title(c, kScreenW / 2, 40, 5);
    ui::draw_text_centered(c, kScreenW / 2, 120, "HIGH SCORES", theme::kAccent, 3);
    ui::draw_panel(c, 80, 170, 540, 420);

    // Column headers
    font::draw_text(c, 110, 190, "#", theme::kDim, 2);
    font::draw_text(c, 160, 190, "NAME", theme::kDim, 2);
    ui::draw_text_right(c, 430, 190, "SCORE", theme::kDim, 2);
    ui::draw_text_right(c, 500, 190, "LV", theme::kDim, 2);
    ui::draw_text_right(c, 590, 190, "LINES", theme::kDim, 2);

    const auto& entries = scores_.entries();
    if (entries.empty()) {
        ui::draw_text_centered(c, kScreenW / 2, 360, "NO SCORES YET", theme::kText, 2);
    }

    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
        const HighScoreEntry& e = entries[static_cast<std::size_t>(i)];
        const int y = 230 + i * 34;
        const Color color = (i == scores_.last_inserted()) ? theme::kHighlight : theme::kText;
        font::draw_text(c, 110, y, std::to_string(i + 1), color, 2);
        font::draw_text(c, 160, y, e.name, color, 2);
        ui::draw_text_right(c, 430, y, std::to_string(e.score), color, 2);
        ui::draw_text_right(c, 500, y, std::to_string(e.level), color, 2);
        ui::draw_text_right(c, 590, y, std::to_string(e.lines), color, 2);
    }

    ui::draw_text_centered(c, kScreenW / 2, 620, "ENTER OR ESC TO RETURN", theme::kDim, 2);
}
