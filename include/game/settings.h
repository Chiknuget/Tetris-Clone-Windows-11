#pragma once

#include <string>

inline constexpr const char* kSettingsFile = "settings.cfg";

struct Settings {
    static constexpr int kMinLevel = 1;
    static constexpr int kMaxLevel = 15;

    int start_level = 1;
    bool ghost_piece = true;
    int handling = 1;  // 0 = slow, 1 = normal, 2 = fast
    bool particles = true;
    bool screen_shake = true;

    double das_seconds() const;
    double arr_seconds() const;
    const char* handling_name() const;

    bool load(const std::string& path);
    bool save(const std::string& path) const;
};
