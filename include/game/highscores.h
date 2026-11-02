#pragma once

#include <string>
#include <vector>

inline constexpr const char* kHighScoreFile = "highscores.txt";

struct HighScoreEntry {
    std::string name;
    long long score = 0;
    int level = 1;
    int lines = 0;
};

class HighScoreTable {
public:
    static constexpr int kMaxEntries = 10;

    bool load(const std::string& path);
    bool save(const std::string& path) const;

    bool qualifies(long long score) const;
    int insert(const HighScoreEntry& entry);  // returns 0-based rank, or -1

    const std::vector<HighScoreEntry>& entries() const { return entries_; }
    long long best() const;
    int last_inserted() const { return last_inserted_; }

private:
    std::vector<HighScoreEntry> entries_;
    int last_inserted_ = -1;
};
