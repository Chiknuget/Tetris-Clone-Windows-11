#include "game/settings.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace {

    constexpr double kDas[3] = { 0.22, 0.17, 0.12 };
    constexpr double kArr[3] = { 0.07, 0.05, 0.033 };
    const char* const kHandlingNames[3] = { "SLOW", "NORMAL", "FAST" };

    int handling_index(int handling) {
        return std::clamp(handling, 0, 2);  // returns a copy, so no reference escapes
    }

}  // namespace

double Settings::das_seconds() const {
    return kDas[handling_index(handling)];
}

double Settings::arr_seconds() const {
    return kArr[handling_index(handling)];
}

const char* Settings::handling_name() const {
    return kHandlingNames[handling_index(handling)];
}

bool Settings::load(const std::string& path) {
    std::ifstream file(path);
    if (!file) return false;

    std::string line;
    while (std::getline(file, line)) {
        const auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        const std::string key = line.substr(0, eq);
        std::istringstream value_stream(line.substr(eq + 1));
        int value = 0;
        if (!(value_stream >> value)) continue;

        if (key == "start_level") start_level = std::clamp(value, kMinLevel, kMaxLevel);
        else if (key == "ghost_piece") ghost_piece = value != 0;
        else if (key == "handling") handling = std::clamp(value, 0, 2);
        else if (key == "particles") particles = value != 0;
        else if (key == "screen_shake") screen_shake = value != 0;
    }
    return true;
}

bool Settings::save(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;
    file << "start_level=" << start_level << '\n';
    file << "ghost_piece=" << (ghost_piece ? 1 : 0) << '\n';
    file << "handling=" << handling << '\n';
    file << "particles=" << (particles ? 1 : 0) << '\n';
    file << "screen_shake=" << (screen_shake ? 1 : 0) << '\n';
    return static_cast<bool>(file);
}
