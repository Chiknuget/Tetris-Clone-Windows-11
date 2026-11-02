#include "ui/widgets.h"

#include "engine/font.h"
#include "game/piece.h"
#include "ui/theme.h"

namespace ui {

    void draw_text_centered(Canvas& c, int center_x, int y, std::string_view text, Color color, int scale) {
        font::draw_text(c, center_x - font::text_width(text, scale) / 2, y, text, color, scale);
    }

    void draw_text_right(Canvas& c, int right_x, int y, std::string_view text, Color color, int scale) {
        font::draw_text(c, right_x - font::text_width(text, scale), y, text, color, scale);
    }

    void draw_panel(Canvas& c, int x, int y, int w, int h) {
        c.fill_rect(x, y, w, h, theme::kPanelBorder);
        c.fill_rect(x + 2, y + 2, w - 4, h - 4, theme::kPanel);
    }

    void draw_title(Canvas& c, int center_x, int y, int scale) {
        const char letters[] = "TETRIS";
        const Color colors[] = {
            pieces::color(PIECE_Z), pieces::color(PIECE_L), pieces::color(PIECE_O),
            pieces::color(PIECE_S), pieces::color(PIECE_I), pieces::color(PIECE_T),
        };
        int x = center_x - font::text_width(letters, scale) / 2;
        for (int i = 0; i < 6; ++i) {
            font::draw_char(c, x + scale, y + scale, letters[i], mix(colors[i], theme::kBlack, 0.6f), scale);
            font::draw_char(c, x, y, letters[i], colors[i], scale);
            x += font::kAdvance * scale;
        }
    }

    bool Menu::update(const Input& in, int count) {
        if (count <= 0) return false;
        if (in.pressed[KeyUp]) selected = (selected + count - 1) % count;
        if (in.pressed[KeyDown]) selected = (selected + 1) % count;
        return in.pressed[KeyEnter];
    }

    void Menu::draw(Canvas& c, int center_x, int y, const std::vector<std::string>& items,
        int scale, int spacing) const {
        for (int i = 0; i < static_cast<int>(items.size()); ++i) {
            const std::string& item = items[static_cast<std::size_t>(i)];
            const bool is_selected = (i == selected);
            const Color color = is_selected ? theme::kHighlight : theme::kText;
            const int ty = y + i * spacing;
            draw_text_centered(c, center_x, ty, item, color, scale);
            if (is_selected) {
                const int w = font::text_width(item, scale);
                font::draw_text(c, center_x - w / 2 - font::kAdvance * scale * 2, ty, ">", color, scale);
                font::draw_text(c, center_x + w / 2 + font::kAdvance * scale, ty, "<", color, scale);
            }
        }
    }

}  // namespace ui
