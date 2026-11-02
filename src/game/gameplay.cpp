#include "game/gameplay.h"

#include <algorithm>
#include <utility>

#include "game/scoring.h"

namespace {

	constexpr double kLockDelay = 0.5;
	constexpr int kMaxLockResets = 15;
	constexpr double kSoftDropInterval = 0.03;

	std::vector<Cell> to_vector(const std::array<Cell, 4>& cells) {
		return std::vector<Cell>(cells.begin(), cells.end());
	}

} // namespace

// Setup

void Gameplay::start(const GameConfig& config) {
	config_ = config;
	board_.clear();
	randomizer_.reset();
	events_.clear();
	clearing_rows_.clear();
	hold_ = -1;
	hold_used_ = false;
	score_ = 0;
	lines_ = 0;
	level_ = config.start_level;
	das_dir_ = 0;
	das_timer_ = 0.0;
	arr_timer_ = 0.0;
	clear_timer_ = 0.0;
	state_ = State::Falling;
	spawn(randomizer_.next());
}

std::vector<GameEvent> Gameplay::take_events() {
    std::vector<GameEvent> out;
    out.swap(events_);
    return out;
}

void Gameplay::emit(GameEventType type, int value, long long points, std::vector<Cell> cells) {
    GameEvent e;
    e.type = type;
    e.value = value;
    e.points = points;
    e.cells = std::move(cells);
    events_.push_back(std::move(e));
}

void Gameplay::spawn(int type) {
    cur_ = Piece{ type, 0, (Board::kCols - pieces::box_size(type)) / 2, 0 };
    fall_timer_ = 0.0;
    lock_timer_ = 0.0;
    lock_resets_ = 0;

    if (!board_.fits(cur_)) {
        state_ = State::GameOver;
        emit(GameEventType::GameOver);
        return;
    }
    try_move(0, 1);  // drop one row right away so the piece is visible
    lowest_y_ = cur_.y;
}

// Queries

Piece Gameplay::ghost() const {
    Piece g = cur_;
    g.y += board_.drop_distance(cur_);
    return g;
}

float Gameplay::clear_progress() const {
    if (state_ != State::Clearing) return 0.0f;
    return static_cast<float>(std::min(clear_timer_ / kClearDuration, 1.0));
}

// Movement

bool Gameplay::try_move(int dx, int dy) {
    Piece p = cur_;
    p.x += dx;
    p.y += dy;
    if (board_.fits(p)) {
        cur_ = p;
        return true;
    }
    return false;
}

bool Gameplay::try_rotate(int dir) {
    const int to = (cur_.rot + (dir > 0 ? 1 : 3)) % 4;
    for (int test = 0; test < pieces::kKickTests; ++test) {
        const Cell k = pieces::kick(cur_.type, cur_.rot, dir, test);
        Piece p = cur_;
        p.rot = to;
        p.x += k.x;
        p.y += k.y;
        if (board_.fits(p)) {
            cur_ = p;
            return true;
        }
    }
    return false;
}

bool Gameplay::on_ground() const {
    Piece p = cur_;
    p.y += 1;
    return !board_.fits(p);
}

void Gameplay::on_successful_move() {
    if (on_ground() && lock_resets_ < kMaxLockResets) {
        lock_timer_ = 0.0;
        ++lock_resets_;
    }
}

void Gameplay::handle_horizontal(const Input& in, double dt) {
    const bool left = in.down[KeyLeft];
    const bool right = in.down[KeyRight];

    int dir = 0;
    if (in.pressed[KeyLeft]) dir = -1;
    else if (in.pressed[KeyRight]) dir = 1;
    else if (left && right) dir = das_dir_;
    else if (left) dir = -1;
    else if (right) dir = 1;

    if (dir == 0) {
        das_dir_ = 0;
        return;
    }

    const bool fresh = (dir < 0 && in.pressed[KeyLeft]) || (dir > 0 && in.pressed[KeyRight]);
    if (dir != das_dir_ || fresh) {
        das_dir_ = dir;
        das_timer_ = 0.0;
        arr_timer_ = 0.0;
        if (try_move(dir, 0)) on_successful_move();
        return;
    }

    das_timer_ += dt;
    if (das_timer_ < config_.das) return;
    arr_timer_ += dt;
    while (arr_timer_ >= config_.arr) {
        arr_timer_ -= config_.arr;
        if (try_move(dir, 0)) {
            on_successful_move();
        }
        else {
            arr_timer_ = 0.0;
            break;
        }
    }
}

void Gameplay::apply_gravity(const Input& in, double dt) {
    const bool soft = in.down[KeyDown];
    double interval = scoring::gravity_interval(level_);
    if (soft) interval = std::min(interval, kSoftDropInterval);

    fall_timer_ = std::min(fall_timer_, interval); // don't carry over time from a slower speed
    fall_timer_ += dt;
    while (fall_timer_ >= interval) {
        fall_timer_ -= interval;
        if (try_move(0, 1)) {
            if (soft) score_ += scoring::kSoftDropPerCell;
            if (cur_.y > lowest_y_) {
                lowest_y_ = cur_.y;
                lock_resets_ = 0;
            }
        }
        else {
            fall_timer_ = 0.0;
            break;
        }
    }
}

void Gameplay::hold() {
    if (hold_used_) return;
    const int current = cur_.type;
    const int next = (hold_ < 0) ? randomizer_.next() : hold_;
    hold_ = current;
    emit(GameEventType::Hold, current);
    spawn(next);
    hold_used_ = true;
}

void Gameplay::hard_drop() {
    const int distance = board_.drop_distance(cur_);
    cur_.y += distance;
    const long long points = static_cast<long long>(distance) * scoring::kHardDropPerCell;
    score_ += points;
    emit(GameEventType::HardDrop, distance, points, to_vector(pieces::cells(cur_)));
    lock_piece();
}

// Locking and Clearing

void Gameplay::lock_piece() {
    const bool lock_out = board_.place(cur_);
    emit(GameEventType::PieceLocked, 0, 0, to_vector(pieces::cells(cur_)));

    if (lock_out) {
        state_ = State::GameOver;
        emit(GameEventType::GameOver);
        return;
    }

    std::vector<int> rows = board_.full_rows();
    if (rows.empty()) {
        hold_used_ = false;
        spawn(randomizer_.next());
        return;
    }

    const int count = static_cast<int>(rows.size());
    const long long points = scoring::line_clear_points(count, level_);
    score_ += points;
    lines_ += count;

    std::vector<Cell> cells;
    for (int row : rows) {
        for (int col = 0; col < Board::kCols; ++col) {
            cells.push_back(Cell{ col, row });
        }
    }
    emit(GameEventType::LinesCleared, count, points, std::move(cells));

    const int new_level = scoring::level_for_lines(lines_, config_.start_level);
    if (new_level > level_) {
        level_ = new_level;
        emit(GameEventType::LevelUp, level_);
    }

    // Rows stay on the board while the animation plays; finish_clear removes them.
    clearing_rows_ = std::move(rows);
    clear_timer_ = 0.0;
    state_ = State::Clearing;
}

void Gameplay::finish_clear() {
    board_.remove_rows(clearing_rows_);
    clearing_rows_.clear();
    state_ = State::Falling;
    hold_used_ = false;
    spawn(randomizer_.next());
}

// Update

void Gameplay::update(const Input& in, double dt) {
    if (state_ == State::GameOver) return;

    if (state_ == State::Clearing) {
        clear_timer_ += dt;
        if (clear_timer_ >= kClearDuration) finish_clear();
        return;
    }

    if (in.pressed[KeyC]) {
        hold();
        if (state_ != State::Falling) return;
    }

    if (in.pressed[KeyUp]) {
        hard_drop();
        return;
    }

    if (in.pressed[KeySpace] || in.pressed[KeyX]) {
        if (try_rotate(+1)) on_successful_move();
    }
    if (in.pressed[KeyZ]) {
        if (try_rotate(-1)) on_successful_move();
    }

    handle_horizontal(in, dt);
    apply_gravity(in, dt);

    if (on_ground()) {
        lock_timer_ += dt;
        if (lock_timer_ >= kLockDelay) lock_piece();
    }
    else {
        lock_timer_ = 0.0;
    }
}
