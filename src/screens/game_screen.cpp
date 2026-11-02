#include "screens/game_screen.h"

#include <cmath>
#include <string>

#include "engine/font.h"
#include "game/scoring.h"
#include "ui/theme.h"

namespace {

    constexpr int kCell = 30;
    constexpr int kBoardX = 200;
    constexpr int kBoardY = 50;
    constexpr int kBoardW = Board::kCols * kCell;
    constexpr int kBoardH = Board::kVisibleRows * kCell;
    constexpr int kLeftX = 30;
    constexpr int kRightX = 520;
    constexpr int kPanelW = 150;
    constexpr float kPi = 3.14159265f;

    int cell_x(int col) {
        return kBoardX + col * kCell;
    }

    int cell_y(int row) {
        return kBoardY + (row - Board::kHiddenRows) * kCell;
    }

    void draw_cell(Canvas& c, int col, int row, Color color) {
        if (row < Board::kHiddenRows) return;
        c.draw_block(cell_x(col) + 1, cell_y(row) + 1, kCell - 1, color);
    }

    void draw_preview(Canvas& c, int type, int x, int y, int w, int h, int cell, bool dim) {
        const std::array<Cell, 4> cells = pieces::shape(type, 0);
        int minx = 4, maxx = -1, miny = 4, maxy = -1;
        for (const Cell& k : cells) {
            minx = std::min(minx, k.x);
            maxx = std::max(maxx, k.x);
            miny = std::min(miny, k.y);
            maxy = std::max(maxy, k.y);
        }
        const int pw = (maxx - minx + 1) * cell;
        const int ph = (maxy - miny + 1) * cell;
        const int ox = x + (w - pw) / 2 - minx * cell;
        const int oy = y + (h - ph) / 2 - miny * cell;

        Color color = pieces::color(type);
        if (dim) color = mix(color, rgb(90, 90, 90), 0.7f);
        for (const Cell& k : cells) {
            c.draw_block(ox + k.x * cell, oy + k.y * cell, cell - 1, color);
        }
    }

    void draw_stat(Canvas& c, int x, int y, const char* label, long long value) {
        font::draw_text(c, x, y, label, theme::kAccent, 2);
        font::draw_text(c, x, y + 22, std::to_string(value), theme::kText, 3);
    }

}  // namespace

GameScreen::GameScreen(Settings& settings, HighScoreTable& scores)
    : settings_(settings), scores_(scores) {
}

void GameScreen::enter() {
    new_game();
}

void GameScreen::new_game() {
    GameConfig config;
    config.start_level = settings_.start_level;
    config.das = settings_.das_seconds();
    config.arr = settings_.arr_seconds();
    game_.start(config);
    effects_.clear();
    mode_ = Mode::Playing;
    final_rank_ = -1;
}

void GameScreen::on_game_over() {
    over_menu_.selected = 0;
    final_rank_ = -1;
    if (scores_.qualifies(game_.score())) {
        mode_ = Mode::EnterName;
        name_slot_ = 0;
    }
    else {
        mode_ = Mode::GameOver;
    }
}

void GameScreen::handle_events() {
    for (const GameEvent& e : game_.take_events()) {
        switch (e.type) {
        case GameEventType::LinesCleared: {
            int row_sum = 0;
            for (const Cell& cell : e.cells) {
                row_sum += cell.y;
                if (settings_.particles && cell.y >= Board::kHiddenRows) {
                    // Cleared rows remain on the board during the animation, preserving colors.
                    const int v = game_.board().cell(cell.x, cell.y);
                    const Color color = v > 0 ? pieces::color(v - 1) : theme::kWhite;
                    effects_.burst(static_cast<float>(cell_x(cell.x) + kCell / 2),
                        static_cast<float>(cell_y(cell.y) + kCell / 2),
                        color, 5, 260.0f);
                }
            }
            const int mid_row = e.cells.empty() ? Board::kRows / 2
                : row_sum / static_cast<int>(e.cells.size());
            const float px = static_cast<float>(kBoardX + kBoardW / 2);
            const float py = static_cast<float>(cell_y(mid_row));
            effects_.popup(scoring::line_clear_name(e.value), px, py - 10.0f,
                theme::kHighlight, e.value == 4 ? 4 : 3);
            effects_.popup("+" + std::to_string(e.points), px, py + 30.0f, theme::kText, 2);
            if (settings_.screen_shake) {
                effects_.shake(e.value == 4 ? 8.0f : 2.0f * static_cast<float>(e.value), 0.3f);
            }
            break;
        }
        case GameEventType::HardDrop:
            if (settings_.particles && e.value > 0) {
                for (const Cell& cell : e.cells) {
                    effects_.burst(static_cast<float>(cell_x(cell.x) + kCell / 2),
                        static_cast<float>(cell_y(cell.y) + kCell),
                        theme::kDim, 2, 120.0f);
                }
            }
            if (settings_.screen_shake && e.value > 2) effects_.shake(3.0f, 0.12f);
            break;
        case GameEventType::LevelUp:
            effects_.popup("LEVEL " + std::to_string(e.value),
                static_cast<float>(kBoardX + kBoardW / 2),
                static_cast<float>(kBoardY + kBoardH / 2 - 60),
                theme::kAccent, 3);
            break;
        default:
            break;
        }
    }
}

ScreenId GameScreen::update(const Input& in, double dt) {
    switch (mode_) {
    case Mode::Playing:
        if (in.pressed[KeyEscape] || in.pressed[KeyP]) {
            mode_ = Mode::Paused;
            pause_menu_.selected = 0;
            return ScreenId::None;
        }
        if (in.pressed[KeyR]) {
            new_game();
            return ScreenId::None;
        }
        game_.update(in, dt);
        handle_events();
        effects_.update(dt);
        if (game_.state() == Gameplay::State::GameOver) on_game_over();
        return ScreenId::None;

    case Mode::Paused:
        if (in.pressed[KeyEscape] || in.pressed[KeyP]) {
            mode_ = Mode::Playing;
            return ScreenId::None;
        }
        if (pause_menu_.update(in, 3)) {
            if (pause_menu_.selected == 0) mode_ = Mode::Playing;
            else if (pause_menu_.selected == 1) new_game();
            else return ScreenId::MainMenu;
        }
        return ScreenId::None;

    case Mode::EnterName:
        effects_.update(dt);
        if (in.pressed[KeyUp]) name_[name_slot_] = (name_[name_slot_] + 1) % 26;
        if (in.pressed[KeyDown]) name_[name_slot_] = (name_[name_slot_] + 25) % 26;
        if (in.pressed[KeyLeft] && name_slot_ > 0) --name_slot_;
        if (in.pressed[KeyRight] && name_slot_ < 2) ++name_slot_;
        if (in.pressed[KeyEnter]) {
            HighScoreEntry entry;
            for (int letter : name_) {
                entry.name.push_back(static_cast<char>('A' + letter));
            }
            entry.score = game_.score();
            entry.level = game_.level();
            entry.lines = game_.lines();
            final_rank_ = scores_.insert(entry);
            scores_.save(kHighScoreFile);
            mode_ = Mode::GameOver;
        }
        return ScreenId::None;

    case Mode::GameOver:
        effects_.update(dt);
        if (in.pressed[KeyEscape]) return ScreenId::MainMenu;
        if (over_menu_.update(in, 2)) {
            if (over_menu_.selected == 0) new_game();
            else return ScreenId::MainMenu;
        }
        return ScreenId::None;
    }
    return ScreenId::None;
}

// Drawing

void GameScreen::draw_board(Canvas& c) const {
    const Board& board = game_.board();

    c.fill_rect(kBoardX - 4, kBoardY - 4, kBoardW + 8, kBoardH + 8, theme::kPanelBorder);
    c.fill_rect(kBoardX, kBoardY, kBoardW, kBoardH, theme::kBoardBg);
    for (int x = 1; x < Board::kCols; ++x) {
        c.fill_rect(kBoardX + x * kCell, kBoardY, 1, kBoardH, theme::kGrid);
    }
    for (int y = 1; y < Board::kVisibleRows; ++y) {
        c.fill_rect(kBoardX, kBoardY + y * kCell, kBoardW, 1, theme::kGrid);
    }

    for (int row = Board::kHiddenRows; row < Board::kRows; ++row) {
        for (int col = 0; col < Board::kCols; ++col) {
            const int v = board.cell(col, row);
            if (v != 0) draw_cell(c, col, row, pieces::color(v - 1));
        }
    }

    // Line-clear animation: rows flash, then get wiped from the center outward.
    if (game_.state() == Gameplay::State::Clearing) {
        const float p = game_.clear_progress();
        const float flash = 0.8f * (0.5f + 0.5f * std::cos(p * kPi * 6.0f)) * (1.0f - p);
        const int wipe = static_cast<int>(p * static_cast<float>(kBoardW));
        for (int row : game_.clearing_rows()) {
            if (row < Board::kHiddenRows) continue;
            const int y = cell_y(row);
            c.blend_rect(kBoardX, y, kBoardW, kCell, theme::kWhite, flash);
            c.fill_rect(kBoardX + (kBoardW - wipe) / 2, y, wipe, kCell, theme::kBoardBg);
        }
    }

    if (game_.state() != Gameplay::State::Falling) return;

    const Piece& cur = game_.current();
    const Color color = pieces::color(cur.type);

    if (settings_.ghost_piece) {
        for (const Cell& cell : pieces::cells(game_.ghost())) {
            if (cell.y < Board::kHiddenRows) continue;
            c.draw_rect(cell_x(cell.x) + 2, cell_y(cell.y) + 2, kCell - 3, kCell - 3,
                mix(color, theme::kBoardBg, 0.5f), 2);
        }
    }

    for (const Cell& cell : pieces::cells(cur)) {
        draw_cell(c, cell.x, cell.y, color);
    }
}

void GameScreen::draw_side_panels(Canvas& c) const {
    font::draw_text(c, kLeftX, 50, "HOLD", theme::kAccent, 2);
    ui::draw_panel(c, kLeftX, 72, kPanelW, 100);
    if (game_.hold_piece() >= 0) {
        draw_preview(c, game_.hold_piece(), kLeftX, 72, kPanelW, 100, 24, game_.hold_used());
    }
    draw_stat(c, kLeftX, 210, "SCORE", game_.score());
    draw_stat(c, kLeftX, 290, "LEVEL", game_.level());
    draw_stat(c, kLeftX, 370, "LINES", game_.lines());

    font::draw_text(c, kRightX, 50, "NEXT", theme::kAccent, 2);
    ui::draw_panel(c, kRightX, 72, kPanelW, 360);
    for (int i = 0; i < Randomizer::kPreviewCount; ++i) {
        draw_preview(c, game_.preview(i), kRightX, 72 + i * 72, kPanelW, 72, i == 0 ? 24 : 20, false);
    }

    const char* help[] = {
        "ARROWS MOVE", "DOWN SOFT", "SPACE/X ROTATE", "Z ROT LEFT",
        "UP DROP",  "C HOLD",    "ESC PAUSE",   "R RESTART",
    };
    int y = 460;
    for (const char* line : help) {
        font::draw_text(c, kRightX, y, line, theme::kDim, 2);
        y += 20;
    }
}

void GameScreen::draw_overlay(Canvas& c) const {
    if (mode_ == Mode::Playing) return;

    c.blend_rect(kBoardX, kBoardY, kBoardW, kBoardH, theme::kBlack, 0.7f);
    const int cx = kBoardX + kBoardW / 2;
    const int cy = kBoardY + kBoardH / 2;

    switch (mode_) {
    case Mode::Paused:
        ui::draw_text_centered(c, cx, cy - 100, "PAUSED", theme::kText, 4);
        pause_menu_.draw(c, cx, cy - 20, { "RESUME", "RESTART", "MAIN MENU" }, 2, 36);
        break;

    case Mode::EnterName:
        ui::draw_text_centered(c, cx, cy - 150, "NEW HIGH SCORE!", theme::kHighlight, 2);
        ui::draw_text_centered(c, cx, cy - 115, std::to_string(game_.score()), theme::kText, 3);
        ui::draw_text_centered(c, cx, cy - 60, "ENTER NAME", theme::kAccent, 2);
        for (int i = 0; i < 3; ++i) {
            const int slot_x = cx - 50 + i * 50;
            const bool is_selected = (i == name_slot_);
            const char letter[2] = { static_cast<char>('A' + name_[static_cast<std::size_t>(i)]), '\0' };
            font::draw_text(c, slot_x - 15, cy - 25, letter,
                is_selected ? theme::kHighlight : theme::kText, 6);
            if (is_selected) c.fill_rect(slot_x - 18, cy + 22, 36, 4, theme::kHighlight);
        }
        ui::draw_text_centered(c, cx, cy + 60, "UP/DOWN: LETTER", theme::kDim, 2);
        ui::draw_text_centered(c, cx, cy + 82, "LEFT/RIGHT: MOVE", theme::kDim, 2);
        ui::draw_text_centered(c, cx, cy + 104, "ENTER: SAVE", theme::kDim, 2);
        break;

    case Mode::GameOver:
        ui::draw_text_centered(c, cx, cy - 130, "GAME OVER", theme::kText, 4);
        ui::draw_text_centered(c, cx, cy - 70, "SCORE", theme::kAccent, 2);
        ui::draw_text_centered(c, cx, cy - 45, std::to_string(game_.score()), theme::kText, 3);
        if (final_rank_ >= 0) {
            ui::draw_text_centered(c, cx, cy, "RANK " + std::to_string(final_rank_ + 1),
                theme::kHighlight, 2);
        }
        over_menu_.draw(c, cx, cy + 50, { "PLAY AGAIN", "MAIN MENU" }, 2, 36);
        break;

    default:
        break;
    }
}

void GameScreen::draw(Canvas& c) const {
    c.clear(theme::kBackground);
    ui::draw_title(c, kScreenW / 2, 10, 4);
    draw_side_panels(c);

    c.set_offset(effects_.shake_x(), effects_.shake_y());
    draw_board(c);
    effects_.draw(c);
    c.set_offset(0, 0);

    draw_overlay(c);
}
