#pragma once

#include <vector>

#include "engine/input.h"
#include "board.h"
#include "piece.h"
#include "randomizer.h"

struct GameConfig {
    int start_level = 1;
    double das = 0.17;  // delay before auto-shift
    double arr = 0.05;  // auto-shift repeat interval
};

enum class GameEventType { PieceLocked, HardDrop, LinesCleared, LevelUp, Hold, GameOver };

struct GameEvent {
    GameEventType type = GameEventType::PieceLocked;
    int value = 0;            // lines cleared, rows dropped, new level, or held piece
    long long points = 0;     // points awarded by this event
    std::vector<Cell> cells;  // board cells involved (x = column, y = row)
};

class Gameplay {
public:
    enum class State { Falling, Clearing, GameOver };
    static constexpr double kClearDuration = 0.4;

    void start(const GameConfig& config);
    void update(const Input& in, double dt);
    // Transfers pending events to the caller and empties the event queue.
    std::vector<GameEvent> take_events();

    const Board& board() const { return board_; }
    const Piece& current() const { return cur_; }
    Piece ghost() const;
    int hold_piece() const { return hold_; }
    bool hold_used() const { return hold_used_; }
    int preview(int index) const { return randomizer_.peek(index); }
    long long score() const { return score_; }
    int lines() const { return lines_; }
    int level() const { return level_; }
    State state() const { return state_; }
    const std::vector<int>& clearing_rows() const { return clearing_rows_; }
    float clear_progress() const;

private:
    void spawn(int type);
    bool try_move(int dx, int dy);
    bool try_rotate(int dir);
    bool on_ground() const;
    void on_successful_move();
    void handle_horizontal(const Input& in, double dt);
    void apply_gravity(const Input& in, double dt);
    void hold();
    void hard_drop();
    void lock_piece();
    void finish_clear();
    void emit(GameEventType type, int value = 0, long long points = 0, std::vector<Cell> cells = {});

    GameConfig config_;
    Board board_;
    Randomizer randomizer_;
    Piece cur_;
    State state_ = State::GameOver;

    int hold_ = -1;
    bool hold_used_ = false;

    long long score_ = 0;
    int lines_ = 0;
    int level_ = 1;

    double fall_timer_ = 0.0;
    double lock_timer_ = 0.0;
    int lock_resets_ = 0;
    int lowest_y_ = 0;

    int das_dir_ = 0;
    double das_timer_ = 0.0;
    double arr_timer_ = 0.0;

    std::vector<int> clearing_rows_;
    double clear_timer_ = 0.0;

    std::vector<GameEvent> events_;
};
