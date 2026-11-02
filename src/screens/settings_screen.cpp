#include "screens/settings_screen.h"

#include <algorithm>

#include "engine/font.h"
#include "ui/theme.h"
#include "ui/widgets.h"

namespace {
    const char* const kLabels[] = {
        "START LEVEL", "GHOST PIECE", "HANDLING", "PARTICLES", "SCREEN SHAKE", "BACK",
    };
    constexpr int kBackRow = 5;
}  // namespace

SettingsScreen::SettingsScreen(Settings& settings) : settings_(settings) {}

void SettingsScreen::enter() {
    selected_ = 0;
}

void SettingsScreen::adjust(int row, int dir) {
    switch (row) {
    case 0:
        settings_.start_level = std::clamp(settings_.start_level + dir, Settings::kMinLevel, Settings::kMaxLevel);
        break;
    case 1: settings_.ghost_piece = !settings_.ghost_piece; break;
    case 2: settings_.handling = (settings_.handling + dir + 3) % 3; break;
    case 3: settings_.particles = !settings_.particles; break;
    case 4: settings_.screen_shake = !settings_.screen_shake; break;
    default: break;
    }
}

std::string SettingsScreen::value_text(int row) const {
    switch (row) {
    case 0:  return std::to_string(settings_.start_level);
    case 1:  return settings_.ghost_piece ? "ON" : "OFF";
    case 2:  return settings_.handling_name();
    case 3:  return settings_.particles ? "ON" : "OFF";
    case 4:  return settings_.screen_shake ? "ON" : "OFF";
    default: return "";
    }
}

ScreenId SettingsScreen::update(const Input& in, double) {
    if (in.pressed[KeyEscape]) {
        settings_.save(kSettingsFile);
        return ScreenId::MainMenu;
    }
    if (in.pressed[KeyUp]) selected_ = (selected_ + kRowCount - 1) % kRowCount;
    if (in.pressed[KeyDown]) selected_ = (selected_ + 1) % kRowCount;
    if (in.pressed[KeyLeft]) adjust(selected_, -1);
    if (in.pressed[KeyRight]) adjust(selected_, +1);

    if (in.pressed[KeyEnter]) {
        if (selected_ == kBackRow) {
            settings_.save(kSettingsFile);
            return ScreenId::MainMenu;
        }
        adjust(selected_, +1);
    }
    return ScreenId::None;
}

void SettingsScreen::draw(Canvas& c) const {
    c.clear(theme::kBackground);
    ui::draw_title(c, kScreenW / 2, 40, 5);
    ui::draw_text_centered(c, kScreenW / 2, 120, "SETTINGS", theme::kAccent, 3);
    ui::draw_panel(c, 90, 170, 520, 350);

    for (int i = 0; i < kRowCount; ++i) {
        const int y = 200 + i * 52;
        const bool is_selected = (i == selected_);
        if (is_selected) {
            c.fill_rect(100, y - 12, 500, 38, mix(theme::kPanel, theme::kHighlight, 0.15f));
        }
        const Color color = is_selected ? theme::kHighlight : theme::kText;
        font::draw_text(c, 120, y, kLabels[i], color, 2);

        const std::string value = value_text(i);
        if (!value.empty()) {
            ui::draw_text_right(c, 580, y, "< " + value + " >", color, 2);
        }
    }

    ui::draw_text_centered(c, kScreenW / 2, 560, "LEFT/RIGHT: CHANGE", theme::kDim, 2);
    ui::draw_text_centered(c, kScreenW / 2, 585, "ESC: SAVE AND GO BACK", theme::kDim, 2);
}
