#include "game/piece.h"

namespace {

    struct ShapeDef {
        int box;
        std::array<Cell, 4> cells;
        Color color;
    };

    const ShapeDef kShapes[PIECE_COUNT] = {
        { 4,{ { { 0, 1 },{ 1, 1 },{ 2, 1 },{ 3, 1 } } }, rgb(0, 220, 230) },   // I
        { 2,{ { { 0, 0 },{ 1, 0 },{ 0, 1 },{ 1, 1 } } }, rgb(240, 210, 0) },   // O
        { 3,{ { { 1, 0 },{ 0, 1 },{ 1, 1 },{ 2, 1 } } }, rgb(170, 60, 220) },  // T
        { 3,{ { { 1, 0 },{ 2, 0 },{ 0, 1 },{ 1, 1 } } }, rgb(60, 210, 70) },   // S
        { 3,{ { { 0, 0 },{ 1, 0 },{ 1, 1 },{ 2, 1 } } }, rgb(230, 50, 50) },   // Z
        { 3,{ { { 0, 0 },{ 0, 1 },{ 1, 1 },{ 2, 1 } } }, rgb(40, 90, 230) },   // J
        { 3,{ { { 2, 0 },{ 0, 1 },{ 1, 1 },{ 2, 1 } } }, rgb(240, 140, 20) },  // L
    };

    // SRS kicks: [from][0 = cw, 1 = ccw][test][x, y], with +y meaning UP.
    constexpr int kKicksJLSTZ[4][2][5][2] = {
        { { { 0, 0 },{ -1, 0 },{ -1, 1 },{ 0, -2 },{ -1, -2 } },
         { { 0, 0 },{ 1, 0 },{ 1, 1 },{ 0, -2 },{ 1, -2 } } },
        { { { 0, 0 },{ 1, 0 },{ 1, -1 },{ 0, 2 },{ 1, 2 } },
         { { 0, 0 },{ 1, 0 },{ 1, -1 },{ 0, 2 },{ 1, 2 } } },
        { { { 0, 0 },{ 1, 0 },{ 1, 1 },{ 0, -2 },{ 1, -2 } },
         { { 0, 0 },{ -1, 0 },{ -1, 1 },{ 0, -2 },{ -1, -2 } } },
        { { { 0, 0 },{ -1, 0 },{ -1, -1 },{ 0, 2 },{ -1, 2 } },
         { { 0, 0 },{ -1, 0 },{ -1, -1 },{ 0, 2 },{ -1, 2 } } },
    };

    constexpr int kKicksI[4][2][5][2] = {
        { { { 0, 0 },{ -2, 0 },{ 1, 0 },{ -2, -1 },{ 1, 2 } },
         { { 0, 0 },{ -1, 0 },{ 2, 0 },{ -1, 2 },{ 2, -1 } } },
        { { { 0, 0 },{ -1, 0 },{ 2, 0 },{ -1, 2 },{ 2, -1 } },
         { { 0, 0 },{ 2, 0 },{ -1, 0 },{ 2, 1 },{ -1, -2 } } },
        { { { 0, 0 },{ 2, 0 },{ -1, 0 },{ 2, 1 },{ -1, -2 } },
         { { 0, 0 },{ 1, 0 },{ -2, 0 },{ 1, -2 },{ -2, 1 } } },
        { { { 0, 0 },{ 1, 0 },{ -2, 0 },{ 1, -2 },{ -2, 1 } },
         { { 0, 0 },{ -2, 0 },{ 1, 0 },{ -2, -1 },{ 1, 2 } } },
    };

}  // namespace

namespace pieces {

    int box_size(int type) {
        return kShapes[type].box;
    }

    Color color(int type) {
        return kShapes[type].color;
    }

    std::array<Cell, 4> shape(int type, int rot) {
        const ShapeDef& s = kShapes[type];
        std::array<Cell, 4> out = s.cells;
        for (int r = 0; r < rot; ++r) {
            for (Cell& c : out) {
                c = Cell{ s.box - 1 - c.y, c.x };  // rotate clockwise inside the box
            }
        }
        return out;
    }

    std::array<Cell, 4> cells(const Piece& p) {
        std::array<Cell, 4> out = shape(p.type, p.rot);
        for (Cell& c : out) {
            c.x += p.x;
            c.y += p.y;
        }
        return out;
    }

    Cell kick(int type, int from, int dir, int test) {
        const int d = dir > 0 ? 0 : 1;
        const int* k = (type == PIECE_I) ? kKicksI[from][d][test] : kKicksJLSTZ[from][d][test];
        return Cell{ k[0], -k[1] };  // flip y: SRS +y is up, ours is down
    }

}  // namespace pieces
