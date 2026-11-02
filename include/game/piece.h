#pragma once

#include <array>

#include "engine/color.h"

struct Cell {
    int x;
    int y;
};

enum PieceType : int { PIECE_I, PIECE_O, PIECE_T, PIECE_S, PIECE_Z, PIECE_J, PIECE_L, PIECE_COUNT };

struct Piece {
    int type = 0;
    int rot = 0;  // 0 = spawn, 1 = right, 2 = 180, 3 = left
    int x = 0;    // top-left of the piece's box on the board
    int y = 0;
};

namespace pieces {

    constexpr int kKickTests = 5;

    int box_size(int type);
    Color color(int type);
    std::array<Cell, 4> shape(int type, int rot);  // cells relative to the piece's box
    std::array<Cell, 4> cells(const Piece& p);     // cells in board coordinates

    // Offset for wall-kick test number 'test' when rotating from 'from'
    // in direction dir (+1 clockwise, -1 counter-clockwise). y grows downward.
    Cell kick(int type, int from, int dir, int test);

}  // namespace pieces
