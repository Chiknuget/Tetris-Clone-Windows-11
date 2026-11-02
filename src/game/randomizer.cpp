#include "game/randomizer.h"

#include <algorithm>
#include <cstddef>

#include "game/piece.h"

Randomizer::Randomizer() : rng_(std::random_device{}()) {
    reset();
}

void Randomizer::reset() {
    bag_.clear();
    queue_.clear();
    refill();
}

int Randomizer::from_bag() {
    if (bag_.empty()) {
        for (int i = 0; i < PIECE_COUNT; ++i) {
            bag_.push_back(i);
        }
        std::shuffle(bag_.begin(), bag_.end(), rng_);
    }
    const int t = bag_.back();
    bag_.pop_back();
    return t;
}

void Randomizer::refill() {
    while (queue_.size() < static_cast<std::size_t>(kPreviewCount)) {
        queue_.push_back(from_bag());
    }
}

int Randomizer::next() {
    const int t = queue_.front();
    queue_.pop_front();
    refill();
    return t;
}

int Randomizer::peek(int index) const {
    return queue_[static_cast<std::size_t>(index)];
}
