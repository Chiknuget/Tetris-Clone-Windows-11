#include "game/highscores.h"

#include <algorithm>
#include <cstddef>
#include <fstream>

bool HighScoreTable::load(const std::string& path) {
    std::ifstream file(path);
    if (!file) return false;

    entries_.clear();
    HighScoreEntry e;
    while (file >> e.name >> e.score >> e.level >> e.lines) {
        entries_.push_back(e);
    }
    std::stable_sort(entries_.begin(), entries_.end(),
        [](const HighScoreEntry& a, const HighScoreEntry& b) { return a.score > b.score; });
    if (entries_.size() > static_cast<std::size_t>(kMaxEntries)) {
        entries_.resize(kMaxEntries);
    }
    return true;
}

bool HighScoreTable::save(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;
    for (const HighScoreEntry& e : entries_) {
        file << e.name << ' ' << e.score << ' ' << e.level << ' ' << e.lines << '\n';
    }
    return static_cast<bool>(file);
}

bool HighScoreTable::qualifies(long long score) const {
    if (score <= 0) return false;
    if (entries_.size() < static_cast<std::size_t>(kMaxEntries)) return true;
    return score > entries_.back().score;
}

int HighScoreTable::insert(const HighScoreEntry& entry) {
    last_inserted_ = -1;
    if (!qualifies(entry.score)) return -1;

    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&](const HighScoreEntry& e) { return entry.score > e.score; });
    const int rank = static_cast<int>(it - entries_.begin());
    entries_.insert(it, entry);
    if (entries_.size() > static_cast<std::size_t>(kMaxEntries)) {
        entries_.resize(kMaxEntries);
    }
    last_inserted_ = rank;
    return rank;
}

long long HighScoreTable::best() const {
    return entries_.empty() ? 0 : entries_.front().score;
}
