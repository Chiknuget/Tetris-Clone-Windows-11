#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "engine/canvas.h"
#include "engine/input.h"

namespace ui {

    void draw_text_centered(Canvas& c, int center_x, int y, std::string_view text, Color color, int scale);
    void draw_text_right(Canvas& c, int right_x, int y, std::string_view text, Color color, int scale);
    void draw_panel(Canvas& c, int x, int y, int w, int h);
    void draw_title(Canvas& c, int center_x, int y, int scale);

    // A vertical list of options navigated with Up/Down and confirmed with Enter.
    class Menu {
    public:
        int selected = 0;

        bool update(const Input& in, int count);  // true when Enter is pressed
        void draw(Canvas& c, int center_x, int y, const std::vector<std::string>& items,
            int scale, int spacing) const;
    };

}  // namespace ui
