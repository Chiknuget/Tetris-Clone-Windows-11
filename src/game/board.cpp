#include "game/board.h"

#include <algorithm>

void Board::clear() {
    for (auto& row : cells_) {
        row.fill(0);
    }
}

bool Board::fits(const Piece& p) const {
    for (const Cell& c : pieces::cells(p)) {
        if (c.x < 0 || c.x >= kCols || c.y < 0 || c.y >= kRows) return false;
        if (cells_[c.y][c.x] != 0) return false;
    }
    return true;
}

bool Board::place(const Piece& p) {
    bool all_hidden = true;
    for (const Cell& c : pieces::cells(p)) {
        cells_[c.y][c.x] = p.type + 1;
        if (c.y >= kHiddenRows) all_hidden = false;
    }
    return all_hidden;
}

std::vector<int> Board::full_rows() const {
    std::vector<int> rows;
    for (int row = 0; row < kRows; ++row) {
        const auto& r = cells_[row];
        if (std::all_of(r.begin(), r.end(), [](int v) { return v != 0; })) {
            rows.push_back(row);
        }
    }
    return rows;
}

void Board::remove_rows(const std::vector<int>& rows) {
    int write = kRows - 1;
    for (int read = kRows - 1; read >= 0; --read) {
        if (std::find(rows.begin(), rows.end(), read) != rows.end()) continue;
        cells_[write] = cells_[read];
        --write;
    }
    while (write >= 0) {
        cells_[write].fill(0);
        --write;
    }
}

int Board::drop_distance(const Piece& p) const {
    Piece q = p;
    int distance = 0;
    for (;;) {
        q.y += 1;
        if (!fits(q)) return distance;
        ++distance;
    }
}
