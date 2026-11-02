#pragma once

#include <array>
#include <vector>

#include "game/piece.h"

class Board {
public:
    static constexpr int kCols = 10;
    static constexpr int kRows = 22;
    static constexpr int kHiddenRows = 2;
    static constexpr int kVisibleRows = kRows - kHiddenRows;

    void clear();
    bool fits(const Piece& p) const;
    int cell(int col, int row) const { return cells_[row][col]; }  // 0 = empty, else type + 1

    // Writes the piece into the grid. Returns true if it landed entirely in the hidden rows.
    bool place(const Piece& p);

    std::vector<int> full_rows() const;
    void remove_rows(const std::vector<int>& rows);
    int drop_distance(const Piece& p) const;

private:
    std::array<std::array<int, kCols>, kRows> cells_{};
};
